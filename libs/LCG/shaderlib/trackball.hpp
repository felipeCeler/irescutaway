#ifndef __TRACKBALL__
#define __TRACKBALL__
#define PI 3.14159265358979323846

#include "shader.hpp"
#include<Eigen/Dense>
#include <cmath>

using namespace std;


/**
 * Trackball Class. This class stores the OpenGL Matrices, as well as the representation of the trackball, using a sphere with 
 * radius equal to 0.8 centered in the middle of the window (represented by the equation: x^2 + y^2 + z^2 = r^2), 
 * nested in an hyperbolic sheet opening (represented by the equation: z = (r^2/2)/sqrt(x^2+y^2).
 * It is used for camera and light positioning.
 * It's important to notice that the x and y mouse coordinates passed to the trackball must be ranging from (-1,-1), being the
 * lower left corner of screen, to (1,1) being the higher right corner. This means the coordinate system is centered in screen
 * and normalized. Also, the view matrix is by default translated by the vector [0,0,-2], in order to remove the camera from the center of the world.
 **/
class Trackball {
	
public:

  ///Default constructor.
  Trackball() : zoom(1.0), rotating(false), translating(false), drawTrackball(true), radius(0.8) { 

    //Creates the mesh that will be used to represent the trackball's sphere.
    createTrackballRepresentation();

    //Initialize the shader used for trackball rendering:
    trackballShader = new Shader("trackballShader", "shaders/trackballShader.vert", "shaders/trackballShader.frag");

    //Initialize buffers used for trackball rendering:
    bufferIDs = new GLuint[3];

    defaultTranslation << 0.0, 0.0, -4.0;

    reset();


  }

  /**
   * @brief Resets trackball to initial position and orientation
   */
  void reset(void) {
      //Initialize the quaternion used for Trackball rotation:
      quaternion = Eigen::Quaternion<float>::Identity();
      zoom = 1.0;
      translationVector << 0.0, 0.0, 0.0;
  }

  /**
   * @return The default translation vector for the view matrix
   **/
  Eigen::Vector3f getDefaultTranslation() {
    return defaultTranslation;
  }

  /**
   * @brief The center of the camera in world space
   * @return Camera center
   */
  Eigen::Vector3f getCenter() {

      return viewMatrix.linear().inverse() * (-viewMatrix.translation());
      //return (viewMatrix.matrix().inverse() * (Eigen::Vector4f(0.0,0.0,0.0,1.0))).head(3);
  }


  /**
   *@return The OpenGL Model Matrix.
   **/
  Eigen::Affine3f getModelMatrix() {
    return modelMatrix;
  }	

  void setModelMatrix ( const Eigen::Affine3f m )
  {
          modelMatrix = m;
  }

  /**
   *@return The OpenGL View Matrix.
   **/
  Eigen::Affine3f getViewMatrix() {
    return viewMatrix;
  }	

  void setViewMatrix ( const Eigen::Affine3f v )
  {
          viewMatrix = v;
  }

  /**
   *@return The OpenGL Projection Matrix.
   **/
  Eigen::Matrix4f getProjectionMatrix() {
    return projectionMatrix;
  }

  /**
   * @return The rotation part of the view matrix.
   **/
  Eigen::Matrix4f getRotationMatrix() {
    Eigen::Affine3f rotation = Eigen::Affine3f::Identity();
    rotation.rotate(quaternion);
    return rotation.matrix();
  }


  /**
   * @return The Translation part of the View Matrix.
   **/
  Eigen::Matrix4f getTranslationMatrix() {
    Eigen::Affine3f translation = Eigen::Affine3f::Identity();
    translation.translate(defaultTranslation);
    translation.translate(translationVector);    
    return translation.matrix();
  }

  /**
   * @return The Scale part of the View Matrix. Typically, this represents the trackball zoom factor.
   **/
  Eigen::Matrix4f getScaleMatrix() {
    Eigen::Affine3f scaling = Eigen::Affine3f::Identity();
    scaling.scale(Eigen::Vector3f(zoom,zoom,zoom));
    return scaling.matrix();
  }

  /**
   * @return The zoom factor that composes the scale matrix
   **/
  float getZoom(void) {
    return zoom;
  }

  /**
   * @return The perspective scale factor
   **/
  float getPerspectiveScale(void) {
    return (float)1.0/tan((fovy/2.0)*(PI/180.0));
  }

   /**
   * Sets the OpenGL Projection Matrix.
   **/
  void setProjectionMatrix(Eigen::Matrix4f mat) {
    projectionMatrix = mat;
  }

