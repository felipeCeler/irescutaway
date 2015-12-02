#ifndef GLWIDGET_H
#define GLWIDGET_H

/// OpenGL
#include <GL/glew.h>

/// IRES Library
#include <ires/ires.h>

/// Celer::Libraries
#include <IRES/CornerPointGrid.hpp>
#include <Loaders/Ply/PlyLoader.hpp>
/// LCG Library
#include <shaderlib/trackball.hpp>
#include <shaderlib/framebuffer.hpp>
#include <samples/imgproc/meanfilter.hpp>

// Tucano
#include <tucano.hpp>
#include <utils/ppmimporter.hpp>

/// C++ Standard Library
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <deque>
#include <limits>
#include <algorithm>
#include <iterator>
#include <functional>
/// Eigen with STL Containers - http://eigen.tuxfamily.org/dox-devel/group__TopicStlContainers.html
/// Maybe, the only way to use Eigen into vector in MS Visual Studio
#include <Eigen/StdVector>

/// Qt 5.5
/// TODO Change it to modern QOpenGLWidget @see http://doc.qt.io/qt-5/qopenglwidget.html#relation-to-qglwidget
#include <QtWidgets/QOpenGLWidget>
#include <QtWidgets/QApplication>
#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtCore/QTimeLine>
#include <QtCore/QElapsedTimer>
#include <QtCore/QProcess>

#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QSet>
#include <QtCore/QMimeData>

#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>

#include <QtGui/QPainter>

class GLWidget: public QOpenGLWidget
{
        Q_OBJECT

        public:

                // From QGLWidget
                explicit GLWidget ( QWidget* parent = 0);
                ~GLWidget ( );
                void initializeGL ( );
                void resizeGL ( int width , int height );

                void paintGL ( );
                void paintEvent ( QPaintEvent * );

                //void timerEvent(QTimerEvent*);
                void processMultiKeys ( );
                void mousePressEvent ( QMouseEvent *event );
                void mouseMoveEvent ( QMouseEvent *event );
                void mouseReleaseEvent ( QMouseEvent *event );
                void wheelEvent ( QWheelEvent *e );
                void keyPressEvent ( QKeyEvent * event );
                void keyReleaseEvent ( QKeyEvent * event );

        public slots:

                /// TODO Physics
                void gameLooping ( );
                void fpsCounter ( );
                void flush ( );

                void property_type ( bool isStatic )
                {
                        if ( isStatic )
                                property_type_ = 1;
                        else
                                property_type_ = 0;

                        update();
                }
                float benchmark ( )
                {
                        return benchmark_;
                }

                /// Shader
                void saturationPrimaries ( const int value )
                {
                        this->saturationPrimaries_ = static_cast<float>(value);
                        update ( );
                }
                void luminancePrimaries ( const int value )
                {
                        this->luminancePrimaries_ = static_cast<float>(value);

                        std::cout << this->luminancePrimaries_ <<  std::endl;
                        update ( );
                }
                void saturationSecondaries ( const int value )
                {
                        this->saturationSecondaries_ = static_cast<float>(value);
                        update ( );
                }
                void luminanceSecondaries ( const int value )
                {
                        this->luminanceSecondaries_ = static_cast<float>(value);
                        update ( );
                }
                void saturationShell ( const int value )
                {
                        this->saturationShell_ = static_cast<float>(value);
                        update ( );
                }
                void luminanceShell ( const int value )
                {
                        this->luminanceShell_ = static_cast<float>(value);
                        update ( );
                }

                /// oO
                void intensitySSAO ( const int value )
                {
                        this->intensity = value;
                        update ( );
                }

                void blurSSAO ( const int value )
                {
                        this->blurRange = value;
                        update ( );
                }

                void radiusSSAO ( const int value )
                {
                        this->radius = (float) value;
                        update ( );
                }

                void loadShaders ( );
                void reloadShaders ( );
                void openIRES_v2 ( const std::string& filename );
                bool isIRESOpen ( ) const;

                void changeIJK ( const int& min_i , const int& max_i , const int& min_j , const int& max_j , const int& min_k , const int& max_k );

                void changeMaxI ( const int& value );
                void changeMaxJ ( const int& value );
                void changeMaxK ( const int& value );

                void changeMinI ( const int& value );
                void changeMinJ ( const int& value );
                void changeMinK ( const int& value );

                // Draw Functions
                void drawBackGround ( ) const;
                void showFault ( bool visibility );
                void showWireframe ( bool visibility );
                void showBorderLines ( bool visibility );

