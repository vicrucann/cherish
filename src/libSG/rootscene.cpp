#include "iostream"

#include <osg/ref_ptr>
#include <osg/Group>
#include <osg/Node>
#include <osgDB/ReadFile>

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
    osg::ref_ptr<Canvas> cnv_xz = new Canvas();
    _userScene->addChild(cnv_xz.get());

    this->addChild(_userScene.get());
}

RootScene::~RootScene(){}

bool RootScene::loadSceneFromFile(const std::string fname){
    osg::ref_ptr<osg::Node> snode = osgDB::readNodeFile(fname);
    if (!snode){
        return false;
    }
    _userScene->addChild(snode.get());
    return true;
}
