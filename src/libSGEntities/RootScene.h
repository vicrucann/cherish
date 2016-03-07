#ifndef ROOTSCENE
#define ROOTSCENE

/* RootScene
 * It contains all the entities that will appear on the scene.
 * It includes both user scene and utility entities (axis).
 * The user scene has the following structure (tree branch example):
 * [Root] -> <Transform1> -> (Canvas11) -> `Stroke111`
 *        -> <Transform2> -> (Canvas21)
 *                        -> (Canvas22) -> `Stroke221`
 *                                      -> `Stroke222`
 * ...
 *       -> {Switch} -> {Axes}
 *
 * Where we denote:
 * [] - an osg::Group inherited
 * <> - osg::MatrixTransform inherited
 * () - osg::Geode inherited
 * `` - osg::Drawable inherited
 * {} - other osg inherited types such as camera or switch nodes
 *
 * When performing addCanvas() or deleteCanvas(), this RootScene
 * tries to delete the canvases directly from m_userScene through the
 * API methods.
 */

#include <iostream>
#include <string>
#include <string>

#include <osg/ref_ptr>

#include "axes.h"
#include "Settings.h"
#include "../libSGUtils/ObserveSceneCallback.h"
#include "../libGUI/hudcamera.h"
#include "UserScene.h"
#include "Photo.h"
#include "Bookmarks.h"
#include "../libGUI/BookmarkWidget.h"

#include <QUndoStack>
#include <QModelIndex>

class AddStrokeCommand;

class RootScene : public osg::Group {
public:
    RootScene(QUndoStack* undoStack);

    entity::UserScene* getUserScene() const;
    void setFilePath(const std::string& name);
    bool isSetFilePath() const;
    bool isSavedToFile() const;
    bool isEmptyScene() const;
    void clearUserData();

    void setToolsVisibility(bool vis);
    bool getToolsVisibility() const;
    void setAxesVisibility(bool vis);
    bool getAxesVisibility() const;
    const Axes* getAxes() const;

    const ObserveSceneCallback* getSceneObserver() const;
    const HUDCamera* getHudCamera() const;
    void setHudCameraVisibility(bool vis);
    bool getHudCameraVisibility() const;

    bool writeScenetoFile();
    bool loadSceneFromFile();

    int getStrokeLevel() const;
    int getCanvasLevel() const;
    int getPhotoLevel() const;

    void addCanvas(const osg::Matrix& R, const osg::Matrix& T);
    void addCanvas(const osg::Matrix& R, const osg::Matrix& T, const std::string& name);
    void addStroke(float u, float v, dureu::EVENT event);
    void selectAllStrokes();
    void addPhoto(const std::string& fname);
    void addBookmark(BookmarkWidget* widget, const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up);
    void updateBookmark(BookmarkWidget* widget, int row);
    void deleteBookmark(BookmarkWidget* widget, const QModelIndex& index);
    void eraseStroke(entity::Stroke* stroke, int first, int last, dureu::EVENT event);

    void setCanvasSelected(entity::Canvas* cnv);
    bool setCanvasCurrent(entity::Canvas* cnv);
    bool setCanvasPrevious(entity::Canvas* cnv);
    void setCanvasesButCurrent(bool enable);
    entity::Canvas* getCanvasSelected() const;
    entity::Canvas* getCanvasCurrent() const;
    entity::Canvas* getCanvasPrevious() const;
    entity::Bookmarks* getBookmarksModel() const;

    void editCanvasOffset(const osg::Vec3f& translate, dureu::EVENT event);
    void editCanvasRotate(const osg::Quat& rotation, dureu::EVENT event);
    void editCanvasClone(const osg::Vec3f& translate, dureu::EVENT event);
    void editCanvasDelete(entity::Canvas* canvas);

    void editPhotoMove(entity::Photo* photo, const double u, const double v, dureu::EVENT event);
    void editPhotoScale(entity::Photo* photo, const double u, const double v, dureu::EVENT event);
    void editPhotoRotate(entity::Photo* photo, const double u, const double v, dureu::EVENT event);
    void editPhotoFlip(entity::Photo* photo, bool horizontal);
    void editPhotoDelete(entity::Photo* photo);

    void editStrokesPush(osg::Camera *camera);
    void editStrokesMove(double u, double v, dureu::EVENT event);
    void editStrokesScale(double u, double v, dureu::EVENT event);
    void editStrokesRotate(double u, double v, dureu::EVENT event);
    void editStrokeDelete(entity::Stroke* stroke);

protected:
    ~RootScene();
    void printScene();

private:
    osg::ref_ptr<entity::UserScene> m_userScene;
    osg::ref_ptr<Axes> m_axisGlo;
    osg::ref_ptr<ObserveSceneCallback> m_observer;
    osg::ref_ptr<HUDCamera> m_hud;
    QUndoStack* m_undoStack;
    bool m_saved;
};

#endif // SCENE

