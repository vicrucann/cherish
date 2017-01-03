#include "CanvasNormalProjector.h"

#include "Utilities.h"

CanvasNormalProjector::CanvasNormalProjector(entity::Canvas *canvas)
    : VirtualPlaneIntersector<entity::Canvas>(canvas)
{
}

VirtualPlaneIntersector<entity::Canvas>::Intersection3D CanvasNormalProjector::getProjection(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    osg::Vec3f XC = osg::Vec3f(0,0,0);
    bool success = this->getProjection(ea,aa,XC);
    return std::make_tuple(XC, success);
}

bool CanvasNormalProjector::getProjection(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, osg::Vec3f &XC)
{
    osg::Matrix VPW, invVPW;
    if (!Utilities::getViewProjectionWorld(aa, VPW, invVPW))
        return false;

    /* get far and near in global 3D coords */
    osg::Vec3f nearPoint, farPoint;
    Utilities::getFarNear(ea.getX(), ea.getY(), invVPW, nearPoint, farPoint);

    osg::Vec3f C = m_geometry->getCenter();
    osg::Vec3f N = m_geometry->getNormal();

    osg::Vec3f X1;
    if (!Utilities::getSkewLinesProjection(C, farPoint, nearPoint, N, X1))
        return false;
    XC = X1 - C;
    return true;
}
