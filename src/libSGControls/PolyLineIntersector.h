#ifndef POLYLINEINTERSECTOR_H
#define POLYLINEINTERSECTOR_H

#include <vector>
#include <osgUtil/LineSegmentIntersector>

/*! \class PolyLineIntersector
 * \brief A class that allows to catch intersections with line loops and polyline OpenGL types.
 * It uses shortest distance between the cast ray and the geometry line which is calculated
 * as a distance between skew lines.
 * In addition, it filters out the geometries whose primitive sets are different than line-types.
*/
class PolyLineIntersector : public osgUtil::LineSegmentIntersector
{
public:
    PolyLineIntersector();
    PolyLineIntersector(const osg::Vec3& start, const osg::Vec3& end);
    PolyLineIntersector(CoordinateFrame cf, double x, double y);
    PolyLineIntersector(CoordinateFrame cf, const osg::Vec3d& start, const osg::Vec3d& end);

    void setOffset(float offset);
    float getOffset() const;
    void getHitIndices(int& first, int& last) const;

    virtual Intersector* clone( osgUtil::IntersectionVisitor& iv );
    virtual void intersect(osgUtil::IntersectionVisitor& iv, osg::Drawable* drawable);

protected:
    virtual bool isRightPrimitive(const osg::Geometry* geometry);

    float m_offset;
    std::vector<unsigned int> m_hitIndices;
};

#endif // POLYLINEINTERSECTOR_H