                void setPaperDemoVisibility ( bool visibility )
                {
                        isPaperDemo_ = visibility;
                        update ( );
                }
                void setIRESCutawayVisibility ( bool visibility )
                {
                        isIRESCutaway_ = visibility;
                        update ( );
                }
                void setIRESCutawayStaticVisibility ( bool visibility )
                {
                        isIRESCutawayStatic_ = visibility;
                        update ( );
                }
                void setIRESCutawayDynamicVisibility ( bool visibility )
                {
                        isIRESCutawayDynamic_ = visibility;
                        update ( );
                }

                void borderLinesSize ( int size )
                {
                        borderLinesSize_ = size;
                        update ( );
                }

                void meanFilterSize ( int size )
                {
                        meanFilterSize_ = size;
                        update ( );
                }

                void freezeView ( );
                // ! Raw IRES model    VIEWER F9
                void drawIRESModel ( );
                // ! Paper Demo        VIEWER F10

                void loadPly ( QString pFilename )
                {
                	makeCurrent();
                        std::string filename = QFile::encodeName ( pFilename ).constData ( );
                        ply_primary_.Load ( filename.c_str ( ) );

                        if ( ply_primary_.open )
                        {
                                std::cout << "Opened succefuly " << ply_primary_.TotalConnectedPoints << std::endl;
                        }
                }

                void PaperPly ( );
                void PaperDemo ( );
                void PaperDrawCutawaySurface ( ) const;
                void PaperPrimary ( ) const;
                void PaperSecondary ( ) const;
                // ! STATIC  VIEWER F11 Static Properties
                void IRESCutawayStatic ( );
                void drawIRESCutawayStaticSurface ( ); //
                void drawSecondaryStatic ( ) const; // Draw only secondary Cells
                void drawPrimaryStatic ( ) const; // Draw only primary   Cells
                void drawPrimaryStaticSilhouette() const;

                void setPrimaryVisibility ( bool );
                void setSecondaryVisibility ( bool );
                void setCutawaySurfaceVisibility ( bool );

                void changeProperty ( const int property_index );
                void changePropertyRangeStatic ( const double& min , const double& max , int property_index );
                void changePropertyRangeDynamic ( const double& min , const double& max );
                // ! DYNAMIC VIEWER F12 Dynamic Properties
                void IRESCutawayDynamic ( );
                void drawIRESCutawayDynamicSurface ( );
                void drawPrimaryDynamic ( ) const;
                void drawSecondaryDynamic ( ) const;

                void changeTimeStep ( const int step );
                void changeDynamicProperty ( int index );

        signals:
                void fpsChanged ( const QString& );
                void cutawayGenerationTime ( const QString& );
                void renderingCutawayTime ( const QString& );
                void renderingPrimaryTime ( const QString& );
                void renderingSSAOBlurTime ( const QString& );
                void renderingMeanFilterTime ( const QString& );
                void primariesPorcentage ( const QString& );
                void changed ( const QMimeData *mimeData = 0 );

        public:
                IRES::CornerPointGrid reservoir_model_;

                float min_range_static_;
                float max_range_static_;
                std::size_t primaries_cuboid;
                std::size_t primaries_face;


                float min_range_dynamic_;
                float max_range_dynamic_;

                // Whith time on the dynamic property time line
                int time_step;
                int dynamic_property_index;
                int property_type_;
        private:

                /// Avoid camera_ movement issues
                QPoint lastPos;
                QSet<int> keysPresseds_;
                bool buttonRelease_;

                Shader* BackGround_;
                Shader* RawModel_;
                Shader* BorderLines_;
                Shader* DummyQuad_;

                bool isIRESOpen_;

                // ! DEMO

                Shader* BurnsPrimarySetup_;
                Shader* BurnsPrimary_;
                Shader* BurnsSecondary_;
                Shader* BurnsPly_;

                /// ! DYNAMIC VIEWER F10 Static Properties

                Shader* IRESCutawaySurfaceStatic_;


                Shader* SSAOIRESPrimaryStaticSilhouette_;
                Shader* SSAOIRESPrimaryStatic_;
                Shader* SSAOIRESCutawayStatic_;
                Shader* SSAOIRESCutawayStaticShell_;

                /// ! DYNAMIC VIEWER F12 Dynamic Properties

                Shader* IRESCutawaySurfaceDynamic_;

                Shader* SSAOIRESPrimaryDynamic_;
                Shader* SSAOIRESCutawayDynamic_;
                Shader* SSAOIRESCutawayDynamicShell_;

                /// Saturation Intensity
                float saturationPrimaries_;
                float saturationSecondaries_;
                float saturationShell_;
                /// Luminance Intensity
                float luminancePrimaries_;
                float luminanceSecondaries_;
                float luminanceShell_;

                QImage fbo;
                float angle;
                float zoom_angle_;
                int cluster;

