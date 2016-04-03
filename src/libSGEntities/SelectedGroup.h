#ifndef SELECTEDGROUP_H
#define SELECTEDGROUP_H

#include <vector>
#include <osg/Geode>
#include "Entity2D.h"

namespace entity {
class SelectedGroup
{
public:
    SelectedGroup(const osg::Vec3f& canvasCenter = dureu::CENTER);

    void addEntity(entity::Entity2D* entity, osg::Geode* geodeData);
    bool removeEntity(entity::Entity2D* entity);
    void resetAll();
    void selectAll(osg::Geode* geodeData);
    const std::vector<Entity2D *> &getEntities() const;
    int getSize() const;
    osg::Vec3f getCenter(const osg::Matrix& M) const;

    void move(double du, double dv);
    void move(std::vector<Entity2D *> &entities, double du, double dv);
    void scale(double sx, double sy);
    void scale(std::vector<Entity2D *> &entities, double sx, double sy, const osg::Vec3f& center);
    void rotate(double theta);
    void rotate(std::vector<Entity2D *> &entities, double theta, const osg::Vec3f& center);

protected:
    int isEntitySelected(entity::Entity2D* entity) const;
    void setEntitySelectedColor(entity::Entity2D* entity, bool selected = true);

    std::vector<entity::Entity2D*> m_group;

    osg::Vec3f m_center; /* local center for rotation and scaling */

}; // class SelectedGroup

} // namespace entity


#endif // SELECTEDGROUP_H
