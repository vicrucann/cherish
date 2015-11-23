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

#include "observescenecallback.h"

RootScene::RootScene():
    _userScene(new osg::Group),
    _axes(new Axes),
    _idCanvas(0)
{
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
    this->addChild(_axes.get());

    this->setSceneObserver();
}

RootScene::~RootScene(){}

void RootScene::setAxesVisibility(bool vis) {
    _axes->setVisibility(vis);
}

void RootScene::addCanvas(const osg::Matrix &R, const osg::Matrix &T, const osg::Vec4 &color){
    std::cout << "  RootScene->addCanvas(R,T, color)" << std::endl;
    osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;
    transform->setMatrix(R*T); // left hand coordinate system, see OSG docs
    this->addCanvas(transform, color);
}

void RootScene::addCanvas(osg::ref_ptr<osg::MatrixTransform>& transform, const osg::Vec4f& color){
    std::cout << "  RootScene->addCanvas(transform, color)" << std::endl;
    osg::ref_ptr<Canvas> cnv = new Canvas(transform);
    cnv->setColor(color);
    this->setCanvasName(cnv);
    _userScene->addChild(cnv.get());
}

bool RootScene::deleteCanvas(const std::string name){
    std::cout << "  RootScene->deleteCanvas(string)" << std::endl;
    Canvas* cnv = this->getCanvas(name);
    if (!cnv){
        std::cerr << "The canvas pointer is NULL" << std::endl;
        return false;
    }
    return deleteCanvas(cnv);
}

bool RootScene::deleteCanvas(const int id){
    std::cout << "  RootScene->deleteCanvas(int)" << std::endl;
    Canvas* cnv = this->getCanvas(id);
    if (!cnv){
        std::cerr << "The canvas pointer is NULL" << std::endl;
        return false;
    }
    return deleteCanvas(cnv);
}

bool RootScene::deleteCanvas(Canvas *cnv){
    std::cout << "  RootScene->deleteCanvas(Canvas*): " << cnv->getName();
    if (!cnv){
        std::cerr << "The canvas pointer is NULL" << std::endl;
        return true;
    }
    bool success = _userScene->removeChild(cnv);
    if (success) std::cout << " success" << std::endl;
    else std::cout << " failure" << std::endl;
    return success;
}

bool RootScene::loadSceneFromFile(const std::string fname){
    std::cout << "RootScene->loadSceneFromFile(string)" << std::endl;
    osg::ref_ptr<osg::Node> snode = osgDB::readNodeFile(fname);
    if (!snode){
        std::cerr << "File could not be loaded: " << fname << std::endl;
        return false;
    }
    _userScene->addChild(snode.get());
    return true;
}

unsigned int RootScene::getMaxCanvasId() const {
    return _idCanvas;
}

Canvas *RootScene::getCanvas(unsigned int id) const {
    return getCanvas(getEntityName(dureu::NAME_CANVAS, id));
}

Canvas *RootScene::getCanvas(const std::string name) const{
    std::cout << "  RootScene->getCanvas(string)" << std::endl;
    FindNodeVisitor fnv(name);
    _userScene->accept(fnv);
    if (fnv.getNode() == NULL){
        std::cerr << "No entity with such name found: " << name << std::endl;
        return NULL;
    }
    return dynamic_cast<Canvas*>(fnv.getNode());
}

void RootScene::setCanvasName(osg::ref_ptr<Canvas>& cnv){
    cnv->setName(getEntityName(dureu::NAME_CANVAS, _idCanvas++));
    std::cout << "  New Canvas created: " << cnv->getName() << std::endl;
}

std::string RootScene::getEntityName(const std::string &name, unsigned int id) const{
    return name + std::to_string(static_cast<long double>(id));
}

bool RootScene::setSceneObserver() {
    osg::ref_ptr<ObserveSceneCallback> observer = new ObserveSceneCallback;
    observer->setScenePointer(_userScene);
    _userScene->addUpdateCallback(observer.get());
    return true;
}
