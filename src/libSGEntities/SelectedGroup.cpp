#include "SelectedGroup.h"

#include <osg/BoundingBox>
#include <osg/BoundingBox>
#include "Settings.h"


entity::SelectedGroup::SelectedGroup(const osg::Vec3f &canvasCenter)
    : m_group(0)
    , m_center(canvasCenter)
{
}

void entity::SelectedGroup::addEntity(entity::Entity2D *entity, osg::Geode *geodeData)
{
    if (!entity){
        outErrMsg("addEntity: ptr is NULL");
        return;
    }
    if (!geodeData->containsDrawable(entity)){
        outErrMsg("The entity does not belong to Canvas, selection is impossible");
        return;
    }

    if (this->isEntitySelected(entity) == -1){
        this->setEntitySelectedColor(entity, true);
        m_group.push_back(entity);
    }

}

bool entity::SelectedGroup::removeEntity(entity::Entity2D *entity)
{
    if (!entity) return false;
    int idx = this->isEntitySelected(entity);
    if (idx >=0 && idx < (int)m_group.size()){
        this->setEntitySelectedColor(entity, false);
        m_group.erase(m_group.begin()+idx);
        return true;
    }
    return false;
}

void entity::SelectedGroup::resetAll()
{
    while (m_group.size() > 0){
        entity::Entity2D* entity = m_group.at(m_group.size()-1);
        if (!entity) {
            outErrMsg("resetEntitiesSelected: entity is NULL");
            return;
        }
        if (!this->removeEntity(entity)){
            outErrMsg("SelectedGroup::resetAll: canot remove entity");
            return;
        }
    }
}

void entity::SelectedGroup::selectAll(osg::Geode *geodeData)
{
    this->resetAll();
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

int entity::SelectedGroup::getSize() const
{
    return m_group.size();
}

osg::Vec3f entity::SelectedGroup::getCenter(const osg::Matrix &M) const
{
    double mu = 0, mv = 0;
    for (size_t i=0; i<m_group.size(); ++i)
    {
        entity::Entity2D* entity = m_group.at(i);
        if (!entity){
            outErrMsg("getStrokesSelecterCenter: one of entities ptr is NULL");
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
    osg::Vec3f center_loc = osg::Vec3f(mu, mv, 0);
    osg::Vec3f center_glo = center_loc * M;
    return center_glo;
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
            outErrMsg("moveEntities: one of entity ptr is NULL");
            break;
        }
        entity->moveDelta(du, dv);
    }
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
            outErrMsg("scaleEntities: one of strokes ptr is NULL");
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
            outErrMsg("rotateEntities: one of entities ptr is NULL");
            break;
        }
        entity->rotate(theta, center);
    }
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
    if (selected) entity->setColor(dureu::STROKE_CLR_SELECTED);
    else entity->setColor(dureu::STROKE_CLR_NORMAL);
}