                /// -- ANIMATION  Stuffs ----------------------------------------------
                // Celer Gaming Looping 0.1 -> For animation
                QTimer fpsTimer_;
                QTimer updateTimer_;
                QElapsedTimer delta_time;
                QTime renderTime;

                QTime cutawayGenerationTime_;
                float accumulateCutawayGenerationTime_;
                std::vector<float> cutawayGenerationTimes_;
                QTime renderingCutawayTime_;
                float accumulateRenderingCutawayTime_;
                std::vector<float> renderingCutawayTimes_;
                QTime renderingPrimaryTime_;
                float accumulateRenderingPrimaryTime_;
                std::vector<float> renderingPrimaryTimes_;
                QTime renderingSSAOBlurTime_;
                float accumulateSSAOBlurTime_;
                std::vector<float> SSAOBlurCutawayTimes_;
                QTime renderingMeanFilterTime_;
                float accumulateMeanFilterTime_;
                std::vector<float> MeanFilterTimes_;

                float cutawayPass_;

                std::size_t renderPass_;


                int fps;
                float benchmark_;
                float renderingPass;
                float accumulateRenderingTimes;
                float averageFPS;

                /// -- RENDERING Stuffs  ----------------------------------------------
                bool isPaperDemo_;
                bool isIRESCutaway_;
                bool isIRESCutawayStatic_;
                bool isIRESCutawayDynamic_;

                bool draw_secondary;
                bool draw_primary;
                bool draw_cutaway_surface_;

                int max_I_;
                int min_I_;
                int max_J_;
                int min_J_;
                int max_K_;
                int min_K_;

                /// SSAO From Tucano

                ///Noise texture
                Tucano::Texture noise_texture;

                /// Framebuffer to store coord/normal buffer
                Framebuffer* fboSSAO;

                /// The per pixel AO computation shader
                Shader* ssaoShader_;
                /// For the Graphics Models Journal Paper
                Shader* ssaoShaderTucanoGitlab_;
                /// For the Graphics Models Journal Paper
                Shader* blurTucanoGitlab_;
                ///
                Shader* deferredShader;

                ///
                Shader* blurShader_;

                /// A quad mesh for framebuffer rendering
                Mesh* quadSSAO;

                /// The ID defining the color attachment to which the depth texture is bound in the framebuffer.
                GLuint depthTextureID;

                /// The ID defining the color attachment to which the normal texture is bound in the framebuffer.
                GLuint normalTextureID;

                /// The ID defining the color attachment to which the color texture is bound in the framebuffer.
                GLuint colorTextureID;

                ///Noise texture dimension. It will be a noiseSize x noiseSize texture.
                int noise_size;
                ///Scale used to tile the noise texture through screen.
                Eigen::Vector2f noise_scale;

                ///Noise texture
                GLuint noiseTexture;

                ///Array of sample points generated inside a unit hemisphere around z axis.
                float *kernel;

                ///Number of sample points that will be used per fragment for occlusion computation.
                int numberOfSamples;

                ///Kernel radius. If the distance between a sample point and the point for which the occlusion is being computed is larger than radius, the occlusion for this sample will be neglected.
                float radius;

                /// Flag indicating wether blur shall be applied or not.
                bool apply_blur;

                /// Flag indicating if the mesh should be rendered only with ambient occlusion pass or with full illumination. If True, mesh will be rendered only with the ambient occlusion pass.
                bool displayAmbientPass;

                /// Number of neighbour pixels used in blurring. The blur will be applied to a blurRange x blurRange square around the current pixel. It's important to notice that blurRange must be an odd number.
                int blurRange;

                /// Global intensity value.
                int intensity;

                /// Global maximum distance value, distance in view space to consider a neighbor or not
                float max_dist;

                /// The ID defining the color attachment to which the blur texture is bound in the framebuffer.
                GLuint blurTextureID;
                /// Lights
                std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f> > lights;
                GLfloat * light_elements;

                /// Normals in Camera Space outputNormals vec4( newNormal.xyz, 1.0 );
                GLuint normalCutawayID_;
                /// Vertices in Camera Space outputVertices vec3 (newVert.xyz, 1.0 );
                GLuint verticesCutawayID_;
                /// Normals in Camera Space outputNormals vec4( newNormal.xyz, 1.0 );
                GLuint normalsSmoothID_;
                /// Vertices in Camera Space outputVertices vec3 (newVert.xyz, 1.0 );
                GLuint verticesSmoothID_;
                /// Silhouette Rendering
                GLuint silhouetteID_;
                /// Cutaway FrameBuffer
                Framebuffer * depthFBO;

                /// Smooth
                MeanFilter * meanFilter;

                bool enable_blend_;

                /// -- SCENE Stuffs ---------------------------------------------------

