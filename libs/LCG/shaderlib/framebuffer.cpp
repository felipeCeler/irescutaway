#include "framebuffer.hpp"

void Framebuffer::createFramebuffer(int viewportWidth, int viewportHeight, int numberOfTextures) {
  
  // update dimensions
  size << viewportWidth, viewportHeight;

  //Creating Framebuffer:
  if(fbo_id) {
    glDeleteFramebuffers(1, &fbo_id);
  }
  glGenFramebuffers(1, &fbo_id);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);


  // clearing textures that already exist
  for (unsigned int i = 0; i < fboTextures.size(); ++i) {
    if (fboTextures[i]) {
      glDeleteTextures(1, &fboTextures[i]);
    }
  }
  fboTextures.clear();
 
  //Creating texture:
  fboTextures.resize( numberOfTextures );
  for (int i = 0; i < numberOfTextures; ++i) {
    createTexture(i);
  }
  
  //Depth Buffer Generation:
  if(depthbuffer){
    glDeleteRenderbuffers(1,&depthbuffer);
  }
  glGenRenderbuffers(1,&depthbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER,depthbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT32, size[0], size[1]);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,depthbuffer);
  
  GLint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "framebuffer error : " << status;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glBindTexture(texture_type, 0);


}


void Framebuffer::createTexture(int tex_id) {

  glGenTextures(1, &fboTextures[tex_id]);
  glBindTexture(texture_type, fboTextures[tex_id]);
  glTexImage2D(texture_type, 0, GL_RGBA32F, size[0], size[1], 0, GL_RGBA, GL_FLOAT, 0);

  glTexParameteri(texture_type, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(texture_type, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+tex_id, texture_type, fboTextures[tex_id] , 0);
}


void Framebuffer::fillTexture(int texID, GLubyte *data) {

    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size[0], size[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Framebuffer::readBuffer (GLenum attach, GLfloat * pixels ) {

  if (pixels)
    delete [] pixels;

  pixels = new GLfloat[(int)(size[0]*size[1]*4)];
  bind();
  glReadBuffer(attach);
  glReadPixels(0, 0, size[0], size[1], GL_RGBA, GL_FLOAT, pixels); 
  unbind();
  cout << pixels << endl;
}

Eigen::Vector4f Framebuffer::readPixel (GLenum attach, Eigen::Vector2i pos ) {

  bind();
  glReadBuffer(attach);
  GLfloat pixel[4];
  glReadPixels(pos[0], pos[1], 1, 1, GL_RGBA, GL_FLOAT, &pixel[0]);
  unbind();

  Eigen::Vector4f result (pixel[0], pixel[1], pixel[2], pixel[3]);
  return result;
}

void Framebuffer::printBuffer (int attach, Eigen::Vector4f exception) {

  GLfloat * pixels = new GLfloat[(int)(size[0]*size[1]*4)];
  bind();
  glReadBuffer(GL_COLOR_ATTACHMENT0+attach);
  glReadPixels(0, 0, size[0], size[1], GL_RGBA, GL_FLOAT, pixels); 

  int count = 0;
  Eigen::Vector4f min = Eigen::Vector4f::Constant(numeric_limits<float>::max());
  Eigen::Vector4f max = Eigen::Vector4f::Constant (numeric_limits<float>::min());
 
  int pos;
  Eigen::Vector4f pixel;
  for (int j = 0; j < size[1]; ++j){ 
    for (int i = 0 ; i < size[0]; ++i) {
      pos = (i + size[0]*j)*4;
      pixel << pixels[pos+0], pixels[pos+1], pixels[pos+2], pixels[pos+3];
      if (pixel != exception) 
	{
	  cout << "(" << i << "," << j << ") = [ " << pixel.transpose() << " ]" << endl;
	  count++;
	
	for (int k = 0; k < 4; ++k) {
	  if (pixel[k] < min[k])
	    min[k] = pixel[k];
	  if (pixel[k] > max[k])
	    max[k] = pixel[k];
	}
      }
    }
  }
  delete [] pixels;

  unbind();

  cout << endl << "info : " << endl;
  cout << "num valid pixels : " << count << endl;
  cout << "min values : " << min.transpose() << endl;
  cout << "max values : " << max.transpose() << endl;
  cout << "tex id : " << attach << endl;
}
