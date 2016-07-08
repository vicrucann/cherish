#ifndef CANVAS
#define CANVAS

#include "Settings.h"
#include "Stroke.h"
#include "Photo.h"
#include "ToolGlobal.h"
#include "SelectedGroup.h"
#include "ProtectedGroup.h"

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
/*! \class Canvas
 * Class description
*/
class Canvas : public osg::ProtectedGroup {
public:
    Canvas();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    Canvas(const Canvas& cnv, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

public:
    /*! Method is either called automatically when running initializeSG or when reading scene from file. */
    virtual void initializeTools();

    /*! Method to initialize canvases' geometrical properties. Must be called each time a canvas is allocated. */
    virtual void initializeSG();

    /*! Method is called automatically from initializeSG() to initialize OpenGL state machine or when reading scene from file. */
    virtual void initializeStateMachine();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
public:
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

    void setGroupData(osg::Group* group);
    const osg::Group* getGroupData() const;

    void setGeodeStrokes(osg::Geode* geode);
    const osg::Geode* getGeodeStrokes() const;

    void setGeodePhotos(osg::Geode* geode);
    const osg::Geode* getGeodePhotos() const;

    void setCenter(const osg::Vec3f& center);
#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    /*! \return virtual canvas' 3D center. \sa getCenter2D() */
    const osg::Vec3f& getCenter() const;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    void setNormal(const osg::Vec3f& normal);
    const osg::Vec3f& getNormal() const;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

    /*! A method to set up canvas frame color. \sa getColor */
    void setColor(const osg::Vec4f& color, const osg::Vec4f& colorIntersection = cher::CANVAS_CLR_PREVIOUS);
    /*! \return color of canvas frame. \sa setColor */
    const osg::Vec4f& getColor() const;

protected:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    void setVisibilityFrame(bool vis);
    bool getVisibilityFrame() const;
#endif // DOXYGEN_SHOULD_SKIP_THIS
public:
    /*! A method to set up tool frame visibility. \param vis is true for visible and false for invisible */
    void setVisibilityFrameInternal(bool vis);

    /*! \return tool frame visibility. \sa setVisibilityFrameInternal(). */
    bool getVisibilityFrameInternal() const;

    /*! \return canvas content visibility flag. \sa setVisibilityAll(). */
    bool getVisibilityAll() const;

    /*! A method to set up canvas content visibility (groupData and frameTool). \param vis is true for visible and false for invisible. */
    void setVisibilityAll(bool vis);

protected:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    bool getVisibilityData() const;
    void setVisibilityData(bool vis);
#endif // DOXYGEN_SHOULD_SKIP_THIS

public:
    /*!
     *  V
     *  |
     *  |____ U
     *  /
     * normal
     *
     * \return direction of U axis in global space.
     * \sa getGlobalAxisV() */
    osg::Vec3f getGlobalAxisU() const;

    /*!
     *  V
     *  |
     *  |____ U
     *  /
     * normal
     *
     * \return direction of V axis in global space.
     * \sa getGlobalAxisU() */
    osg::Vec3f getGlobalAxisV() const;

    /*! \return a pointer on a pickage geometry, to be used from EventHandler. */
    const osg::Geometry* getGeometryPickable() const;

    /*! A method to translate canvas parameters. \param mt is the translation matrix. */
    void translate(const osg::Matrix& mt);

    /*! A method to rotate canvas parameters. \param mr is the rotation matrix, \param c3d_new is the point in 3D global space aroung which the rotation is performed. */
    void rotate(const osg::Matrix& mr, const osg::Vec3f& c3d_new);

    /*! Convinience method that is normally used before changing a canvas status, e.g. from current to previous. It deselects SelectedGroup, as well as current stroke. */
    void unselectAll();

    /*! Method that removes all entities from entity::SelectedGroup. \sa selectAllEntities() */
    void unselectEntities();

    /*! Method that adds all the entities of current canvas to entity::SelectedGroup. \sa unselectEntities() */
    void selectAllEntities();

    /*! \param stroke is the stroke to mark as current, i.e., for a continious editing and point addition. */
    void setStrokeCurrent(entity::Stroke* stroke);

    /*! \param current is a boolean variable; if false then an observer point on current stroke turns NULL. */
    void setStrokeCurrent(bool current);

    /*! \return a pointer on current stroke, editable, for example, to append a point to the stroke. */
    entity::Stroke* getStrokeCurrent() const;

    /*! \param entity is entity to add to entity::SelectedGroup. */
    void addEntitySelected(entity::Entity2D* entity);

    /*! \param entity is entity to substract from entity::SelectedGroup. */
    void removeEntitySelected(entity::Entity2D* entity);

    /*! \return vector of pointers on selected entities within the canvas. */
    const std::vector<Entity2D *> &getEntitiesSelected() const;

    /*! \return a number of selelcted entities within the canvas. */
    int getEntitiesSelectedSize() const;

    /*! \return true if there are any entitie in SelectedGroup, false if the group is empty. */
    bool isEntitiesSelected() const;

    /*! \return global 3D center of SelectedGroup. If the group is empty, returns global 3D canvas center. */
    const osg::Vec3f& getEntitiesSelectedCenter3D() const;

    /*! \return local 2D center of SelectedGroup. */
    const osg::Vec3f& getEntitiesSelectedCenter2D() const;

    /*! \return local 2D center of canvas. \sa getCenter() */
    osg::Vec3f getCenter2D() const;

    /*! \return global 3D mean center of canvas; used as a default rotation point when rotating the canvas. */
    const osg::Vec3f& getBoundingBoxCenter3D() const;

    /*! \return local 2D center which is calculated based on bounding box of the whole canvas. */
    const osg::Vec3f& getBoundingBoxCenter2D() const;

    /*! \return a bounding box of the whole canvas. */
    osg::BoundingBox getBoundingBox() const;


    /*! \param entities is the vector of entities to move, \param du is delta-u local 2D coordinate, \param dv is delta-V local 2D coordinate.  \sa moveEntitiesSelected() */
    void moveEntities(std::vector<Entity2D *> &entities, double du, double dv);

    /*! \param du is delta-u local 2D coordinate, \param dv is delta-V local 2D coordinate.  \sa moveEntitiesSelected(). \sa moveEntities() */
    void moveEntitiesSelected(double du, double dv);

    /*! \param entities is the vector of entities to scale,
     * \param sx is scale along X axis,
     * \param sy is scale along Y axis,
     * \param center is 2D center along which the scaling is performed.
     * \sa scaleEntitiesSelected() */
    void scaleEntities(std::vector<Entity2D *> &entities, double sx, double sy, osg::Vec3f center);

    /*! \param sx is scale along X axis, \param sy is scale along Y axis, \sa scaleEntities() */
    void scaleEntitiesSelected(double sx, double sy);

    /*! \param entities is the vector of entities to rotate,
     * \param theta is angle of rotation,
     * \param center the rotation local 2D center
     * \sa rotateEntitiesSelected() */
    void rotateEntities(std::vector<entity::Entity2D*> entities, double theta, osg::Vec3f center);

    /*! \param theta is angle of rotation. \sa rotateEntities() */
    void rotateEntitiesSelected(double theta);

    /*! Method to re-calculate frame's geometry and plane center transform based on canvas content location.
     * \param against is a Canvas against which an intersection is calculated. Normally it is UserScene::m_canvasPrevious. */
    void updateFrame(entity::Canvas *against = 0);

    /*! \return 4 coordinates of canvas frame vertices. */
    const osg::Vec3Array* getFrameVertices() const;

    /*! A method which is called when performing RootScene::writeScenetoFile to temporarly detach the frame tools.
     * The memory for tools is not freed since it is managed by a smart pointer (osg::ref_ptr).
     * \sa detachFrame */
    bool detachFrame();

    /*! A method which is called when performing RootScene::writeScenetoFile to attach the frame tools back.
     * The memory for tools is not freed since it is managed by a smart pointer (osg::ref_ptr).
     * \sa detachFrame */
    bool attachFrame();

    /*! Method to switch the normal canvas mode to edit mode, used for editing canvas position and rotation.
     * \param on is true when the canvas is in the process of editing, and false otherwise. */
    void setModeEdit(bool on);

    /*! \return canvas edit mode. \sa setModeEdit() */
    bool getModeEdit() const;

    /*! \return plane equation based on canvas center and normal */
    osg::Plane getPlane() const;

    /*! Methor to perform a clone operation of canvas. It creates a new canvas that does not belong to the scene graph. \sa separate() */
    entity::Canvas* clone() const;

    /*! Method to clone the canvas with inclusion of selected entities into the new canvas. A created canvas does not belong the scene graph. \sa clone() */
    entity::Canvas* separate();

    /*! \return pointer on FrameTool of canvas */
    const entity::FrameTool* getToolFrame() const;

    /*! \return total number of entities like strokes and photos that canvas contains. */
    unsigned int getNumEntities() const;

    /*! \return total number of photos that canvas contains. */
    unsigned int getNumPhotos() const;

    /*! \return pointer on a photo with the given index. */
    entity::Photo* getPhoto(int row) const;

    /*! Method to iterate throught all the entities: both strokes and photos
     * \param i is the index of desired entity
     * \return const pointer on entity. */
    entity::Entity2D* getEntity(unsigned int i) const;

    /*! Method to add an entity to the canvas. \param entity is a pointer on the entity, \return true if operation was successfull, false otherwise. */
    bool addEntity(entity::Entity2D* entity);

    /*! Method to remove an entity from the canvas. \param entity is a pointer on the entity, \return true if operation was successfull, false otherwise. */
    bool removeEntity(entity::Entity2D* entity);

    /*! \param entity is the pointer on entity, \return true if the canvas contains given entity, false otherwise. */
    bool containsEntity(entity::Entity2D* entity) const;

protected:
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
    osg::ref_ptr<osg::Group> m_groupData; /* keeps user canvas 2d entities such as strokes and photos */
    osg::ref_ptr<osg::Geode> m_geodeStrokes; // contains all the strokes as children
    osg::ref_ptr<osg::Geode> m_geodePhotos; // contains all the photos as children

    /* construction geodes */
    osg::ref_ptr<entity::FrameTool> m_toolFrame;

    osg::observer_ptr<entity::Stroke> m_strokeCurrent; /* for stroke drawing */
    entity::SelectedGroup m_selectedGroup;
    osg::Vec3f m_center; /* 3D global - virtual plane parameter */
    osg::Vec3f m_normal; /* 3D global - virtual plane parameter*/

    bool m_edit;

};
}

#endif // CANVAS

