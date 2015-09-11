#include "trackball.hpp"

void Trackball::initOpenGLMatrices() {
  modelMatrix = Eigen::Affine3f::Identity();
  projectionMatrix = Eigen::Matrix4f::Identity();
  viewMatrix = Eigen::Affine3f::Identity();

  //Translate viewMatrix outside the origin to be able to see the model.
  viewMatrix.translate(defaultTranslation);

}

Eigen::Matrix4f Trackball::createPerspectiveMatrix(float fy, float aspect_ratio, float near_plane,float far_plane){

  Eigen::Matrix4f out = Eigen::Matrix4f::Zero();
  nearPlane = near_plane;
  farPlane = far_plane;
	 
  fovy = fy;

  const float
    y_scale = (float)1.0/tan((fovy/2.0)*(PI/180.0)),
    x_scale = y_scale / aspect_ratio,
    frustum_length = far_plane - near_plane;

  focal_length = x_scale;
	 
  out(0,0) = x_scale;
  out(1,1) = y_scale;
  out(2,2) = -((far_plane + near_plane) / frustum_length);
  out(3,2) = -1.0;
  out(2,3) = -((2.0 * near_plane * far_plane) / frustum_length);
	    
  return out;
}

void Trackball::usePerspectiveMatrix(float fy, float aspect_ratio, float near_plane,float far_plane) {
	Eigen::Matrix4f proj = createPerspectiveMatrix(fy , aspect_ratio, near_plane , far_plane );
	setProjectionMatrix(proj);
}

void Trackball::useTrackballPerspectiveMatrix(float fy, float aspect_ratio, float near_plane,float far_plane) {
	Eigen::Matrix4f proj = createPerspectiveMatrix(fy , aspect_ratio, near_plane , far_plane );
	setTrackballProjectionMatrix(proj);
}

Eigen::Matrix4f Trackball::createOrthographicMatrix(float left, float right, float bottom, float top, float near, float far) {

  Eigen::Matrix4f out = Eigen::Matrix4f::Zero();

  out(0,0) = 2/(right-left);
  out(1,1) = 2/(top-bottom);
  out(2,2) = -2/(far-near);
  out(3,3) = 1;
  out(0,3) = -(right+left)/(right-left);
  out(1,3) = -(top+bottom)/(top-bottom);
  out(2,3) = -(far+near)/(far-near);

  return out;

}

Eigen::Matrix4f Trackball::useOrthographicMatrix(float left, float right, float bottom, float top, float near, float far) {
	Eigen::Matrix4f proj = createOrthographicMatrix(left,right,bottom,top,near,far);
	setProjectionMatrix(proj);
	return proj;
}

Eigen::Matrix4f Trackball::useTrackballOrthographicMatrix(float left, float right, float bottom, float top, float near, float far) {
        Eigen::Matrix4f proj = createOrthographicMatrix(left,right,bottom,top,near,far);
	setTrackballProjectionMatrix(proj);
	return proj;
}

Eigen::Vector3f Trackball::calculateZCoord(float x, float y) {
  //In order to handle points outside the sphere, we will use two surfaces: A Sphere with radius = 0.8 and a Hyperbolic Sheet. More information about this usage in: http://www.opengl.org/wiki/Trackball.

  float z;

  //If (x,y) corresponds to a point before the intersection of the two surfaces, z can be calculated by the Sphere's equation.
  if(x*x + y*y <= (radius*radius)/2.0) {
    z = sqrt(radius*radius - x*x - y*y);
  }

  //Else, it will be calculated by the Hyperbolic Sheet's equation.
  else {
    z = (radius*radius)/(2.0*sqrt(x*x+y*y));
  }

  Eigen::Vector3f position = Eigen::Vector3f(x,y,z);
	
  return position;
}


void Trackball::spin( Eigen::Quaternionf axis)
{

        this->quaternion =  axis * this->quaternion;
        quaternion.normalize();

        applyTransformations();

}
void Trackball::calculateRotation() {
  //Given two position vectors, corresponding to the initial and final mouse coordinates, calculate the rotation of the sphere that will keep the mouse always in the initial position.

  if(initialPosition.norm() > 0) {
	  initialPosition.normalize();
  }
	if(finalPosition.norm() > 0) {
	  finalPosition.normalize();
	}

  //cout << "Initial Position: " << initialPosition.transpose() << " Final Position: " << finalPosition.transpose() << endl << endl;

  Eigen::Vector3f rotationAxis = initialPosition.cross(finalPosition);

  if(rotationAxis.norm() != 0) {
    rotationAxis.normalize();
  }

  float dot = initialPosition.dot(finalPosition);

  float rotationAngle = (dot <= 1) ? acos(dot) : 0;//If, by losing floating point precision, the dot product between the initial and final positions is bigger than one, ignore the rotation.

  Eigen::Quaternion<float> q (Eigen::AngleAxis<float>(rotationAngle,rotationAxis));

  quaternion = q * quaternion;
  quaternion.normalize();

}

void Trackball::calculateTranslation() {

  Eigen::Vector2f translationFactor = finalTranslationPosition - initialTranslationPosition;

  translationVector += quaternion.inverse() * Eigen::Vector3f(translationFactor[0],translationFactor[1], 0.0);
	
  //translationVector = (translationVector*focal_length)/zCoordinate;

  //translationVector += Eigen::Vector3f(translationFactor[0],translationFactor[1],0.0);

}

void Trackball::rotateCamera() {
  calculateRotation();
  applyTransformations();
	
}

void Trackball::translateCamera() {
	
  calculateTranslation();
  applyTransformations();
	
}

void Trackball::setInitialRotationPosition(float x, float y) {

  initialPosition = calculateZCoord(x,y);


}

