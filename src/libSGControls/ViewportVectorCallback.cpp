#include "ViewportVectorCallback.h"

#include <osg/Viewport>

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
