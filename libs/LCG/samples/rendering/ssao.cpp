#include "ssao.hpp"

void SSAO::initialize() {
	initializeShaders();
	generateKernel();
	generateNoiseTexture();
	fbo = new Framebuffer(viewportSize[0], viewportSize[1], 2);
}

float SSAO::random(int min, int max) {
	//srand ( time(NULL) );
	int random = rand();
	float ret = random/float(RAND_MAX);
	ret *= (max - min);
	ret += min;
	assert(ret >= min && ret <= max);
	return ret;
}

float SSAO::lerp(const float a, const float b, const float s) {
	return a + s * (b - a);
}

void SSAO::computeNoiseScale() {
	noiseScale = Eigen::Vector2f((float)viewportSize[0]/(float)noiseSize, (float)viewportSize[1]/(float)noiseSize);
}

void SSAO::initializeShaders() {

		//SSAO Computation Shader:
		shader = new Shader("ssaoShader", "shaders/ssaoShader.vert","shaders/ssaoShader.frag");
		shader->initialize();
		errorCheckFunc(__FILE__, __LINE__);

		//Deferred Shader, used to store depth information:
		deferredShader = new Shader("deferredShader", "shaders/deferredShader.vert","shaders/deferredShader.frag");
		deferredShader->initialize();
		errorCheckFunc(__FILE__, __LINE__);

		//Blur Shader:
		blurShader = new Shader("blurShader", "shaders/blurShader.vert","shaders/blurShader.frag");
		blurShader->initialize();
		errorCheckFunc(__FILE__, __LINE__);

}

void SSAO::generateKernel() {

	float scale;
	Eigen::Vector3f sample;
	kernel = new float[numberOfSamples*3];

	int temp = 0;
	for (int i = 0; i<numberOfSamples; i++) {
		sample = Eigen::Vector3f( random(-1.0f,1.0f) , random(-1.0f,1.0f) , random(0.0f,1.0f) );
		sample.normalize();
		//cout << "Kernel: " << sample.transpose() << endl;
		sample *= random(0.0f,1.0f); //Distribute sample points randomly around the kernel.
		scale = float(i)/float(numberOfSamples);
		scale = lerp(0.1f, 1.0f, scale * scale);//Cluster sample points towards origin.
		sample *= scale;
		kernel[temp] = sample[0];
		kernel[temp+1] = sample[1];
		kernel[temp+2] = sample[2];
		temp += 3;
	}

}

