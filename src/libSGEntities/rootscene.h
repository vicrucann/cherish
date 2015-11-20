#ifndef ROOTSCENE
#define ROOTSCENE

/* RootScene
 * It contains all the entities that will appear on the scene.
 * It includes both user scene and default entities (axis).
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
 */

#include <iostream>
#include <string>

#include <osg/ref_ptr>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Node>
#include <osg/Matrix>
#include <osg/Switch>

#include "axes.h"
#include "canvas.h"
#include "settings.h"
//#include "observescenecallback.h"

class RootScene : public osg::Group {
public:
    RootScene();
    ~RootScene();

    void setAxesVisibility(bool vis);

    void addCanvas(const osg::Matrix& R, const osg::Matrix& T, const osg::Vec4& color = dureu::CANVAS_CLR_CURRENT);
    void addCanvas(osg::ref_ptr<osg::MatrixTransform>& transform, const osg::Vec4f &color = dureu::CANVAS_CLR_CURRENT);
    bool deleteCanvas(const std::string name);
    bool deleteCanvas(const int id);
    bool deleteCanvas(Canvas* cnv);

    bool loadSceneFromFile(const std::string fname);
private:
    void setCanvasName(osg::ref_ptr<Canvas> &cnv);

    osg::ref_ptr<osg::Group> _userScene;
    osg::ref_ptr<osg::Switch> _axesSwitch;
    osg::ref_ptr<Axes> _axes;
    unsigned int _idCanvas;
    //osg::ref_ptr<ObserveSceneCallback> _observer;
};

#endif // SCENE

