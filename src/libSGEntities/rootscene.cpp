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
    _idNode(0),
    _observer(new ObserveSceneCallback),
    _hud(new HUDCamera(dureu::HUD_LEFT, dureu::HUD_RIGHT, dureu::HUD_BOTTOM, dureu::HUD_TOP)),
    _canvasCurrent(0),
    _canvasPrevious(0)
{
    this->initialize();
}

RootScene::~RootScene(){}

void RootScene::initialize(){
    _userScene->setName("UserScene");

    osg::ref_ptr<osg::MatrixTransform> trans_xz = new osg::MatrixTransform;
    trans_xz->setMatrix(osg::Matrix::identity());
    Canvas* cnv0 = this->addCanvas(trans_xz);

    Canvas* cnv1 = this->addCanvas(osg::Matrix::rotate(-dureu::PI*0.5, 0, 0, 1),
                    osg::Matrix::translate(0.f, dureu::CANVAS_MINW, 0.f));

    this->addCanvas(osg::Matrix::rotate(-dureu::PI*0.5, 1, 0, 0),
                    osg::Matrix::translate(0.f, dureu::CANVAS_MINW, 0.f));
    this->setCanvasCurrent(cnv0);
    this->setCanvasPrevious(cnv1);

    this->addChild(_userScene.get());
    this->addChild(_axes.get());

    this->setSceneObserver();
    _hud->addChild(_observer->getTextGeode());
    this->addChild(_hud.get());
    this->setName("RootScene");

    // to test the rename of canvas:
    //this->setCanvasName(getCanvas(0)); // Canvas0 should be renamed to Canvas3
}

void RootScene::setAxesVisibility(bool vis) {
    _axes->setVisibility(vis);
}

std::string RootScene::getNameUserScene() const{
    return _userScene->getName();
}

Canvas *RootScene::addCanvas(){
    osg::MatrixTransform* transform = new osg::MatrixTransform;
    transform->setMatrix(osg::Matrix::identity());
    return addCanvas(transform);
}

Canvas *RootScene::addCanvas(const osg::Matrix &R, const osg::Matrix &T){
    std::cout << "  RootScene->addCanvas(R,T, color)" << std::endl;
    osg::MatrixTransform* transform = new osg::MatrixTransform;
    transform->setMatrix(R*T); // left hand coordinate system, see OSG docs
    return this->addCanvas(transform);
}

Canvas* RootScene::addCanvas(osg::MatrixTransform* transform){
    std::cout << "  RootScene->addCanvas(transform)" << std::endl;
    Canvas* cnv = new Canvas(transform, getEntityName(dureu::NAME_CANVAS, _idCanvas++));
    this->setCanvasCurrent(cnv);
    bool success = _userScene->addChild(cnv);
    if (!success){
        std::cerr << "Could not add a canvas as a child of _userScene" << std::endl;
        cnv = 0;
    }
    return cnv;
}

Canvas *RootScene::addCanvas(Canvas *canvasCopy){
    osg::MatrixTransform* transform = canvasCopy->getTransform();
    //canvasCopy->setColor(dureu::CANVAS_CLR_PREVIOUS);
    // make offset along the normal
    return this->addCanvas(transform);
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
    std::cout << "--RootScene->deleteCanvas(int)" << std::endl;
    Canvas* cnv = this->getCanvas(id);
    if (!cnv){
        std::cerr << "--the canvas pointer is NULL" << std::endl;
        return false;
    }
    return deleteCanvas(cnv);
}

bool RootScene::deleteCanvas(Canvas *cnv){
    std::cout << "--RootScene->deleteCanvas(Canvas*)" << std::endl;
    if (cnv == this->getCanvasCurrent()){
        std::cout << "--current camera is set to previous, to name: " << this->getCanvasCurrent()->getName() << std::endl;
        this->setCanvasCurrent(this->getCanvasPrevious());
    }
    if (cnv == this->getCanvasPrevious() || getCanvasCurrent() == getCanvasCurrent()){
        for (unsigned int i=0;i<this->getMaxCanvasId();++i){
            Canvas* cnvi = this->getCanvas(i);
            if (cnvi != NULL && cnvi != this->getCanvasCurrent() && cnvi != cnv){
                this->setCanvasPrevious(cnvi);
                std::cout << "--previous canvas is set to name: " << this->getCanvasPrevious()->getName() << std::endl;
                break;
            }
        }
    }
    return deleteNode(cnv);
}

