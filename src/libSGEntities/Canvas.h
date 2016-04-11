#ifndef CANVAS
#define CANVAS

/* The canvas is defined by its center location -R3
 * and its normal -R3. The default size values are provided
 * when construction the Canvas.
 * When more drawables are introduced by user, e.g., strokes,
 * the canvas borders are redifined (expanded) using osg::BoundingBox().
 *
 * TODO:
 * For current canvas only: intersections (projection) line with
 * previous canvas.
 * Proper canvas transform when rotating (the offset is already done).
 *
 * Canvas has the next branch structure:
 * Canvas -> Switch (-> Transform) -> Geometry -> Canvas drawables
 *                                           |-> User drawables (strokes)
 */

#include "Settings.h"
#include "Stroke.h"
#include "Photo.h"
#include "ToolGlobal.h"
#include "SelectedGroup.h"

#include <osg/ref_ptr>
#include <osg/Geode>
#include <osg/Group>
#include <osg/BoundingBox>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Switch>
#include <osgDB/ObjectWrapper>
#include <osg/AutoTransform>

namespace entity {
class Canvas : public osg::Group {
public:
    Canvas();

    /* ctor to be used only within serializer, do not use it anywhere else */
    Canvas(const Canvas& cnv, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

    virtual void initializeTools();
    virtual void initializeSG();

    META_Node(entity, Canvas)

    /* setters and getters for serialization only! */
    void setMatrixRotation(const osg::Matrix& R);
    const osg::Matrix& getMatrixRotation() const;

    void setMatrixTranslation(const osg::Matrix& T);
    const osg::Matrix& getMatrixTranslation() const;

    void setTransform(osg::MatrixTransform* t);
    const osg::MatrixTransform* getTransform() const;
    osg::MatrixTransform* getTransform();

    void setSwitch(osg::Switch* sw);
    const osg::Switch* getSwitch() const;

    void setGeodeData(osg::Geode* geode);
    const osg::Geode* getGeodeData() const;
    osg::Geode* getGeodeData();

    void setCenter(const osg::Vec3f& center);
    const osg::Vec3f& getCenter() const;

    void setNormal(const osg::Vec3f& normal);
    const osg::Vec3f& getNormal() const;

    /* other API methods */

    void setColor(const osg::Vec4f& color);
    const osg::Vec4f& getColor() const;

    void setRotationAxis(const osg::Vec3f& axis);
    const osg::Vec3f& getRotationAxis() const;

    void setVisibilityFrame(bool vis);
    bool getVisibility() const;

    void setVisibilityAll(bool vis);
    bool getVisibilityData() const;

    osg::Vec3f getGlobalAxisU() const;
    osg::Vec3f getGlobalAxisV() const;

    osg::Geometry* getGeometryPickable() const;

    void translate(const osg::Matrix& mt);
    void rotate(const osg::Matrix& mr);

    void unselectAll(); /* includes current strokes and selection group */
    void unselectEntities();
    void selectAllStrokes();

    void setStrokeCurrent(entity::Stroke* stroke);
    void setStrokeCurrent(bool current);
    entity::Stroke* getStrokeCurrent() const;

    void addEntitySelected(entity::Entity2D* entity);
    void removeEntitySelected(entity::Entity2D* entity);
    const std::vector<Entity2D *> &getStrokesSelected() const;
    int getStrokesSelectedSize() const;
    bool isEntitiesSelected() const;
    osg::Vec3f getStrokesSelectedCenter() const;
    osg::Vec3f getSelectedEntitiesCenter2D() const;

    void moveEntities(std::vector<Entity2D *> &entities, double du, double dv);
    void moveEntitiesSelected(double du, double dv);
    void scaleEntities(std::vector<Entity2D *> &entities, double sx, double sy, osg::Vec3f center);
    void scaleEntitiesSelected(double sx, double sy);
    void rotateEntities(std::vector<entity::Entity2D*> entities, double theta, osg::Vec3f center);
    void rotateEntitiesSelected(double theta);

    /* re-calculate frame's geometry and plane center transform
     * based on canvas content location */
    void updateFrame(entity::Canvas *against = 0);
    const osg::Vec3Array* getFrame() const;

    void setModeEdit(bool on); // changes certain colors, shows or hides normal
    bool getModeEdit() const;

    osg::Plane getPlane() const;
    osg::MatrixTransform* getMatrixTransform() const;
    entity::Canvas* clone() const;

    osg::Node* getTool(const std::string& name);

protected:
    ~Canvas();

    void updateTransforms();
    void resetTransforms();
    void setVertices(const osg::Vec3f& center, float szX, float szY, float szCr, float szAx);
    void setVerticesDefault(const osg::Vec3f& center);
    void setIntersection(entity::Canvas* against = 0);

private:
    osg::Matrix m_mR; /* part of m_transform */
    osg::Matrix m_mT; /* part of m_transform */
    osg::ref_ptr<osg::MatrixTransform> m_transform; /* matrix transform in 3D space */
    osg::ref_ptr<osg::Switch> m_switch; /* inisible or not, the whole canvas content */
    osg::ref_ptr<osg::Geode> m_geodeData; /* keeps user canvas 2d entities such as strokes and photos */

    /* construction geodes */
    entity::FrameTool* m_toolFrame;

    osg::observer_ptr<entity::Stroke> m_strokeCurrent; /* for stroke drawing */
//    std::vector<entity::Entity2D*> m_selectedEntities; /* list of selected entities */
    entity::SelectedGroup m_selectedGroup;
    osg::Vec3f m_center; /* 3D global - virtual plane parameter */
    osg::Vec3f m_normal; /* 3D global - virtual plane parameter*/

    bool m_edit;
    osg::Vec3f m_rotaxis;

};
}

#endif // CANVAS

