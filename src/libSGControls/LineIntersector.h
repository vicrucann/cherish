#ifndef LINEINTERSECTOR_H
#define LINEINTERSECTOR_H

#include <vector>

#include <osg/ref_ptr>
#include <osgUtil/LineSegmentIntersector>

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