  /**
   * Sets the OpenGL Projection Matrix used for the trackball rendering.
   **/
  void setTrackballProjectionMatrix(Eigen::Matrix4f mat) {
    trackballProjectionMatrix = mat;
  }

  ///Renders the trackball representation. 
  void render();

	
  ///Enable the rotating flag, indicating that the mouse callback functions will begin reading the mouse coordinates in order to start the trackball's rotation.
  void beginRotation() {
    rotating = true;
  }

	
  ///Disable the rotating flag, indicating that the mouse callback functions will stop reading the mouse coordinates in order to stop the trackball's rotation.
  void endRotation() {
    rotating = false;
  }

	
  ///Enable the translating flag, indicating that the mouse callback functions will begin reading the mouse coordinates in order to start the trackball's translation.
  void beginTranslation() {
    translating = true;
  }

	
  ///Disable the translating flag, indicating that the mouse callback functions will stop reading the mouse coordinates in order to stop the trackball's translation.
  void endTranslation() {
    translating = false;
  }

  /**
   * Indicates wether the trackball is being rotated or not.
   **/
  bool isRotating() {
    return rotating;
  }

  /**
   * Indicates wether the trackball is currently being translated or not.
   **/
  bool isTranslating() {
    return translating;
  }

  ///Calculates and applies the rotation transformations to the trackball.
  void rotateCamera();

  ///Calculates and applies the translation transformations to the trackball.
  void translateCamera();

  /**
   * @return The initial position vector used for trackball's rotation computation.
   **/
  Eigen::Vector3f getInitialRotationPosition() {
    return initialPosition;
  }

  /**
   * Sets the initial position used for trackball's rotation computation, returned by the mouse callback functions, individually. The coordinates must be centered in screen, ranging from -1 to 1.
   * @param x X coordinate of the initial rotation position vector.
   * @param y Y coordinate of the initial rotation position vector.
   **/
  void setInitialRotationPosition(float x, float y);

  /**
   * Sets the initial position used for trackball's rotation computation, returned by the mouse callback functions, by vector. The coordinates must be centered in screen, ranging from -1 to 1.
   * @param pos Initial rotation position vector.
   **/
  void setInitialRotationPosition(Eigen::Vector2f pos);

  /**
   * @return The initial position vector used for trackball's translation computation.
   **/
  Eigen::Vector2f getInitialTranslationPosition() {
    return initialTranslationPosition;
  }
	
  /**
   * Sets the initial position used for trackball's translation computation, returned by the mouse callback functions, individually. The coordinates must be centered in screen, ranging from -1 to 1.
   * @param x X coordinate of the initial translation position vector.
   * @param y Y coordinate of the initial translation position vector.
   **/
  void setInitialTranslationPosition(float x, float y);

  /**
   * Sets the initial position used for trackball's translation computation, returned by the mouse callback functions, by vector. The coordinates must be centered in screen, ranging from -1 to 1.
   * @param pos Initial translation position vector.
   **/
  void setInitialTranslationPosition(Eigen::Vector2f pos);

  /**
   * @return The final position vector used for trackball's rotation computation.
   **/
  Eigen::Vector3f getFinalRotationPosition() {
    return finalPosition;
  }

  /**
   * Sets the final position used for trackball's rotation computation, returned by the mouse callback functions, individually. The coordinates must be centered in screen, ranging from -1 to 1.
   * @param x X coordinate of the final rotation position vector.
   * @param y Y coordinate of the final rotation position vector.
   **/
  void setFinalRotationPosition(float x, float y);

  /**
   * Sets the final position used for trackball's rotation computation, returned by the mouse callback functions, by vector. The coordinates must be centered in screen, ranging from -1 to 1.
   * @param pos Final rotation position vector.
   **/
  void setFinalRotationPosition(Eigen::Vector2f pos);

  /**
   * @return The final position vector used for trackball's translation computation.
   **/
  Eigen::Vector2f getFinalTranslationPosition() {
    return finalTranslationPosition;
  }	
	
  /**
   * Sets the final position used for trackball's translation computation, returned by the mouse callback functions, individually. The coordinates must be centered in screen, ranging from -1 to 1.
   * @param x X coordinate of the final translation position vector.
   * @param y Y coordinate of the final translation position vector.
   **/
  void setFinalTranslationPosition(float x, float y);

  /**
   * Sets the final position used for trackball's translation computation, returned by the mouse callback functions, by vector. The coordinates must be centered in screen, ranging from -1 to 1.
   * @param pos Final translation position vector.
   **/
  void setFinalTranslationPosition(Eigen::Vector2f pos);

  /**
   * @return A pointer to the flag indicating wether the trackball representation should be drawn. Useful for interface callbacks.
   **/
  bool* getDrawTrackballFlag() {
    return &drawTrackball;
  }

