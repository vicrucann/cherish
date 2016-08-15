#include "SelectedGroup.h"

#include <iostream>
#include <stdio.h>
#include <algorithm>

#include <osg/BoundingBox>
#include <osg/BoundingBox>
#include "Settings.h"


entity::SelectedGroup::SelectedGroup(const osg::Vec3f &canvasCenter)
    : m_group(0)
    , m_center(canvasCenter)
    , m_theta(0)
    , m_centerEdited(false)
{
}

void entity::SelectedGroup::addEntity(entity::Entity2D *entity, osg::Geode *geodeData)
{
    if (!entity){
        qWarning("addEntity: ptr is NULL");
        return;
    }
    if (!geodeData->containsDrawable(entity)){
        qWarning("The entity does not belong to Canvas, selection is impossible");
        return;
    }

    /* initialize center, axis params for ToolFrame */
    if (m_group.size() == 0){
        m_theta = 0;
        m_centerEdited = false;
    }

    if (this->isEntitySelected(entity) == -1){
        this->setEntitySelectedColor(entity, true);
        m_group.push_back(entity);
        if (!m_centerEdited) m_center = this->getCenter2D();
    }
}

bool entity::SelectedGroup::removeEntity(entity::Entity2D *entity)
{
    if (!entity) return false;
    int idx = this->isEntitySelected(entity);
    if (idx >=0 && idx < static_cast<int>(m_group.size())){
        this->setEntitySelectedColor(entity, false);
        m_group.erase(m_group.begin()+idx);
        if (!m_centerEdited) m_center = this->getCenter2D();
        if (m_group.size() == 0) m_centerEdited = false;
        return true;
    }
    return false;
}

void entity::SelectedGroup::resetAll()
{
    while (m_group.size() > 0){
        entity::Entity2D* entity = m_group.at(m_group.size()-1);
        if (!entity) {
            qWarning("resetEntitiesSelected: entity is NULL");
            return;
        }
        if (!this->removeEntity(entity)){
            qWarning("SelectedGroup::resetAll: canot remove entity");
            return;
        }
    }
    m_centerEdited = false;
}

void entity::SelectedGroup::selectAll(osg::Geode *geodeData)
{
    bool tmp = m_centerEdited;
    this->resetAll();
    m_centerEdited = tmp;
    for (size_t i = 0; i <geodeData->getNumChildren(); ++i){
        entity::Entity2D* entity = dynamic_cast<entity::Entity2D*>(geodeData->getChild(i));
        if (!entity) continue;
        this->addEntity(entity, geodeData);
    }
}

const std::vector<entity::Entity2D *> &entity::SelectedGroup::getEntities() const
{
    return m_group;
}

entity::Entity2D *entity::SelectedGroup::getEntity(int i) const
{
    if (i<0 || i>= static_cast<int>(m_group.size())) return 0;
    return m_group.at(i);
}

int entity::SelectedGroup::getSize() const
{
    return m_group.size();
}

bool entity::SelectedGroup::isEmpty() const
{
    return (m_group.size() == 0);
}

osg::Vec3f entity::SelectedGroup::getCenter3D(const osg::Matrix &M) const
{
    osg::Vec3f center_loc = this->getCenter2D();
    osg::Vec3f center_glo = center_loc * M;
    return center_glo;
}

osg::Vec3f entity::SelectedGroup::getCenter2D() const
{
    double mu = 0, mv = 0;
    for (size_t i=0; i<m_group.size(); ++i)
    {
        entity::Entity2D* entity = m_group.at(i);
        if (!entity){
            qWarning("getStrokesSelecterCenter: one of entities ptr is NULL");
            break;
        }
        osg::BoundingBox bb = entity->getBoundingBox();
        mu += bb.center().x();
        mv += bb.center().y();
    }
    if (m_group.size() > 0){
        mu /= m_group.size();
        mv /= m_group.size();
    }
    return osg::Vec3f(mu, mv, 0);
}

