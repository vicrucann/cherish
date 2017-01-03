#ifndef CAMERACALLBACKS_H
#define CAMERACALLBACKS_H

#include <osg/Uniform>
#include <osg/Camera>
#include <osg/MatrixTransform>
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


#endif // CAMERACALLBACKS_H
