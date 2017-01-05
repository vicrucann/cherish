#include "SVMData.h"

#include <osg/Geometry>
#include <osg/Geode>
#include <osg/StateSet>
#include <osg/Point>

#include "QtGlobal"
#include "QDebug"

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

bool entity::SVMData::getVisibility() const
{
    Q_ASSERT(m_switch->getChildValue(m_wire1) == m_switch->getChildValue(m_wire2));
    return m_switch->getChildValue(m_wire1);
}

osg::Vec3f entity::SVMData::getLocalWall(int i) const
{
    if (i<0 || i>3){
        qCritical("Index exceeds limits");
    }

    return m_wire1->getPoint2D(i);
}

osg::Vec3f entity::SVMData::getGlobalFloor(int i) const
{
    if (i<0 || i>3){
        qCritical("Index exceeds limits");
    }

    return m_wire2->getPoint3D(i);
}

entity::SVMData *entity::SVMData::getParentSVM(entity::DraggableWire *wire)
{
    Q_CHECK_PTR(wire);
    if (wire->getNumParents() != 1){
        qWarning("DraggableWire must have one parents");
        return NULL;
    }

    osg::Switch* sw = wire->getParent(0)->asSwitch();
    if (!sw){
        qWarning("Could not obtain switch of SVMData.");
        return NULL;
    }

    Q_CHECK_PTR(sw->getParent(0));
    return dynamic_cast<entity::SVMData*>(sw->getParent(0));
}
