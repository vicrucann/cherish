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
    if (!this->getViewProjectionWorld(aa, VPW, invVPW))
        return false;

    /* get far and near in global 3D coords */
    osg::Vec3f nearPoint, farPoint;
    this->getFarNear(ea.getX(), ea.getY(), invVPW, nearPoint, farPoint);

    /* get intersection point in global 3D coords */
    osg::Vec3f P;
    if (!this->getRayPlaneIntersection(plane,center, nearPoint, farPoint, P))
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
    if (!this->getViewProjectionWorld(aa, VPW, invVPW))
        return false;

    /* get far and near in global 3D coords */
    osg::Vec3f nearPoint, farPoint;
    this->getFarNear(ea.getX(), ea.getY(), invVPW, nearPoint, farPoint);

    /* get intersection point in global 3D coords */
    if (!this->getRayPlaneIntersection(plane,center, nearPoint, farPoint, P))
        return false;

    return true;
}

template <typename Geometry>
bool VirtualPlaneIntersector<Geometry>::getViewProjectionWorld(osgGA::GUIActionAdapter &aa, osg::Matrix &VPW, osg::Matrix &invVPW)
{
    if (!m_geometry) return false;

    osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
    if (!viewer){
        qWarning("getVPW: could not dynamic_cast to View*");
        return false;
    }

    osg::Camera* camera = viewer->getCamera();
    if (!camera){
        qWarning("getVPW: could not obtain camera");
        return false;
    }

    if (!camera->getViewport()){
        qWarning("getVPW: could not obtain viewport");
        return false;
    }

    /* get far and near points of the ray */
    VPW = camera->getViewMatrix()
            * camera->getProjectionMatrix()
            * camera->getViewport()->computeWindowMatrix();
    if (!invVPW.invert(VPW)){
        qWarning("getVPW: could not invert VPW matrix");
        return false;
    }

    return true;
}

template <typename Geometry>
void VirtualPlaneIntersector<Geometry>::getFarNear(double x, double y, const osg::Matrix &invVPW, osg::Vec3f &near, osg::Vec3f &far)
{
    near = osg::Vec3f(x, y, 0.f) * invVPW;
    far = osg::Vec3f(x, y, 1.f) * invVPW;
}

template <typename Geometry>
bool VirtualPlaneIntersector<Geometry>::getRayPlaneIntersection(const osg::Plane &plane, const osg::Vec3f &center, const osg::Vec3f &nearPoint, const osg::Vec3f &farPoint, osg::Vec3f &P)
{
    if (!plane.valid()){
        qWarning("rayPlaneIntersection: plane is not valid");
        return false;
    }

    std::vector<osg::Vec3f> ray(2);
    ray[0] = nearPoint;
    ray[1] = farPoint;
    if (plane.intersect(ray)) { // 1 or -1 means no intersection
        qWarning("rayPlaneIntersection: not intersection with ray");
        return false;
    }

    osg::Vec3f dir = farPoint-nearPoint;
    if (!plane.dotProductNormal(dir)){
        qWarning("rayPlaneIntersection: projected ray is almost parallel to plane. "
                  "Change view point.");
        return false;
    }

    if (! plane.dotProductNormal(center-nearPoint)){
        qWarning("rayPlaneIntersection: plane contains the line. "
                  "Change view point");
        return false;
    }

    double len = plane.dotProductNormal(center-nearPoint) / plane.dotProductNormal(dir);
    P = dir * len + nearPoint;

    return true;
}


// provide the template definitions
// see more on this: https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
template class VirtualPlaneIntersector<entity::Canvas>;
