/**
 * Tucano - A library for rapid prototying with Modern OpenGL and GLSL
 * Copyright (C) 2014
 * LCG - Laboratório de Computação Gráfica (Computer Graphics Lab) - COPPE
 * UFRJ - Federal University of Rio de Janeiro
 *
 * This file is part of Tucano Library.
 *
 * Tucano Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Tucano Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tucano Library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __TRACKBALL__
#define __TRACKBALL__

#include "camera.hpp"
#include <shapes/arrow.hpp>
#include <Eigen/Dense>
#include <cmath>

namespace Tucano
{

/// Default fragment shader for rendering trackball representation.
const string trackball_fragment_code = "\n"
        "#version 430\n"
        "in vec4 ex_Color;\n"
        "out vec4 out_Color;\n"
        "in float depth;\n"
        "void main(void)\n"
        "{\n"
        "    out_Color = ex_Color;\n"
        "    gl_FragDepth = depth;\n"
        "}\n";

/// Default vertex shader for rendering trackball representation.
const string trackball_vertex_code = "\n"
        "#version 430\n"
        "layout(location=0) in vec4 in_Position;\n"
        "out vec4 ex_Color;\n"
        "out float depth;\n"
        "uniform mat4 modelMatrix;\n"
        "uniform mat4 viewMatrix;\n"
        "uniform mat4 projectionMatrix;\n"
        "uniform vec4 in_Color;\n"
        "uniform float nearPlane;\n"
        "uniform float farPlane;\n"
        "void main(void)\n"
        "{\n"
        "   gl_Position = (viewMatrix * modelMatrix) * in_Position;\n"
        "   depth = (farPlane+nearPlane)/(farPlane-nearPlane) + ( (2*nearPlane*farPlane)/(farPlane-nearPlane) ) * (1/gl_Position[2]);\n"
        "   depth = (depth+1.0)/2.0;\n"
        "   gl_Position = projectionMatrix * gl_Position;\n"
        "   ex_Color = in_Color;\n"
        "}\n";

/**
 * @brief Trackball class for manipulating a camera.
 *
 * This class simulates a standard trackball manipulation.
 * The visual representation is a sphere with radius equal to 0.8 centered in the middle of the window (represented by the equation: x^2 + y^2 + z^2 = r^2),
 * nested in an hyperbolic sheet opening (represented by the equation: z = (r^2/2)/sqrt(x^2+y^2).
 * It is usually used for camera and light manipulation.
 * It's important to notice that the x and y mouse coordinates passed to the trackball must be ranging from (-1,-1), being the
 * lower left corner of screen, to (1,1) being the higher right corner. This means the coordinate system is centered in screen
 * and normalized. Also, the view matrix is by default translated by the vector [0,0,-2], in order to remove the camera from the center of the world.
 **/
class Trackball : public Tucano::Camera {


protected:
    /// The current scale being applied to the View Matrix.
    float zoom;

    /// Flag that indicates wether the trackball is being rotated.
    bool rotating;

    /// Flag that indicates wether the trackball is being translated.
    bool translating;

    /// Flag that indicates wether the trackball's representation should be drawn.
    bool drawTrackball;

    /// Projection matrix used for the trackball's drawing. By default it is defined as an orthogonal projection matrix.
    Eigen::Matrix4f trackballProjectionMatrix;

    /// Initial position vector used to calculate trackball's rotation.
    Eigen::Vector3f initialPosition;

    /// Final position vector used to calculate trackball's rotation.
    Eigen::Vector3f finalPosition;

    /// Initial position vector used to calculate trackball's translation.
    Eigen::Vector2f initialTranslationPosition;

    /// Final position vector used to calculate trackball's translation.
    Eigen::Vector2f finalTranslationPosition;

    /// Trackball's quaternion.
    Eigen::Quaternion<float> quaternion;

    /// Trackball's default rotation
    Eigen::Quaternion<float> default_quaternion;

    /// Trackball's translation vector.
    Eigen::Vector3f translationVector;

