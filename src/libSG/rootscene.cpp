#include "iostream"

#include <osg/Group>

#include "rootscene.h"
#include "axes.h"
#include "settings.h"
#include "canvas.h"

RootScene::RootScene():
    _userScene(new osg::Group),
    _axes(new Axes(osg::Vec3(0.0f,0.0f,0.0f),
                   osg::Vec3(dureu::AXES_SIZE,0.0f,0.0f),
                   osg::Vec3(0.0f,dureu::AXES_SIZE,0.0f),
                   osg::Vec3(0.0f,0.0f,dureu::AXES_SIZE)))
{
    this->addChild(_axes.get());
    osg::ref_ptr<Canvas> cnv_xz = new Canvas(osg::Vec3(1.0f,0.0f,1.0f),
                                             osg::Vec3(dureu::CANVAS_MINW, 0.0f, dureu::CANVAS_MINH),
                                             osg::Vec3(-dureu::CANVAS_MINW,0.0f, dureu::CANVAS_MINH),
                                             dureu::CANVAS_CLR_CURRENT);
    _userScene->addChild(cnv_xz.get());

    this->addChild(_userScene.get());
}

RootScene::~RootScene(){}
