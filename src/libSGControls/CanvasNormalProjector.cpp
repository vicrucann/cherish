#include "CanvasNormalProjector.h"

CanvasNormalProjector::CanvasNormalProjector(entity::Canvas *canvas)
    : VirtualPlaneIntersector<entity::Canvas>(canvas)
{
}

CanvasNormalProjector::Projection CanvasNormalProjector::getProjection(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    osg::Vec3f XC = osg::Vec3f(0,0,0);
    bool success = this->getProjection(ea,aa,XC);
    return std::make_tuple(XC, success);
}

bool CanvasNormalProjector::getProjection(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, osg::Vec3f &XC)
{
    osg::Matrix VPW, invVPW;
    if (!this->getViewProjectionWorld(aa, VPW, invVPW))
        return false;

    /* get far and near in global 3D coords */
    osg::Vec3f nearPoint, farPoint;
    this->getFarNear(ea.getX(), ea.getY(), invVPW, nearPoint, farPoint);

    osg::Vec3f C = m_geometry->getCenter();
    osg::Vec3f N = m_geometry->getNormal();

    osg::Vec3f X1;
    if (!this->getSkewLinesProjection(C, farPoint, nearPoint, N, X1))
        return false;
    XC = X1 - C;
    return true;
}

bool CanvasNormalProjector::getSkewLinesProjection(const osg::Vec3f &center, const osg::Vec3f &farPoint, const osg::Vec3f &nearPoint, const osg::Vec3f &normal, osg::Vec3f &X1)
{
    osg::Vec3f P1 = center;
    osg::Vec3f P2 = nearPoint;
    osg::Vec3f d = P2 - P1;
    osg::Vec3f u1 = normal;
    u1.normalize();
    osg::Vec3f u2 = farPoint - nearPoint;
    u2.normalize();
    osg::Vec3f u3 = u1^u2;

    if (std::fabs(u3.x())<=cher::EPSILON && std::fabs(u3.y())<=cher::EPSILON && std::fabs(u3.z())<=cher::EPSILON){
        qWarning("getSkewLinesProjection: cast ray and normal are almost parallel."
                  "Switch view point.");
        return false;
    }

    // X1 and X2 are the closest points on lines
    // we want to find X1 (u1 corresponds to normal)
    // solving the linear equation in r1 and r2: Xi = Pi + ri*ui
    // we are only interested in X1 so we only solve for r1.
    float a1 = u1*u1, b1 = u1*u2, c1 = u1*d;
    float a2 = u1*u2, b2 = u2*u2, c2 = u2*d;
    if (!(std::fabs(b1) > cher::EPSILON)){
        qWarning("getSkewLinesProjection: denominator is zero");
        return false;
    }
    if (!(a2!=-1 && a2!=1)){
        qWarning("getSkewLinesProjection: lines are parallel");
        return false;
    }

    double r1 = (c2 - b2*c1/b1)/(a2-b2*a1/b1);
    X1 = P1 + u1*r1;

    return true;
}