    /// Default translation to move camera away from center
    Eigen::Vector3f defaultTranslation;

    /// The vertices for the first circle of the trackball's representation.
    float vertices[800];

    /// Buffer Objects for trackball's representation:
    GLuint* bufferIDs;

    /// Trackball Shader, used just for rendering the trackball's representation.
    Shader trackball_shader;

    /// The trackball radius. It's defined as 0.8 times the smallest viewport dimension.
    float radius;

    /// Flag to use default shaders set as const strings in class, or pass a directory with custom shaders
    bool use_default_shaders;

public:

    /**
     * @brief Default constructor.
     * @param shader_dir Given directory containing trackball shaders, optional, otherwise uses default shaders.
     */
    Trackball (string shader_dir = "")
    {

        // creates the mesh that will be used to represent the trackball's sphere.
        createTrackballRepresentation();

        // initialize the shader used for trackball rendering:
        if (shader_dir.empty())
        {
        	trackball_shader.setShaderName("trackballShader");
            use_default_shaders = true;
        }
        else
        {
            trackball_shader.load("trackballShader", shader_dir);
            use_default_shaders = false;
        }

        // initialize buffers used for trackball rendering:
        bufferIDs = new GLuint[3];

        defaultTranslation << 0.0, 0.0, -4.0;

        zoom = 1.0;

        drawTrackball = true;
        radius = 0.8f;

        initOpenGLMatrices();
        initializeBuffers();
        loadShader();
        reset();
    }



    /**
     * @brief Resets trackball to initial position and orientation
     */
    void reset (void)
    {
        quaternion = Eigen::Quaternion<float>::Identity();
        default_quaternion = Eigen::Quaternion<float>::Identity();
        zoom = 1.0;
        translationVector << 0.0, 0.0, 0.0;
        rotating = false;
        translating = false;
        Camera::resetViewMatrix();
        updateViewMatrix();
    }

    /**
     * @brief Returns wether the trackball is being rotated or not.
     * @return True if rotating, false otherwise.
     */
    bool isRotating (void)
    {
        return rotating;
    }

    /**
     * @brief Returns wether the trackball is being translated or not.
     * @return True if translating, false otherwise.
     */
    bool isTranslating (void)
    {
        return translating;
    }

    /**
     * @brief Returns the default translation for placing the camera outside the trackball sphere.
     * @return The default translation vector for the view matrix
     */
    Eigen::Vector3f getDefaultTranslation (void)
    {
        return defaultTranslation;
    }

    /**
    * @brief Sets the default translation vector
    * @param t Given new default translation vector.
    */
    void setDefaultTranslation (Eigen::Vector3f t)
    {
        defaultTranslation = t;
    }


    /**
    * @brief Returns the default rotation quaternion
    * @return Default rotation quaternion
    */
    Eigen::Quaternion<float> getDefaultRotation (void)
    {
        return default_quaternion;
    }


    /**
    * @brief Returns the rotation (without the default part) as a quaternion
    * @return Rotation as quaternion
    */
    virtual Eigen::Quaternion<float> getRotation (void)
    {
        return quaternion*default_quaternion;
    }

    /**
    * @brief Sets the default rotation quaternion
    * @param rot Given default rotation as matrix3f
    */
    void setDefaultRotation (Eigen::Matrix3f rot)
    {
        default_quaternion = rot;
    }

    /**
     * @brief Retuns the zoom factor (the radius of the trackball).
     * @return Zoom factor.
     */
    float getZoom (void)
    {
        return zoom;
    }


    /**
     * @brief Sets the projection matrix used for the trackball rendering.
     * Note that this is usually different than the projection matrix for the scene.
     * @param mat Given trackball projection matrix.
     */
    void setTrackballProjectionMatrix (const Eigen::Matrix4f& mat)
    {
        trackballProjectionMatrix = mat;
    }


    /**
     * @brief Indicates that a rotation has ended.
     * Disables the rotating flag, indicating that the mouse callback functions will stop reading the mouse coordinates in order to stop the trackball's rotation.
     */
    void endRotation (void)
    {
        rotating = false;
    }

