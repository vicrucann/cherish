#ifndef BOOKMARKTOOLINTERSECTOR_H
#define BOOKMARKTOOLINTERSECTOR_H

#include <osgUtil/LineSegmentIntersector>
#include <osg/Drawable>

#include "PolyLineIntersector.h"

/*! \class BookmarkToolIntersector
 * \brief It allows to catch intersection between the mouse ray and entity::BookmarkTool.
 * Inherits most of the functionality from PolyLineIntersector, except the thresholds are set up
 * larger.
*/

class BookmarkToolIntersector : public PolyLineIntersector
{
public:
    BookmarkToolIntersector();
    BookmarkToolIntersector(const osg::Vec3& start,  const osg::Vec3& end);
    BookmarkToolIntersector(CoordinateFrame cf, double x, double y);
    BookmarkToolIntersector(CoordinateFrame cf, const osg::Vec3d& start, const osg::Vec3d& end);

    virtual Intersector* clone(osgUtil::IntersectionVisitor &iv);
    virtual void intersect(osgUtil::IntersectionVisitor& iv, osg::Drawable* drawable);

private:
    const float threshold = 0.5f;
};

#endif // BOOKMARKTOOLINTERSECTOR_H
