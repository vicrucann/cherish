#include "assert.h"

#include "UserScene.h"

#include "settings.h"
#include "AddEntityCommand.h"
#include "EditEntityCommand.h"
#include "findnodevisitor.h"

#include <osgDB/WriteFile>
#include <osgDB/ReadFile>

entity::UserScene::UserScene()
    : osg::Group()
    , m_canvasCurrent(NULL)
    , m_canvasPrevious(NULL)
    , m_canvasSelected(NULL)
    , m_deltaT(osg::Vec3f(0.f,0.f,0.f))
    , m_deltaR(osg::Quat(0,0,0,1))
    , m_u(0)
    , m_v(0)
    , m_idCanvas(0)
    , m_idPhoto(0)
    , m_filePath("")
{
    this->setName("UserScene");
}

entity::UserScene::UserScene(const entity::UserScene& scene, const osg::CopyOp& copyop)
    : osg::Group(scene, copyop)
    , m_canvasCurrent(scene.m_canvasCurrent)
    , m_canvasPrevious(scene.m_canvasPrevious)
    , m_canvasSelected(NULL)
    , m_deltaT(scene.m_deltaT)
    , m_deltaR(scene.m_deltaR)
    , m_u(scene.m_u)
    , m_v(scene.m_v)
    , m_idCanvas(scene.m_idCanvas)
    , m_idPhoto(scene.m_idPhoto)
    , m_filePath(scene.m_filePath)
{

}

void entity::UserScene::setIdCanvas(unsigned int id)
{
    m_idCanvas = id;
}

unsigned int entity::UserScene::getIdCanvas() const
{
    return m_idCanvas;
}

void entity::UserScene::setIdPhoto(unsigned int id)
{
    m_idPhoto = id;
}