    /**
     * @brief Indicates that a translation has ended.
     * Disable the translating flag, indicating that the mouse callback functions will stop reading the mouse coordinates in order to stop the trackball's translation.
     */
    void endTranslation (void)
    {
        translating = false;
    }



    /**
     * @brief Returns wether the trackball representation should be drawn or not.
     * @return True if trackball representation is being rendered, false otherwise.
     */
    void setRenderFlag(bool flag)
    {
        drawTrackball = flag;
    }

    ///Load Trackball Shader file.
    void loadShader (void)
    {
        if (use_default_shaders)
        {
            trackball_shader.initializeFromStrings(trackball_vertex_code, trackball_fragment_code);
        }
        else
        {
            trackball_shader.initialize();
        }
    }

    ///Default destructor.
    ~Trackball() {
        ///Delete buffers:
        if (bufferIDs) {
            delete [] bufferIDs;
        }
    }

private:

    /**
     * @brief Computes 3d coordinates on sphere from 2d position.
     * @param pos Given 2d screen position.
     */
    virtual Eigen::Vector3f computeSpherePosition (const Eigen::Vector2f& pos)
    {
        //In order to handle points outside the sphere, we will use two surfaces: A Sphere with radius = 0.8 and a Hyperbolic Sheet. More information about this usage in: http://www.opengl.org/wiki/Trackball.
        float z;

        Eigen::Vector2f p = pos;
        float r = radius;

        //If pos corresponds to a point before the intersection of the two surfaces, z can be calculated by the Sphere's equation.
        if(p[0]*p[0] + p[1]*p[1] <= (r*r)/2.0) {
            z = sqrt(r*r - p[0]*p[0] - p[1]*p[1]);
        }

        //Else, it will be calculated by the Hyperbolic Sheet's equation.
        else {
            z = (r*r)/(2.0*sqrt(p[0]*p[0] + p[1]*p[1]));
        }

        Eigen::Vector3f position = Eigen::Vector3f(p[0], p[1], z);

        return position;
    }

public:

    /**
     * @brief Initializes the view and projection matrices.
     * They are all initialized as Identity matrices, but view is then translated by the defaultTranslation vector.
     */
    void initOpenGLMatrices (void)
    {
        // reset all matrices
        reset();

        // translate viewMatrix outside the origin to be able to see the model.
        translate(defaultTranslation);

        trackballProjectionMatrix = Eigen::Matrix4f::Identity();
    }

    /**
     * @brief Sets the trackball projection matrix as perspective.
     * @param fy Vertical field of view angle.
     * @param aspect_ratio Ratio of width to the height of the viewport.
     * @param near_plane Near plane.
     * @param far_plane Far plane.
     * @return Return the created perspective matrix.
     */
    Eigen::Matrix4f setTrackballPerspectiveMatrix (float fy, float aspect_ratio, float near_plane, float far_plane)
    {
        Eigen::Matrix4f proj = createPerspectiveMatrix(fy, aspect_ratio, near_plane, far_plane);
        setTrackballProjectionMatrix(proj);
        return proj;
    }

    /**
     * @brief Sets the trackball projection matrix as orthographic.
     * @param left Left plane for orthographic view.
     * @param right Right plane for orthographic view.
     * @param bottom Bottom plane for orthographic view.
     * @param top Top lane for orthographic view.
     * @param near_plane Near plane for orthographic view.
     * @param far_plane Far plane for orthographic view.
     * @return Return the created orthographic matrix.
     */
    Eigen::Matrix4f setTrackballOrthographicMatrix (float left, float right, float bottom, float top, float near_plane, float far_plane)
    {
        Eigen::Matrix4f proj = createOrthographicMatrix(left, right, bottom, top, near_plane, far_plane);
        setTrackballProjectionMatrix(proj);
        return proj;
    }

