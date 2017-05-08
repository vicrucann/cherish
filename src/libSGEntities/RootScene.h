#ifndef ROOTSCENE
#define ROOTSCENE

#include <iostream>
#include <string>
#include <string>

#include <osg/ref_ptr>
#include <osg/AutoTransform>
#include <osg/Node>

#include "ProtectedGroup.h"
#include "Settings.h"
#include "../libGUI/hudcamera.h"
#include "UserScene.h"
#include "Entity2D.h"
#include "Stroke.h"
#include "Photo.h"
#include "Bookmarks.h"
#include "../libGUI/ListWidget.h"
#include "ToolGlobal.h"
#include "SceneState.h"
#include "SVMData.h"
#include "CamPoseData.h"
#include "DraggableWire.h"

#include <QUndoStack>
#include <QModelIndex>

namespace fur{
class AddStrokeCommand;
}
namespace entity{
class SceneState;
class UserScene;
class Bookmarks;
}

/*! \class RootScene
 * \brief Contains all the entities that will appear on the scene:
 * both user scene and utility entities (i.e. global axis).
 *
 * This class also represents an intersface for entity::UserScene.
 *
*/
class RootScene : public osg::ProtectedGroup {
public:
    /*! Constructor: initializes structure of entity::UserScene, initializes global tools. */
    RootScene(QUndoStack* undoStack);

    /*! \return pointer on user scene. */
    entity::UserScene* getUserScene() const;

    /*! \return pointer on undo/redo stack. */
    QUndoStack* getUndoStack() const;

    /*! A method to attach current scene graph to a specified file name where the scene will be kept on disk. */
    void setFilePath(const std::string& name);

    /*! \return true if the scene is already attached to a file name. */
    bool isSetFilePath() const;

    /*! \return true if there is no difference between what was saved on disk and current scene graph. */
    bool isSavedToFile() const;

    /*! Method is only used by testers. Not to be used otherwise. */
    void setSavedToFile(bool saved);

    /*! \return true if there are no elements introduced by used on the scene. \sa clearUserData(). */
    bool isEmptyScene() const;

    /*! A method to make the scene empty. \sa isEmptyScene(). */
    void clearUserData();

    /*! A method to make all the tools hide or appear. */
    void setToolsVisibility(bool vis);

    /*! \return true if the tools are visible. */
    bool getToolsVisibility() const;

    /*! A method to make the global axis tool to hide or appear. */
    void setAxesVisibility(bool vis);

    /*! \return true if the global axis are visible. */
    bool getAxesVisibility() const;

    /*! A method to set up canvas visibility for both scene graph and GUI. */
    void setCanvasVisibilityAll(entity::Canvas* canvas, bool vis);

    /*! \return true if canvas visible. */
    bool getCanvasVisibilityAll(entity::Canvas* canvas) const;

    /*! A method to write the user scene to file. */
    bool writeScenetoFile();

    /*! A method to export the user scene to OBJ or 3DS format. It uses Parallel Transport Frame algorithm
     * in order to convert shadered strokes into triangular meshes. */
    bool exportSceneToFile(const std::string& name);

    /*! \return true if scene was loaded successfully from file. */
    bool loadSceneFromFile();

    /*! \return the depth of where entity::Stroke geometries are located. */
    int getStrokeLevel() const;

    /*! \return the depth of where entity::Canvas geomtries are located. */
    int getCanvasLevel() const;

    /*! \return the depth of where entity::Photo geometries are located. */
    int getPhotoLevel() const;

    /*! A method to add new canvas to scene with provided rotation and translation matrices. */
    void addCanvas(const osg::Matrix& R, const osg::Matrix& T);

    /*! A method to add new canvas to scene with provided normal and center point. */
    void addCanvas(const osg::Vec3f& normal, const osg::Vec3f& center);

    /*! A method to add canvas to scene with provided rotation and translation matrices, as well as a name. */
    void addCanvas(const osg::Matrix& R, const osg::Matrix& T, const std::string& name);

    /*! A method to add/contribute to a stroke given local coordinates. */
    void addStroke(float u, float v, cher::EVENT event);

    /*! A method to add/contribute to a polygon given local coordinates. */
    void addPolygon(float u, float v, cher::EVENT event);

    /*! Add line segment point given local coordinates. */
    void addLineSegment(float u, float v, cher::EVENT event);

    /*! A method to select all strokes of the current canvas. */
    void selectAllEntities();

    /*! A method to add specified photo to the current canvas. */
    void addPhoto(const std::string& fname);

    /*! A method to add a new camera position as a bookmark to the BookmarkWidget, and also to the scene graph as entity::SceneState. */
    void addBookmark(BookmarkWidget* widget, const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up, const double& fov);

    /*! A method to suplement a last added entity::SceneState with entity::SVMData as a child.
     * Used to create a new bookmark using correspondence between four points in two different planes
     * This method requires presense of at least two canvases on the screen. */
    bool addSVMData();

    /*! A method to add SVMData as a child to RootScene when using photo re-scaling. */
    bool addPhotoScaleData();

    /*! A method to hide SVMData which is a child of RootScene. Used when doing photo re-scaling. */
    bool hidePhotoScaleData();

