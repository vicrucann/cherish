#include "ModelViewProjectionMatrixCallback.h"

#include <osg/Matrixd>

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
