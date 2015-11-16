#include "iostream"
#include <sstream>

#include <osg/ref_ptr>
#include <osg/Group>
#include <osg/Node>
#include <osgDB/ReadFile>
#include "osg/MatrixTransform"
#include <osgUtil/SceneView>

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
    this->addCanvas(trans_xz, dureu::CANVAS_CLR_CURRENT);

    this->addCanvas(osg::Matrix::rotate(-dureu::PI*0.5, 0, 0, 1),
                    osg::Matrix::translate(0.f, dureu::CANVAS_MINW, 0.f),
                    dureu::CANVAS_CLR_PREVIOUS);

    this->addCanvas(osg::Matrix::rotate(-dureu::PI*0.5, 1, 0, 0),
                    osg::Matrix::translate(0.f, dureu::CANVAS_MINW, 0.f),
                    dureu::CANVAS_CLR_REST);

    this->addChild(_userScene.get());
}

RootScene::~RootScene(){}

void RootScene::setAxesVisible() // not working version
{
    const int imask = (int)osgUtil::SceneView::VariablesMask::ALL_VARIABLES;
    _axes->getChild(0)->setNodeMask(0xffffffff);
    _axes->setInheritanceMask(imask);
    _axes->setCullMask(0xffffffff);
    _axes->setNodeMask(0xffffffff);
}

void RootScene::setAxesInvisible() // not working version
{
    const int imask = (int)(osgUtil::SceneView::VariablesMask::ALL_VARIABLES &
                 ~osgUtil::SceneView::VariablesMask::CULL_MASK);
    _axes->getChild(0)->setNodeMask(0x02);
    _axes->setInheritanceMask(imask);
    _axes->setCullMask(0x04);
}

void RootScene::addCanvas(const osg::Matrix &R, const osg::Matrix &T, const osg::Vec4 &color){
    osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;
    transform->setMatrix(R*T); // left hand coordinate system, see OSG docs
    this->addCanvas(transform, color);
}

void RootScene::addCanvas(osg::ref_ptr<osg::MatrixTransform>& transform, const osg::Vec4f& color){
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
    // better way to deal with the name:
    // node->setName("Canvas1"); // as before
    // node->setUserValue("id", _idCanvas);
    // to retirive:
    // node->getUserValue("id", returnedStrings);
    // std::string name = node->getName();
    cnv->setName(dureu::CANVAS_NAME +
                 static_cast<std::ostringstream*>( &(std::ostringstream() << _idCanvas) )->str());
    _idCanvas++;
}
