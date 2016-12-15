#include "SVMData.h"

#include <osg/Geometry>
#include <osg/Geode>
#include <osg/StateSet>
#include <osg/Point>

entity::SVMData::SVMData()
    : osg::ProtectedGroup()
    , m_switch(new osg::Switch)
{
    this->addChild(m_switch.get());
}
