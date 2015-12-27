#include "iostream"
#include <sstream>
#include <stdlib.h>

#include <osg/ref_ptr>
#include <osg/Group>
#include <osg/Node>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include "osg/MatrixTransform"
#include <osgUtil/SceneView>
#include <osg/ValueObject>
#include <osg/Switch>

#include "rootscene.h"
#include "axes.h"
#include "settings.h"
#include "canvas.h"
#include "photo.h"
#include "findnodevisitor.h"
#include "AddGroupCommand.h"

RootScene::RootScene(QUndoStack *undoStack)
    : _userScene(new osg::Group)
    , _axes(new Axes)
    , _observer(new ObserveSceneCallback)
    , _hud(new HUDCamera(dureu::HUD_LEFT, dureu::HUD_RIGHT, dureu::HUD_BOTTOM, dureu::HUD_TOP))
    , _canvasCurrent(0)
    , _canvasPrevious(0)
    , _idCanvas(0)
    , _idNode(0)
    , _undoStack(undoStack)
{
    osg::ref_ptr<osg::MatrixTransform> trans_i = new osg::MatrixTransform;
    trans_i->setMatrix(osg::Matrix::translate(0.f, dureu::CANVAS_MINW*0.5f, 0.f) );
    //Canvas* cnv0 = this->addCanvas(trans_xz);
    Canvas* cnv0 = this->addCanvas(osg::Matrix::rotate(dureu::PI*0.5, 1, 0, 0),
                                   osg::Matrix::translate(0.f, 0.f, 0.f));

    Canvas* cnv1 = this->addCanvas(osg::Matrix::rotate(-dureu::PI*0.5, 0, 1, 0),
                    osg::Matrix::translate(0.f, dureu::CANVAS_MINW*0.5f, 0.f));

    this->addCanvas(trans_i);

    this->setCanvasCurrent(cnv0);
    this->setCanvasPrevious(cnv1);

    this->addUserScene(); // child #0
    this->addAxes(); // child #1
    this->setSceneObserver();
    this->addHudCamera();
    this->setHudCameraObserve(); // child #2
    this->setName("RootScene");
}

RootScene::~RootScene(){}

bool RootScene::addUserScene()
{
    if (!_userScene){
         std::cerr << "addUserScene(): UserScene pointer is NULL" << std::endl;
        return false;
    }
    for (unsigned int i=0; i<this->getNumChildren(); ++i){
        Node* node = this->getChild(i);
        if (node == _userScene){
             std::cerr << "addUserScene(): UserScene is already a child of the scene" << std::endl;
            return false;
        }
    }
    _userScene->setName("UserScene");
    return this->addChild(_userScene.get());
}

const osg::Group *RootScene::getUserScene() const{
    return _userScene.get();
}

void RootScene::setAxesVisibility(bool vis) {
    _axes->setVisibility(vis);
}

bool RootScene::getAxesVisibility() const{
    return _axes->getVisibility();
}

bool RootScene::addAxes(){
    if (!_axes.get()){
         std::cerr << "addAxes(): Axes pointer is NULL" << std::endl;
        return false;
    }
    for (unsigned int i=0; i<this->getNumChildren(); ++i){
        Node* node = this->getChild(i);
        if (node == _axes.get()){
             std::cerr << "addAxes(): Global axes is already on the scene" << std::endl;
            return false;
        }
    }
    _axes->setName("Axes");
    return this->addChild(_axes.get());
}

const Axes* RootScene::getAxes() const{
    return _axes.get();
}

void RootScene::setNameUserScene(const std::string &name){
    _userScene->setName(name);
}

std::string RootScene::getNameUserScene() const{
    return _userScene->getName();
}

const ObserveSceneCallback *RootScene::getSceneObserver() const{
    return _observer.get();
}

bool RootScene::addHudCamera(){
    if (!_hud.get()){
         std::cerr << "addHudCamera(): HUD is null" << std::endl;
        return false;
    }
    for (unsigned int i=0; i<this->getNumChildren(); ++i){
        Node* node = this->getChild(i);
        if (node == _hud.get()){
             std::cerr << "addHudCamera(): HUD is already on the scene" << std::endl;
            return false;
        }
    }
    _hud->setName("HUDCamera");
    return this->addChild(_hud.get());
}

const HUDCamera *RootScene::getHudCamera() const{
    return _hud.get();
}

