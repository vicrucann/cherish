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
#include "settings.h"
#include "../libSGUtils/observescenecallback.h"
#include "../libGUI/hudcamera.h"
#include "UserScene.h"

#include <QUndoStack>

class AddStrokeCommand;

class RootScene : public osg::Group {
public:
    RootScene(QUndoStack* undoStack);

    entity::UserScene* getUserScene() const;

    void setAxesVisibility(bool vis);
    bool getAxesVisibility() const;
    const Axes* getAxes() const;

    const ObserveSceneCallback* getSceneObserver() const;
    const HUDCamera* getHudCamera() const;
    void setHudCameraVisibility(bool vis);
    bool getHudCameraVisibility() const;

    bool writeScenetoFile();
    bool loadSceneFromFile();

protected:
    ~RootScene();

private:
    osg::ref_ptr<entity::UserScene> m_userScene;
    osg::ref_ptr<Axes> m_axisGlo;
    osg::ref_ptr<ObserveSceneCallback> m_observer;
    osg::ref_ptr<HUDCamera> m_hud;
};

#endif // SCENE

