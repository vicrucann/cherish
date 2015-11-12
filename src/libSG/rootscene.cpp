#include "iostream"
#include <sstream>

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
                   osg::Vec3(0.0f,0.0f,dureu::AXES_SIZE))),
    _idCanvas(0)
{
    this->addChild(_axes.get());

    osg::ref_ptr<osg::MatrixTransform> trans_xz = new osg::MatrixTransform;
    trans_xz->setMatrix(osg::Matrix::identity());
    this->addCanvas(dureu::CANVAS_CLR_CURRENT, trans_xz);

    osg::ref_ptr<osg::MatrixTransform> trans_xy = new osg::MatrixTransform;
    trans_xy->setMatrix(osg::Matrix::rotate(-dureu::PI*0.5, 0, 0, 1) * osg::Matrix::translate(0.f, dureu::CANVAS_MINW, 0.f));
    this->addCanvas(dureu::CANVAS_CLR_PREVIOUS, trans_xy);

    osg::ref_ptr<osg::MatrixTransform> trans_yz = new osg::MatrixTransform;
    trans_yz->setMatrix(osg::Matrix::rotate(-dureu::PI*0.5, 1, 0, 0) * osg::Matrix::translate(0.f, dureu::CANVAS_MINW, 0.f));
    this->addCanvas(dureu::CANVAS_CLR_REST, trans_yz);

    this->addChild(_userScene.get());
}

RootScene::~RootScene(){}

void RootScene::addCanvas(const osg::Vec4f& color, osg::ref_ptr<osg::MatrixTransform>& transform){
    osg::ref_ptr<Canvas> cnv = new Canvas();
    cnv->setColor(color);
    this->setCanvasName(cnv);
    transform->addChild(cnv.get());
    _userScene->addChild(transform.get());
}

bool RootScene::loadSceneFromFile(const std::string fname){
    osg::ref_ptr<osg::Node> snode = osgDB::readNodeFile(fname);
    if (!snode){
        return false;
    }
    _userScene->addChild(snode.get());
    return true;
}

void RootScene::setCanvasName(osg::ref_ptr<Canvas>& cnv){
    // name = Canvas1, Canvas2, ... where number is defined by _idCanvas
    cnv->setName(dureu::CANVAS_NAME +
                 static_cast<std::ostringstream*>( &(std::ostringstream() << _idCanvas) )->str());
    _idCanvas++;
}