    /*! A method to remove any SVMData as a child of RootScene when done using photo re-scaling. */
    bool removePhotoScaleData();

    /*! \return a pointer on SVMData which is a direct child of RootScene. */
    const entity::SVMData* getPhotoScaleData() const;

    /* A method to supplement a last added entity::SceneState with entity::CamPoseData as a child.
     * Used to create a new bookmark using user interaction. This method requires presense of at least
     * one canvas on the scene. */
    bool addCamPoseData();

    /*! A method to hide all the visible SVMData from scene.*/
    void hideAndUpdateSVMData();

    /*! A method to hide all the visible CamPoseData from scene. */
    void hideAndUpdateCamPoseData();

    /*! A method to obtain a pointer on SVMData that is in the state of editing by user.
     * If no such data available, return NULL. */
    entity::SVMData* getSVMDataCurrent() const;

    /*! A method to obtain a pointer on campose data that is in the state of editing by user.
     * If no such data available, return 0. */
    entity::CamPoseData* getCamPoseDataCurrent() const;

    void addBookmarkTool(const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up);
    void updateBookmark(BookmarkWidget* widget, int row);
    void deleteBookmark(BookmarkWidget* widget, const QModelIndex& index);
    void deleteBookmarkTool(int first, int last);

    /*! A method which is called when a file is opened. It resets the screenshot bookmarks.
     * \param widget is a pointer on bookmark widget (member of MainWindow). */
    void resetBookmarks(BookmarkWidget* widget);

    /*! A method to set visibility of all the bookmarks tools. The visibility cannot be set for individual bookmarks tool, only for the
     * whole group.
     * \param vis is the visibility flag: true for visibile and false for invisible state. */
    void setBookmarkToolVisibility(bool vis);

    /*! \return A visibility flag for the set of all the bookmark tools. Since the visibility cannot be set for individual bookmarks,
     * the returned value represents the visibility of the whole group: true for visibile and false for being invisible. */
    bool getBookmarkToolVisibility() const;

    void eraseStroke(entity::Stroke* stroke, int first, int last, cher::EVENT event);

    bool setCanvasCurrent(entity::Canvas* cnv);
    bool setCanvasPrevious(entity::Canvas* cnv);

    /*! A method to set up traversal masks for all the canvases but current.
     * \sa entity::UserScene::setCanvasesButCurrent(), RootScene::setAllCanvases(). */
    void setCanvasesButCurrent(bool enable);

    /*! A method to set up traversal masks for all the canvases.
     * \sa entity::UserScene::setAllCanvases(), entity::UserScene::setCanvasesButCurrent(). */
    void setAllCanvases(bool enable);

    entity::Canvas* getCanvasCurrent() const;
    entity::Canvas* getCanvasPrevious() const;
    entity::Bookmarks* getBookmarksModel() const;

    void editCanvasOffset(const osg::Vec3f& translate, cher::EVENT event);
    void editCanvasRotate(const osg::Quat& rotation, const osg::Vec3f& center3d, cher::EVENT event);
    void editCanvasClone(const osg::Vec3f& translate, cher::EVENT event);
    void editCanvasSeparate(const osg::Vec3f& translate, cher::EVENT event);
    void editCanvasDelete(entity::Canvas* canvas);

    void editPhotoDelete(entity::Photo* photo, entity::Canvas* canvas);
    void editPhotoPush(entity::Photo* photo, entity::Canvas* source, entity::Canvas* destination);

    /*! A method to remove a given polygon from the canvas; called from EventHandler. */
    void editPolygonDelete(entity::Polygon* poly, entity::Canvas* canvas);

    void editStrokesPush(osg::Camera *camera);
    void editStrokesMove(double u, double v, cher::EVENT event);
    void editStrokesScale(double u, double v, cher::EVENT event);
    void editStrokesRotate(double u, double v, cher::EVENT event);
    void editStrokeDelete(entity::Stroke* stroke);

    void copyToBuffer();
    void cutToBuffer();
    void pasteFromBuffer();

    const std::vector< osg::ref_ptr<entity::Entity2D> >& getBuffer() const;

    /*! \return A pointer on SceneState allocated instance. Note: this pointer is not managed by smart pointers and must
     * be deleted manually.
     * \sa entity::SceneState::stripDataFrom() */
    entity::SceneState* createSceneState() const;

    /*! \param A pointer on SceneState instance which is served as a source for RootScene settings.
     * \return Whether the operation was performed successfully (no size discrepancies found) or not. */
    bool setSceneState(const entity::SceneState* state);

    /*! A method to obtain a bookmark tool associated with the entity::Bookmarks.
     * \param index is the tool index associated with the bookmark data. */
    entity::BookmarkTool* getBookmarkTool(int index);

protected:

private:
    osg::ref_ptr<entity::UserScene> m_userScene;
    osg::ref_ptr<entity::AxisGlobalTool> m_axisTool;
    osg::ref_ptr<osg::Group> m_bookmarkTools;
    std::vector< osg::ref_ptr<entity::Entity2D> > m_buffer; /* copy-paste buffer */
    QUndoStack* m_undoStack;
    bool m_saved;
    bool m_visibilityBookmarkTool;
};

#endif // SCENE