    /**
     * @brief Computes the trackball's rotation, using stored initial and final position vectors.
     */
    virtual void computeRotationAngle (void)
    {
        //Given two position vectors, corresponding to the initial and final mouse coordinates, calculate the rotation of the sphere that will keep the mouse always in the initial position.

        if(initialPosition.norm() > 0) {
            initialPosition.normalize();
        }
        if(finalPosition.norm() > 0) {
            finalPosition.normalize();
        }

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

    /**
     * @brief Compute the trackball's translation, using stored initial and final position vectors.
     */
    virtual void computeTranslationVector (void)
    {
        Eigen::Vector2f translationFactor = finalTranslationPosition - initialTranslationPosition;
        translationVector += quaternion.inverse() * Eigen::Vector3f(translationFactor[0],translationFactor[1], 0.0);
    }

    /**
    * @brief Nomalizes a screen position to range [-1,1].
    * @param pos Screen position
    * @return Returns position in normalized coordinates.
    */
    Eigen::Vector2f normalizePosition (const Eigen::Vector2f& pos)
    {
        return Eigen::Vector2f ((pos[0]/((viewport[2]-viewport[0])/2.0)) - 1.0,
                                1.0 - (pos[1]/((viewport[3] - viewport[1])/2.0)));
    }

    /**
     * @brief Computes and applies the rotation transformations to the trackball given new position.
     * @param pos New mouse position in normalized trackball system
     */
    void rotateCamera (const Eigen::Vector2f& pos)
    {
        Eigen::Vector3f normalized_pos = computeSpherePosition(normalizePosition(pos));

        if (!rotating)
        {
            rotating = true;
            initialPosition = normalized_pos;
        }
        else if ( pos != initialPosition.head(2))
        {
            finalPosition = normalized_pos;
            computeRotationAngle();
            updateViewMatrix();
            initialPosition = finalPosition;
        }
    }

    /**
     * @brief Computes and applies the translation transformations to the trackball given new position.
     * @param pos New mouse position in normalized trackball system
     */
    void translateCamera (const Eigen::Vector2f& pos)
    {
        Eigen::Vector2f normalized_pos = normalizePosition(pos);
        if (!translating)
        {
            translating = true;
            initialTranslationPosition = normalized_pos;
        }
        else if (pos != initialPosition.head(2))
        {
            finalTranslationPosition = normalized_pos;
            computeTranslationVector();
            updateViewMatrix();
            initialTranslationPosition = finalTranslationPosition;
        }
    }


    /**
     * @brief Increases the zoom on the scene by appling a scale to the View Matrix.
     * The current scale used in View Matrix is multiplied by the given scale factor.
     * @param scale The given scale to be multiplied by current scale factor.
     */
    void increaseZoom (float scale)
    {
        zoom *= scale;
        updateViewMatrix();
    }

    /**
     * @brief Decreases the zoom on the scene by appling a scale to the View Matrix.
     * The current scale used in View Matrix is divided by the given scale factor.
     * @param scale The given scale factor, to which the current scale factor will be divided by.
     */
    void decreaseZoom (float scale)
    {
        zoom /= scale;
        updateViewMatrix();
    }

    /**
     * @brief Applies a scale factor to the viewMatrix.
     * The current scale used in view matrix will be substituted by the given scale factor.
     * @param scale The new scale factor to be applied to View Matrix.
     */
    void applyScaleToViewMatrix (float scale)
    {
        zoom = scale;
        updateViewMatrix();
    }

    /**
     * @brief Translates the view matrix by a given vector trans.
     * This directly modifies the translation vector of the trackball.
     * Note that the view matrix is always recomputed using the translation, quaternion and zoom
     * @param translation Given translation vector.
     */
    void translateViewMatrix (const Eigen::Vector3f& translation)
    {
        translationVector += translation;
    }

     /**
     * @brief Rotates the view matrix by a given rotation matrix.
     * This directly modifies the quaternion of the trackball.
     * Note that the view matrix is always recomputed using the translation, quaternion and zoom
     * @param rot Given rotation matrix.
     */
    void rotateViewMatrix (const Eigen::Affine3f &rot)
    {
        Eigen::Quaternion<float> q (rot.rotation().matrix()); 
        quaternion *= q;
        quaternion.normalize();
    }

   
    /**
     * @brief Applies all trackball's transformations to the view matrix.
     */
    virtual void updateViewMatrix (void)
    {
        resetViewMatrix();
        rotate(default_quaternion);
        translate(defaultTranslation);
        rotate(quaternion);
        translate(translationVector);
        scale(zoom);
    }

    /**
     * @brief Creates the circle representating rotation around Z axis.
     * The other circles will be created by simply rotating this one around the desired axis.
     */
    void createTrackballRepresentation (void)
    {
        int index = 0;
        for (float theta = 0; theta<2*M_PI; theta += (2*M_PI)/200.0) {

            vertices[index] = 0.8*cos(theta);
            vertices[index+1] = 0.8*sin(theta);
            vertices[index+2] = 0.0f;
            vertices[index+3] = 1.0f;
            index += 4;
        }
    }

    /**
     * @brief Initializes buffers for trackball's representation.
     * Creates and fills the buffers.
     */
    void initializeBuffers (void)
    {
        createBuffers();
        bufferData();

    }

    /**
     * @brief Creates buffers for trackball's representation.
     */
    void createBuffers (void)
    {
        glGenVertexArrays(1, &bufferIDs[0]);
        glGenBuffers(2, &bufferIDs[1]);
    }

    /**
     * @brief Buffer vertices and indices data for trackball's representation.
     */
    void bufferData (void)
    {
        glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    /**
     * @brief Bind buffers for rendering trackball's representation.
     */
    void bindBuffers (void)
    {
        //VAO:
        glBindVertexArray(bufferIDs[0]);

        //VBO:
        glBindBuffer(GL_ARRAY_BUFFER,bufferIDs[1]);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);
    }

    /**
     * @brief Unbind buffers for rendering trackball's representation.
     */
    void unbindBuffers (void)
    {
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glDisableVertexAttribArray(0);
    }

    /**
     * @brief Renders the trackball representation.
     * @todo setTrackballOrthographicMatrix should be set during viewport resize
     * The external matrices are useful when the trackball should follow a specif object for example, so the main camera matrices should be passed here
     * @param ext_view_matrix External view matrix from other global camera
     * @param ext_proj_matrix External projection matrix from other global camera
     */
    virtual void render (Eigen::Affine3f ext_view_matrix = Eigen::Affine3f::Identity(), Eigen::Matrix4f ext_proj_matrix = Eigen::Matrix4f::Identity())
    {
        if(drawTrackball)
        {
            float ratio = (viewport[2] - viewport[0]) / (viewport[3] - viewport[1]);
            setTrackballOrthographicMatrix(-ratio, ratio, -1.0, 1.0, 0.1, 100.0);
            if (ext_proj_matrix != Eigen::Matrix4f::Identity())
                trackballProjectionMatrix = ext_proj_matrix;

            trackball_shader.bind();

            //Using unique viewMatrix for the trackball, considering only the rotation to be visualized.
            Eigen::Affine3f trackballViewMatrix = ext_view_matrix;
            trackballViewMatrix.translate(defaultTranslation);
            trackballViewMatrix.rotate(quaternion);
            trackball_shader.setUniform("viewMatrix", trackballViewMatrix);
            trackball_shader.setUniform("projectionMatrix", trackballProjectionMatrix);
            trackball_shader.setUniform("nearPlane", near_plane);
            trackball_shader.setUniform("farPlane", far_plane);


            bindBuffers();

            //X:
            Eigen::Vector4f colorVector(1.0, 0.0, 0.0, 1.0);
            trackball_shader.setUniform("modelMatrix", Eigen::Affine3f::Identity()*Eigen::AngleAxis<float>(M_PI/2.0,Eigen::Vector3f(0.0,1.0,0.0)));
            trackball_shader.setUniform("in_Color", colorVector);
            glDrawArrays(GL_LINE_LOOP, 0, 200);

            //Y:
            colorVector << 0.0, 1.0, 0.0, 1.0;
            trackball_shader.setUniform("modelMatrix", Eigen::Affine3f::Identity()*Eigen::AngleAxis<float>(M_PI/2.0,Eigen::Vector3f(1.0,0.0,0.0)));
            trackball_shader.setUniform("in_Color", colorVector);
            glDrawArrays(GL_LINE_LOOP, 0, 200);

            //Z:
            colorVector << 0.0, 0.0, 1.0, 1.0;            
            trackball_shader.setUniform("modelMatrix", Eigen::Affine3f::Identity());
            trackball_shader.setUniform("in_Color", colorVector);
            glDrawArrays(GL_LINE_LOOP, 0, 200);

            unbindBuffers();

            trackball_shader.unbind();
        }
    }

};

/**
 * @brief A directional trackball, useful for light direction for example
 */
class DirectionalTrackball : public Tucano::Trackball
{
private:

