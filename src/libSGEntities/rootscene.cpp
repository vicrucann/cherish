#include "iostream"
#include <sstream>
#include <stdlib.h>
#include <assert.h>

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
#include "Canvas.h"
#include "Photo.h"
#include "findnodevisitor.h"
#include "AddStrokeCommand.h"
#include "AddCanvasCommand.h"
#include "AddPhotoCommand.h"

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
    , current_stroke(0)
    , m_filePath("")
{
    _userScene->setName("UserScene");
    // child #0
    if (!this->addChild(_userScene.get())){
        outErrMsg("RootScene(): could not add user scene as a child");
        fatalMsg("RootScene(): could not add user scene as a child");
    }

    _axes->setName("Axes");
    // child #1
    if (!this->addChild(_axes.get())){
        outErrMsg("RootScene(): could not add axes as a child");
        fatalMsg("RootScene(): could not add axes as a child");
    }

    _observer->setName("SceneObserver");
    _observer->setScenePointer(_userScene.get());
    _userScene->addUpdateCallback(_observer.get());


    _hud->setName("HUDCamera");
    // child #2
    if (!this->addChild(_hud.get())){
        outErrMsg("RootScene(): could not add hud as a child");
        fatalMsg("RootScene(): could not add hud as a child");
    }

    if (!_hud->setText(_observer->getTextGeode())){
        outErrMsg("RootScene(): could not set text to hud camera");
        fatalMsg("RootScene(): could not set text to hud camera");
    }

    this->setName("RootScene");
}

RootScene::~RootScene(){}

osg::Group* RootScene::getUserScene() const{
    return _userScene.get();
}

void RootScene::setAxesVisibility(bool vis) {
    _axes->setVisibility(vis);
}

