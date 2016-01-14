#include "UserScene.h"

#include "settings.h"
#include "AddCanvasCommand.h"
#include "AddStrokeCommand.h"
#include "AddPhotoCommand.h"
#include "findnodevisitor.h"

entity::UserScene::UserScene()
    : osg::Group()
    , m_undoStack(0)
    , m_canvasCurrent(0)
    , m_canvasPrevious(0)
    , m_strokeCurrent(0)
    , m_idCanvas(0)
    , m_filePath("")
{
    this->setName("UserScene");
}

entity::UserScene::UserScene(const entity::UserScene& scene, const osg::CopyOp& copyop)
    : osg::Group(scene, copyop)
    , m_canvasCurrent(scene.m_canvasCurrent)
    , m_canvasPrevious(scene.m_canvasPrevious)
    , m_strokeCurrent(scene.m_strokeCurrent)
    , m_idCanvas(scene.m_idCanvas)
    , m_filePath(scene.m_filePath)
{

}

void entity::UserScene::setUndoStack(QUndoStack* stack)
{
    m_undoStack = stack;
}

void entity::UserScene::setIdCanvas(unsigned int id)
{
    m_idCanvas = id;
}

unsigned int entity::UserScene::getIdCanvas() const
{
    return m_idCanvas;
}

void entity::UserScene::setFilePath(const std::string &name)
{
    m_filePath = name;
}

const std::string &entity::UserScene::getFilePath() const
{
    return m_filePath;
}

bool entity::UserScene::isSetFilePath() const
{
    return m_filePath == ""? false : true;
}

void entity::UserScene::addCanvas(const osg::Matrix& R, const osg::Matrix& T)
{
    this->addCanvas(R,T, getEntityName(dureu::NAME_CANVAS, m_idCanvas++));
}

