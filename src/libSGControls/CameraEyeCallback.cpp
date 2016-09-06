#include "CameraEyeCallback.h"

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