    /// Arrow for visualizing view direction (very useful for light trackball system)
    Tucano::Shapes::Arrow arrow;

    Eigen::Quaternion<float> quaternion_h;
    Eigen::Quaternion<float> quaternion_v;

protected:


    /**
     * @brief Computes the trackball's rotation, using stored initial and final position vectors.
     */
    void computeRotationAngle (void)
    {
        //Given two position vectors, corresponding to the initial and final mouse coordinates, calculate the rotation of the sphere that will keep the mouse always in the initial position.

        if(initialPosition.norm() > 0) {
            initialPosition.normalize();
        }
        if(finalPosition.norm() > 0) {
            finalPosition.normalize();
        }

        Eigen::Vector3f ini;
        Eigen::Vector3f fin;
        float dot, angle;

        ini << initialPosition[0], 0.0, initialPosition[2];
        ini.normalize();
        fin << finalPosition[0], 0.0, finalPosition[2];
        fin.normalize();
        dot = ini.dot(fin);
        angle = (dot <= 1) ? acos(dot) : 0;
        if (initialPosition[0] > finalPosition[0])
            angle *= -1;
        Eigen::Quaternion<float> qx (Eigen::AngleAxis<float>(angle, Eigen::Vector3f::UnitY()));

        ini << 0.0, initialPosition[1], initialPosition[2];
        ini.normalize();
        fin << 0.0, finalPosition[1], finalPosition[2];
        fin.normalize();
        dot = ini.dot(fin);
        angle = (dot <= 1) ? acos(dot) : 0;
         if (initialPosition[1] < finalPosition[1])
            angle *= -1;
        Eigen::Quaternion<float> qy (Eigen::AngleAxis<float>(angle, Eigen::Vector3f::UnitX()));

        quaternion_h = qx * quaternion_h;
        quaternion_v = qy * quaternion_v;
        quaternion_h.normalize();
        quaternion_v.normalize();

        quaternion = quaternion_h * quaternion_v;
        quaternion.normalize();
    }



public:
 