bool RootScene::setHudCameraObserve(){
    if (!_observer.get()){
         std::cerr << "setHudCameraObserve(): Observer is empty" << std::endl;
        return false;
    }
    if (!_hud.get()){
         std::cerr << "setHudCameraObserve(): HUD is NULL" << std::endl;
        return false;
    }
    return _hud->setText(_observer->getTextGeode());
}

void RootScene::setHudCameraVisibility(bool vis){
    _hud->setVisibility(vis);
}

bool RootScene::getHudCameraVisibility() const{
    return _hud->getVisibility();
}

Canvas *RootScene::addCanvas(){
    osg::MatrixTransform* transform = new osg::MatrixTransform;
    transform->setMatrix(osg::Matrix::identity());
    return addCanvas(transform);
}

Canvas *RootScene::addCanvas(const osg::Matrix &R, const osg::Matrix &T){
    osg::MatrixTransform* transform = new osg::MatrixTransform;
    transform->setMatrix(R*T); // left hand coordinate system, see OSG docs
    return this->addCanvas(transform);
}

// base addCanvas which calls for AddGroupCommand()
Canvas* RootScene::addCanvas(osg::MatrixTransform* transform){
    outLogMsg("addCanvas(*transform)");
    Canvas* cnv = new Canvas(transform, getEntityName(dureu::NAME_CANVAS, _idCanvas++));
    QUndoCommand* cmd = new AddGroupCommand(this, cnv);
    if (!_undoStack){
        outErrMsg("undo stack is not initalized");
        return cnv;
    }
    _undoStack->push(cmd);
    /*this->setCanvasCurrent(cnv);
    bool success = _userScene->addChild(cnv);
    if (!success){
        std::cerr << "addCanvas(): Could not add a canvas as a child of _userScene" << std::endl;
        cnv = 0;
    }*/
    return cnv;
}

// this is a debug version of addCanvas, and it is not supposed
// to be called from anywhere, but from addGroupCommand()
// later this should be removed at all and the inside code moved to
// addGroupCommand() class
bool RootScene::addCanvas(Canvas *cnv){
    outLogMsg("addCanvas(): Canvas*");
    if (!cnv){
        outErrMsg("Canvas pointer is NULL");
        return false;
    }
    outLogVal("Trying to add canvas with name", cnv->getName());
    this->setCanvasCurrent(cnv);
    return _userScene->addChild(cnv);
}

bool RootScene::deleteCanvas(const std::string &name){
    std::cout << "deleteCanvas(): (string&)" << std::endl;
    Canvas* cnv = this->getCanvas(name);
    if (!cnv){
        std::cerr << "deleteCanvas(): The canvas pointer is NULL" << std::endl;
        return false;
    }
    return deleteCanvas(cnv);
}

bool RootScene::deleteCanvas(const int id){
    std::cout << "deleteCanvas(): (int)" << std::endl;
    Canvas* cnv = this->getCanvas(id);
    if (!cnv){
        std::cerr << "deleteCanvas(): the canvas pointer is NULL" << std::endl;
        return false;
    }
    return deleteCanvas(cnv);
}

bool RootScene::deleteCanvas(Canvas *cnv){
    std::cout << "deleteCanvas(): (Canvas*)" << std::endl;
    if (cnv == this->getCanvasCurrent()){
        std::cout << "deleteCanvas(): current canvas is set to previous, to name: " << this->getCanvasCurrent()->getName() << std::endl;
        this->setCanvasCurrent(this->getCanvasPrevious());
    }

    // replace the search block to the next:
    // iterate over all the children
    // see if the type is canvas, check if it's not current or prev or to delete
    if (cnv == this->getCanvasPrevious() || getCanvasCurrent() == getCanvasCurrent()){
        for (unsigned int i=0;i<this->getMaxCanvasId();++i){
            Canvas* cnvi = this->getCanvas(i);
            if (cnvi != NULL && cnvi != this->getCanvasCurrent() && cnvi != cnv){
                this->setCanvasPrevious(cnvi);
                std::cout << "deleteCanvas(): previous canvas is set to name: " << this->getCanvasPrevious()->getName() << std::endl;
                break;
            }
        }
    }
    return deleteNode(cnv);
}

// we return bool and not osg::Node like it is done for the Canvas type
// because we already pass pointer on that node
bool RootScene::addNode(osg::Node *node){
    std::cout << "addNode(): (Node*)" << std::endl;
    node->setName(getEntityName(dureu::NAME_ENTITY, _idNode++));
    return _userScene->addChild(node);
}

