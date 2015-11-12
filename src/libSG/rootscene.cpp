#include "iostream"

#include <osg/ref_ptr>
#include <osg/Group>
#include <osg/Node>
#include <osgDB/ReadFile>
#include "osg/MatrixTransform"

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
    cnv_xz->setColor(dureu::CANVAS_CLR_CURRENT);
    _userScene->addChild(cnv_xz.get());

    osg::ref_ptr<Canvas> cnv_xy = new Canvas();
    cnv_xy->setColor(dureu::CANVAS_CLR_PREVIOUS);
    osg::ref_ptr<osg::MatrixTransform> trans_xy = new osg::MatrixTransform;
    trans_xy->setMatrix(osg::Matrix::rotate(-dureu::PI*0.5, 0, 0, 1));
    trans_xy->addChild(cnv_xy.get());
    _userScene->addChild(trans_xy.get());

    osg::ref_ptr<Canvas> cnv_yz = new Canvas();
    cnv_yz->setColor(dureu::CANVAS_CLR_REST);
    osg::ref_ptr<osg::MatrixTransform> trans_yz = new osg::MatrixTransform;
    trans_yz->setMatrix(osg::Matrix::rotate(-dureu::PI*0.5, 1, 0, 0));
    trans_yz->addChild(cnv_yz.get());
    _userScene->addChild(trans_yz.get());

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