// we return bool and not osg::Node like it is done for the Canvas type
// because we already pass pointer on that node
bool RootScene::addNode(osg::Node *node){
    std::cout << "RootScene->addNode(Node*)" << std::endl;
    node->setName(getEntityName(dureu::NAME_ENTITY, _idNode++));
    return _userScene->addChild(node);
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

osg::Node* RootScene::loadSceneFromFile(const std::string& fname){
    std::cout << "--RootScene->loadSceneFromFile(string)" << std::endl;
    osg::Node* node = osgDB::readNodeFile(fname);
    if (!node){
        std::cerr << "--File could not be loaded: " << fname << std::endl;
        return 0;
    }
    std::cout << "--Trying to load node with ptr: " << node << std::endl;
    this->addNode(node);
    return node;
}

unsigned int RootScene::getMaxCanvasId() const {
    return _idCanvas;
}

unsigned int RootScene::getMaxNodeId() const{
    return _idNode;
}

Canvas *RootScene::getCanvas(unsigned int id) const {
    return getCanvas(getEntityName(dureu::NAME_CANVAS, id));
}

Canvas *RootScene::getCanvas(const std::string &name) const{
    std::cout << "--RootScene->getCanvas(string)" << std::endl;
    FindNodeVisitor fnv(name);
    _userScene->accept(fnv);
    if (fnv.getNode() == NULL){
        std::cout << "--No entity with such name found: " << name << std::endl;
        return NULL;
    }
    return dynamic_cast<Canvas*>(fnv.getNode());
}

osg::Node *RootScene::getNode(unsigned int id) const{
    return getNode(getEntityName(dureu::NAME_ENTITY, id));
}

osg::Node *RootScene::getNode(const std::string &name) const
{
    std::cout << "--RootScene->getNode(string)" << std::endl;
    FindNodeVisitor fnv(name);
    _userScene->accept(fnv);
    if (fnv.getNode() == NULL){
        std::cerr << "--No entity with such name found: " << name << std::endl;
        return NULL;
    }
    return fnv.getNode();
}

bool RootScene::setCanvasName(Canvas *cnv, const std::string &name)
{
    std::cout << "--RootScene->setCanvasName(Canvas*, string&)" << std::endl;
    return setNodeName(cnv, name);
}

bool RootScene::setNodeName(osg::Node *node, const std::string &name)
{
    std::cout << "  RootScene->setNodeName(osg::Node*, string&)" << std::endl;
    unsigned int idx = _userScene->getChildIndex(node);
    if (_userScene->getNumChildren()<=idx){
        std::cout << "The entity of given name is not direct child of the _userScene" << std::endl;
        std::cout << "The name was not setup" << std::endl;
        return false;
    }
    for (unsigned int i = 0; i<_userScene->getNumChildren(); ++i){
        if (_userScene->getChild(i)->getName() == name){
            std::cout << "The requested name is already in use by another entity" << std::endl;
            std::cout << "The name was not setup" << std::endl;
            return false;
        }
    }
    node->setName(name);
    std::cout << "The new name is " << node->getName() << std::endl;
    return true;
}

bool RootScene::setCanvasCurrent(Canvas *cnv)
{
    // if canvasCurr and canvasPrev are equal, search for the nearest
    // valiable candidate to assign the previous to
    // if no canvases available at all, the observer ptrs are set to NULL
    if (cnv == _canvasCurrent.get())
        return true;
    if (_canvasCurrent.valid()){
        if (_canvasPrevious.valid()){
            _canvasPrevious->setColor(dureu::CANVAS_CLR_REST);
            _canvasPrevious = NULL;
        }
        _canvasCurrent->setColor(dureu::CANVAS_CLR_PREVIOUS);
        _canvasPrevious = _canvasCurrent;
        _canvasCurrent = NULL;
    }
    if (!cnv){
        std::cout << "The input canvas pointer is NULL, no current canvas is assigned" << std::endl;
        return false;
    }
    _canvasCurrent = cnv;
    _canvasCurrent->setColor(dureu::CANVAS_CLR_CURRENT);
    return true;
}

bool RootScene::setCanvasPrevious(Canvas *cnv)
{
    if (cnv == _canvasPrevious.get())
        return true;
    if (_canvasPrevious.valid()){
        _canvasPrevious->setColor(dureu::CANVAS_CLR_REST);
        _canvasPrevious = NULL;
    }
    if (!cnv){
        std::cerr << "The input canvas pointed is not valid" << std::endl;
        return false;
    }
    _canvasPrevious = cnv;
    _canvasPrevious->setColor(dureu::CANVAS_CLR_PREVIOUS);
    return true;
}

Canvas *RootScene::getCanvasCurrent() const{
    return _canvasCurrent.get();
}

Canvas *RootScene::getCanvasPrevious() const{
    return _canvasPrevious.get();
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
    sprintf_s(buffer, sizeof(buffer), "%d", id);  // replace back to snprintf in final
    //itoa(id, buffer, 10);
    return name + std::string(buffer);//std::to_string(static_cast<long double>(id));
}

bool RootScene::setSceneObserver() {
    _observer->setScenePointer(_userScene);
    _userScene->addUpdateCallback(_observer.get());
    return true;
}

