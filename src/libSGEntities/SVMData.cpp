#include "SVMData.h"

#include <osg/Geometry>
#include <osg/Geode>
#include <osg/StateSet>
#include <osg/Point>

entity::SVMData::SVMData()
    : osg::ProtectedGroup()
    , m_switch(new osg::Switch)
    , m_wire1(new entity::DraggableWire())
    , m_wire2(new entity::DraggableWire())
{
    this->addChild(m_switch.get());
    m_switch->addChild(m_wire1);
    m_switch->addChild(m_wire2);
}

void entity::SVMData::setTransformWall(osg::Matrix m)
{
    m_wire1->setMatrix(m);
}

void entity::SVMData::setTransformFloor(osg::Matrix m)
{
    m_wire2->setMatrix(m);
}
