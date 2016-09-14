#ifndef CAMERAEYECALLBACK_H
#define CAMERAEYECALLBACK_H

#include <osg/Uniform>
#include <osg/Camera>
#include <osg/NodeVisitor>

/*! \class CameraEyeCallback
 * \brief Callback which is used by entity::Stroke when re-defining the stroke geometry into a shader.
 * It provides information on the camera eye position given camera which is used in the stroke shader.
*/
class CameraEyeCallback : public osg::Uniform::Callback
{
public:
    /*! Constructor. \param camera is the main camera of GLWidget. */
    CameraEyeCallback(osg::Camera* camera);

    /*! Re-defined method where the uniform assignment is performed. */
    virtual void operator()(osg::Uniform* uniform, osg::NodeVisitor* nv);

    osg::Camera* m_camera; /*!< is the camera variable, normally obtained from GLWidget. */
};

#endif // CAMERAEYECALLBACK_H
