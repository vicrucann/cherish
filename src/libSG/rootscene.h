#ifndef ROOTSCENE
#define ROOTSCENE

#include <iostream>
#include <string>

#include <osg/ref_ptr>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Node>
#include <osg/Matrix>

#include "axes.h"
#include "canvas.h"
#include "settings.h"

class RootScene : public osg::Group {
public:
    RootScene();
    ~RootScene();

    void setAxesVisible(); // replace by one function: setAxesVisibility(bool)
    void setAxesInvisible();

    void addCanvas(const osg::Matrix& R, const osg::Matrix& T, const osg::Vec4& color = dureu::CANVAS_CLR_CURRENT);
    void addCanvas(osg::ref_ptr<osg::MatrixTransform>& transform, const osg::Vec4f &color = dureu::CANVAS_CLR_CURRENT);
    bool deleteCanvas(const std::string name);
    bool deleteCanvas(const int id);
    bool deleteCanvas(Canvas* cnv);

    bool loadSceneFromFile(const std::string fname);
private:
    void setCanvasName(osg::ref_ptr<Canvas> &cnv);

    osg::ref_ptr<osg::Group> _userScene;
    osg::ref_ptr<Axes> _axes;
    unsigned int _idCanvas;
};

#endif // SCENE