    DirectionalTrackball (void)
    {
        arrow.create(0.05, 0.8, 0.12, 0.2);
        arrow.setColor(Eigen::Vector4f(0.8, 0.8, 0.2, 1.0));
        quaternion_h = Eigen::Quaternion<float>::Identity();
        quaternion_v = Eigen::Quaternion<float>::Identity();
    }

    ~DirectionalTrackball (void)
    {}


	/**
	* @brief Sets directly the rotation quaternion
	* @param q Given quaternion
	*/
	void setRotation (Eigen::Quaternion<float> q)
	{
		quaternion = q;
		updateViewMatrix();
	}

	/**
	* @brief Renders the camera's view direction at the top right corner of the screen.
	*/
	void renderDirection (Tucano::Camera &ext_camera, bool render_from_ext = false)
	{
	    float ratio = (viewport[2] - viewport[0]) / (viewport[3] - viewport[1]);
    
        Eigen::Matrix4f rep_projection_matrix = createOrthographicMatrix(-ratio, ratio, -1.0, 1.0, 0.1, 100.0);

        Eigen::Affine3f rep_view_matrix = Eigen::Affine3f::Identity();
        rep_view_matrix.translate( Eigen::Vector3f(1.0, 0.75, -5.0));

        Camera rep_camera;
        rep_camera.setViewMatrix(rep_view_matrix);
        rep_camera.viewMatrix()->rotate( ext_camera.getViewMatrix().rotation() );
        rep_camera.setProjectionMatrix(rep_projection_matrix);
        rep_camera.setViewport(ext_camera.getViewport());
        
		Camera lightcam;
        lightcam.viewMatrix()->translate(Eigen::Vector3f(0.0, 0.0, -3.0));

		arrow.resetModelMatrix();
        arrow.modelMatrix()->rotate(quaternion);
        if (render_from_ext)
        {
            arrow.modelMatrix()->translate(defaultTranslation);
		    arrow.modelMatrix()->scale(0.3);
        }
        else
        {
            arrow.modelMatrix()->scale(0.12);
        }
        arrow.modelMatrix()->rotate ( Eigen::AngleAxis<float>(M_PI, Eigen::Vector3f::UnitY()));
        arrow.normalizeModelMatrix();

        if (render_from_ext)
            arrow.render(ext_camera, lightcam);
        else
    		arrow.render(rep_camera, lightcam);

	}

   
    /**
     * @brief Applies all trackball's transformations to the view matrix.
     */
    void updateViewMatrix (void)
    {
        resetViewMatrix();
        rotate(quaternion);
        translate(defaultTranslation);
    //        view_matrix = view_matrix.inverse();
    }


};

/**
 * @brief Trackball for object manipulator
 *
 * Extends trackball class to work as an object manipulator,
 * it should always envelop a specific mesh and be used to
 * manipulate it (translation and rotation)
 * 
 * Typically, it receives a pointer to the main camera, so
 * it can also act in camera space according to the mesh and
 * the camera
 */
class ManipulatorTrackball : public Tucano::Trackball {

private:

