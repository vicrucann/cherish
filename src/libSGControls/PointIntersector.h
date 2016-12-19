#ifndef POINTINTERSECTOR_H
#define POINTINTERSECTOR_H

#include <osgUtil/LineSegmentIntersector>
#include <osg/Drawable>

#include "PolyLineIntersector.h"

/*! \class PointIntersector
 * \brief It allows to catch intersections with point OpenGL types.
 * It uses shortest distance between the cast ray and geometry's vertices.
 * In addition, it filters out the geometries whose primitive sets are different than GL_POINTS.
 * The result primitive index is saved to `hit.primitiveIndex`.
*/
class PointIntersector : public PolyLineIntersector
{
public:
    PointIntersector();
    PointIntersector(const osg::Vec3& start,  const osg::Vec3& end);
    PointIntersector(CoordinateFrame cf, double x, double y);
    PointIntersector(CoordinateFrame cf, const osg::Vec3d& start, const osg::Vec3d& end);

    virtual Intersector* clone(osgUtil::IntersectionVisitor &iv);
    virtual void intersect(osgUtil::IntersectionVisitor& iv, osg::Drawable* drawable);

protected:
    virtual bool isRightPrimitive(const osg::Geometry* geometry);
};

#endif // POINTINTERSECTOR_H
