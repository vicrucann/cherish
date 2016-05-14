#ifndef SELECTEDGROUP_H
#define SELECTEDGROUP_H

#include <vector>
#include <osg/Geode>
#include <osg/BoundingBox>
#include "Entity2D.h"

namespace entity {

/*! \class SelectedGroup
 * Class description
*/
class SelectedGroup
{
public:
    SelectedGroup(const osg::Vec3f& canvasCenter = cher::CENTER);

    void addEntity(entity::Entity2D* entity, osg::Geode* geodeData);
    bool removeEntity(entity::Entity2D* entity);
    void resetAll();
    void selectAll(osg::Geode* geodeData);
    const std::vector<Entity2D *> &getEntities() const;
    entity::Entity2D* getEntity(int i) const;
    int getSize() const;
    bool isEmpty() const;
    osg::Vec3f getCenter3D(const osg::Matrix& M) const;
    osg::Vec3f getCenter2D() const;
    osg::Vec3f getCenter3DCustom(const osg::Matrix& M) const;
    void setCenter3DCustom(const osg::Vec3f& center, const osg::Matrix& M);
    osg::Vec3f getCenter2DCustom() const;
    void setCenter2DCustom(const osg::Vec3f& center);
    osg::BoundingBox getBoundingBox() const;
    double getRotationAngle() const;

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
    float m_theta; /* whether axis was rotated */
    bool m_centerEdited; /* whether center was edited by user or not */

}; // class SelectedGroup

} // namespace entity


#endif // SELECTEDGROUP_H
