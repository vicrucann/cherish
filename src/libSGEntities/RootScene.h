#ifndef ROOTSCENE
#define ROOTSCENE

/* RootScene
 * It contains all the entities that will appear on the scene.
 * It includes both user scene and utility entities (axis).
 */

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
 * Class description
*/
class RootScene : public osg::ProtectedGroup {
public:
    RootScene(QUndoStack* undoStack);

    entity::UserScene* getUserScene() const;
    void setFilePath(const std::string& name);
    bool isSetFilePath() const;
    bool isSavedToFile() const;
    void setSavedToFile(bool saved);
    bool isEmptyScene() const;
    void clearUserData();

    void setToolsVisibility(bool vis);
    bool getToolsVisibility() const;
    void setAxesVisibility(bool vis);
    bool getAxesVisibility() const;
    void setCanvasVisibilityAll(entity::Canvas* canvas, bool vis);
    bool getCanvasVisibilityAll(entity::Canvas* canvas) const;

    bool writeScenetoFile();
    bool exportSceneToFile(const std::string& name);
    bool loadSceneFromFile();

    int getStrokeLevel() const;
    int getCanvasLevel() const;
    int getPhotoLevel() const;

    void addCanvas(const osg::Matrix& R, const osg::Matrix& T);
    void addCanvas(const osg::Vec3f& normal, const osg::Vec3f& center);
    void addCanvas(const osg::Matrix& R, const osg::Matrix& T, const std::string& name);
    void addStroke(float u, float v, cher::EVENT event);
    void addPolygon(float u, float v, cher::EVENT event);
    void selectAllEntities();
    void addPhoto(const std::string& fname);

    /*! A method to add a new camera position as a bookmark to the BookmarkWidget, and also to the scene graph as entity::SceneState. */
    void addBookmark(BookmarkWidget* widget, const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up, const double& fov);

    /*! A method to suplement a last added entity::SceneState with entity::SVMData as a child.
     * Used to create a new bookmark using correspondence between four points in two different planes
     * This method requires presense of at least two canvases on the screen. */
    bool addSVMData();

    void addBookmarkTool(const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up);
    void updateBookmark(BookmarkWidget* widget, int row);
    void deleteBookmark(BookmarkWidget* widget, const QModelIndex& index);
    void deleteBookmarkTool(int first, int last);
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
    void setCanvasesButCurrent(bool enable);
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