    /// Pointer to external camera
    Tucano::Camera *external_camera;

    /// Projected center in regards to external camera 
    Eigen::Vector3f projected_center;

    /// Projected radius in regards to external camera
    float projected_radius;


protected:
     /**
     * @brief Computes 3d coordinates on sphere from 2d position.
     * @param pos Given 2d screen position.
     */
    Eigen::Vector3f computeSpherePosition (const Eigen::Vector2f& pos)
    {
        //In order to handle points outside the sphere, we will use two surfaces: A Sphere with radius = 0.8 and a Hyperbolic Sheet. More information about this usage in: http://www.opengl.org/wiki/Trackball.
        float z;

        Eigen::Vector2f p = pos - projected_center.head(2);
        float r = projected_radius;

        //If pos corresponds to a point before the intersection of the two surfaces, z can be calculated by the Sphere's equation.
        if(p[0]*p[0] + p[1]*p[1] <= (r*r)/2.0) 
        {
            z = sqrt(r*r - p[0]*p[0] - p[1]*p[1]);
        }
        //Else, it will be calculated by the Hyperbolic Sheet's equation.
        else 
        {
            z = (r*r)/(2.0*sqrt(p[0]*p[0] + p[1]*p[1]));
        }
        Eigen::Vector3f position = Eigen::Vector3f(p[0], p[1], z);

        return position;
    }

public:

    /**
    * @brief Default constructor
    */
    ManipulatorTrackball (void)
    {
        external_camera = 0;
    }

    /**
    * @brief Sets the pointer to the external camera
    * The external camera is the actual view camera since the trackball is used here
    * as a mesh manipulator
    * @param ext_cam Pointer to external camera
    */
    void setExternalCamera (Tucano::Camera *ext_cam)
    {
        external_camera = ext_cam;
    }