bool RootScene::deleteNode(const std::string &name)
{
    std::cout << "deleteNode(): (string&): " << name << std::endl;
    osg::Node* node = this->getNode(name);
    std::cout << "deleteNode(): Node child idx: " << _userScene->getChildIndex(node) << std::endl;
    if (!node){
        std::cerr << "deleteNode(): The node pointer is NULL" << std::endl;
        return false;
    }
    return deleteNode(node);
}

bool RootScene::deleteNode(osg::Node *node)
{
    std::cout << "deleteNode(): (osg::Node*): " << node->getName() << std::endl;
    if (!node){
        std::cerr << "deleteNode(): The node pointer is NULL" << std::endl;
        return true;
    }
    std::cout << "deleteNode(): Trying to delete node with ptr: " << node << std::endl;
    std::cout << "deleteNode():   and name: " << node->getName() << std::endl;
    //bool success = _userScene->removeChild(tn);
    bool success = _userScene->removeChild(node);
    if (success) std::cout << "deleteNode(): Success on removeChild(osg::Node*)" << std::endl;
    else std::cerr << "deleteNode(): Failure to removeChild(osg::Node*)" << std::endl;
    return success;
}

bool RootScene::loadSceneFromFile(const std::string& fname){
    outLogVal("loadSceneFromFile", fname);
    osg::ref_ptr<osg::Group> userScene = dynamic_cast<osg::Group*>(osgDB::readNodeFile(fname));
    // check the pointer is not empty
    if (!userScene.get()){
        outErrMsg("loadSceneFromFile: could not load from file, or could not perform the dynamic_cast<osg::Group*>");
        return false;
    }
    // clean the current scene data
    if (_userScene->getNumChildren() > 0){
        outLogMsg("loadSceneFromFile(): the current _userScene will be replaced by a scene from file");
        if (!this->clearUserData()){
            outErrMsg("loadSceneFromFile(): could not clear the current user scene data");
            userScene = 0;
            return false;
        }
    }
    // assign the loaded scene to the internal user data
    _userScene = userScene.get();
    return true;
}

bool RootScene::writeSceneToFile(const std::string &fname) const
{
    outLogVal("writeSceneToFile", fname);
    osg::Node* node = dynamic_cast<osg::Node*>(_userScene.get());
    bool written = osgDB::writeNodeFile(*node, fname);
    if (!written)
        outErrMsg("writeSceneToFile(): Could not write scene to the specified file");
    return written;
}

Photo *RootScene::loadPhotoFromFile(const std::string &fname)
{
    outLogMsg("loadPhotoFromFile()");
    Photo* photo = new Photo(fname);
    if (!photo){
        outErrMsg("loadPhotoFromFile(): File could not be loaded");
        outLogVal("file name", fname);
        return 0;
    }
    if (!_canvasCurrent.get()){
        outErrMsg("There is not active canvas to load the photo to. Create new canvas first");
        return 0;
    }
    this->addPhoto(photo);
    return photo;
}

bool RootScene::addPhoto(Photo* photo)
{
    if (!photo){
        outErrMsg("addPhoto(): photo pointer is null, photo is not added to RootScene");
        return false;
    }
    return _canvasCurrent->addPhoto(photo);
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
    std::cout << "getCanvas(): (string&)" << std::endl;
    FindNodeVisitor fnv(name);
    _userScene->accept(fnv);
    if (fnv.getNode() == NULL){
        std::cerr << "getCanvas(): No entity with such name found: " << name << std::endl;
        return NULL;
    }
    return dynamic_cast<Canvas*>(fnv.getNode());
}

osg::Node *RootScene::getNode(unsigned int id) const{
    return getNode(getEntityName(dureu::NAME_ENTITY, id));
}

osg::Node *RootScene::getNode(const std::string &name) const
{
    std::cout << "getNode(): (string&)" << std::endl;
    FindNodeVisitor fnv(name);
    _userScene->accept(fnv);
    if (fnv.getNode() == NULL){
        std::cerr << "getNode(): No entity with such name found: " << name << std::endl;
        return NULL;
    }
    return fnv.getNode();
}

bool RootScene::setCanvasName(Canvas *cnv, const std::string &name)
{
    std::cout << "setCanvasName(): (Canvas*, string&)" << std::endl;
    return setNodeName(cnv, name);
}

