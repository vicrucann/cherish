#include "CanvasTransformCallback.h"

CanvasTransformCallback::CanvasTransformCallback(osg::MatrixTransform *t)
    : m_transform(t)
{

}

void CanvasTransformCallback::operator()(osg::Uniform *uniform, osg::NodeVisitor *nv)
{
    osg::Matrix M =  m_transform->getMatrix();
    uniform->set(M);
}
