#include "toon.hpp"

void Toon::initialize() {

	shader = new Shader("toonShader", "shaders/toonShader.vert","shaders/toonShader.frag");
	shader->initialize();
	errorCheckFunc(__FILE__, __LINE__);

}

void Toon::reloadShaders() {

	shader->reloadShaders();

}

void Toon::render(Mesh* mesh, Trackball* cameraTrackball, Trackball* lightTrackball){

	shader->enable();

	shader->setUniform("quantizationLevel",quantizationLevel);

	//Define standard color for mesh rendering. Toon shading works best with saturated colors:
	Eigen::Vector4f colorVector;
	colorVector << 1.0,0.0,0.0,1.0;

	//Setting Uniforms:
	shader->setUniform("in_Color",colorVector.data(),4,1);
	shader->setUniform("projectionMatrix", cameraTrackball->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);
	shader->setUniform("modelMatrix",cameraTrackball->getModelMatrix().data(), 4, GL_FALSE, 1);
	shader->setUniform("viewMatrix",cameraTrackball->getViewMatrix().data(), 4, GL_FALSE, 1);
	Eigen::Matrix3f lightViewMatrix = lightTrackball->getViewMatrix().linear();
	shader->setUniform("lightViewMatrix",lightViewMatrix.data(),3,GL_FALSE,1);
	errorCheckFunc(__FILE__, __LINE__);

	//Mesh Rendering:
	mesh->render();

	shader->disable();

}
