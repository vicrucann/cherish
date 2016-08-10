#ifndef VIEWPORTVECTORCALLBACK_H
#define VIEWPORTVECTORCALLBACK_H

#include <osg/Uniform>
#include <osg/Camera>
#include <osg/NodeVisitor>

/*! \class ViewportVectorCallback
 * \brief Callback which is used by entity::Stroke when re-defining the stroke geometry into a shader.
 * It provides information on the viewport. It needs to be implemented as a callback since the GLWidget window
 * is likely to be re-sized some times.
*/
class ViewportVectorCallback : public osg::Uniform::Callback
{
public:
    /*! Constructor. \param camera is the main camera of GLWidget. */
    ViewportVectorCallback(osg::Camera* camera);

    /*! Re-defined method where the uniform assignment is performed. */
    virtual void operator()(osg::Uniform* uniform, osg::NodeVisitor* /*nv*/);

    osg::Camera* m_camera; /*!< is the camera variable, normally obtained from GLWidget. */
};

#endif // VIEWPORTVECTORCALLBACK_H