void Trackball::setInitialRotationPosition(Eigen::Vector2f pos) {

  initialPosition = calculateZCoord(pos[0],pos[1]);

}

void Trackball::setFinalRotationPosition(float x, float y) {

  finalPosition = calculateZCoord(x,y);
	
}

void Trackball::setFinalRotationPosition(Eigen::Vector2f pos) {

  finalPosition = calculateZCoord(pos[0],pos[1]);
	
}

void Trackball::setInitialTranslationPosition(float x, float y) {

  initialTranslationPosition = Eigen::Vector2f(x,y);

}

void Trackball::setInitialTranslationPosition(Eigen::Vector2f pos) {

  initialTranslationPosition = Eigen::Vector2f(pos[0],pos[1]);

}

void Trackball::setFinalTranslationPosition(float x, float y) {

  finalTranslationPosition = Eigen::Vector2f(x,y);
	
}

void Trackball::setFinalTranslationPosition(Eigen::Vector2f pos) {

  finalTranslationPosition = Eigen::Vector2f(pos[0],pos[1]);
	
}

void Trackball::increaseZoom(float scale) {
  zoom *= scale;
  applyTransformations();
}

void Trackball::decreaseZoom(float scale) {
  zoom /= scale;
  applyTransformations();
}

void Trackball::applyScaleToViewMatrix(float scale) {
  zoom = scale;
  applyTransformations();
}

void Trackball::applyScaleToModelMatrix(float scale) {
  modelMatrix.scale(scale);
}

void Trackball::translateViewMatrix(Eigen::Vector3f trans) {
  viewMatrix.translate(trans);
}

void Trackball::translateModelMatrix(Eigen::Vector3f trans) {
  modelMatrix.translate(trans);
}

void Trackball::applyTransformations() {
  viewMatrix = Eigen::Affine3f::Identity();
  viewMatrix.translate(defaultTranslation);
  viewMatrix.rotate(quaternion);
  viewMatrix.translate(translationVector);
  viewMatrix.scale(Eigen::Vector3f(zoom,zoom,zoom));
  updateAngleAxisArray();
}

void Trackball::createTrackballRepresentation() {

  int index = 0;
  for (float theta = 0; theta<2*PI; theta += (2*PI)/200.0) {

    vertices[index] = 0.8*cos(theta);
    vertices[index+1] = 0.8*sin(theta);
    vertices[index+2] = 0.0f;
    vertices[index+3] = 1.0f;

    index += 4;
  }

}

void Trackball::initializeBuffers() {

	createBuffers();
	bufferData();

}

void Trackball::createBuffers() {

  glGenVertexArrays(1,&bufferIDs[0]);
  glGenBuffers(2,&bufferIDs[1]);
}

void Trackball::bufferData() {

  glBindBuffer(GL_ARRAY_BUFFER,bufferIDs[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
  glBindBuffer(GL_ARRAY_BUFFER,0);

}

void Trackball::bindBuffers() {

  //VAO:
  glBindVertexArray(bufferIDs[0]);

  //VBO:
  glBindBuffer(GL_ARRAY_BUFFER,bufferIDs[1]);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);


}

void Trackball::unbindBuffers() {

  glBindBuffer(GL_ARRAY_BUFFER,0);

  glDisableVertexAttribArray(0);
}

void Trackball::render() {

  if(drawTrackball) {

    trackballShader->enable();

    //Using unique viewMatrix for the trackball, considering only the rotation to be visualized.
    Eigen::Affine3f trackballViewMatrix = Eigen::Affine3f::Identity();
    trackballViewMatrix.translate(defaultTranslation);
    trackballViewMatrix.rotate(quaternion);

    trackballShader->setUniform("viewMatrix", trackballViewMatrix.data(), 4, GL_FALSE, 1);
    trackballShader->setUniform("projectionMatrix", trackballProjectionMatrix.data(), 4 ,GL_FALSE, 1);
    trackballShader->setUniform("nearPlane", nearPlane);
    trackballShader->setUniform("farPlane", farPlane);

    bindBuffers();

    //X:
    Eigen::Vector4f colorVector(1.0, 0.0, 0.0, 1.0);
    trackballShader->setUniform("modelMatrix",(Eigen::Affine3f::Identity()*Eigen::AngleAxis<float>(PI/2.0,Eigen::Vector3f(0.0,1.0,0.0))).data(), 4, GL_FALSE, 1);
    trackballShader->setUniform("in_Color",colorVector.data(),4,1);
    glDrawArrays(GL_LINE_LOOP, 0, 200);

    //Y:
    colorVector << 0.0, 1.0, 0.0, 1.0;
    trackballShader->setUniform("modelMatrix",(Eigen::Affine3f::Identity()*Eigen::AngleAxis<float>(PI/2.0,Eigen::Vector3f(1.0,0.0,0.0))).data(), 4, GL_FALSE, 1);
    trackballShader->setUniform("in_Color",colorVector.data(),4,1);
    glDrawArrays(GL_LINE_LOOP, 0, 200);

    //Z:
    colorVector << 0.0, 0.0, 1.0, 1.0;
    trackballShader->setUniform("in_Color",colorVector.data(),4,1);
    trackballShader->setUniform("modelMatrix",Eigen::Affine3f::Identity().data(), 4, GL_FALSE, 1);
    glDrawArrays(GL_LINE_LOOP, 0, 200);

    unbindBuffers();

    trackballShader->disable();

  }

}

void Trackball::errorCheckFunc(string file, int line){
  //OpenGL Error Handling Function:
  GLenum ErrorCheckValue = glGetError();
  if (ErrorCheckValue != GL_NO_ERROR)
    {
      cerr << "GL error in " << file << "  line " << line << " : " << gluErrorString(ErrorCheckValue) << endl;
      exit(-1);
    }
}

