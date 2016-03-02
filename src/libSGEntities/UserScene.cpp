#include "assert.h"

#include "UserScene.h"

#include "Settings.h"
#include "Utilities.h"
#include "AddEntityCommand.h"
#include "EditEntityCommand.h"
#include "FindNodeVisitor.h"

#include <osgDB/WriteFile>
#include <osgDB/ReadFile>

entity::UserScene::UserScene()
    : osg::Group()
    , m_bookmarks(new entity::Bookmarks)
    , m_canvasCurrent(NULL)
    , m_canvasPrevious(NULL)
    , m_canvasSelected(NULL)
    , m_canvasClone(0)
    , m_deltaT(osg::Vec3f(0.f,0.f,0.f))
    , m_deltaR(osg::Quat(0,0,0,1))
    , m_u(0)
    , m_v(0)
    , m_inits(false)
    , m_du(0)
    , m_dv(0)
    , m_scale(1)
    , m_rotate(0)
    , m_idCanvas(0)
    , m_idPhoto(0)
    , m_idBookmark(0)
    , m_filePath("")
{
    this->setName("UserScene");
}

entity::UserScene::UserScene(const entity::UserScene& scene, const osg::CopyOp& copyop)
    : osg::Group(scene, copyop)
    , m_bookmarks(scene.m_bookmarks)
    , m_canvasCurrent(scene.m_canvasCurrent)
    , m_canvasPrevious(scene.m_canvasPrevious)
    , m_canvasSelected(NULL)
    , m_canvasClone(0)
    , m_deltaT(scene.m_deltaT)
    , m_deltaR(scene.m_deltaR)
    , m_u(scene.m_u)
    , m_v(scene.m_v)
    , m_inits(scene.m_inits)
    , m_du(scene.m_du)
    , m_dv(scene.m_dv)
    , m_scale(scene.m_scale)
    , m_rotate(scene.m_rotate)
    , m_idCanvas(scene.m_idCanvas)
    , m_idPhoto(scene.m_idPhoto)
    , m_idBookmark(scene.m_idBookmark)
    , m_filePath(scene.m_filePath)
{
}

void entity::UserScene::setBookmarks(entity::Bookmarks *group)
{
    m_bookmarks = group;
}

const entity::Bookmarks *entity::UserScene::getBookmarks() const
{
    return m_bookmarks;
}

