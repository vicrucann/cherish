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
 * tries to delete the canvases directly from _userScene through the
 * API methods.
 */

#include <iostream>
#include <string>

#include <osg/ref_ptr>
#include <osg/observer_ptr>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Node>
#include <osg/Matrix>

#include "axes.h"
#include "canvas.h"
#include "settings.h"
#include "../libSGUtils/observescenecallback.h"
#include "../libGUI/hudcamera.h"

class RootScene : public osg::Group {
public:
    RootScene();
    ~RootScene();

    bool addUserScene();
    const osg::Group* getUserScene() const;

    void setAxesVisibility(bool vis);
    bool getAxesVisibility() const;
    bool addAxes();
    const Axes* getAxes() const;

    void setNameUserScene(const std::string& name);
    std::string getNameUserScene() const;

    const ObserveSceneCallback* getSceneObserver() const;

    bool addHudCamera();
    const HUDCamera* getHudCamera() const;
    bool setHudCameraObserve();
    void setHudCameraVisibility(bool vis);
    bool getHudCameraVisibility() const;

    Canvas* addCanvas();
    Canvas* addCanvas(const osg::Matrix& R, const osg::Matrix& T);
    Canvas* addCanvas(osg::MatrixTransform* transform);
    Canvas* addCanvas(Canvas* canvasCopy);
    bool deleteCanvas(const std::string& name);
    bool deleteCanvas(const int id);
    bool deleteCanvas(Canvas* cnv);

    bool addNode(osg::Node* node);
    bool deleteNode(const std::string& name);
    bool deleteNode(osg::Node* node);

    void addStroke(const osg::Vec3f& nearPoint, const osg::Vec3f& farPoint, int mouse);

    osg::Node *loadSceneFromFile(const std::string& fname);

    unsigned int getMaxCanvasId() const;
    unsigned int getMaxNodeId() const;

    Canvas* getCanvas(unsigned int id) const;
    Canvas* getCanvas(const std::string& name) const;

    osg::Node* getNode(unsigned int id) const;
    osg::Node* getNode(const std::string& name) const;

    bool setCanvasName(Canvas* cnv, const std::string& name);
    bool setNodeName(osg::Node* node, const std::string& name);

    bool setCanvasCurrent(Canvas* cnv);
    bool setCanvasPrevious(Canvas* cnv);
    Canvas* getCanvasCurrent() const;
    Canvas* getCanvasPrevious() const;

    unsigned int getCanvasLevel() const;

    void setTransformOffset(const osg::Vec3f& nearPoint, const osg::Vec3f& farPoint, const int mouse);

protected:
    void setCanvasName(Canvas* cnv);
    std::string getEntityName(const std::string& name, unsigned int id) const;
    bool setSceneObserver();
private:
    osg::ref_ptr<osg::Group> _userScene;
    osg::ref_ptr<Axes> _axes;
    osg::ref_ptr<ObserveSceneCallback> _observer;
    osg::ref_ptr<HUDCamera> _hud;
    osg::observer_ptr<Canvas> _canvasCurrent;
    osg::observer_ptr<Canvas> _canvasPrevious;
    unsigned int _idCanvas;
    unsigned int _idNode; // for misc entities
};

#endif // SCENE

