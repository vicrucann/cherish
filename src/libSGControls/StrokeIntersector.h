#ifndef STROKEINTERSECTOR_H
#define STROKEINTERSECTOR_H

#include <vector>

#include <osg/ref_ptr>
#include <osgUtil/LineSegmentIntersector>

class StrokeIntersector : public osgUtil::LineSegmentIntersector
{
public:
    StrokeIntersector();
    StrokeIntersector(const osg::Vec3& start, const osg::Vec3& end);
    StrokeIntersector(CoordinateFrame cf, double x, double y);
    StrokeIntersector(CoordinateFrame cf, const osg::Vec3d& start, const osg::Vec3d& end);

    void setOffset(float offset);
    float getOffset() const;
    void getHitIndices(int& first, int& last) const;

    virtual Intersector* clone( osgUtil::IntersectionVisitor& iv );
    virtual void intersect( osgUtil::IntersectionVisitor& iv, osg::Drawable* drawable );

protected:
    virtual ~StrokeIntersector(){}

private:
    float m_offset;
    std::vector<unsigned int> m_hitIndices;
};

#endif // STROKEINTERSECTOR_H