entity::Bookmarks *entity::UserScene::getBookmarksModel() const
{
    return m_bookmarks.get();
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

void entity::UserScene::setIdBookmark(unsigned int id)
{
    m_idBookmark = id;
}

unsigned int entity::UserScene::getIdBookmark() const
{
    return m_idBookmark;
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

void entity::UserScene::addBookmark(BookmarkWidget *widget, const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up)
{
    if (!m_bookmarks.get()){
        outErrMsg("addBookmark: bookmarks pointer is NULL");
        return;
    }
    if (!this->containsNode(m_bookmarks)){
        if (!this->addChild(m_bookmarks)){
            outErrMsg("addBookmark: could not set up root pointer correctly");
            return;
        }
    }
    m_bookmarks->addBookmark(widget, eye, center, up, this->getBookmarkName());
}

void entity::UserScene::eraseStroke(QUndoStack *stack, entity::Stroke *stroke, int first, int last, dureu::EVENT event)
{
    if (!stack){
        fatalMsg("eraseStroke(): undo stack is NULL, it is not initialized. "
                 "Erase is not possible. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }

    stroke->removePoints(first, last);

    switch (event){
    case dureu::EVENT_OFF:
        outLogMsg("EVENT_OFF");
        break;
    case dureu::EVENT_PRESSED:
        outLogMsg("EVENT_PRESSED");
        break;
    case dureu::EVENT_DRAGGED:
        break;
    case dureu::EVENT_RELEASED:
        outLogMsg("EVENT_RELEASED");
        break;
    default:
        break;
    }
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

/* Sets the traversal mask so that all the canvases (but current)
 * would not be traversable if enabled variable is false.
 * It sets them back to traversable if enabled is true.
 */
void entity::UserScene::setCanvasesButCurrent(bool enabled)
{
    outLogVal("setCanvasesButCurrent to ", enabled);
    if (!m_canvasCurrent.get()){
        std::cerr << "setCanvasesButCurrent(): No current canvas on scene" << std::endl;
        return;
    }

    for (unsigned int i=0; i<this->getNumChildren(); ++i){
        entity::Canvas* cnv = this->getCanvas(i);
        if (!cnv){
            outErrMsg("setCanvasesButCurrent(): Could not obtain one of the canvases");
            return;
        }
        if (cnv != m_canvasCurrent.get()){
            if (enabled)
                cnv->setNodeMask(~0x0);
            else
                cnv->setNodeMask(0x1); // see EventHandler when we set iv.setTraversalMask(~0x1);
        }
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
        outLogMsg("editCanvasRotate: OFF");
        this->canvasRotateFinish(stack);
        break;
    case dureu::EVENT_PRESSED:
        outLogMsg("editCanvasRotate: pressed");
        this->canvasRotateStart();
        this->canvasRotateAppend(rotation);
        break;
    case dureu::EVENT_DRAGGED:
        if (!this->canvasEditValid())
            this->canvasRotateStart();
        this->canvasRotateAppend(rotation);
        break;
    case dureu::EVENT_RELEASED:
        outLogMsg("editCanvasRotate: release");
        if (!this->canvasEditValid())
            break;
        outLogMsg("editCanvasRotate: released");
        this->canvasRotateAppend(rotation);
        this->canvasRotateFinish(stack);
        break;
    default:
        break;
    }
}

void entity::UserScene::editCanvasClone(QUndoStack *stack, const osg::Vec3f &translate, dureu::EVENT event)
{
    if (!stack){
        fatalMsg("editCanvasOffset(): undo stack is NULL, it is not initialized. "
                 "Editing is not possible. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }

    switch (event){
    case dureu::EVENT_OFF:
        this->canvasCloneFinish(stack);
        break;
    case dureu::EVENT_PRESSED:
        this->canvasCloneStart();
        this->canvasCloneAppend(translate);
        break;
    case dureu::EVENT_DRAGGED:
        if (!this->canvasCloneValid())
            this->canvasCloneStart();
        this->canvasCloneAppend(translate);
        break;
    case dureu::EVENT_RELEASED:
        if (!this->canvasCloneValid())
            break;
        this->canvasCloneAppend(translate);
        this->canvasCloneFinish(stack);
        break;
    default:
        break;
    }
}

void entity::UserScene::editCanvasDelete(QUndoStack *stack, entity::Canvas *canvas)
{
    outLogVal("Attempting to delete", canvas->getName());
    if (!stack){
        fatalMsg("editCanvasRotate(): undo stack is NULL, it is not initialized. "
                 "Editing is not possible. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }
    if (!canvas){
        outErrMsg("editCanvasDelete: canvas is NULL");
        return;
    }
    EditCanvasDeleteCommand* cmd = new EditCanvasDeleteCommand(this, canvas);
    if (!cmd){
        outErrMsg("editCanvasDelete: could not allocate DeleteCommand");
        return;
    }
    stack->push(cmd);
}

void entity::UserScene::editPhotoMove(QUndoStack* stack, Photo *photo, const double u, const double v, dureu::EVENT event)
{
    if (!stack){
        fatalMsg("editCanvasRotate(): undo stack is NULL, it is not initialized. "
                 "Editing is not possible. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }
    switch (event){
    case dureu::EVENT_OFF:
        outLogMsg("EditPhotoMove: event off called");
        if (this->photoEditValid())
            this->photoMoveFinish(stack, this->getCanvasCurrent()->getPhotoCurrent()->getCenter().x(),
                                  this->getCanvasCurrent()->getPhotoCurrent()->getCenter().y());
        break;
    case dureu::EVENT_PRESSED:
        outLogMsg("EditPhotoMove: event pressed called");
        this->photoMoveStart(photo);
        this->photoMoveAppend(u,v);
        break;
    case dureu::EVENT_DRAGGED:
        if (!this->photoEditValid())
            this->photoMoveStart(photo);
        this->photoMoveAppend(u,v);
        break;
    case dureu::EVENT_RELEASED:
        if (!this->photoEditValid())
            break;
        outLogMsg("EditPhotoMove: event release called");
        this->photoMoveAppend(u,v);
        this->photoMoveFinish(stack, u, v);
        break;
    default:
        break;
    }
}

void entity::UserScene::editPhotoScale(QUndoStack *stack, entity::Photo *photo, const double u, const double v, dureu::EVENT event)
{
    if (!stack){
        fatalMsg("editCanvasRotate(): undo stack is NULL, it is not initialized. "
                 "Editing is not possible. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }
    switch (event){
    case dureu::EVENT_OFF:
        outLogMsg("EditPhotoScale: event off called");
        if (this->photoEditValid())
            this->photoScaleFinish(stack, this->getCanvasCurrent()->getPhotoCurrent()->getWidth(),
                                   this->getCanvasCurrent()->getPhotoCurrent()->getHeight() );
        break;
    case dureu::EVENT_PRESSED:
        outLogMsg("EditPhotoScale: event pressed called");
        this->photoScaleStart(photo);
        this->photoScaleAppend(u,v);
        break;
    case dureu::EVENT_DRAGGED:
        if (!this->photoEditValid())
            this->photoScaleStart(photo);
        this->photoScaleAppend(u,v);
        break;
    case dureu::EVENT_RELEASED:
        if (!this->photoEditValid())
            break;
        outLogMsg("EditPhotoScale: event release called");
        this->photoScaleAppend(u,v);
        this->photoScaleFinish(stack, u, v);
        break;
    default:
        break;
    }
}

void entity::UserScene::editPhotoRotate(QUndoStack *stack, entity::Photo *photo, const double u, const double v, dureu::EVENT event)
{
    if (!stack){
        fatalMsg("editCanvasRotate(): undo stack is NULL, it is not initialized. "
                 "Editing is not possible. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }
    switch (event){
    case dureu::EVENT_OFF:
        outLogMsg("EditPhotoRotate: event off called");
        if (this->photoEditValid())
            this->photoRotateFinish(stack, this->getCanvasCurrent()->getPhotoCurrent()->getWidth(),
                                   this->getCanvasCurrent()->getPhotoCurrent()->getHeight() );
        break;
    case dureu::EVENT_PRESSED:
        outLogMsg("EditPhotoRotate: event pressed called");
        this->photoRotateStart(photo,u,v);
        this->photoRotateAppend(u,v);
        break;
    case dureu::EVENT_DRAGGED:
        if (!this->photoEditValid())
            this->photoRotateStart(photo,u,v);
        this->photoRotateAppend(u,v);
        break;
    case dureu::EVENT_RELEASED:
        if (!this->photoEditValid())
            break;
        outLogMsg("EditPhotoRotate: event release called");
        this->photoRotateAppend(u,v);
        this->photoRotateFinish(stack, u, v);
        break;
    default:
        break;
    }
}

void entity::UserScene::editPhotoFlip(QUndoStack* stack, entity::Photo *photo, bool horizontal)
{
    if (!stack){
        fatalMsg("editCanvasRotate(): undo stack is NULL, it is not initialized. "
                 "Editing is not possible. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }
    if (!photo)
        return;

    if (!this->getCanvasCurrent())
        return;

    if (!this->getCanvasCurrent()->setPhotoCurrent(photo))
        return;

    EditPhotoFlipCommand* cmd = new EditPhotoFlipCommand(this, horizontal);
    if (!cmd){
        outErrMsg("EditPhotoFlip: could not initiate undo/redo command");
        return;
    }
    stack->push(cmd);
    this->getCanvasCurrent()->setPhotoCurrent(false);
}

void entity::UserScene::editPhotoDelete(QUndoStack *stack, entity::Photo *photo)
{
    if (!stack){
        outErrMsg("editPhotoDelete(): undo stack is NULL.");
        return;
    }
    if (!photo)
        return;

    if (!this->getCanvasCurrent()->getGeodeData()->containsDrawable(photo)){
        outErrMsg("editPhotoDelete: current canvas does not contain that photo."
                  "Deletion is not possible.");
        return;
    }
    EditPhotoDeleteCommand* cmd = new EditPhotoDeleteCommand(this, m_canvasCurrent.get(), photo);
    if (!cmd){
        outErrMsg("editPhotoDelete: undo/redo command is NULL");
        return;
    }
    stack->push(cmd);
}

void entity::UserScene::editStrokesPush(QUndoStack *stack, osg::Camera *camera)
{
    if (!stack){
        outErrMsg("editStrokesPush: stack is NULL");
        return;
    }
    outLogMsg("Pushing strokes to previously selected canvas (highlighted violet)");
    osg::Vec3f eye, c, u;
    camera->getViewMatrixAsLookAt(eye, c, u);

    const std::vector<entity::Stroke*>& strokes = this->getCanvasCurrent()->getStrokesSelected();

    if (!Utilities::areStrokesProjectable(strokes, m_canvasCurrent.get(), m_canvasPrevious.get(), camera)){
        outErrMsg("Strokes are not pushable under this point of view. Try to change camera position.");
        return;
    }

    EditStrokesPushCommand* cmd = new EditStrokesPushCommand(this, strokes,
                                                             m_canvasCurrent.get(),
                                                             m_canvasPrevious.get(),
                                                             eye);
    if (!cmd){
        outErrMsg("editStrokePush: undo/redo command is NULL");
        return;
    }
    stack->push(cmd);
}

void entity::UserScene::editStrokesMove(QUndoStack *stack, double u, double v, dureu::EVENT event)
{
    if (!stack){
        fatalMsg("editStrokesMove(): undo stack is NULL.");
        return;
    }
    switch (event){
    case dureu::EVENT_OFF:
        outLogMsg("EditStrokesMove: event off called");
        if (this->strokesSelectedValid())
            this->strokesMoveFinish(stack);
        break;
    case dureu::EVENT_PRESSED:
        outLogMsg("EditStrokesMove: event pressed called");
        this->strokesMoveStart(u,v);
        this->strokesMoveAppend(u,v);
        break;
    case dureu::EVENT_DRAGGED:
        if (!this->strokesSelectedValid())
            this->strokesMoveStart(u,v);
        this->strokesMoveAppend(u,v);
        break;
    case dureu::EVENT_RELEASED:
        if (!this->strokesSelectedValid())
            break;
        outLogMsg("EditStrokesMove: event release called");
        this->strokesMoveAppend(u,v);
        this->strokesMoveFinish(stack);
        break;
    default:
        break;
    }
}

void entity::UserScene::editStrokesScale(QUndoStack *stack, double u, double v, dureu::EVENT event)
{
    if (!stack){
        fatalMsg("editStrokesScale(): undo stack is NULL.");
        return;
    }
    switch (event){
    case dureu::EVENT_OFF:
        outLogMsg("EditStrokesScale: event off called");
        if (this->strokesSelectedValid()){
            outLogMsg("EditStrokesScale: event off performed");
            this->strokesScaleFinish(stack);
        }
        break;
    case dureu::EVENT_PRESSED:
        outLogMsg("EditStrokesScale: event pressed called");
        this->strokesScaleStart(u,v);
        this->strokesScaleAppend(u,v);
        break;
    case dureu::EVENT_DRAGGED:
        if (!this->strokesSelectedValid())
            this->strokesScaleStart(u,v);
        this->strokesScaleAppend(u,v);
        break;
    case dureu::EVENT_RELEASED:
        if (!this->strokesSelectedValid())
            break;
        outLogMsg("EditStrokesScale: event release called");
        this->strokesScaleAppend(u,v);
        this->strokesScaleFinish(stack);
        break;
    default:
        break;
    }
}

void entity::UserScene::editStrokesRotate(QUndoStack *stack, double u, double v, dureu::EVENT event)
{
    if (!stack){
        fatalMsg("editStrokesScale(): undo stack is NULL.");
        return;
    }
    switch (event){
    case dureu::EVENT_OFF:
        outLogMsg("EditStrokesScale: event off called");
        if (this->strokesSelectedValid()){
            outLogMsg("EditStrokesScale: event off performed");
            this->strokesRotateFinish(stack);
        }
        break;
    case dureu::EVENT_PRESSED:
        outLogMsg("EditStrokesScale: event pressed called");
        this->strokesRotateStart(u,v);
        this->strokesRotateAppend(u,v);
        break;
    case dureu::EVENT_DRAGGED:
        if (!this->strokesSelectedValid())
            this->strokesRotateStart(u,v);
        this->strokesRotateAppend(u,v);
        break;
    case dureu::EVENT_RELEASED:
        if (!this->strokesSelectedValid())
            break;
        outLogMsg("EditStrokesScale: event release called");
        this->strokesRotateAppend(u,v);
        this->strokesRotateFinish(stack);
        break;
    default:
        break;
    }
}

void entity::UserScene::editStrokeDelete(QUndoStack *stack, entity::Stroke *stroke)
{
    if (!stack){
        outErrMsg("editStrokeDelete: stack is NULL");
        return;
    }
    if (!stroke){
        outErrMsg("editStrokeDelete: stroke is NULL");
        return;
    }
    if (!this->getCanvasCurrent()->getGeodeData()->containsDrawable(stroke)){
        outErrMsg("editStrokeDelete: current canvas does not contain that stroke."
                  "Deletion is not possible.");
        return;
    }
    EditStrokeDeleteCommand* cmd = new EditStrokeDeleteCommand(this, m_canvasCurrent.get(), stroke);
    if (!cmd){
        outErrMsg("editStrokeDelete: undo/redo command is NULL");
        return;
    }
    stack->push(cmd);
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
    /* clear SG content */
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

void entity::UserScene::updateWidgets()
{
    emit sendRequestUpdate();
}

std::string entity::UserScene::getCanvasName()
{
    return this->getEntityName(dureu::NAME_CANVAS, m_idCanvas++);
}

std::string entity::UserScene::getPhotoName()
{
    return this->getEntityName(dureu::NAME_PHOTO, m_idPhoto++);
}

std::string entity::UserScene::getBookmarkName()
{
    return this->getEntityName(dureu::NAME_BOOKMARK, m_idBookmark++);
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
        this->updateWidgets();
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

void entity::UserScene::strokesMoveStart(double u, double v)
{
    m_u = u;
    m_v = v;
    m_du = m_dv = 0;
    m_inits = true;
}

void entity::UserScene::strokesMoveAppend(double u, double v)
{
    double du = u - m_u;
    double dv = v - m_v;

    /* perform delta movement */
    m_canvasCurrent->moveStrokesSelected(du, dv);

    this->updateWidgets();
    m_du += du;
    m_dv += dv;
    m_u = u;
    m_v = v;
}

void entity::UserScene::strokesMoveFinish(QUndoStack *stack)
{
    /* move things back so that to perform this operation in undo/redo FW */
    m_canvasCurrent->moveStrokesSelected(-m_du, -m_dv);

    EditStrokesMoveCommand* cmd = new EditStrokesMoveCommand(this,
                                                             m_canvasCurrent->getStrokesSelected(),
                                                             m_canvasCurrent.get(),
                                                             m_du, m_dv);
    if (!cmd){
        outErrMsg("strokeMoveFinish: Could not allocate command");
        return;
    }
    stack->push(cmd);

    m_u = m_v = 0;
    m_du = m_dv = 0;
    //m_canvasCurrent->resetStrokesSelected();
    m_inits = false;
}

bool entity::UserScene::strokesSelectedValid() const
{
    return ((m_canvasCurrent->getStrokesSelectedSize() > 0? true : false) && m_inits);
}

void entity::UserScene::strokesScaleStart(double u, double v)
{
    osg::Vec3f center = m_canvasCurrent->getGeodeData()->getBoundingBox().center(); //m_canvasCurrent->getCenter();
    m_du = center.x();
    m_dv = center.y();
    m_v = center.x();

    m_scale = 1;
    m_u = std::fabs(u-m_v);
    m_inits = true;
}

void entity::UserScene::strokesScaleAppend(double u, double v)
{
    double s = 1;
    // make sure it's not smaller than allowed
    if (m_u != 0 && std::fabs(u-m_v) >= 0.1)
        s = std::fabs(u-m_v)/m_u;

    outLogVal("s", s);

    m_scale *= s;
    m_canvasCurrent->scaleStrokesSelected(s, osg::Vec3f(m_du, m_dv, 0));

    this->updateWidgets();
    m_u = std::fabs(u-m_v);
}

void entity::UserScene::strokesScaleFinish(QUndoStack *stack)
{
    m_canvasCurrent->scaleStrokesSelected(1/m_scale, osg::Vec3f(m_du, m_dv, 0));

    EditStrokesScaleCommand* cmd = new EditStrokesScaleCommand(this,
                                                               m_canvasCurrent->getStrokesSelected(),
                                                               m_canvasCurrent.get(),
                                                               m_scale, osg::Vec3f(m_du, m_dv, 0));
    m_u = m_v = 0;
    m_du = m_dv = 0;
    m_inits = false;
    m_scale = 1;

    if (!cmd){
        outErrMsg("strokeScaleFinish: Could not allocate command");
        return;
    }
    stack->push(cmd);

}

void entity::UserScene::strokesRotateStart(double u, double v)
{
    osg::Vec3f center = m_canvasCurrent->getGeodeData()->getBoundingBox().center();
    if (center.z() > dureu::EPSILON){
        outLogVec("center", center.x(), center.y(), center.z());
        outErrMsg("strokesRotateStart: z coordiante is not close to zero");
    }

    m_u = u;
    m_v = v;
    m_du = center.x();
    m_dv = center.y();
    m_rotate = 0;
    m_inits = true;
}

void entity::UserScene::strokesRotateAppend(double u, double v)
{
    double cx = m_du;
    double cy = m_dv;

    osg::Vec2f P1 = osg::Vec2f(m_u - cx, m_v - cy);
    osg::Vec2f P2 = osg::Vec2f(u - cx, v - cy);
    P1.normalize();
    P2.normalize();

    double theta = 0;
    if (P1.length()*P2.length() != 0){
        double atheta = (P1*P2) / (P1.length()*P2.length());
        if (atheta <-1 || atheta > 1)
            atheta = 1;
        theta = std::acos(atheta);

        /* when moving counter clock wise, have to switch rotation direction */
        if (u>=cx && v>=cy){
            if (P2.y() < P1.y())
                theta *= -1; }
        else if (u<=cx && v>=cy){
            if (P2.y() > P1.y())
                theta *= -1;}
        else if (u<=cx && v<=cy){
            if (P2.y() > P1.y())
                theta *= -1; }
        else if (u>=cx && v<=cy){
            if (P2.y() < P1.y())
                theta *= -1; }
    }

    /* theta must be within [-PI, PI] */
    if (theta < -dureu::PI || theta > dureu::PI)
        theta = 0;

    outLogVal("theta", theta);

    /* now rotate stroke on theta around center of coords [m_du m_dv] */
    m_canvasCurrent->rotateStrokesSelected(theta, osg::Vec3f(m_du, m_dv, 0));

    m_rotate += theta;
    m_u = u;
    m_v = v;

    this->updateWidgets();
}

void entity::UserScene::strokesRotateFinish(QUndoStack *stack)
{
    m_canvasCurrent->rotateStrokesSelected(-m_rotate, osg::Vec3f(m_du, m_dv, 0));

    EditStrokesRotateCommand* cmd = new EditStrokesRotateCommand(this,
                                                                 m_canvasCurrent->getStrokesSelected(),
                                                                 m_canvasCurrent.get(),
                                                                 m_rotate, osg::Vec3f(m_du, m_dv, 0));
    m_u = m_v = 0;
    m_du = m_dv = 0;
    m_rotate = 0;
    m_inits = false;

    if (!cmd){
        outErrMsg("strokesRotateFinish: Could not allocate command");
        return;
    }
    stack->push(cmd);
}

void entity::UserScene::eraseStart(entity::Stroke *stroke, osg::Vec3d &hit)
{

}

void entity::UserScene::eraseAppend(entity::Stroke *stroke, osg::Vec3d &hit)
{

}

void entity::UserScene::eraseFinish(QUndoStack *stack, entity::Stroke *stroke)
{
    if (!this->eraseValid(stroke)){
        outErrMsg("eraseFinish: stroke ptr is NULL, impossible to finish erase");
        return;
    }

}

bool entity::UserScene::eraseValid(Stroke *stroke) const
{
    return stroke;
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
    this->updateWidgets();
}

void entity::UserScene::canvasOffsetFinish(QUndoStack *stack)
{
    if (!this->canvasEditValid()){
        outErrMsg("canvasOffsetFinish: no canvas in edit mode, impossible to finish offset mode");
        return;
    }
    m_canvasCurrent->setModeEdit(false);
    m_canvasCurrent->translate(osg::Matrix::translate(-m_deltaT.x(), -m_deltaT.y(), -m_deltaT.z()));
    EditCanvasOffsetCommand* cmd = new EditCanvasOffsetCommand(this, m_deltaT);
    stack->push(cmd);
    m_deltaT = osg::Vec3f(0.f,0.f,0.f);
}

bool entity::UserScene::canvasEditValid() const
{
    return this->getCanvasCurrent()->getModeEdit();
}

void entity::UserScene::canvasCloneStart()
{
    entity::Canvas* cnv = m_canvasCurrent->clone();
    cnv->setName(this->getCanvasName());
    if (!cnv){
        outErrMsg("canvasCloneStart: could not clone the canvas, ptr is NULL");
        return;
    }
    m_canvasClone = cnv;

    if (!this->addChild(m_canvasClone.get())){
        outErrMsg("canvasCloneStart: could not add clone as a child");
        return;
    }

    /* have to set the clone as current so that to calculate offset correctly */
    if (!this->setCanvasCurrent(m_canvasClone.get())){
        outErrMsg("canvasCloneStart: could not set clone as current");
        this->removeChild(m_canvasClone.get());
        m_canvasClone = 0;
        return;
    }
}

void entity::UserScene::canvasCloneAppend(const osg::Vec3f &t)
{
    if (!this->canvasCloneValid()){
        outErrMsg("canvasCloneAppend: clone is not valid");
        return;
    }

    /* clone is also current */
    m_canvasCurrent->translate(osg::Matrix::translate(t.x(), t.y(), t.z()));
    this->updateWidgets();
}

void entity::UserScene::canvasCloneFinish(QUndoStack *stack)
{
    if (!this->canvasCloneValid()){
        outErrMsg("canvasCloneFinish: clone is not valid");
        return;
    }

    this->setCanvasCurrent(m_canvasPrevious.get());

    AddCanvasCommand* cmd = new AddCanvasCommand(this, *(m_canvasClone.get()));
    this->removeChild(m_canvasClone.get());
    m_canvasClone = 0;
    if (!cmd){
        outErrMsg("canvasCloneFinish: could not allocated AddCanvasCommand");
        return;
    }
    stack->push(cmd);
}

bool entity::UserScene::canvasCloneValid() const
{
    return m_canvasClone.get();
}

void entity::UserScene::canvasRotateStart()
{
    if (this->canvasEditValid()){
        outErrMsg("CanvasRotateStart: cannot start editing since the canvas is already in edit mode");
        return;
    }
    if (!m_canvasCurrent.get()){
        outErrMsg("canvasRotateStart: current canvas is NULL");
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
    if (!m_canvasCurrent.get()){
        outErrMsg("canvasRotateAppend: canvas is NULL");
        return;
    }

    m_canvasCurrent->rotate(osg::Matrix::rotate(r));
    m_deltaR = r * m_deltaR;
    this->updateWidgets();
}

void entity::UserScene::canvasRotateFinish(QUndoStack *stack)
{
    if (!this->canvasEditValid()){
        outErrMsg("canvasRotateFinish: no canvas in edit mode, impossible to finish offset mode");
        return;
    }
    m_canvasCurrent->setModeEdit(false);
    m_canvasCurrent->rotate(osg::Matrix::rotate(m_deltaR.inverse()));
    EditCanvasRotateCommand* cmd = new EditCanvasRotateCommand(this, m_deltaR);
    stack->push(cmd);
    m_deltaR = osg::Quat(0,0,0,1);
}

void entity::UserScene::photoMoveStart(Photo *photo)
{
    if (this->canvasEditValid()){
        outErrMsg("photoMoveStart: cannot start editing since the photo is already in edit mode");
        return;
    }
    if (!photo){
        outErrMsg("photoMoveStart(): photo pointer is NULL");
        return;
    }

    this->getCanvasCurrent()->setPhotoCurrent(photo);
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
    this->updateWidgets();
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
    EditPhotoMoveCommand* cmd = new EditPhotoMoveCommand(this, u, v);
    stack->push(cmd);
    m_u = 0;
    m_v = 0;
    this->getCanvasCurrent()->setPhotoCurrent(false);
}

bool entity::UserScene::photoEditValid() const
{
    if (this->getCanvasCurrent()){
        entity::Photo* photo = this->getCanvasCurrent()->getPhotoCurrent();
        if (!photo)
            return false;
    }
    return this->getCanvasCurrent()->getPhotoCurrent()->getModeEdit();
}

void entity::UserScene::photoScaleStart(entity::Photo *photo)
{
    if (this->canvasEditValid()){
        outErrMsg("photoScaleStart: cannot start editing since the photo is already in edit mode");
        return;
    }
    if (!photo){
        outErrMsg("photoScaleStart: photo ptr is NULL");
        return;
    }
    this->getCanvasCurrent()->setPhotoCurrent(photo);
    photo->setModeEdit(true);
    m_scale = 1;
    m_u = photo->getWidth();
    m_v = photo->getHeight();
}

void entity::UserScene::photoScaleAppend(double u, double v)
{
    entity::Photo* photo = this->getCanvasCurrent()->getPhotoCurrent();
    if (!photo){
        outErrMsg("photoScaleAppend(): photo pointer is NULL");
        return;
    }
    double s = 1;

    /* make sure the scaling down does not exceed the minimal photo size */
    if (u > photo->getCenter().x() - photo->getWidth() &&
            std::fabs(u - photo->getCenter().x() + photo->getWidth()) >= dureu::PHOTO_MINW)
        s = std::fabs(u - photo->getCenter().x() + photo->getWidth()) / std::fabs(2.f*photo->getWidth());
    m_scale *= s;
    photo->scale(s, s, photo->getCenter());
    this->updateWidgets();
}

void entity::UserScene::photoScaleFinish(QUndoStack *stack, double u, double v)
{
    entity::Photo* photo = this->getCanvasCurrent()->getPhotoCurrent();
    if (!photo){
        outErrMsg("photoScaleFinish(): photo pointer is NULL");
        return;
    }
    photo->setModeEdit(false);
    photo->setWidth(m_u);
    photo->setHeight(m_v);
    /* push to stack */
    EditPhotoScaleCommand* cmd = new EditPhotoScaleCommand(this, m_scale);
    m_u = m_v = 0;
    this->getCanvasCurrent()->setPhotoCurrent(false);
    if (!cmd){
        outErrMsg("photoScaleFinish(): could not allocate command");
        return;
    }
    stack->push(cmd);
}

void entity::UserScene::photoRotateStart(entity::Photo *photo, double u, double v)
{
    if (this->canvasEditValid()){
        outErrMsg("photoScaleStart: cannot start editing since the photo is already in edit mode");
        return;
    }
    if (!photo){
        outErrMsg("photoScaleStart: photo ptr is NULL");
        return;
    }
    this->getCanvasCurrent()->setPhotoCurrent(photo);
    photo->setModeEdit(true);
    m_rotate = 0;
    m_u = u;
    m_v = v;
}

void entity::UserScene::photoRotateAppend(double u, double v)
{
    entity::Photo* photo = this->getCanvasCurrent()->getPhotoCurrent();
    if (!photo){
        outErrMsg("photoScaleAppend(): photo pointer is NULL");
        return;
    }
    double cx = photo->getCenter().x();
    double cy = photo->getCenter().y();

    osg::Vec2f P1 = osg::Vec2f(m_u - cx, m_v - cy);
    osg::Vec2f P2 = osg::Vec2f(u - cx, v - cy);

    P1.normalize();
    P2.normalize();

    double theta = 0;
    if (P1.length()*P2.length() != 0){
        double atheta = (P1*P2) / (P1.length()*P2.length());
        if (atheta <-1 || atheta > 1)
            atheta = 1;
        theta = std::acos(atheta);

        /* when moving counter clock wise, have to switch rotation direction */
        if (u>=cx && v>=cy){
            if (P2.y() < P1.y())
                theta *= -1; }
        else if (u<=cx && v>=cy){
            if (P2.y() > P1.y())
                theta *= -1;}
        else if (u<=cx && v<=cy){
            if (P2.y() > P1.y())
                theta *= -1; }
        else if (u>=cx && v<=cy){
            if (P2.y() < P1.y())
                theta *= -1; }
    }

    /* theta must be within [-PI, PI] */
    if (theta < -dureu::PI || theta > dureu::PI)
        theta = 0;

    m_rotate += theta;
    photo->rotate(theta);

    m_u = u;
    m_v = v;
    this->updateWidgets();
}

void entity::UserScene::photoRotateFinish(QUndoStack *stack, double u, double v)
{
    entity::Photo* photo = this->getCanvasCurrent()->getPhotoCurrent();
    if (!photo){
        outErrMsg("photoScaleFinish(): photo pointer is NULL");
        return;
    }
    photo->setModeEdit(false);
    photo->rotate(-m_rotate);
    /* create command */
    EditPhotoRotateCommand* cmd = new EditPhotoRotateCommand(this, m_rotate);
    m_u = m_v = 0;
    this->getCanvasCurrent()->setPhotoCurrent(false);
    /* push to stack */
    if (!cmd){
        outErrMsg("photoScaleFinish(): could not allocate command");
        return;
    }
    stack->push(cmd);
}

REGISTER_OBJECT_WRAPPER(UserScene_Wrapper
                        , new entity::UserScene
                        , entity::UserScene
                        , "osg::Object osg::Group entity::UserScene")
{
    ADD_OBJECT_SERIALIZER(Bookmarks, entity::Bookmarks, NULL);
    ADD_UINT_SERIALIZER(IdCanvas, 0);
    ADD_UINT_SERIALIZER(IdPhoto, 0);
    ADD_UINT_SERIALIZER(IdBookmark, 0);
    ADD_STRING_SERIALIZER(FilePath, "");
}