unsigned int entity::UserScene::getIdPhoto() const
{
    return m_idPhoto;
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

void entity::UserScene::addCanvas(QUndoStack* stack, const osg::Matrix& R, const osg::Matrix& T)
{
    this->addCanvas(stack, R,T, getEntityName(dureu::NAME_CANVAS, m_idCanvas++));
}

void entity::UserScene::addCanvas(QUndoStack* stack, const osg::Matrix& R, const osg::Matrix& T, const std::string& name)
{
    if (!stack){
        fatalMsg("addCanvas(): undo stack is NULL, Canvas will not be added. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }
    AddCanvasCommand* cmd = new AddCanvasCommand(this, R, T, name);
    stack->push(cmd);
}

void entity::UserScene::addStroke(QUndoStack* stack, float u, float v, dureu::EVENT event)
{
    if (!stack){
        fatalMsg("addStroke(): undo stack is NULL, it is not initialized. "
                 "Sketching is not possible. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }
    switch (event){
    case dureu::EVENT_OFF:
        outLogMsg("EVENT_OFF");
        this->strokeFinish(stack);
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
            break;
        this->strokeAppend(u, v);
        this->strokeFinish(stack);
        break;
    default:
        break;
    }
}

void entity::UserScene::addPhoto(QUndoStack* stack, const std::string& fname)
{
    outLogMsg("loadPhotoFromFile()");
    if (!stack){
        fatalMsg("addPhoto(): undo stack is NULL, Canvas will not be added. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }
    AddPhotoCommand* cmd = new AddPhotoCommand(this, fname, this->getPhotoName());
    if (!cmd){
        fatalMsg("addPhoto(): could not allocate AddPhotoCommand.");
        return;
    }
    stack->push(cmd);
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

int entity::UserScene::getStrokeLevel() const
{
    return this->getCanvasLevel() + 4;
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
        m_canvasCurrent->unselectAll();

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

void entity::UserScene::setCanvasSelected(entity::Canvas *cnv)
{
    if (m_canvasSelected.get() == cnv)
        return;
    if (m_canvasSelected.get() != NULL)
        this->setCanvasSelected(false);
    m_canvasSelected = cnv;
    this->setCanvasSelected(true);
}

/* When doing select / deselct, we need to take care of:
 * Canvas color change
 * Setting / unsetting of traversal mask
*/
void entity::UserScene::setCanvasSelected(bool selected)
{
    if (!m_canvasSelected.get())
        return;
    if (!selected){
        if (m_canvasSelected.get() == m_canvasCurrent.get())
            m_canvasSelected->setColor(dureu::CANVAS_CLR_CURRENT);
        else if (m_canvasSelected.get() == m_canvasPrevious.get())
            m_canvasSelected->setColor(dureu::CANVAS_CLR_PREVIOUS);
        else
            m_canvasSelected->setColor(dureu::CANVAS_CLR_REST);
        m_canvasSelected = NULL;
    }
    else {
        m_canvasSelected->setColor(dureu::CANVAS_CLR_SELECTED);
    }
}

entity::Canvas*entity::UserScene::getCanvasCurrent() const
{
    return m_canvasCurrent.get();
}

entity::Canvas*entity::UserScene::getCanvasPrevious() const
{
    return m_canvasPrevious.get();
}

entity::Canvas *entity::UserScene::getCanvasSelected() const
{
    return m_canvasSelected.get();
}

void entity::UserScene::editCanvasOffset(QUndoStack* stack, const osg::Vec3f& translate, dureu::EVENT event)
{
    if (!stack){
        fatalMsg("editCanvasOffset(): undo stack is NULL, it is not initialized. "
                 "Editing is not possible. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }

    switch (event){
    case dureu::EVENT_OFF:
        this->canvasOffsetFinish(stack);
        break;
    case dureu::EVENT_PRESSED:
        this->canvasOffsetStart();
        this->canvasOffsetAppend(translate);
        break;
    case dureu::EVENT_DRAGGED:
        if (!this->canvasEditValid())
            this->canvasOffsetStart();
        this->canvasOffsetAppend(translate);
        break;
    case dureu::EVENT_RELEASED:
        if (!this->canvasEditValid())
            break;
        this->canvasOffsetAppend(translate);
        this->canvasOffsetFinish(stack);
        break;
    default:
        break;
    }
}

void entity::UserScene::editCanvasRotate(QUndoStack* stack, const osg::Quat& rotation, dureu::EVENT event)
{
    if (!stack){
        fatalMsg("editCanvasRotate(): undo stack is NULL, it is not initialized. "
                 "Editing is not possible. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }

    switch (event){
    case dureu::EVENT_OFF:
        this->canvasRotateFinish(stack);
        break;
    case dureu::EVENT_PRESSED:
        this->canvasRotateStart();
        this->canvasRotateAppend(rotation);
        break;
    case dureu::EVENT_DRAGGED:
        if (!this->canvasEditValid())
            this->canvasRotateStart();
        this->canvasRotateAppend(rotation);
        break;
    case dureu::EVENT_RELEASED:
        if (!this->canvasEditValid())
            break;
        this->canvasRotateAppend(rotation);
        this->canvasRotateFinish(stack);
        break;
    default:
        break;
    }
}

void entity::UserScene::editPhotoMove(QUndoStack* stack, const double u, const double v, dureu::EVENT event)
{
    if (!stack){
        fatalMsg("editCanvasRotate(): undo stack is NULL, it is not initialized. "
                 "Editing is not possible. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }
    switch (event){
    case dureu::EVENT_OFF:
        this->photoMoveFinish(stack, u, v);
        break;
    case dureu::EVENT_PRESSED:
        this->photoMoveStart();
        this->photoMoveAppend(u,v);
        break;
    case dureu::EVENT_DRAGGED:
        if (!this->photoEditValid())
            this->photoMoveStart();
        this->photoMoveAppend(u,v);
        break;
    case dureu::EVENT_RELEASED:
        if (!this->photoEditValid())
            break;
        this->photoMoveAppend(u,v);
        this->photoMoveFinish(stack, u, v);
        break;
    default:
        break;
    }
}

bool entity::UserScene::isEmptyScene() const
{
    return this->getNumChildren()==0? true : false;
}

entity::UserScene::~UserScene()
{

}

bool entity::UserScene::clearUserData()
{
    return this->removeChildren(0, this->getNumChildren());
}

bool entity::UserScene::printScene()
{
    for (unsigned int i = 0; i < this->getNumChildren(); ++i){
        Canvas* cnv = this->getCanvas(i);
        if (!cnv){
            outErrMsg("printScene: could not retrive canvas");
            return false;
        }
        outLogVal("Canvas name", cnv->getName());

        osg::MatrixTransform* t = dynamic_cast<osg::MatrixTransform*>(cnv->getChild(0));
        assert(t == cnv->getTransform());
        outLogVal("Transform name", t->getName());

        osg::Switch* sw = dynamic_cast<osg::Switch*>(t->getChild(0));
        assert(sw == cnv->getSwitch());
        outLogVal("Switch name", sw->getName());

        osg::Geode* data = dynamic_cast<osg::Geode*>(sw->getChild(3));
        assert(data == cnv->getGeodeData());
        outLogVal("Geode data name", data->getName());
    }
    return true;
}

std::string entity::UserScene::getCanvasName()
{
    return this->getEntityName(dureu::NAME_CANVAS, m_idCanvas++);
}

std::string entity::UserScene::getPhotoName()
{
    return this->getEntityName(dureu::NAME_PHOTO, m_idPhoto++);
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
    m_canvasCurrent->unselectStrokes();
    outLogMsg("strokeStart()");
    if (this->strokeValid()){
        outErrMsg("strokeStart(): Cannot start new stroke since the pointer is not NULL");
        return;
    }
    entity::Stroke* stroke = new entity::Stroke();
    m_canvasCurrent->setStrokeCurrent(stroke);
    m_canvasCurrent->getGeodeData()->addDrawable(stroke);
}

void entity::UserScene::strokeAppend(float u, float v)
{
    if (this->strokeValid()){
        entity::Stroke* stroke = m_canvasCurrent->getStrokeCurrent();
        stroke->appendPoint(u, v);
        m_canvasCurrent->updateFrame();
    }
    else
        outErrMsg("strokeAppend: pointer is NULL");
}

/* if command is still a valid pointer,
   if stroke is long enough to be kept,
   clone the AddStrokeCommand and push the cloned instance to stack
   set the shared pointer to zero and return*/
void entity::UserScene::strokeFinish(QUndoStack* stack)
{
    entity::Stroke* stroke = m_canvasCurrent->getStrokeCurrent();
    if (this->strokeValid()){
        if (stroke->isLengthy()){
            entity::Stroke* stroke_clone = new entity::Stroke(*stroke, osg::CopyOp::DEEP_COPY_ALL);
            AddStrokeCommand* cmd = new AddStrokeCommand(this, stroke_clone);
            stack->push(cmd);
        }
    }
    else{
        outErrMsg("strokeFinish(): stroke pointer is NULL, impossible to finish the stroke");
        return;
    }
    m_canvasCurrent->getGeodeData()->removeChild(stroke); // remove the "current" copy
    m_canvasCurrent->setStrokeCurrent(false);
    outLogMsg("strokeFinish()");
}

bool entity::UserScene::strokeValid() const
{
    return m_canvasCurrent->getStrokeCurrent();
}

void entity::UserScene::canvasOffsetStart()
{
    if (this->canvasEditValid()){
        outErrMsg("CanvasOffsetStart: cannot start editing since the canvas is already in edit mode");
        return;
    }
    m_canvasCurrent->setModeEdit(true);
    m_deltaT = osg::Vec3f(0.f,0.f,0.f);
}

void entity::UserScene::canvasOffsetAppend(const osg::Vec3f &t)
{
    if (!this->canvasEditValid()){
        outErrMsg("canvasOffsetAppend: canvas edit mode is not valid");
        return;
    }
    m_canvasCurrent->translate(osg::Matrix::translate(t.x(), t.y(), t.z()));
    m_deltaT = m_deltaT + t;
}

void entity::UserScene::canvasOffsetFinish(QUndoStack *stack)
{
    if (!this->canvasEditValid()){
        outErrMsg("canvasOffsetFinish: no canvas in edit mode, impossible to finish offset mode");
        return;
    }
    m_canvasCurrent->setModeEdit(false);
    m_canvasCurrent->translate(osg::Matrix::translate(-m_deltaT.x(), -m_deltaT.y(), -m_deltaT.z()));
    EditCanvasOffsetCommand* cmd = new EditCanvasOffsetCommand(m_canvasCurrent.get(), m_deltaT);
    stack->push(cmd);
    m_deltaT = osg::Vec3f(0.f,0.f,0.f);
}

bool entity::UserScene::canvasEditValid() const
{
    return this->getCanvasCurrent()->getModeEdit();
}

void entity::UserScene::canvasRotateStart()
{
    if (this->canvasEditValid()){
        outErrMsg("CanvasRotateStart: cannot start editing since the canvas is already in edit mode");
        return;
    }
    m_canvasCurrent->setModeEdit(true);
    m_deltaR = osg::Quat();
}

void entity::UserScene::canvasRotateAppend(const osg::Quat &r)
{
    if (!this->canvasEditValid()){
        outErrMsg("canvasRotateAppend: canvas edit mode is not valid");
        return;
    }
    m_canvasCurrent->rotate(osg::Matrix::rotate(r));
    m_deltaR = r * m_deltaR;
}

void entity::UserScene::canvasRotateFinish(QUndoStack *stack)
{
    if (!this->canvasEditValid()){
        outErrMsg("canvasRotateFinish: no canvas in edit mode, impossible to finish offset mode");
        return;
    }
    m_canvasCurrent->setModeEdit(false);
    m_canvasCurrent->rotate(osg::Matrix::rotate(m_deltaR.inverse()));
    EditCanvasRotateCommand* cmd = new EditCanvasRotateCommand(m_canvasCurrent.get(), m_deltaR);
    stack->push(cmd);
    m_deltaR = osg::Quat();
}

void entity::UserScene::photoMoveStart()
{
    if (this->canvasEditValid()){
        outErrMsg("photoMoveStart: cannot start editing since the photo is already in edit mode");
        return;
    }
    Photo* photo = this->getCanvasCurrent()->getPhotoCurrent();
    if (!photo){
        outErrMsg("photoMoveStart(): photo pointer is NULL");
        return;
    }
    photo->setModeEdit(true);
    m_u = photo->getCenter().x();
    m_v = photo->getCenter().y();
}

void entity::UserScene::photoMoveAppend(const double u, const double v)
{
    Photo* photo = this->getCanvasCurrent()->getPhotoCurrent();
    if (!photo){
        outErrMsg("photoMoveAppend(): photo pointer is NULL");
        return;
    }
    photo->move(u,v);
    this->getCanvasCurrent()->updateFrame();
}

void entity::UserScene::photoMoveFinish(QUndoStack *stack, const double u, const double v)
{
    Photo* photo = this->getCanvasCurrent()->getPhotoCurrent();
    if (!photo){
        outErrMsg("photoMoveFinish(): photo pointer is NULL");
        return;
    }
    photo->setModeEdit(false);
    photo->move(m_u, m_v);
    EditPhotoMoveCommand* cmd = new EditPhotoMoveCommand(m_canvasCurrent.get(), u, v);
    stack->push(cmd);
    m_u = 0;
    m_v = 0;
}

bool entity::UserScene::photoEditValid() const
{
    return this->getCanvasCurrent()->getPhotoCurrent()->getModeEdit();
}

REGISTER_OBJECT_WRAPPER(UserScene_Wrapper
                        , new entity::UserScene
                        , entity::UserScene
                        , "osg::Object osg::Group entity::UserScene")
{
    ADD_UINT_SERIALIZER(IdCanvas, 0);
    ADD_UINT_SERIALIZER(IdPhoto, 0);
    ADD_STRING_SERIALIZER(FilePath, "");
}
