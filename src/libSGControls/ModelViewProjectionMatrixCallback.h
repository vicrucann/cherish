#ifndef MODELVIEWPROJECTIONMATRIXCALLBACK_H
#define MODELVIEWPROJECTIONMATRIXCALLBACK_H

#include <osg/Uniform>
#include <osg/Camera>
#include <osg/NodeVisitor>

/*! \class ModelViewMatrixCallback
 * \brief Callback which is used by entity::Stroke when re-defining the stroke geometry into a shader.
 * It provides information on the MVP matrix of the given camera which is used in the stroke shader.
*/
class ModelViewProjectionMatrixCallback : public osg::Uniform::Callback
{
public:
    /*! Constructor. \param camera is the main camera of GLWidget. */
    ModelViewProjectionMatrixCallback(osg::Camera* camera);

    /*! Re-defined method where the uniform assignment is performed. */
    virtual void operator()(osg::Uniform* uniform, osg::NodeVisitor* nv);

    osg::Camera* m_camera; /*!< is the camera variable, normally obtained from GLWidget. */
};

#endif // MODELVIEWPROJECTIONMATRIXCALLBACK_H
