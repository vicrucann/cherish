#ifndef ENTITY_H
#define ENTITY_H

#include "Settings.h"

#include <osg/Geometry>

namespace entity {

/*! \class Entity2D
 * Class description
*/
class Entity2D : public osg::Geometry
{
public:
    Entity2D();
    Entity2D(const Entity2D& copy, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

    virtual void moveDelta(double du, double dv) = 0;
    virtual void scale(double scaleX, double scaleY, osg::Vec3f center) = 0;
    virtual void scale(double scale, osg::Vec3f center) = 0;
    virtual void rotate(double theta, osg::Vec3f center) = 0;
    virtual void setColor(const osg::Vec4f& color) = 0;
    virtual const osg::Vec4f& getColor() const = 0;
    virtual void setSelected(float alpha) = 0;
    virtual void setUnselected(float alpha) = 0;

    virtual cher::ENTITY_TYPE getEntityType() const = 0;

protected:
    using osg::Geometry::addPrimitiveSet;
    using osg::Geometry::addUpdateCallback;
    using osg::Geometry::asCamera;
    using osg::Geometry::asDrawable;
    using osg::Geometry::asGeode;
    using osg::Geometry::asGeometry;
    using osg::Geometry::asGroup;
    using osg::Geometry::asNode;
    using osg::Geometry::asSwitch;
    using osg::Geometry::asTransform;
};
}


#endif // ENTITY_H
