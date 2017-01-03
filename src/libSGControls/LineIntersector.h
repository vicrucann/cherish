#ifndef LINEINTERSECTOR_H
#define LINEINTERSECTOR_H

#include <vector>

#include <osg/ref_ptr>
#include <osgUtil/LineSegmentIntersector>

/*! \class LineIntersector
 * \brief A class that allows to catch intersections with a single line OpenGL type.
 * It uses shortest distance between the cast ray and the geometry line which is calculated
 * as a distance between skew lines.
 * In addition, it filters out the geometries whose primitive sets are different than line-types.
*/
class LineIntersector : public osgUtil::LineSegmentIntersector
{
public:
    LineIntersector();
    LineIntersector(const osg::Vec3& start, const osg::Vec3& end);
    LineIntersector(CoordinateFrame cf, double x, double y);
    LineIntersector(CoordinateFrame cf, const osg::Vec3d& start, const osg::Vec3d& end);

    void setOffset(float offset);
    float getOffset() const;
    void getHitIndices(int& first, int& last) const;

    virtual Intersector* clone( osgUtil::IntersectionVisitor& iv );
    virtual void intersect( osgUtil::IntersectionVisitor& iv, osg::Drawable* drawable );

private:
    float m_offset;
    std::vector<unsigned int> m_hitIndices;
};

#endif // LINEINTERSECTOR_H
