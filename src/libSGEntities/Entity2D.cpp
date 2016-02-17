#include "Entity2D.h"

entity::Entity2D::Entity2D()
    : osg::Geometry()
{

}

entity::Entity2D::Entity2D(const entity::Entity2D &copy, const osg::CopyOp &copyop)
    : osg::Geometry(copy, copyop)
{

}