bool RootScene::setNodeName(osg::Node *node, const std::string &name)
{
    std::cout << "setNodeName(): (osg::Node*, string&)" << std::endl;
    unsigned int idx = _userScene->getChildIndex(node);
    if (_userScene->getNumChildren()<=idx){
        std::cerr << "setNodeName(): The entity of given name is not direct child of the _userScene" << std::endl;
        std::cerr << "setNodeName(): The name was not setup" << std::endl;
        return false;
    }
    for (unsigned int i = 0; i<_userScene->getNumChildren(); ++i){
        if (_userScene->getChild(i)->getName() == name){
            std::cerr << "setNodeName(): The requested name is already in use by another entity" << std::endl;
            std::cerr << "setNodeName(): The name was not setup" << std::endl;
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
        Photo* photo = _canvasCurrent->getPhotoCurrent();
        if (photo){
            _canvasCurrent->setPhotoCurrent(false);
        }
        _canvasPrevious = _canvasCurrent;
        _canvasCurrent = NULL;
    }
    if (!cnv){
        std::cerr << "setCanvasCurrent(): The input canvas pointer is NULL, no current canvas is assigned" << std::endl;
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
        std::cerr << "setCanvasPrevious(): The input canvas pointed is not valid" << std::endl;
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

// what is the deepness (degree) in a RootScene tree?
// this -> _userScene -> Canvas
unsigned int RootScene::getCanvasLevel() const{
    return 3;
}

// levelCanvas -> Switch -> Transform -> TransformData -> GeodeData -> Photo
unsigned int RootScene::getPhotoLevel() const
{
    return this->getCanvasLevel() + 5;
}

void RootScene::setTransformOffset(const osg::Vec3f &translate, const int mouse)
{
    // initialize offset mode
    if (mouse == 0){
        _canvasCurrent->setModeOffset(true);
    }

    // back to normal mode
    else if (mouse == 2){
        _canvasCurrent->setModeOffset(false);
    }
    else {
        osg::ref_ptr<osg::MatrixTransform> T = new osg::MatrixTransform;
        T->setMatrix(osg::Matrix::translate(translate.x(), translate.y(), translate.z()));
        _canvasCurrent->setTransformPost(T.get());
    }
}

void RootScene::setTransformRotate(const osg::Vec3f &normal, const int mouse)
{
    // initialize offset mode
    if (mouse == 0){
        _canvasCurrent->setModeOffset(true);
    }

    // back to normal mode
    else if (mouse == 2){
        _canvasCurrent->setModeOffset(false);
    }
    else {
        osg::ref_ptr<osg::MatrixTransform> R = new osg::MatrixTransform;
//        R->setMatrix(osg::Matrix::rotate(dureu::PI/24, osg::Vec3f(0,1,0)));
        R->setMatrix(osg::Matrix::rotate(dureu::PI/24, osg::Vec3f(0,0,1)));
        //_canvasCurrent->setTransformRotate(R.get());
        _canvasCurrent->setTransformPost(R.get()); // must be fixed
        // organize transforms inside canvas so that there would be two parts:
        // M = M1*M2,
        // where Mr - accumulation of all rotate transforms
        // Mt - accumulation of all translate transforms
        // so when we need to rotate more, we have to use Mr:
        // M = Mr*R*Mt; Mr = Mr*R;
        // when we do translate, we can simply post multiply:
        // M = Mr*Mt*T; Mt = Mt*T;
    }
}

void RootScene::setUndoStack(QUndoStack *stack)
{
    if (!stack){
        outErrMsg("UndoStack pointer is empty, it is not set for the RootScene");
        return;
    }
    if (_undoStack){
        outErrMsg("UndoStack is already initialized");
        return;
    }
    _undoStack = stack;
}

QUndoStack *RootScene::getUndoStack() const
{
    return _undoStack;
}

bool RootScene::clearUserData()
{
    return _userScene->removeChildren(0, _userScene->getNumChildren());
}

void RootScene::setCanvasName(Canvas *cnv){
    cnv->setCanvasName(getEntityName(dureu::NAME_CANVAS, _idCanvas++));
    std::cout << "setCanvasName(): Canvas renamed: " << cnv->getName() << std::endl;

}

std::string RootScene::getEntityName(const std::string &name, unsigned int id) const{
    char buffer[10];
    //sprintf_s(buffer, sizeof(buffer), "%d", id);  // replace back to snprintf in final
    snprintf(buffer, sizeof(buffer), "%d", id);
    //itoa(id, buffer, 10);
    return name + std::string(buffer);//std::to_string(static_cast<long double>(id));
}

bool RootScene::setSceneObserver() {
    _observer->setName("SceneObserver");
    if (_observer->getScenePointer()!=NULL){
        std::cerr << "setSceneObserver(): SceneObserver will be overriden" << std::endl;
    }
    _observer->setScenePointer(_userScene.get());
    _userScene->addUpdateCallback(_observer.get());
    return true;
}