  ///Updates the quaternion angle axis array after each rotation.
  void updateAngleAxisArray() {

    quaternionVector[0] = quaternion.x();
    quaternionVector[1] = quaternion.y();
    quaternionVector[2] = quaternion.z();
    quaternionVector[3] = quaternion.w();
		
  }

  /**
   * @return The pointer to the quaternion vector.
   **/
  float* getQuaternionVector() {
    return &quaternionVector[0];
  }

  void setQuaternion( const Eigen::Quaternionf& q) {
   this->quaternion = q;
   quaternion.normalize();
   applyTransformations();

  }

  /**
     * @return The pointer to the quaternion vector.
     **/
    Eigen::Quaternionf getQuaternion() {
      return quaternion;
    }

  ///Initializes the Model, View and Projection matrices. They are all initialized as Identity matrices, but view is then translated by the defaultTranslation vector.
  void initOpenGLMatrices();

  /**
   * Creates and returns a perspective matrix with the given vertical field of view angle, aspect ratio and near and far planes.
   * @param fovy Vertical field of view angle
   * @param aspect_ratio Ratio of width to the height of the viewport
   * @param near_plane Near plane
   * @param near_plane Far plane
   * @return Return the created perspective matrix.
   **/
  Eigen::Matrix4f createPerspectiveMatrix(float fovy,float aspect_ratio, float near_plane,float far_plane);

  /**
   * Creates a perspective matrix with the given vertical field of view angle, aspect ratio and near and far planes, then set the projection matrix to it.
   * @param fovy Vertical field of view angle
   * @param aspect_ratio Ratio of width to the height of the viewport
   * @param near_plane Near plane
   * @param near_plane Far plane
   * @return Return the created perspective matrix.
   **/
  void usePerspectiveMatrix(float fovy,float aspect_ratio, float near_plane,float far_plane);

  /**
   * Creates a perspective matrix with the given vertical field of view angle, aspect ratio and near and far planes, then set the trackball projection matrix to it.
   * @param fovy Vertical field of view angle
   * @param aspect_ratio Ratio of width to the height of the viewport
   * @param near_plane Near plane
   * @param near_plane Far plane
   * @return Return the created perspective matrix.
   **/
  void useTrackballPerspectiveMatrix(float fovy,float aspect_ratio, float near_plane,float far_plane);

  /**
   * Creates and returns an orthographic matrix with the given left, right, bottom, top, near and far planes.
   * @param left Left plane for orthographic view.
   * @param right Right plane for orthographic view.
   * @param bottom Bottom plane for orthographic view.
   * @param top Top lane for orthographic view.
   * @param near Near plane for orthographic view.
   * @param far Far plane for orthographic view.
   * @return Return the created orthographic matrix.
   **/
  Eigen::Matrix4f createOrthographicMatrix(float left, float right, float bottom, float top, float near, float far);

  /**
   * Creates an orthographic matrix with the given left, right, bottom, top, near and far planes, then set the projection matrix to it.
   * @param left Left plane for orthographic view.
   * @param right Right plane for orthographic view.
   * @param bottom Bottom plane for orthographic view.
   * @param top Top lane for orthographic view.
   * @param near Near plane for orthographic view.
   * @param far Far plane for orthographic view.
   * @return Return the created orthographic matrix.
   **/
  Eigen::Matrix4f useOrthographicMatrix(float left, float right, float bottom, float top, float near, float far);

  /**
   * Creates an orthographic matrix with the given left, right, bottom, top, near and far planes, then set the trackball projection matrix to it.
   * @param left Left plane for orthographic view.
   * @param right Right plane for orthographic view.
   * @param bottom Bottom plane for orthographic view.
   * @param top Top lane for orthographic view.
   * @param near Near plane for orthographic view.
   * @param far Far plane for orthographic view.
   * @return Return the created orthographic matrix.
   **/
  Eigen::Matrix4f useTrackballOrthographicMatrix(float left, float right, float bottom, float top, float near, float far);

  /**
   * Increases the zoom on the scene by appling a scale to the View Matrix. The current scale used in View Matrix is multiplied by the given scale factor.
   * @param scale The given scale to be multiplied by current scale factor.
   **/
  void increaseZoom(float scale);

  /**
   * Decreases the zoom on the scene by appling a scale to the View Matrix. The current scale used in View Matrix is divided by the given scale factor.
   * @param scale The given scale factor, to which the current scale factor will be divided by.
   **/
  void decreaseZoom(float scale);



  void spin( Eigen::Quaternionf axis);

