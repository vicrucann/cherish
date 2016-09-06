#ifndef CANVASTRANSFORMCALLBACK_H
#define CANVASTRANSFORMCALLBACK_H

#include <osg/Uniform>
#include <osg/MatrixTransform>
#include <osg/NodeVisitor>

/*! \class CanvasTransformCallback
 * \brief Callback which is used by entity::Stroke when re-defining the stroke geometry into a shader.
 * It provides information on the canvas matrix transform so that to obtain vertecies in 3D for fog effect.
*/
class CanvasTransformCallback : public osg::Uniform::Callback
{
public:
    /*! Constructor. \param t is a certain canvas' matrix transform. */
    CanvasTransformCallback(osg::MatrixTransform* t);

    /*! Re-defined method where the uniform assignment is performed. */
    virtual void operator()(osg::Uniform* uniform, osg::NodeVisitor* nv);

    osg::MatrixTransform* m_transform;/*!< is the canvas transform. */
};

#endif // CANVASTRANSFORMCALLBACK_H
