#include "iostream"
#include <sstream>

#include <osg/ref_ptr>
#include <osg/Group>
#include <osg/Node>
#include <osgDB/ReadFile>
#include "osg/MatrixTransform"
#include <osgUtil/SceneView>
#include <osg/ValueObject>
#include <osg/Switch>

#include "rootscene.h"
#include "axes.h"
#include "settings.h"
#include "canvas.h"
#include "findnodevisitor.h"

RootScene::RootScene():
    _userScene(new osg::Group),
    _axesSwitch(new osg::Switch),
    _axes(new Axes(osg::Vec3(0.0f,0.0f,0.0f),
                   osg::Vec3(dureu::AXES_SIZE,0.0f,0.0f),
                   osg::Vec3(0.0f,dureu::AXES_SIZE,0.0f),
                   osg::Vec3(0.0f,0.0f,dureu::AXES_SIZE))),
    _idCanvas(0)
{
    // Just to show an example of how the axis type should be used
    // The switch is so that to have possibility to chose whether to
    // render the axis or not
    _axesSwitch->addChild(_axes.get(), true);
    this->addChild(_axesSwitch.get());
    //this->addChild(_axes.get());

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

void RootScene::setAxesVisibility(bool vis)
{
    _axesSwitch->setChildValue(_axes.get(), vis);
}

void RootScene::addCanvas(const osg::Matrix &R, const osg::Matrix &T, const osg::Vec4 &color){
    osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;
    transform->setMatrix(R*T); // left hand coordinate system, see OSG docs
    this->addCanvas(transform, color);
}

// The tree branch will always look like this:
// Toot -> Transform -> Canvas
// So whenever trying to deleteCanvas, it is actually necessary to delete
// its parental transform only (when using removeChild() from root)
void RootScene::addCanvas(osg::ref_ptr<osg::MatrixTransform>& transform, const osg::Vec4f& color){
    osg::ref_ptr<Canvas> cnv = new Canvas();
    cnv->setColor(color);
    this->setCanvasName(cnv);
    transform->addChild(cnv.get());
    _userScene->addChild(transform.get());
}

bool RootScene::deleteCanvas(const std::string name)
{
    findNodeVisitor fnv(name);
    _userScene->accept(fnv);
    return deleteCanvas(dynamic_cast<Canvas*>(fnv.getNode()));
}

bool RootScene::deleteCanvas(const int id)
{
    return deleteCanvas(dureu::NAME_CANVAS + std::to_string(static_cast<long double>(id)));
}

bool RootScene::deleteCanvas(Canvas *cnv)
{
    std::cout << "Trying to delete canvas with name " << cnv->getName();
    if (!cnv){
        std::cerr << "The canvas pointer is NULL" << std::endl;
        return true;
    }
    // delete the parental transform and the child canvas
    // will be deleted automatically
    bool success = _userScene->removeChild(cnv->getParent(0));
    if (success)
        std::cout << " success" << std::endl;
    else
        std::cout << " failure" << std::endl;
    return success;
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
    cnv->setName(dureu::NAME_CANVAS + std::to_string(static_cast<long double>(_idCanvas++)));
    std::cout << "New Canvas created: " << cnv->getName() << std::endl;
}