  /**
   * Applies a scale factor to the viewMatrix. The current scale used in View Matrix will be substituted by the given scale factor.
   * @param scale The new scale factor to be applied to View Matrix.
   **/
  void applyScaleToViewMatrix(float scale);

  /**
   * Applies a scale factor to the modelMatrix. The current scale used in Model Matrix will be substituted by the given scale factor.
   * @param scale The scale factor to be applied.
   **/
  void applyScaleToModelMatrix(float scale);

  /**
   * Translates the view matrix by a given vector trans.
   **/
  void translateViewMatrix(Eigen::Vector3f trans);

  /**
   * Translates the model matrix by a given vector trans.
   **/
  void translateModelMatrix(Eigen::Vector3f trans);

  ///Initializes buffers for trackball's representation by creating them and buffering the data.
  void initializeBuffers();
	
  ///Creates buffers for trackball's representation.
  void createBuffers();
	
  ///Buffer vertices and indices data for trackball's representation.
  void bufferData();

  ///Bind buffers for trackball's representation.
  void bindBuffers();

  ///Unbind buffers for trackball's representation.
  void unbindBuffers();

  ///Load Trackball Shader file.
  void loadShader() {
    trackballShader->initialize();
  }

  ///Default destructor.
  ~Trackball() {
    ///Delete buffers:
    if (bufferIDs) {
      delete [] bufferIDs;
    }

    //Delete trackball shader.
    if(trackballShader) {
      delete trackballShader;
    }
  }

private:
	
  /**
   *Calculates z coordinate in the trackball's coordinate system, relative to a given position (x,y).
   *@param x X coordinate.
   *@param y Y coordinate.
   **/
  Eigen::Vector3f calculateZCoord(float x, float y);

  ///Calculates the trackball's rotation, using initial and final position vectors stored in the object.
  void calculateRotation();

  ///Calculates the trackball's translation, using initial and final position vectors stored in the object.
  void calculateTranslation();

  ///Applies all trackball's transformations to the View Matrix.
  void applyTransformations();

  ///Creates the circle representating rotation around Z axis. The other circles will be created by simply rotating this one around the desired axis.
  void createTrackballRepresentation();

  ///The current scale being applied to the View Matrix.
  float zoom;

  ///Flag that indicates wether the trackball is being rotated.
  bool rotating;

  ///Flag that indicates wether the trackball is being translated.
  bool translating;

  ///Flag that indicates wether the trackball's representation should be drawn.
  bool drawTrackball;

  ///Projection matrix used for the trackball's drawing. By default it is defined as an orthogonal projection matrix.
  Eigen::Matrix4f	 trackballProjectionMatrix;

  ///OpenGL Projection Matrix. By default it is defined as a perspective projection matrix.
  Eigen::Matrix4f	 projectionMatrix;

  ///OpenGL View Matrix. It is composed by the Trackball's Translation, the Trackball's Scale factor (given by the mouse) and the Trackball's Rotation Matrix.
  Eigen::Affine3f viewMatrix;

  ///OpenGL Model Matrix. It is composed by the Normalization Scale Factor and the Translation used for model's normalization.
  Eigen::Affine3f modelMatrix;

  ///Initial position vector used to calculate trackball's rotation.
  Eigen::Vector3f initialPosition;

  ///Final position vector used to calculate trackball's rotation.
  Eigen::Vector3f finalPosition;

  ///Initial position vector used to calculate trackball's translation.
  Eigen::Vector2f initialTranslationPosition;

  ///Final position vector used to calculate trackball's translation.
  Eigen::Vector2f finalTranslationPosition;

  ///Near plane used for projection matrix.
  float nearPlane;

  ///Far plane used for projection matrix.
  float farPlane;

  ///Camera's Focal Lenght.
  float focal_length;

  /// Field of view angle in y axis
  float fovy;

  ///Trackball's quaternion.
  Eigen::Quaternion<float> quaternion;  

  ///Trackball's translation vector.
  Eigen::Vector3f translationVector;

  /// Default translation to move camera away from center
  Eigen::Vector3f defaultTranslation;

  ///The vertices for the first circle of the trackball's representation.
  float vertices[800];

  ///Buffer Objects for trackball's representation:
  GLuint* bufferIDs;

  ///Trackball Shader, used just for rendering the trackball's representation.
  Shader* trackballShader;

  ///Quaternion angle axis representation. It consists in a 4 floats array defined by [x,y,z,w] from Eigen lib.
  float quaternionVector[4];

	///The trackball radius. It's defined as 0.8 times the smallest viewport dimension.
	float radius;

  void errorCheckFunc(string file, int line);

};

#endif