void SSAO::generateNoiseTexture() {

	float noise[noiseSize*noiseSize*4];//Noise is a texture with dimensions noiseSize x noiseSize.
	Eigen::Vector3f randomVector;

	for(int i = 0; i<numberOfSamples; i++) {

		randomVector = Eigen::Vector3f ( random(-1.0f,1.0f), random(-1.0f,1.0f), 0.0f);
		randomVector.normalize();
		(randomVector + Eigen::Vector3f(1.0,1.0,1.0)) * 0.5;
		noise[(i*4)] = randomVector[0];
		noise[(i*4)+1] = randomVector[1];
		noise[(i*4)+2] = 0.0;
		noise[(i*4)+3] = 1.0f;
	}

	//Generate Texture:
	noiseTexture = TextureManager::Instance()->createTexture(GL_TEXTURE_2D, GL_RGBA32F, 4, 4, GL_RGBA, GL_FLOAT, &noise[0]);

	//Set Parameters
	int unit = TextureManager::Instance()->bindTexture(GL_TEXTURE_2D,noiseTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	TextureManager::Instance()->unbindTexture(GL_TEXTURE_2D,unit);
}

void SSAO::setOpenGLMatricesUniforms(Shader* currentShader, Trackball* cameraTrackball, Trackball* lightTrackball) {

	//OpenGL Matrices uniforms:
	currentShader->setUniform("projectionMatrix", cameraTrackball->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);
	currentShader->setUniform("modelMatrix",cameraTrackball->getModelMatrix().data(), 4, GL_FALSE, 1);
	currentShader->setUniform("viewMatrix",cameraTrackball->getViewMatrix().data(), 4, GL_FALSE, 1);
	errorCheckFunc(__FILE__, __LINE__);

	//Light Rotation matrix uniform:
	Eigen::Matrix3f lightViewMatrix = lightTrackball->getViewMatrix().linear();
	currentShader->setUniform("lightViewMatrix",lightViewMatrix.data(),3,GL_FALSE,1);
}

void SSAO::changeBlurFlag() {

	if(applyBlur) {
		applyBlur = false;
	}

	else {
		applyBlur = true;
	}

}

void SSAO::changeAmbientPassFlag() {

	if(displayAmbientPass) {
		displayAmbientPass = false;
	}

	else {
		displayAmbientPass = true;
	}

}

void SSAO::reloadShaders() {
	deferredShader->reloadShaders();
	shader->reloadShaders();
	blurShader->reloadShaders();
}

void SSAO::render(Mesh* mesh, Trackball* cameraTrackball, Trackball* lightTrackball) {

	deferredShader->enable();

	//Set uniforms:

	setOpenGLMatricesUniforms(deferredShader, cameraTrackball, lightTrackball);

	//First pass - Depth Storage:

	//Bind buffer to store depth information in framebuffer:
	fbo->bind();
	glDrawBuffer(GL_COLOR_ATTACHMENT0 + depthTextureID);
	glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	errorCheckFunc(__FILE__, __LINE__);

	//First pass mesh rendering:
	mesh->render();

	deferredShader->disable();

	//Second pass - SSAO Computation:

	//Enable ssao shader:
	shader->enable();

	//Define standard color for mesh rendering:
	Eigen::Vector4f colorVector;
	colorVector << 0.686, 0.933, 0.933, 1.0;

	if(applyBlur) {
		//Set draw buffer to blur texture:
		fbo->bind();
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + blurTextureID);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		errorCheckFunc(__FILE__, __LINE__);
	}
	else {
		//Unbind framebuffer and set rendering to the back buffer:
		fbo->unbind();
		glDrawBuffer(GL_BACK);
		errorCheckFunc(__FILE__, __LINE__);
	}

	//Enable noise texture reading:
	int unit = TextureManager::Instance()->bindTexture(GL_TEXTURE_2D,noiseTexture);	
	//Enable depth texture reading:
	fbo->bindAttachment(depthTextureID,1);//Sets Active Texture to 1 and binds the color attachment 0 to it.

	//Setting Uniforms:
	setOpenGLMatricesUniforms(shader, cameraTrackball, lightTrackball);
	shader->setUniform("in_Color",colorVector.data(),4,1);
	shader->setUniform("noiseScale", &noiseScale[0], 2, 1);
	shader->setUniform("radius", radius);
	shader->setUniform("kernel",kernel,3,numberOfSamples);
	shader->setUniform("noiseTexture",unit);
	shader->setUniform("depthTexture",1);
	shader->setUniform("displayAmbientPass",displayAmbientPass);
	shader->setUniform("viewportSize",&viewportSize[0],2,1);
	errorCheckFunc(__FILE__, __LINE__);

	//Second pass mesh rendering:
	mesh->render();

	//Disable ssao shader:
	shader->disable();

	//Unbind noise texture:
	TextureManager::Instance()->unbindTexture(GL_TEXTURE_2D,unit);

	//Unbind framebuffer textures:
	fbo->unbindAttachments();

	if(applyBlur) {
		//Third Pass - Blurring the scene:

		//Enable blur shader:
		blurShader->enable();

		//Enable blur texture reading:
		unit = TextureManager::Instance()->getAvailableUnit();
		fbo->bindAttachment(blurTextureID,unit);

		//Unbind framebuffer:
		fbo->unbind();

		//Set rendering to the back buffer:
		glDrawBuffer(GL_BACK);
		errorCheckFunc(__FILE__, __LINE__);

		float texelSize[2];
		texelSize[0] = 1.0/(float)viewportSize[0];
		texelSize[1] = 1.0/(float)viewportSize[1];

		//Setting Uniforms:
		setOpenGLMatricesUniforms(blurShader, cameraTrackball, lightTrackball);
		blurShader->setUniform("texelSize", &texelSize[0], 2, 1);
		blurShader->setUniform("blurTexture", unit);
		blurShader->setUniform("viewportSize",&viewportSize[0], 2, 1);
		blurShader->setUniform("blurRange", blurRange);

		//Third pass mesh rendering:
		mesh->render();

		//Unbind framebuffer Textures:
		fbo->unbindAttachments();

		glDisable(GL_TEXTURE_2D);

		blurShader->disable();
	}
}