    /**
    * @brief Compute projected center and radius
    * This is necessary when the trackball is in follow mode, since it will be projected off the screen center and with some arbitrary radius
    */
    void computeProjectedCoords (void)
    {
        if (!external_camera)
            return;

        // compute projected center is screen space coords
        Eigen::Affine3f mat = external_camera->getViewMatrix();
        mat.translate (translationVector);

        Eigen::Vector4f center;
        center << defaultTranslation.head(3), 1.0;
        center = external_camera->getProjectionMatrix() * mat.matrix() * center;
        center /= center[3];

        projected_center = center.head(3);

        Eigen::Vector4f endpoint;
        endpoint << Eigen::Vector3f::UnitX() * radius, 1.0;
        endpoint.head(3) += defaultTranslation.head(3);
        endpoint = external_camera->getProjectionMatrix() * mat.matrix() * endpoint;
        endpoint /= endpoint[3];

        Eigen::Vector2f diff = endpoint.head(2) - projected_center.head(2);
        projected_radius = diff.norm();


    }


    /**
     * @brief Compute the trackball's translation, using stored initial and final position vectors.
     */
    void computeTranslationVector (void)
    {
        Eigen::Vector2f translationFactor = finalTranslationPosition - initialTranslationPosition;

        translationVector += external_camera->getViewMatrix().rotation().inverse()*Eigen::Vector3f(translationFactor[0],translationFactor[1], 0.0);
    }

 
    /**
     * @brief Applies all trackball's transformations to the view matrix.
     */
    void updateViewMatrix (void)
    {
        if (!external_camera)
            return;

        resetViewMatrix();
        translate(defaultTranslation);
        translate(translationVector);

        rotate(quaternion);
        rotate(default_quaternion);
        scale(zoom);
    }

    /**
     * @brief Renders the trackball representation.
     * @todo setTrackballOrthographicMatrix should be set during viewport resize
     * The external matrices are useful when the trackball should follow a specif object for example, so the main camera matrices should be passed here
     * @param ext_view_matrix External view matrix from other global camera
     * @param ext_proj_matrix External projection matrix from other global camera
     */
    void render (void)
    {
        if(drawTrackball && external_camera)
        {
            viewport = external_camera->getViewport();
            trackballProjectionMatrix = external_camera->getProjectionMatrix();

            trackball_shader.bind();

            Eigen::Affine3f trackballViewMatrix = external_camera->getViewMatrix();
            trackballViewMatrix.translate(defaultTranslation);
            trackballViewMatrix.translate(translationVector);
            trackballViewMatrix.rotate(quaternion);
            trackballViewMatrix.rotate(default_quaternion);

            trackball_shader.setUniform("viewMatrix", trackballViewMatrix);
            trackball_shader.setUniform("projectionMatrix", trackballProjectionMatrix);
            trackball_shader.setUniform("nearPlane", near_plane);
            trackball_shader.setUniform("farPlane", far_plane);

            bindBuffers();

            //X:
            Eigen::Vector4f colorVector(1.0, 0.0, 0.0, 1.0);
            trackball_shader.setUniform("modelMatrix", Eigen::Affine3f::Identity()*Eigen::AngleAxis<float>(M_PI/2.0,Eigen::Vector3f(0.0,1.0,0.0)));
            trackball_shader.setUniform("in_Color", colorVector);
            glDrawArrays(GL_LINE_LOOP, 0, 200);

            //Y:
            colorVector << 0.0, 1.0, 0.0, 1.0;
            trackball_shader.setUniform("modelMatrix", Eigen::Affine3f::Identity()*Eigen::AngleAxis<float>(M_PI/2.0,Eigen::Vector3f(1.0,0.0,0.0)));
            trackball_shader.setUniform("in_Color", colorVector);
            glDrawArrays(GL_LINE_LOOP, 0, 200);

            //Z:
            colorVector << 0.0, 0.0, 1.0, 1.0;            
            trackball_shader.setUniform("modelMatrix", Eigen::Affine3f::Identity());
            trackball_shader.setUniform("in_Color", colorVector);
            glDrawArrays(GL_LINE_LOOP, 0, 200);

            unbindBuffers();

            trackball_shader.unbind();
        }
    }


};


}
#endif