osg::Vec3f entity::SelectedGroup::getCenter3DCustom(const osg::Matrix &M) const
{
    return m_center * M;
}

void entity::SelectedGroup::setCenter3DCustom(const osg::Vec3f &center, const osg::Matrix &M)
{
    m_centerEdited = true;
    osg::Matrix invM;
    if (!invM.invert(M)) return;
    m_center = center * invM;
}

osg::Vec3f entity::SelectedGroup::getCenter2DCustom() const
{
    return m_center;
}

void entity::SelectedGroup::setCenter2DCustom(const osg::Vec3f &center)
{
    m_centerEdited = true;
    m_center = center;
}

osg::BoundingBox entity::SelectedGroup::getBoundingBox() const
{
    osg::BoundingBox bb;
    double xmin = FLT_MAX, ymin = FLT_MAX, xmax = -FLT_MAX, ymax = -FLT_MAX;
    for (size_t i=0; i<m_group.size(); ++i){
        entity::Entity2D* entity = m_group.at(i);
        if (!entity){
            qWarning("getStrokesSelecterCenter: one of entities ptr is NULL");
            break;
        }
        osg::BoundingBox bbi = entity->getBoundingBox();
        if (!bbi.valid()) continue;
        xmin = std::min(xmin, double(bbi.xMin()));
        ymin = std::min(ymin, double(bbi.yMin()));
        xmax = std::max(xmax, double(bbi.xMax()));
        ymax = std::max(ymax, double(bbi.yMax()));

    }
    bb.set(xmin, ymin, 0, xmax, ymax, 0);
    return bb;
}

double entity::SelectedGroup::getRotationAngle() const
{
    return m_theta;
}

void entity::SelectedGroup::move(double du, double dv)
{
    this->move(m_group, du, dv);
}

void entity::SelectedGroup::move(std::vector<entity::Entity2D *> &entities, double du, double dv)
{
    for (size_t i=0; i<entities.size(); ++i){
        entity::Entity2D* entity = entities.at(i);
        if (!entity){
            qWarning("moveEntities: one of entity ptr is NULL");
            break;
        }
        entity->moveDelta(du, dv);
    }
    m_center = m_center + osg::Vec3f(du, dv, 0);
}

void entity::SelectedGroup::scale(double sx, double sy)
{
    this->scale(m_group, sx, sy, m_center);
}

void entity::SelectedGroup::scale(std::vector<entity::Entity2D *> &entities, double sx, double sy, const osg::Vec3f &center)
{
    for (size_t i=0; i<entities.size(); ++i){
        entity::Entity2D* entity = entities.at(i);
        if (!entity){
            qWarning("scaleEntities: one of strokes ptr is NULL");
            break;
        }
        entity->scale(sx, sy, center);
    }
}

void entity::SelectedGroup::rotate(double theta)
{
    this->rotate(m_group, theta, m_center);
}

void entity::SelectedGroup::rotate(std::vector<entity::Entity2D *> &entities, double theta, const osg::Vec3f &center)
{
    for (size_t i=0; i<entities.size(); ++i){
        entity::Entity2D* entity = entities.at(i);
        if (!entity){
            qWarning("rotateEntities: one of entities ptr is NULL");
            break;
        }
        entity->rotate(theta, center);
    }
    m_theta += theta;
}

int entity::SelectedGroup::isEntitySelected(entity::Entity2D *entity) const
{
    for (size_t i = 0; i < m_group.size(); ++i){
        if (entity == m_group.at(i))
            return i;
    }
    return -1;
}

void entity::SelectedGroup::setEntitySelectedColor(entity::Entity2D *entity, bool selected)
{
    if (!entity) return;
    if (selected) entity->setColor(cher::STROKE_CLR_SELECTED);
    else entity->setColor(cher::STROKE_CLR_NORMAL);
}
