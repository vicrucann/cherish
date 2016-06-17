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

    virtual cher::ENTITY_TYPE getEntityType() const = 0;
};
}


#endif // ENTITY_H