                /// -- Aperture of Cutaway
                float volume_width;
                float volume_height;

                int wall_;
                int line_;

                float move_x;
                float move_y;
                float move_z;

                int borderLinesSize_;  // Border Lines  Emilio's Algorithm
                int meanFilterSize_;   // Mean Filter on the Image Cutaway.

                float justWireFrame;

                // Stores the froze camera (view matrix)
                Eigen::Matrix4f freeze_viewmatrix_;
                bool freezeView_;

                Eigen::Affine3f position_one;
                Eigen::Affine3f position_two;

                Trackball * trackball_;
                /// Trackball for manipulating the camera.

                float nearPlane_;
                float farPlane_;
                float fovy_;
                float aspect_;

                bool isPerspective_;
                float orthoZoom;

                bool zoom_start_;
                float zoom_increment_;

                // LCG procedure
                Eigen::Vector2f convertToNormalizedDeviceCoordinates ( Eigen::Vector2i position );

                // TODO NPR Stuffs - Mario's request
                GLuint xtoon_texture_;
                Mesh * picture;

                Shader * xtoon_texture_viewer;

                // Ply Models Emilio's No sense !
                Model_PLY ply_primary_;
                Model_PLY ply_secondary_;
                bool isply_;

                GLuint vertexBuffer_Dummy;
                GLuint vertexArray_Dummy;

                Eigen::Vector3f displacement;
                Eigen::Vector3f max_displacement;

                // Animation Controls
                long unsigned int videoSequence;
                std::vector<QImage> frames;

                struct Animation
                {
                                std::vector<Eigen::Quaternionf,Eigen::aligned_allocator<Eigen::Quaternionf> > keyframes_;
                                std::vector<Eigen::Quaternionf,Eigen::aligned_allocator<Eigen::Quaternionf> > tangents_;
                                std::size_t nextKeyframe_;
                                std::size_t number_of_keyframes_;

                                void reset ( )
                                {
                                        number_of_keyframes_ = 0;
                                        nextKeyframe_ = 0;
                                }
                                Animation ( )
                                {
                                        keyframes_.resize ( 30 );
                                        tangents_.resize ( 30 );
                                        number_of_keyframes_ = 0;
                                        nextKeyframe_ = 0;
                                }
                };


                /// ! From libQGLViewer
                void AnimationInitializer ( );

                unsigned int take_index_;
                unsigned int number_of_takes_;

                std::vector<Animation> takes_;

                Eigen::Quaternionf targetPosition_;
                Eigen::Quaternionf sourcePosition_;

                Eigen::Quaternionf targetPositionTangent_;
                Eigen::Quaternionf sourcePositionTangent_;

                float time_steps_;
                float time_interval_;
                bool  play_;
                //---------------------------------------------------------------------
                /*! Returns the slerp interpolation of Quaternions \p a and \p b, at time \p t.

                 \p t should range in [0,1]. Result is \p a when \p t=0 and \p b when \p t=1.

                 When \p allowFlip is \c true (default) the slerp interpolation will always use the "shortest path"
                 between the Quaternions' orientations, by "flipping" the source Quaternion if needed (see
                 negate()). */
                Eigen::Quaternionf slerp ( const Eigen::Quaternionf& a , const Eigen::Quaternionf& b , float t , bool allowFlip );

                /// Useful for smooth spline interpolation of Quaternion with squad() and slerp(). */
                Eigen::Quaternionf squadTangent ( const Eigen::Quaternionf& before , const Eigen::Quaternionf& center , const Eigen::Quaternionf& after );

                /*! Returns the slerp interpolation of the two Quaternions \p a and \p b, at time \p t, using
                 tangents \p tgA and \p tgB.

                 The resulting Quaternion is "between" \p a and \p b (result is \p a when \p t=0 and \p b for \p
                 t=1).

                 Use squadTangent() to define the Quaternion tangents \p tgA and \p tgB. */
                Eigen::Quaternionf squad ( const Eigen::Quaternionf& a , const Eigen::Quaternionf& tgA , const Eigen::Quaternionf& tgB , const Eigen::Quaternionf& b , float t );

                /*! Returns the logarithm of the Quaternion. See also exp(). */
                Eigen::Quaternionf log ( const Eigen::Quaternionf& q ) const;

                /*! Returns the exponential of the Eigen::Quaternionf. See also log(). */
                Eigen::Quaternionf exp ( const Eigen::Quaternionf& q ) const;

                /*! Returns log(a. inverse() * b). Useful for squadTangent(). */
                Eigen::Quaternionf lnDif ( const Eigen::Quaternionf& a , const Eigen::Quaternionf& b );

                void calculeTangents ( int index );

                void setPlay ( int index );

};

#endif

