#include "CamPoseData.h"

#include <osg/Geometry>
#include <osg/Geode>
#include <osg/StateSet>
#include <osg/Point>

#include "QtGlobal"
#include "QDebug"

entity::CamPoseData::CamPoseData(double fov)
    : osg::ProtectedGroup()
    , m_switch(new osg::Switch())
    , m_wire(new entity::EditableWire(fov))
    , m_camera(new osg::Camera)
{
    this->addChild(m_switch.get());
    m_switch->addChild(m_camera);
    m_camera->setClearMask(GL_DEPTH_BUFFER_BIT);
    m_camera->setRenderOrder(osg::Camera::POST_RENDER);
    m_camera->addChild(m_wire);

    this->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    this->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
    this->getOrCreateStateSet()->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
}

void entity::CamPoseData::setTransform(osg::Matrix m)
{
    m_wire->setMatrix(m);
}

void entity::CamPoseData::setVisibility(bool visibility)
{
    m_switch->setChildValue(m_camera, visibility);
}

bool entity::CamPoseData::getVisibility() const
{
    return m_switch->getChildValue(m_camera);
}

entity::CamPoseData *entity::CamPoseData::getParentCamPose(entity::EditableWire *wire)
{
    Q_CHECK_PTR(wire);
    if (wire->getNumParents() != 1){
        qWarning("EditbaleWire must have one parent");
        return NULL;
    }

    osg::Switch* sw = wire->getParent(0)->asSwitch();
    if (!sw){
        qWarning("Could not obtain switch of SVMData.");
        return NULL;
    }

    Q_CHECK_PTR(sw->getParent(0));
    return dynamic_cast<entity::CamPoseData*>(sw->getParent(0));
}

entity::EditableWire *entity::CamPoseData::getWire() const
{
    return m_wire;
}

bool entity::CamPoseData::getCamera(osg::Vec3f &eye, osg::Vec3f &center, osg::Vec3f &up)
{
    up = m_wire->getUp();
    // get a bit elevated - imitate human height (up*0.5)
    eye = m_wire->getEye3D();
    center = m_wire->getCenter3D();
    return true;
}
