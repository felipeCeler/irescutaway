#ifndef __FRAMEBUFFER__
#define __FRAMEBUFFER__

#include "shader.hpp"
#include <Eigen/Dense>
#include <vector>
#include <iostream>
#include <limits>
#include <GL/glew.h>

/**
 * The Framebuffer class is responsible for framebuffer generation and storage. It holds many shortcut methods to bind, draw to buffer, read buffer for debug, etc...
**/
class Framebuffer {

public:

	/**
	 * Framebuffer Constructor. It creates a framebuffer with the given size and number of buffers, as well as set all texture units as not used.
	 * @param w: Width of the framebuffer texture object.
	 * @param h: Height of the framebuffer texture object.
	 * @param num_buffers: Number of textures that the framebuffer object will hold.
	 * @param textype: Type of the framebuffer texture object.
	**/
  Framebuffer (int w = 256, int h = 256, int num_buffers = 1, GLenum textype = GL_TEXTURE_2D  ) : texture_type(textype) {

    size << w, h;
    fbo_id = 0;
    fboTextures.clear();

    // get maximum number of texture units, and set all as not used
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_units);


    for (int i = 0; i < max_texture_units; ++i) {
      used_units.push_back(0);
    }

    createFramebuffer(size[0], size[1], num_buffers);
  }

  /**
     * @return The id of the texture in given color attachment
     * @param color attachment
     * @return ID of the texture
    **/
  GLuint getTexID (int texID) {
      return fboTextures[texID];
  }

  /**
	 * @return The texture type of the attachments
	**/
  GLenum textureType (void) {
    return texture_type;
  }


  /**
   * @brief Fill a texture with given data
   * @param texID Id of texture to be filled
   * @param data Pointer to the data
   */
  void fillTexture(int texID, GLubyte *data);

  ///Bind framebuffer object
  virtual void bind(void) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
  }

 ///Unbind framebuffer object
  virtual void unbind(void) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  ///Unbind fbo and all texture units in use
  void unbindAll(void) {
    unbind();
    unbindAttachments();
  }

  /// Clears all attachments with zeros
  void clearAttachments(void) {
    bind();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    for (unsigned int i = 0; i < fboTextures.size(); ++i) {
      glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    unbind();
  }


  /// Clears an attachment with zeros
  void clearAttachment(int attachment) {
    bind();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0 + attachment);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    unbind();
  }

  /**
   * Bind a texture attachment to a texture unit.
   * @param attachment Number of color attachment
   * @param texture_unit Number of unit to bind texture
   **/
  void bindAttachment (int attachment, int texture_unit) {
    glActiveTexture(GL_TEXTURE0 + texture_unit);
    glBindTexture(texture_type, fboTextures[attachment]);
    used_units[texture_unit] = true;
  }

    /**
   * Bind a texture attachment to the first free unit.
   * @param attachment Number of color attachment
   * @return Number of unit attached, or -1 if no unit available
   **/
  int bindAttachment (int attachment) {
    for (int i = 0; i < max_texture_units; ++i) {
      if (!used_units[i]) {
	glActiveTexture(GL_TEXTURE0 + i);
	glBindTexture(texture_type, fboTextures[attachment]);
	used_units[i] = true;
	return i;
      }
    }
    cerr << "Warning: no free texture unit found!";
    return -1;
  }

  /**
   * Unbind all texture units in use.
   **/
  void unbindAttachments ( void ) {
    for (int i = 0; i < max_texture_units; ++i) {
      if (used_units[i]) {
	glActiveTexture(GL_TEXTURE0 + i);
	glBindTexture(texture_type, 0);
	used_units[i] = false;
      }
    }
  }
  
  ///Default Destructor. Deletes framebuffer, depthbuffer, fbo texture and unbind the framebuffer and all attachments.
  virtual ~Framebuffer() {

    unbindAll();

    if(fbo_id) {
      glDeleteFramebuffers(1, &fbo_id);
    }

    if(depthbuffer) {
      glDeleteRenderbuffers(1,&depthbuffer);
    }

    for (unsigned int i = 0; i < fboTextures.size(); ++i) {
      if (fboTextures[i]) {
        glDeleteTextures(1, &fboTextures[i]);
      }
    }

  }

  /** FROM TUCANO
   * @brief Bind framebuffer object and set render buffer to 3 given attachments.
   * @param attachID0 The first color attachment to be used for writing.
   * @param attachID1 The second color attachment to be used for writing.
   * @param attachID2 The third color attachment to be used for writing.
   */
  void bindRenderBuffers (GLuint attachID0, GLuint attachID1, GLuint attachID2)
  {
      bind();
      GLenum buffers[3] = {GL_COLOR_ATTACHMENT0+attachID0, GL_COLOR_ATTACHMENT0+attachID1,
                           GL_COLOR_ATTACHMENT0+attachID2};
      glDrawBuffers(3, buffers);
  }
  /** FROM TUCANO
   * @brief Bind framebuffer object and set render buffer to 3 given attachments.
   * @param attachID0 The first color attachment to be used for writing.
   * @param attachID1 The second color attachment to be used for writing.
   * @param attachID2 The third color attachment to be used for writing.
   */
  void bindRenderBuffers (GLuint attachID0, GLuint attachID1, GLuint attachID2, GLuint attachID3)
  {
      bind();
      GLenum buffers[4] = {GL_COLOR_ATTACHMENT0+attachID0, GL_COLOR_ATTACHMENT0+attachID1,
                           GL_COLOR_ATTACHMENT0+attachID2, GL_COLOR_ATTACHMENT0+attachID3};
      glDrawBuffers(4, buffers);
  }

  /** FROM TUCANO
     * @brief Bind framebuffer object and set render buffer to 3 given attachments.
     * @param attachID0 The first color attachment to be used for writing.
     * @param attachID1 The second color attachment to be used for writing.
     * @param attachID2 The third color attachment to be used for writing.
     */
    void bindRenderBuffers (GLuint attachID0, GLuint attachID1)
    {
        bind();
        GLenum buffers[2] = {GL_COLOR_ATTACHMENT0+attachID0, GL_COLOR_ATTACHMENT0+attachID1};
        glDrawBuffers(2, buffers);
    }
    void bindRenderBuffer (GLuint attachID)
    {
        bind();
        glDrawBuffer(GL_COLOR_ATTACHMENT0+attachID);
    }

  void clearDepth (void)
  {
      bind();
      glClear(GL_DEPTH_BUFFER_BIT);
      unbind();
  }


  /**
   * Reads a pixel from a buffer and return it as an Eigen vector.
   * Assumes that a pixel has four GLfloat elements.
   * @param attach Buffer to be read, usually a color attachment from a framebuffer
   * @param pos Pixel position to be read
   * @return Read pixel
   **/
  Eigen::Vector4f readPixel (GLenum attach,  Eigen::Vector2i pos);

  /**
   * Reads a GPU buffer and stores it in a CPU array.
   * Assumes each pixel has four GLfloat elements.
   * @param attach Buffer to be read, usually a color attachment from a framebuffer
   * @param pixels Pointer to array of pixels;
   **/
  void readBuffer (GLenum attach,  GLfloat * pixels);

  /**
   * Prints the content of a GPU. Usually used for debugging.
   * Assumes each pixel has four GLfloat elements.
   * @param attach Number of buffer to be read
   * @param exception Pixel value that should not be printed, usually the background.
   **/
  void printBuffer (int attach, Eigen::Vector4f exception = Eigen::Vector4f(0.0,0.0,0.0,0.0) );

  /**
   * @return The dimensions of the FBO
   **/
  Eigen::Vector2i getDimensions (void) {
    return size;
  }

  /**
   * @return The width of the FBO
   **/
  int getWidth (void) {
    return size[0];
  }

  /**
   * @return The height of the FBO
   **/
  int getHeight (void) {
    return size[1];
  }

protected:
	
  /**
   * Creates the framebuffer and the depthbuffer. The viewport width and height are needed in order to create the FBO. The number of texture attachments may be greater than one.
   * @param viewportWidth The current viewport width.
   * @param viewportHeight The current viewport height.
   * @param numberOfTextures Number of output attachments
   **/
  virtual void createFramebuffer(int viewportWidth, int viewportHeight, int numberOfTextures = 1);

  /**
   * Generate the ith texture for the fbo as the ith color attachment
   * @param tex_id Number of the fbo texture
   **/
  virtual void createTexture(int tex_id);

  ///The Framebuffer Object.
  GLuint fbo_id;

  ///The Depthbuffer Object.
  GLuint depthbuffer;

  ///For handling framebuffers with multiple attachments
  std::vector<GLuint> fboTextures;
  
  /// Texture type for framebuffer attachments, default is GL_TEXTURE_2D
  GLenum texture_type;

  /// Framebuffer dimensions
  Eigen::Vector2i size;

  /// Maximum number of color attachments
  int max_texture_units;

  /// Texture units in use
  std::vector<bool> used_units;

};

#endif
