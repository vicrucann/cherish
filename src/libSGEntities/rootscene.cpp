#include "iostream"
#include <sstream>
#include <stdlib.h>

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
    _axes(new Axes),
    _idCanvas(0),
    _observer(new ObserveSceneCallback),
    _hud(new HUDCamera(dureu::HUD_LEFT, dureu::HUD_RIGHT, dureu::HUD_BOTTOM, dureu::HUD_TOP))
{
    _userScene->setName("UserScene");

    osg::ref_ptr<osg::MatrixTransform> trans_xz = new osg::MatrixTransform;
    trans_xz->setMatrix(osg::Matrix::identity());
    this->addCanvas(trans_xz, dureu::CANVAS_CLR_REST);

    this->addCanvas(osg::Matrix::rotate(-dureu::PI*0.5, 0, 0, 1),
                    osg::Matrix::translate(0.f, dureu::CANVAS_MINW, 0.f),
                    dureu::CANVAS_CLR_REST);

    this->addCanvas(osg::Matrix::rotate(-dureu::PI*0.5, 1, 0, 0),
                    osg::Matrix::translate(0.f, dureu::CANVAS_MINW, 0.f),
                    dureu::CANVAS_CLR_REST);

    this->addChild(_userScene.get());
    this->addChild(_axes.get());

    this->setSceneObserver();
    _hud->addChild(_observer->getTextGeode());
    this->addChild(_hud.get());
    this->setName("RootScene");

    // to test the rename of canvas:
    //this->setCanvasName(getCanvas(0)); // Canvas0 should be renamed to Canvas3
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
    osg::ref_ptr<Canvas> cnv = new Canvas(transform, getEntityName(dureu::NAME_CANVAS, _idCanvas++));
    cnv->setColor(color);
    _userScene->addChild(cnv.get());
}

bool RootScene::deleteCanvas(const std::string &name){
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
    return deleteNode(cnv);
}

bool RootScene::deleteNode(const std::string &name)
{
    std::cout << "  RootScene->deleteNode(string): " << name << std::endl;
    osg::Node* node = this->getNode(name);
    std::cout << "Node child idx: " << _userScene->getChildIndex(node) << std::endl;
    if (!node){
        std::cerr << "The node pointer is NULL" << std::endl;
        return false;
    }
    return deleteNode(node);
}

bool RootScene::deleteNode(osg::Node *node)
{
    std::cout << "  RootScene->deleteNode(osg::Node*): " << node->getName() << std::endl;
    if (!node){
        std::cerr << "The node pointer is NULL" << std::endl;
        return true;
    }
    std::cout << "Trying to delete node with ptr: " << node << std::endl;
    std::cout << " and name: " << node->getName() << std::endl;
    //bool success = _userScene->removeChild(tn);
    bool success = _userScene->removeChild(node);
    if (success) std::cout << "Success on removeChild(osg::Node*)" << std::endl;
    else std::cerr << "Failure to removeChild(osg::Node*)" << std::endl;
    return success;
}

bool RootScene::loadSceneFromFile(const std::string& fname, const std::string& name){
    std::cout << "  RootScene->loadSceneFromFile(string)" << std::endl;
    osg::Node* node = osgDB::readNodeFile(fname);
    //osg::ref_ptr<osg::Node> snode = osgDB::readNodeFile(fname);
    if (!node){
        std::cerr << "File could not be loaded: " << fname << std::endl;
        return false;
    }
    std::cout << "Trying to load node with ptr: " << node << std::endl;
    bool success = _userScene->addChild(node);
    this->setNodeName(node, name);
    std::cout << "New _userSceme child idx is: " << _userScene->getChildIndex(node) << std::endl;
    return success;
}

unsigned int RootScene::getMaxCanvasId() const {
    return _idCanvas;
}

Canvas *RootScene::getCanvas(unsigned int id) const {
    return getCanvas(getEntityName(dureu::NAME_CANVAS, id));
}

Canvas *RootScene::getCanvas(const std::string &name) const{
    std::cout << "  RootScene->getCanvas(string)" << std::endl;
    FindNodeVisitor fnv(name);
    _userScene->accept(fnv);
    if (fnv.getNode() == NULL){
        std::cerr << "No entity with such name found: " << name << std::endl;
        return NULL;
    }
    return dynamic_cast<Canvas*>(fnv.getNode());
}

osg::Node *RootScene::getNode(const std::string &name) const
{
    std::cout << "  RootScene->getNode(string)" << std::endl;
    FindNodeVisitor fnv(name);
    _userScene->accept(fnv);
    if (fnv.getNode() == NULL){
        std::cerr << "No entity with such name found: " << name << std::endl;
        return NULL;
    }
    return fnv.getNode();
}

void RootScene::setNodeName(osg::Node *node, const std::string &name)
{
    std::cout << "  RootScene->setNodeName(osg::Node*, string&)" << std::endl;
    unsigned int idx = _userScene->getChildIndex(node);
    if (_userScene->getNumChildren()<=idx){
        std::cerr << "The entity of given name is not direct child of the _userScene" << std::endl;
        std::cerr << "The name was not setup" << std::endl;
        return;
    }
    node->setName(name);
    std::cout << "The new name is " << node->getName() << std::endl;
}

void RootScene::setCanvasName(Canvas *cnv){
    cnv->setName(getEntityName(dureu::NAME_CANVAS, _idCanvas++));
    std::cout << "  Canvas renamed: " << cnv->getName() << std::endl;
    cnv->setSwitchName(cnv->getName());
    cnv->setTransformName(cnv->getName());
    cnv->setGeodeName(cnv->getName());
    cnv->setGeometryName(cnv->getName());
}

std::string RootScene::getEntityName(const std::string &name, unsigned int id) const{
    char buffer[10];
    itoa(id, buffer, 10);
    return name + std::string(buffer);//std::to_string(static_cast<long double>(id));
}

bool RootScene::setSceneObserver() {
    _observer->setScenePointer(_userScene);
    _userScene->addUpdateCallback(_observer.get());
    return true;
}

