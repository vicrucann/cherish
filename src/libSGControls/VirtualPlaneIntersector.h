#ifndef VIRTUALPLANEINTERSECTOR_H
#define VIRTUALPLANEINTERSECTOR_H

#include <tuple>

#include <osg/Referenced>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>

#include "Canvas.h"

/*! \class VirtualPlaneIntersector
 * \brief Class that allows calculation of a local intersection point between the raycast and
 * provided geometry's plane.
 * It is a template class and the template type must posses the following methods:
 *
 * * Geometry::getCenter() for a global center point within the Geometry's plane.
 * * Geometry::getPlane()` to obtain osg::Plane type.
 * * Geometry::getMatrix() to obtain model matrix of the provided type.
 *
 * It is assumed that all the points of the Geometry type lie within the same plane in 3D.
 * The return result is expressed through the local coordinate system, i.e., `(u,v)`.
*/
template <typename Geometry>
class VirtualPlaneIntersector : public osg::Referenced
{
public:
    typedef std::tuple<double, double, bool> Intersection2D;
    typedef std::tuple<osg::Vec3f, bool> Intersection3D;

    /*! Default constructor.
     * \param g is the user provided Geometry type whose points are assuminly lying within the same
     * plane in 3D. */
    VirtualPlaneIntersector(Geometry* g);

    /*! A convinience method to obtain a local intersection point between the raycast and
     * a virtual plane of the presented Geometry.
     * \return A tuple of a form (double, double, bool), where the first two variables are the
     * local intersection coordinates, and the last variables denotes whether intersection has
     * occured or not. */
    virtual Intersection2D getIntersection2D(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    /*! A convinience method to obtain a global intersection point between the raycast and
     * a virtual plane of the presented Geometry.
     * \return A tuple of a form (osg::Vec3f, bool), where the first variable is the
     * global intersection coordinates, and the last variable denotes whether intersection has
     * occured or not. */
    virtual Intersection3D getIntersection3D(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    virtual Intersection3D getIntersection3D(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, const osg::Plane& plane);

protected:
    /*! Algorithm: use ray-tracking techinique; calcualte near and far point in global 3D;
     * intersect that segment with plane of canvas - 3D intersection point;
     * extract local 3D coords so that to create a stroke (or append that point to a current stroke)
     * \return true if the no errors occured, false otherwise
     */
    virtual bool getIntersection2D(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa,
                                   const osg::Vec3f& center, const osg::Plane& plane, double& u, double& v);

    /*! Algorithm: use ray-tracking techinique; calcualte near and far point in global 3D;
     * intersect that segment with plane of canvas  and obtain a 3D intersection point.
     * \return true if the no errors occured, false otherwise */
    virtual bool getIntersection3D(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa,
                                   const osg::Vec3f& center, const osg::Plane& plane, osg::Vec3f& P);

    /*! A method to obtain two matrices - VPW and its invertse given the action adapter.
     * \param aa is OSG-based variable from which camera, view and other matrices can be derived.
     * \param VPW is the output View-Projection-World matrix
     * \param invVPW is the ourput inverse of View-Projection-World matrix.
     * \return true if both matrices were obtained without an error. */
    bool getViewProjectionWorld(osgGA::GUIActionAdapter &aa, osg::Matrix &VPW, osg::Matrix &invVPW);

    /*! A method to obtain far and near points of the screen point that cast a projection ray.
     * \param x - x-coordinate of the screen
     * \param y - y-coordinate of the screen
     * \param invVPW - inverse matrix of View-Projection-World matrix
     * \param near is the result 3D point on the near point on the cast ray
     * \param far is the result 3D point as the far point on the cast ray.
     * \return true if the points were obtained successfully.
     * \sa getViewProjectionWorld(). */
    void getFarNear(double x, double y, const osg::Matrix &invVPW, osg::Vec3f &near, osg::Vec3f &far);

    /*! A method to obtain intersection point between a given plane and a line (ray).
     * \param plane is the given plane
     * \param center is an arbitrary point on the plane that represents the plane's center.
     * \param nearPoint is the 3D near point of the ray that was cast from a screen point
     * \param farPoint is the 3D far point of the ray that was cast from a screen point
     * \param P is the result point that represents intersection of the plane and a ray that is given by far and near points
     * \return true if the point was obtained without any errors
     * \sa getFarNear() */
    bool getRayPlaneIntersection(const osg::Plane &plane, const osg::Vec3f &center, const osg::Vec3f &nearPoint, const osg::Vec3f &farPoint, osg::Vec3f &P);

protected:
    Geometry* m_geometry;
};

#endif // VIRTUALPLANEINTERSECTOR_H
