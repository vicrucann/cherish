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

    this->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    this->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
    this->getOrCreateStateSet()->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
}

void entity::SVMData::setTransformWall(osg::Matrix m)
{
    m_wire1->setMatrix(m);
}

void entity::SVMData::setTransformFloor(osg::Matrix m)
{
    m_wire2->setMatrix(m);
}

void entity::SVMData::setVisibility(bool visibility)
{
    m_switch->setChildValue(m_wire1, visibility);
    m_switch->setChildValue(m_wire2, visibility);
}
