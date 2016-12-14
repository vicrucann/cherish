#include "CameraCallbacks.h"

#include <osg/Matrixd>
#include <osg/Viewport>

CameraEyeCallback::CameraEyeCallback(osg::Camera *camera)
    : m_camera(camera)
{

}

void CameraEyeCallback::operator()(osg::Uniform* uniform, osg::NodeVisitor* nv)
{
    osg::Vec3f eye, center, up;
    m_camera->getViewMatrixAsLookAt(eye, center, up);
    osg::Vec4f eye_vec = osg::Vec4f(eye.x(), eye.y(), eye.z(), 1);
    uniform->set(eye_vec);
}

CanvasTransformCallback::CanvasTransformCallback(osg::MatrixTransform *t)
    : m_transform(t)
{

}

void CanvasTransformCallback::operator()(osg::Uniform *uniform, osg::NodeVisitor *nv)
{
    osg::Matrix M =  m_transform->getMatrix();
    uniform->set(M);
}

ModelViewProjectionMatrixCallback::ModelViewProjectionMatrixCallback(osg::Camera *camera)
    : m_camera(camera)
{

}

void ModelViewProjectionMatrixCallback::operator()(osg::Uniform *uniform, osg::NodeVisitor *nv)
{
    osg::Matrixd viewMatrix = m_camera->getViewMatrix();
    osg::Matrixd modelMatrix = osg::computeLocalToWorld(nv->getNodePath());
    osg::Matrixd modelViewProjectionMatrix = modelMatrix * viewMatrix * m_camera->getProjectionMatrix();
    uniform->set(modelViewProjectionMatrix);
}

ViewportVectorCallback::ViewportVectorCallback(osg::Camera *camera)
    : m_camera(camera)
{

}

void ViewportVectorCallback::operator()(osg::Uniform *uniform, osg::NodeVisitor * /*nv*/)
{
    const osg::Viewport* viewport = m_camera->getViewport();
    osg::Vec2f viewportVector = osg::Vec2f(viewport->width(), viewport->height());
    uniform->set(viewportVector);
}