void entity::UserScene::addCanvas(const osg::Matrix& R, const osg::Matrix& T, const std::string& name)
{
    if (!m_undoStack){
        fatalMsg("addCanvas(): undo stack is NULL, Canvas will not be added. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }
    AddCanvasCommand* cmd = new AddCanvasCommand(this, R, T, name);
    m_undoStack->push(cmd);
}

void entity::UserScene::addStroke(float u, float v, dureu::EVENT event)
{
    if (!m_undoStack){
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

void entity::UserScene::addPhoto(const std::string& fname)
{
    outLogMsg("loadPhotoFromFile()");
    if (!m_undoStack){
        fatalMsg("addCanvas(): undo stack is NULL, Canvas will not be added. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }
    AddPhotoCommand* cmd = new AddPhotoCommand(this, fname);
    m_undoStack->push(cmd);
}

entity::Canvas* entity::UserScene::getCanvas(unsigned int id)
{
    //return dynamic_cast<entity::Canvas*>(this->getChild(id));
    return getCanvas(this->getChild(id)->getName());
}

entity::Canvas* entity::UserScene::getCanvas(const std::string& name)
{
    std::cout << "getCanvas(): (string&)" << std::endl;
    FindNodeVisitor fnv(name);
    this->accept(fnv);
    if (fnv.getNode() == NULL){
        std::cerr << "getCanvas(): No entity with such name found: " << name << std::endl;
        return NULL;
    }
    return dynamic_cast<entity::Canvas*>(fnv.getNode());
}

int entity::UserScene::getCanvasLevel() const
{
    return 3;
}

int entity::UserScene::getPhotoLevel() const
{
    return this->getCanvasLevel() + 4;
}

bool entity::UserScene::setCanvasCurrent(entity::Canvas* cnv)
{
    // if canvasCurr and canvasPrev are equal, search for the nearest
    // valiable candidate to assign the previous to
    // if no canvases available at all, the observer ptrs are set to NULL
    if (cnv == m_canvasCurrent.get())
        return true;
    if (m_canvasCurrent.valid()){
        if (m_canvasPrevious.valid()){
            m_canvasPrevious->setColor(dureu::CANVAS_CLR_REST);
            m_canvasPrevious = NULL;
        }
        m_canvasCurrent->setColor(dureu::CANVAS_CLR_PREVIOUS);
        entity::Photo* photo = m_canvasCurrent->getPhotoCurrent();
        if (photo){
            m_canvasCurrent->setPhotoCurrent(false);
        }
        m_canvasPrevious = m_canvasCurrent;
        m_canvasCurrent = NULL;
    }
    if (!cnv){
        std::cerr << "setCanvasCurrent(): The input canvas pointer is NULL, no current canvas is assigned" << std::endl;
        return false;
    }
    m_canvasCurrent = cnv;
    m_canvasCurrent->setColor(dureu::CANVAS_CLR_CURRENT);
    return true;
}

bool entity::UserScene::setCanvasPrevious(entity::Canvas* cnv)
{
    if (cnv == m_canvasPrevious.get())
        return true;
    if (m_canvasPrevious.valid()){
        m_canvasPrevious->setColor(dureu::CANVAS_CLR_REST);
        m_canvasPrevious = NULL;
    }
    if (!cnv){
        std::cerr << "setCanvasPrevious(): The input canvas pointed is not valid" << std::endl;
        return false;
    }
    m_canvasPrevious = cnv;
    m_canvasPrevious->setColor(dureu::CANVAS_CLR_PREVIOUS);
    return true;
}

entity::Canvas*entity::UserScene::getCanvasCurrent() const
{
    return m_canvasCurrent.get();
}

entity::Canvas*entity::UserScene::getCanvasPrevious() const
{
    return m_canvasPrevious.get();
}

void entity::UserScene::setTransformOffset(const osg::Vec3f& translate, const int mouse)
{
    // initialize offset mode
    if (mouse == 0){
        m_canvasCurrent->setModeOffset(true);
    }

    // back to normal mode
    else if (mouse == 2){
        m_canvasCurrent->setModeOffset(false);
    }
    else {
        m_canvasCurrent->translate(osg::Matrix::translate(translate.x(), translate.y(), translate.z()));
    }
}

void entity::UserScene::setTransformRotate(const osg::Vec3f& normal, const int mouse)
{
    // initialize offset mode
    if (mouse == 0){
        m_canvasCurrent->setModeOffset(true);
    }

    // back to normal mode
    else if (mouse == 2){
        m_canvasCurrent->setModeOffset(false);
    }
    else {
       m_canvasCurrent->rotate(osg::Matrix::rotate(dureu::PI/24, osg::Vec3f(0,0,1)));
    }
}

entity::UserScene::~UserScene()
{

}

bool entity::UserScene::clearUserData()
{
    return this->removeChildren(0, this->getNumChildren());
}

std::string entity::UserScene::getCanvasName()
{
    return this->getEntityName(dureu::NAME_CANVAS, m_idCanvas++);
}

std::string entity::UserScene::getEntityName(const std::string &name, unsigned int id) const
{
    char buffer[10];
    sprintf_s(buffer, sizeof(buffer), "%d", id);  // replace back to snprintf in final
    //snprintf(buffer, sizeof(buffer), "%d", id);
    //itoa(id, buffer, 10);
    return name + std::string(buffer);//std::to_string(static_cast<long double>(id));
}

void entity::UserScene::strokeStart()
{
    outLogMsg("strokeStart()");
    if (this->strokeValid()){
        outErrMsg("strokeStart(): Cannot start new stroke since the pointer is not NULL");
        return;
    }
    m_strokeCurrent = new entity::Stroke();
    this->getCanvasCurrent()->getGeodeData()->addDrawable(m_strokeCurrent);
}

void entity::UserScene::strokeAppend(float u, float v)
{
    if (this->strokeValid()){
        m_strokeCurrent->appendPoint(u, v);
        this->getCanvasCurrent()->updateFrame();
    }
    else
        outErrMsg("strokeAppend: pointer is NULL");
}

/* if command is still a valid pointer,
   if stroke is long enough to be kept,
   clone the AddStrokeCommand and push the cloned instance to stack
   set the shared pointer to zero and return*/
void entity::UserScene::strokeFinish()
{
    if (this->strokeValid()){
        if (m_strokeCurrent->isLengthy()){
            entity::Stroke* stroke = new entity::Stroke(*m_strokeCurrent, osg::CopyOp::DEEP_COPY_ALL);
            AddStrokeCommand* cmd = new AddStrokeCommand(this, stroke);
            m_undoStack->push(cmd);
        }
    }
    else{
        outErrMsg("strokeFinish(): stroke pointer is NULL, impossible to finish the stroke");
        return;
    }
    this->getCanvasCurrent()->getGeodeData()->removeChild(m_strokeCurrent.get()); // remove the "current" copy
    m_strokeCurrent = 0;
    outLogMsg("strokeFinish()");
}

bool entity::UserScene::strokeValid() const
{
    return m_strokeCurrent.get();
}

REGISTER_OBJECT_WRAPPER(UserScene_Wrapper
                        , new entity::UserScene
                        , entity::UserScene
                        , "osg::Object osg::Group entity::UserScene")
{
    ADD_UINT_SERIALIZER(IdCanvas, 0);
    ADD_STRING_SERIALIZER(FilePath, "");
}