bool RootScene::getAxesVisibility() const{
    return _axes->getVisibility();
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

const HUDCamera *RootScene::getHudCamera() const{
    return _hud.get();
}

void RootScene::setHudCameraVisibility(bool vis){
    _hud->setVisibility(vis);
}

bool RootScene::getHudCameraVisibility() const{
    return _hud->getVisibility();
}

// use QUndoStack framework
// more on Qt smart pointers:
// http://blog.qt.io/blog/2009/08/25/count-with-me-how-many-smart-pointer-classes-does-qt-have/
// we chose QPointer to track QUndoCommand
// see why: http://www.macieira.org/blog/2012/07/continue-using-qpointer/
void RootScene::addStroke(float u, float v, dureu::EVENT event)
{
    if (!_undoStack){
        fatalMsg("addStroke(): undo stack is NULL, it is not initialized. "
                 "Sketching is not possible. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }
    switch (event){
    case dureu::EVENT_OFF:
        outLogMsg("EVENT_OFF");
        this->strokeFinish();
        break;
    case dureu::EVENT_PRESSED:
        outLogMsg("EVENT_PRESSED");
        this->strokeStart();
        this->strokeAppend(u, v);
        break;
    case dureu::EVENT_DRAGGED:
        if (!this->strokeValid())
            this->strokeStart();
        this->strokeAppend(u, v);
        break;
    case dureu::EVENT_RELEASED:
        outLogMsg("EVENT_RELEASED");
        if (!this->strokeValid())
            this->strokeStart();
        this->strokeAppend(u, v);
        this->strokeFinish();
        break;
    default:
        break;
    }
}

void RootScene::addCanvas(osg::MatrixTransform *transform)
{
    this->addCanvas(transform, getEntityName(dureu::NAME_CANVAS, _idCanvas++));
}

void RootScene::addCanvas(osg::MatrixTransform *transform, const std::string &name)
{
    if (!_undoStack){
        fatalMsg("addCanvas(): undo stack is NULL, Canvas will not be added. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }
    AddCanvasCommand* cmd = new AddCanvasCommand(this, transform, name);
    _undoStack->push(cmd);
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

bool RootScene::loadSceneFromFile()
{
    if (!_undoStack){
        fatalMsg("addCanvas(): undo stack is NULL, Canvas will not be added. "
                 "Restart the program to ensure undo stack initialization.");
        return false;
    }
    osg::ref_ptr<osg::Group> userScene = dynamic_cast<osg::Group*>(osgDB::readNodeFile(m_filePath));
    if (!userScene.get()){
        outErrMsg("loadSceneFromFile: could not load from file, or could not perform the dynamic_cast<osg::Group*>");
        return false;
    }
    if (_userScene->getNumChildren() > 0){
        outLogMsg("loadSceneFromFile(): the current _userScene will be replaced by a scene from file");
        if (!this->clearUserData()){
            outErrMsg("loadSceneFromFile(): could not clear the current user scene data");
            userScene = 0;
            return false;
        }
    }
    for (unsigned int i=0; i<userScene->getNumChildren(); ++i){
        Canvas* cnv = dynamic_cast<Canvas*>(userScene->getChild(i));
        if (!cnv){
            outErrMsg("loadSceneFromFile: could not dynamic_cast to Canvas*.");
            return false;
        }
        osg::MatrixTransform* t = cnv->getTransform();
        AddCanvasCommand* cmd = new AddCanvasCommand(this, t, this->getCanvasName());
        if (!cmd){
            outErrMsg("loadSceneFromFile: could not create AddCanvasCommand.");
            return false;
        }
        _undoStack->push(cmd);
    }
    userScene = 0;
    return true;
}

// read more in OSG beginner's guide, Chapter 10,
// "Serializing OSG native scenes"
bool RootScene::writeSceneToFile(const std::string &fname) const
{
    outLogVal("writeSceneToFile", fname);
    osg::Node* node = dynamic_cast<osg::Node*>(_userScene.get());
    bool written = osgDB::writeNodeFile(*node, fname);
    if (!written)
        outErrMsg("writeSceneToFile(): Could not write scene to the specified file");
    return written;
}

bool RootScene::writeSceneToFile() const
{
    if (_userScene->getNumChildren() == 0){
        outErrMsg("The scene is empty, there is nothing to save.");
        return false;
    }
    osg::Node* node = dynamic_cast<osg::Node*>(_userScene.get());
    if (!node){
        outErrMsg("writeSceneToFile: could not perform the dynamic_cast. File will not be saved.");
        return false;
    }
    return osgDB::writeNodeFile(*node, m_filePath);
}

void RootScene::loadPhotoFromFile(const std::string &fname)
{
    outLogMsg("loadPhotoFromFile()");
    if (!_undoStack){
        fatalMsg("addCanvas(): undo stack is NULL, Canvas will not be added. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }
    AddPhotoCommand* cmd = new AddPhotoCommand(this, fname);
    _undoStack->push(cmd);
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
        entity::Photo* photo = _canvasCurrent->getPhotoCurrent();
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

void RootScene::setFilePath(const std::string &fname)
{
    m_filePath = fname;
    outLogMsg("file path is set");
}

const std::string &RootScene::getFilePath() const
{
    return m_filePath;
}

bool RootScene::isSetFilePath() const
{
    return m_filePath == ""? false : true;
}

bool RootScene::clearUserData()
{
    return _userScene->removeChildren(0, _userScene->getNumChildren());
}

std::string RootScene::getCanvasName()
{
    return getEntityName(dureu::NAME_CANVAS, _idCanvas++);
}

std::string RootScene::getEntityName(const std::string &name, unsigned int id) const{
    char buffer[10];
    //sprintf_s(buffer, sizeof(buffer), "%d", id);  // replace back to snprintf in final
    snprintf(buffer, sizeof(buffer), "%d", id);
    //itoa(id, buffer, 10);
    return name + std::string(buffer);//std::to_string(static_cast<long double>(id));
}

void RootScene::strokeStart()
{
    outLogMsg("strokeStart()");
    if (this->strokeValid()){
        outErrMsg("strokeStart(): Cannot start new stroke since the pointer is not NULL");
        return;
    }
    current_stroke = new entity::Stroke();
    this->getCanvasCurrent()->getGeodeData()->addDrawable(current_stroke);
}

void RootScene::strokeAppend(float u, float v)
{
    if (this->strokeValid()){
        current_stroke->appendPoint(u, v);
        this->getCanvasCurrent()->updateFrame();
    }
    else
        outErrMsg("strokeAppend: pointer is NULL");
}

/* if command is still a valid pointer,
   if stroke is long enough to be kept,
   clone the AddStrokeCommand and push the cloned instance to stack
   set the shared pointer to zero and return*/
void RootScene::strokeFinish()
{
    if (this->strokeValid()){
        if (current_stroke->isLengthy()){
            entity::Stroke* stroke = new entity::Stroke(*current_stroke, osg::CopyOp::DEEP_COPY_ALL);
            AddStrokeCommand* cmd = new AddStrokeCommand(this, stroke);
            _undoStack->push(cmd);
        }
    }
    else{
        outErrMsg("strokeFinish(): stroke pointer is NULL, impossible to finish the stroke");
        return;
    }
    this->getCanvasCurrent()->getGeodeData()->removeChild(current_stroke.get()); // remove the "current" copy
    current_stroke = 0;
    outLogMsg("strokeFinish()");
}

// checks if command pointer is NULL or not
bool RootScene::strokeValid() const
{
    return current_stroke.get();
}

