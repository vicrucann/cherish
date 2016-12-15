#include "VirtualPlaneIntersector.h"

#include "Utilities.h"

template <typename Geometry>
VirtualPlaneIntersector<Geometry>::VirtualPlaneIntersector(Geometry *g)
    : osg::Referenced()
    , m_geometry(g)
{
}

template <typename Geometry>
typename VirtualPlaneIntersector<Geometry>::Intersection2D VirtualPlaneIntersector<Geometry>::getIntersection2D(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    double u=0,v=0;
    osg::Plane plane = m_geometry->getPlane();
    osg::Vec3f center = m_geometry->getCenter3D();
    bool success = this->getIntersection2D(ea, aa, center, plane, u, v);
    return std::make_tuple(u,v,success);
}

template <typename Geometry>
typename VirtualPlaneIntersector<Geometry>::Intersection3D VirtualPlaneIntersector<Geometry>::getIntersection3D(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    osg::Vec3f P;
    osg::Plane plane = m_geometry->getPlane();
    osg::Vec3f center = m_geometry->getCenter3D();
    bool success = this->getIntersection3D(ea, aa, center, plane, P);
    return std::make_tuple(P, success);
}

template <typename Geometry>
typename VirtualPlaneIntersector<Geometry>::Intersection3D VirtualPlaneIntersector<Geometry>::getIntersection3D(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, const osg::Plane &plane)
{
    osg::Vec3f P;
    osg::Vec3f center = m_geometry->getCenter3D();
    bool success = this->getIntersection3D(ea, aa, center, plane, P);
    return std::make_tuple(P, success);
}

template <typename Geometry>
bool VirtualPlaneIntersector<Geometry>::getIntersection2D(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, const osg::Vec3f &center, const osg::Plane &plane, double &u, double &v)
{
    osg::Matrix VPW, invVPW;
    if (!Utilities::getViewProjectionWorld(aa, VPW, invVPW))
        return false;

    /* get far and near in global 3D coords */
    osg::Vec3f nearPoint, farPoint;
    Utilities::getFarNear(ea.getX(), ea.getY(), invVPW, nearPoint, farPoint);

    /* get intersection point in global 3D coords */
    osg::Vec3f P;
    if (!Utilities::getRayPlaneIntersection(plane,center, nearPoint, farPoint, P))
        return false;

    /* get model matrix and its inverse */
    osg::Matrix M = m_geometry->getMatrix();
    osg::Matrix invM;
    if (!invM.invert(M)) return false;

    /* obtain intersection in local 2D point */
    osg::Vec3f p;
    if (!Utilities::getLocalFromGlobal(P, invM, p))
        return false;

    u=p.x();
    v=p.y();

    return true;
}

template <typename Geometry>
bool VirtualPlaneIntersector<Geometry>::getIntersection3D(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, const osg::Vec3f &center, const osg::Plane &plane, osg::Vec3f &P)
{
    osg::Matrix VPW, invVPW;
    if (!Utilities::getViewProjectionWorld(aa, VPW, invVPW))
        return false;

    /* get far and near in global 3D coords */
    osg::Vec3f nearPoint, farPoint;
    Utilities::getFarNear(ea.getX(), ea.getY(), invVPW, nearPoint, farPoint);

    /* get intersection point in global 3D coords */
    if (!Utilities::getRayPlaneIntersection(plane,center, nearPoint, farPoint, P))
        return false;

    return true;
}


// provide the template definitions
// see more on this: https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
template class VirtualPlaneIntersector<entity::Canvas>;
