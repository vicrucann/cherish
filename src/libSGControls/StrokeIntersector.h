#ifndef STROKEINTERSECTOR_H
#define STROKEINTERSECTOR_H

#include <osg/ref_ptr>
#include <osgUtil/LineSegmentIntersector>

class StrokeIntersector : public osgUtil::LineSegmentIntersector
{
public:
    StrokeIntersector();
    StrokeIntersector(const osg::Vec3& start, const osg::Vec3& end);
    StrokeIntersector(CoordinateFrame cf, double x, double y);

    void setOffset(float offset);
    float getOffset() const;

    virtual Intersector* clone( osgUtil::IntersectionVisitor& iv );
    virtual void intersect( osgUtil::IntersectionVisitor& iv, osg::Drawable* drawable );

protected:
    virtual ~StrokeIntersector(){}

private:
    float m_offset;
};

#endif // STROKEINTERSECTOR_H
