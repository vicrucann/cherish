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
    , m_groupCanvases(new osg::Group)
    , m_groupBookmarks(new entity::Bookmarks)
    , m_canvasCurrent(NULL)
    , m_canvasPrevious(NULL)
    , m_canvasClone(0)
    , m_deltaT(osg::Vec3f(0.f,0.f,0.f))
    , m_deltaR(osg::Quat(0,0,0,1))
    , m_u(0)
    , m_v(0)
    , m_inits(false)
    , m_du(0)
    , m_dv(0)
    , m_scaleX(1)
    , m_scaleY(1)
    , m_rotate(0)
    , m_idCanvas(0)
    , m_idPhoto(0)
    , m_idBookmark(0)
    , m_filePath("")
{
    this->setName("UserScene");
    m_groupBookmarks->setName("groupBookmarks");
    m_groupCanvases->setName("groupCanvases");
}

entity::UserScene::UserScene(const entity::UserScene& scene, const osg::CopyOp& copyop)
    : osg::Group(scene, copyop)
    , m_groupCanvases(scene.m_groupCanvases)
    , m_groupBookmarks(scene.m_groupBookmarks)
    , m_canvasCurrent(scene.m_canvasCurrent)
    , m_canvasPrevious(scene.m_canvasPrevious)
    , m_canvasClone(0)
    , m_deltaT(scene.m_deltaT)
    , m_deltaR(scene.m_deltaR)
    , m_u(scene.m_u)
    , m_v(scene.m_v)
    , m_inits(scene.m_inits)
    , m_du(scene.m_du)
    , m_dv(scene.m_dv)
    , m_scaleX(scene.m_scaleX)
    , m_scaleY(scene.m_scaleY)
    , m_rotate(scene.m_rotate)
    , m_idCanvas(scene.m_idCanvas)
    , m_idPhoto(scene.m_idPhoto)
    , m_idBookmark(scene.m_idBookmark)
    , m_filePath(scene.m_filePath)
{
}

void entity::UserScene::initializeSG()
{
    this->addChild(m_groupBookmarks);
    this->addChild(m_groupCanvases);
}

void entity::UserScene::setGroupCanvases(osg::Group *group)
{
    m_groupCanvases = group;
}

const osg::Group *entity::UserScene::getGroupCanvases() const
{
    return m_groupCanvases.get();
}

void entity::UserScene::setBookmarks(entity::Bookmarks *group)
{
    m_groupBookmarks = group;
}

const entity::Bookmarks *entity::UserScene::getBookmarks() const
{
    return m_groupBookmarks;
}

entity::Bookmarks *entity::UserScene::getBookmarksModel() const
{
    return m_groupBookmarks.get();
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
    this->addCanvas(stack, R,T, getEntityName(cher::NAME_CANVAS, m_idCanvas++));
}

void entity::UserScene::addCanvas(QUndoStack *stack, const osg::Vec3f &normal, const osg::Vec3f &center)
{
    if (!stack){
        outErrMsg("addCanvas(): undo stack is NULL, Canvas will not be added. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }
    AddCanvasCommand* cmd = new AddCanvasCommand(this, normal, center,
                                                 getEntityName(cher::NAME_CANVAS, m_idCanvas++));
    if (!cmd){
        outErrMsg("addCanvas: cmd is NULL");
        return;
    }
    stack->push(cmd);
}

void entity::UserScene::addCanvas(QUndoStack* stack, const osg::Matrix& R, const osg::Matrix& T, const std::string& name)
{
    if (!stack){
        outErrMsg("addCanvas(): undo stack is NULL, Canvas will not be added. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }
    AddCanvasCommand* cmd = new AddCanvasCommand(this, R, T, name);
    if (!cmd){
        outErrMsg("addCanvas: cmd is NULL");
        return;
    }
    stack->push(cmd);

}

void entity::UserScene::addStroke(QUndoStack* stack, float u, float v, cher::EVENT event)
{
    if (!stack){
        outErrMsg("addStroke(): undo stack is NULL, it is not initialized. "
                 "Sketching is not possible. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }
    switch (event){
    case cher::EVENT_OFF:
        outLogMsg("EVENT_OFF");
        this->strokeFinish(stack);
        break;
    case cher::EVENT_PRESSED:
        outLogMsg("EVENT_PRESSED");
        this->strokeStart();
        this->strokeAppend(u, v);
        break;
    case cher::EVENT_DRAGGED:
        if (!this->strokeValid())
            this->strokeStart();
        this->strokeAppend(u, v);
        break;
    case cher::EVENT_RELEASED:
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
        outErrMsg("addPhoto(): undo stack is NULL, Canvas will not be added. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }
    AddPhotoCommand* cmd = new AddPhotoCommand(this, fname, this->getPhotoName());
    if (!cmd){
        outErrMsg("addPhoto(): could not allocate AddPhotoCommand.");
        return;
    }
    stack->push(cmd);
}

void entity::UserScene::addBookmark(BookmarkWidget *widget, const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up, const double &fov)
{
    if (!m_groupBookmarks.get())
        qFatal("UserScene::addBookmark() - bookmarks were not initialized properly");

    m_groupBookmarks->addBookmark(widget, eye, center, up, this->getBookmarkName(), fov);
}

void entity::UserScene::updateBookmark(BookmarkWidget *widget, int row)
{
    if (!m_groupBookmarks.get())
        qFatal("UserScene::updateBookmark() - bookmarks were not initialized properly");

    m_groupBookmarks->updateBookmark(widget, row);
}

void entity::UserScene::deleteBookmark(BookmarkWidget *widget, const QModelIndex &index)
{
    if (!m_groupBookmarks.get())
        qFatal("UserScene::deleteBookmark() - bookmarks were not initialized properly");

    m_groupBookmarks->deleteBookmark(widget, index);
}

void entity::UserScene::eraseStroke(QUndoStack *stack, entity::Stroke *stroke, int first, int last, cher::EVENT event)
{
    if (!stack){
        outErrMsg("eraseStroke(): undo stack is NULL, it is not initialized. "
                 "Erase is not possible. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }

    stroke->removePoints(first, last);

    switch (event){
    case cher::EVENT_OFF:
        outLogMsg("EVENT_OFF");
        break;
    case cher::EVENT_PRESSED:
        outLogMsg("EVENT_PRESSED");
        break;
    case cher::EVENT_DRAGGED:
        break;
    case cher::EVENT_RELEASED:
        outLogMsg("EVENT_RELEASED");
        break;
    default:
        break;
    }
}

entity::Canvas* entity::UserScene::getCanvas(unsigned int id)
{
    if (!m_groupCanvases.get())
        qFatal("UserScene::getCanvas() canvas group is NULL");
    return dynamic_cast<entity::Canvas*>(m_groupCanvases->getChild(id));
}

entity::Canvas* entity::UserScene::getCanvas(const std::string& name)
{
    FindNodeVisitor fnv(name);
    this->accept(fnv);
    if (fnv.getNode() == NULL){
        qDebug("UserScene::getCanvas() no entity with such name exists within the scene graph");
        return NULL;
    }
    return dynamic_cast<entity::Canvas*>(fnv.getNode());
}

/* to use in EventHandler */
int entity::UserScene::getStrokeLevel() const
{
    return this->getCanvasLevel() + 4;
}

/* to use in EventHandler */
int entity::UserScene::getCanvasLevel() const
{
    return 4;
}

/* to use in EventHandler */
int entity::UserScene::getPhotoLevel() const
{
    return this->getCanvasLevel() + 4;
}

bool entity::UserScene::setCanvasCurrent(entity::Canvas* cnv)
{
    // if current and previous are equal, search for the nearest
    // valiable candidate to assign the previous to;
    // if no canvases available at all, the observer ptrs are set to NULL
    if (cnv == m_canvasCurrent.get()){
        m_canvasCurrent->setColor(cher::CANVAS_CLR_CURRENT);
        emit this->canvasSelectedColor(this->getCanvasIndex(m_canvasCurrent.get()), 1);
        return true;
    }
    if (m_canvasCurrent.valid()){
        if (m_canvasPrevious.valid()){
            m_canvasPrevious->setColor(cher::CANVAS_CLR_REST);
            emit this->canvasSelectedColor(this->getCanvasIndex(m_canvasPrevious.get()), 0);
            m_canvasPrevious = NULL;
        }
        m_canvasCurrent->setColor(cher::CANVAS_CLR_PREVIOUS);
        emit this->canvasSelectedColor(this->getCanvasIndex(m_canvasCurrent.get()), 2);
        m_canvasCurrent->unselectAll();

        m_canvasPrevious = m_canvasCurrent;
        m_canvasCurrent = NULL;
    }
    if (!cnv){
        std::cerr << "setCanvasCurrent(): The input canvas pointer is NULL, no current canvas is assigned" << std::endl;
        return false;
    }
    m_canvasCurrent = cnv;
    m_canvasCurrent->setColor(cher::CANVAS_CLR_CURRENT);
    emit this->canvasSelectedColor(this->getCanvasIndex(m_canvasCurrent.get()), 1);

    /* make sure node masks are holding as before for the current canvas */
    if (m_canvasPrevious.get()){
        if (!this->getCanvasesButCurrent()){
            m_canvasCurrent->setNodeMask(cher::MASK_ALL_IN);
            m_canvasPrevious->setNodeMask(cher::MASK_CANVAS_OUT);
        }
    }

    /* update frames of current and previous so that intersection drawable is updated */
    if (m_canvasPrevious.get()) m_canvasPrevious->updateFrame();
    m_canvasCurrent->updateFrame(m_canvasPrevious.get());

    return true;
}

bool entity::UserScene::setCanvasPrevious(entity::Canvas* cnv)
{
    if (cnv == m_canvasPrevious.get())
        return true;
    if (m_canvasPrevious.valid()){
        m_canvasPrevious->setColor(cher::CANVAS_CLR_REST);
        emit this->canvasSelectedColor(this->getCanvasIndex(m_canvasPrevious.get()), 0);
        m_canvasPrevious = NULL;
    }
    if (!cnv){
        m_canvasPrevious = 0;
        if (m_canvasCurrent.get()) m_canvasCurrent->updateFrame(0);
        return true;
    }
    m_canvasPrevious = cnv;
    m_canvasPrevious->setColor(cher::CANVAS_CLR_PREVIOUS);
    emit this->canvasSelectedColor(this->getCanvasIndex(m_canvasPrevious.get()), 2);
    m_canvasPrevious->updateFrame();
    m_canvasCurrent->updateFrame(m_canvasPrevious.get());
    return true;
}

void entity::UserScene::setCanvasesButCurrent(bool enabled)
{
    if (!m_canvasCurrent.get())
        return;

    for (int i=0; i<this->getNumCanvases(); ++i){
        entity::Canvas* cnv = this->getCanvas(i);
        if (!cnv) return;
        if (cnv != m_canvasCurrent.get()){
            if (enabled)
                cnv->setNodeMask(cher::MASK_CANVAS_IN);
            else
                cnv->setNodeMask(cher::MASK_CANVAS_OUT); // see EventHandler when we set iv.setTraversalMask(~0x1);
        }
    }
}

bool entity::UserScene::getCanvasesButCurrent() const
{
    if (!m_canvasCurrent.get() || !m_canvasPrevious.get())
        return false;

    return m_canvasCurrent->getNodeMask() == m_canvasPrevious->getNodeMask();
}

entity::Canvas*entity::UserScene::getCanvasCurrent() const
{
    return m_canvasCurrent.get();
}

entity::Canvas*entity::UserScene::getCanvasPrevious() const
{
    return m_canvasPrevious.get();
}

int entity::UserScene::getCanvasIndex(entity::Canvas *canvas) const
{
    if (!m_groupCanvases.get())
        return -1;
    return m_groupCanvases->getChildIndex(canvas);
}

int entity::UserScene::getPhotoIndex(entity::Photo *photo, Canvas *canvas) const
{
    int idx = -1;
    if (!canvas->getGroupData()) return idx;
    for (int i=0; i< canvas->getNumPhotos(); ++i){
        entity::Photo* pi = canvas->getPhotoFromIndex(i);
        if (!pi) qFatal("UserScene::getPhotoFromIndex() failed");
        idx++;
        if (pi == photo) break;
    }
    return idx;
}

entity::Canvas *entity::UserScene::getCanvasFromIndex(int row)
{
    if (!m_groupCanvases.get())
        return 0;
    return dynamic_cast<entity::Canvas*>(m_groupCanvases->getChild(row));
}

entity::Photo *entity::UserScene::getPhotoFromIndex(Canvas *canvas, int row)
{
    if (!canvas) return NULL;
    return canvas->getPhotoFromIndex(row);
}

int entity::UserScene::getNumCanvases() const
{
    return m_groupCanvases->getNumChildren();
}

int entity::UserScene::getNumPhotos(entity::Canvas *canvas) const
{
    return canvas->getNumPhotos();
}

int entity::UserScene::getNumPhotos()
{
    int res = 0;
    for (int i=0; i<this->getNumCanvases(); ++i){
        entity::Canvas* cnv = this->getCanvas(i);
        if (!cnv) continue;
        res += cnv->getNumPhotos();
    }
    return res;
}

int entity::UserScene::getNumPhotosTill(entity::Canvas *canvas)
{
    int result = 0;
    if (!canvas) {
        qWarning("UserScene::getNumPhotosTill() - input canvas is NULL");
        return result;
    }

    for (int i=0; i<this->getNumCanvases(); ++i){
        entity::Canvas* cnv = this->getCanvas(i);
        if (!cnv) qFatal("UserScene::getNumPhotosTill() - one of the canvases is NULL");

        if (canvas == cnv)
            break;
        result += cnv->getNumPhotos();
    }
    return result;
}

void entity::UserScene::editCanvasOffset(QUndoStack* stack, const osg::Vec3f& translate, cher::EVENT event)
{
    if (!stack) qFatal("editCanvasOffset(): undo stack is NULL, it is not initialized. Editing is not possible.");

    switch (event){
    case cher::EVENT_OFF:
        this->canvasOffsetFinish(stack);
        break;
    case cher::EVENT_PRESSED:
        this->canvasOffsetStart();
        this->canvasOffsetAppend(translate);
        break;
    case cher::EVENT_DRAGGED:
        if (!this->canvasEditValid())
            this->canvasOffsetStart();
        this->canvasOffsetAppend(translate);
        break;
    case cher::EVENT_RELEASED:
        if (!this->canvasEditValid())
            break;
        this->canvasOffsetAppend(translate);
        this->canvasOffsetFinish(stack);
        break;
    default:
        break;
    }
}

void entity::UserScene::editCanvasRotate(QUndoStack* stack, const osg::Quat& rotation, const osg::Vec3f &center3d, cher::EVENT event)
{
    if (!stack) qFatal("editCanvasRotate(): undo stack is NULL, it is not initialized. Editing is not possible.");

    switch (event){
    case cher::EVENT_OFF:
        outLogMsg("editCanvasRotate: OFF");
        this->canvasRotateFinish(stack);
        break;
    case cher::EVENT_PRESSED:
        outLogMsg("editCanvasRotate: pressed");
        this->canvasRotateStart();
        this->canvasRotateAppend(rotation, center3d);
        break;
    case cher::EVENT_DRAGGED:
        if (!this->canvasEditValid())
            this->canvasRotateStart();
        this->canvasRotateAppend(rotation, center3d);
        break;
    case cher::EVENT_RELEASED:
        outLogMsg("editCanvasRotate: release");
        if (!this->canvasEditValid())
            break;
        outLogMsg("editCanvasRotate: released");
        this->canvasRotateAppend(rotation, center3d);
        this->canvasRotateFinish(stack);
        break;
    default:
        break;
    }
}

void entity::UserScene::editCanvasClone(QUndoStack *stack, const osg::Vec3f &translate, cher::EVENT event)
{
    if (!stack) qFatal("editCanvasClone(): undo stack is NULL, it is not initialized. Editing is not possible.");

    switch (event){
    case cher::EVENT_OFF:
        this->canvasCloneFinish(stack);
        break;
    case cher::EVENT_PRESSED:
        this->canvasCloneStart();
        this->canvasCloneAppend(translate);
        break;
    case cher::EVENT_DRAGGED:
        if (!this->canvasCloneValid())
            this->canvasCloneStart();
        this->canvasCloneAppend(translate);
        break;
    case cher::EVENT_RELEASED:
        if (!this->canvasCloneValid())
            break;
        this->canvasCloneAppend(translate);
        this->canvasCloneFinish(stack);
        break;
    default:
        break;
    }
}

void entity::UserScene::editCanvasSeparate(QUndoStack *stack, const osg::Vec3f &translate, cher::EVENT event)
{
    if (!stack) qFatal("editCanvasSeparate(): undo stack is NULL, it is not initialized. Editing is not possible.");

    switch (event){
    case cher::EVENT_OFF:
        this->canvasSeparateFinish(stack);
        break;
    case cher::EVENT_PRESSED:
        this->canvasSeparateStart();
        this->canvasSeparateAppend(translate);
        break;
    case cher::EVENT_DRAGGED:
        if (!this->canvasCloneValid())
            this->canvasSeparateStart();
        this->canvasSeparateAppend(translate);
        break;
    case cher::EVENT_RELEASED:
        if (!this->canvasCloneValid())
            break;
        this->canvasSeparateAppend(translate);
        this->canvasSeparateFinish(stack);
        break;
    default:
        break;
    }
}

void entity::UserScene::editCanvasDelete(QUndoStack *stack, entity::Canvas *canvas)
{
    qDebug() << "Attempting to delete: " << canvas->getName().c_str();
    if (!stack) qFatal("editCanvasDelete(): undo stack is NULL, it is not initialized. Editing is not possible.");
    if (!canvas){
        qCritical("editCanvasDelete: canvas is NULL");
        return;
    }

    EditCanvasDeleteCommand* cmd = new EditCanvasDeleteCommand(this, canvas);
    if (!cmd){
        qCritical("editCanvasDelete: could not allocate DeleteCommand");
        return;
    }
    stack->push(cmd);
}

void entity::UserScene::editPhotoDelete(QUndoStack *stack, entity::Photo *photo, Canvas *canvas)
{
    if (!stack) qFatal("editPhotoDelete(): undo stack is NULL, it is not initialized. Editing is not possible.");
    if (!photo) {
        qCritical("editPhotoDelete: photo is NULL");
        return;
    }

    if (!canvas->getGroupData()->containsDrawable(photo)){
        qCritical("editPhotoDelete: current canvas does not contain that photo."
                  "Deletion is not possible.");
        return;
    }

    EditPhotoDeleteCommand* cmd = new EditPhotoDeleteCommand(this, canvas, photo);
    if (!cmd){
        qCritical("editPhotoDelete: undo/redo command is NULL");
        return;
    }
    stack->push(cmd);
}

void entity::UserScene::editPhotoPush(QUndoStack *stack, entity::Photo *photo, Canvas *source, Canvas *destination)
{
    if (!stack) qFatal("editPhotoPush(): undo stack is NULL, it is not initialized. Editing is not possible.");
    if (!photo || !destination || !source){
        qCritical("editPhotoPush: pointers are NULL");
        return;
    }


    if (!source->getGroupData()->containsDrawable(photo)){
        qCritical("editPhotoDelete: source canvas does not contain that photo."
                          "Scene movement is not possible.");
                return;
    }

    EditPhotoPushCommand* cmd = new EditPhotoPushCommand(this, source, destination, photo);
    if (!cmd){
        qCritical("editPhotoPush: undo/redo command is NULL");
        return;}
    stack->push(cmd);
}

void entity::UserScene::editPhotoTransparency(entity::Photo *photo, entity::Canvas *canvas, float t)
{
    if (!photo || !canvas) return;
    // TODO:
    // if (canvas->containsPhoto(photo))
    photo->setTransparency(t);
    int index = this->getPhotoIndex(photo, canvas);
    for (int i=0; i<m_groupBookmarks->getNumBookmarks(); ++i){
        entity::SceneState* state = m_groupBookmarks->getSceneState(i);
        if (!state) {
            qCritical("editPhotoTransparency : state is NULL");
            continue;
        }
        state->resetTransparency(index, t);
    }
}

void entity::UserScene::editStrokesPush(QUndoStack *stack, osg::Camera *camera)
{
    if (!stack) qFatal("editPhotoPush(): undo stack is NULL, it is not initialized. Editing is not possible.");
    qDebug("Pushing strokes to previously selected canvas (highlighted violet)");
    osg::Vec3f eye, c, u;
    camera->getViewMatrixAsLookAt(eye, c, u);

    if (!m_canvasCurrent || !m_canvasPrevious) {
        qCritical("editStrokesPush: pointer are NULL");
        return;}
    if (eye.isNaN()) {
        qWarning("editStrokesPush: camera's eye is NAN");
        return;
    }

    const std::vector<entity::Entity2D*>& strokes = m_canvasCurrent->getStrokesSelected();

    if (strokes.size() == 0) return;

//    if (!Utilities::areStrokesProjectable(strokes, m_canvasCurrent.get(), m_canvasPrevious.get(), camera)){
//        outErrMsg("Strokes are not pushable under this point of view. Try to change camera position.");
//        return;
//    }

    EditStrokesPushCommand* cmd = new EditStrokesPushCommand(this, strokes,
                                                             m_canvasCurrent.get(),
                                                             m_canvasPrevious.get(),
                                                             eye);
    if (!cmd){
        qCritical("editStrokePush: undo/redo command is NULL");
        return;
    }
    stack->push(cmd);
}

void entity::UserScene::editStrokesMove(QUndoStack *stack, double u, double v, cher::EVENT event)
{
    if (!stack) qFatal("editStrokeMove(): undo stack is NULL, it is not initialized. Editing is not possible.");
    switch (event){
    case cher::EVENT_OFF:
        qDebug("EditStrokesMove: event off called");
        if (this->entitiesSelectedValid())
            this->entitiesMoveFinish(stack);
        break;
    case cher::EVENT_PRESSED:
        qDebug("EditStrokesMove: event pressed called");
        this->entitiesMoveStart(u,v);
        this->entitiesMoveAppend(u,v);
        break;
    case cher::EVENT_DRAGGED:
        if (!this->entitiesSelectedValid())
            this->entitiesMoveStart(u,v);
        this->entitiesMoveAppend(u,v);
        break;
    case cher::EVENT_RELEASED:
        if (!this->entitiesSelectedValid())
            break;
        qDebug("EditStrokesMove: event release called");
        this->entitiesMoveAppend(u,v);
        this->entitiesMoveFinish(stack);
        break;
    default:
        break;
    }
}

void entity::UserScene::editStrokesScale(QUndoStack *stack, double u, double v, cher::EVENT event)
{
    if (!stack) qFatal("editStrokesScale(): undo stack is NULL, it is not initialized. Editing is not possible.");
    switch (event){
    case cher::EVENT_OFF:
        qDebug("EditStrokesScale: event off called");
        if (this->entitiesSelectedValid()){
            qDebug("EditStrokesScale: event off performed");
            this->entitiesScaleFinish(stack);
        }
        break;
    case cher::EVENT_PRESSED:
        qDebug("EditStrokesScale: event pressed called");
        this->entitiesScaleStart(u,v);
        this->entitiesScaleAppend(u,v);
        break;
    case cher::EVENT_DRAGGED:
        if (!this->entitiesSelectedValid())
            this->entitiesScaleStart(u,v);
        this->entitiesScaleAppend(u,v);
        break;
    case cher::EVENT_RELEASED:
        if (!this->entitiesSelectedValid())
            break;
        qDebug("EditStrokesScale: event release called");
        this->entitiesScaleAppend(u,v);
        this->entitiesScaleFinish(stack);
        break;
    default:
        break;
    }
}

void entity::UserScene::editStrokesRotate(QUndoStack *stack, double u, double v, cher::EVENT event)
{
    if (!stack) qFatal("editStrokesRotate(): undo stack is NULL, it is not initialized. Editing is not possible.");
    switch (event){
    case cher::EVENT_OFF:
        qDebug("EditStrokesScale: event off called");
        if (this->entitiesSelectedValid()){
            qDebug("EditStrokesScale: event off performed");
            this->entitiesRotateFinish(stack);
        }
        break;
    case cher::EVENT_PRESSED:
        qDebug("EditStrokesScale: event pressed called");
        this->entitiesRotateStart(u,v);
        this->entitiesRotateAppend(u,v);
        break;
    case cher::EVENT_DRAGGED:
        if (!this->entitiesSelectedValid())
            this->entitiesRotateStart(u,v);
        this->entitiesRotateAppend(u,v);
        break;
    case cher::EVENT_RELEASED:
        if (!this->entitiesSelectedValid())
            break;
        qDebug("EditStrokesScale: event release called");
        this->entitiesRotateAppend(u,v);
        this->entitiesRotateFinish(stack);
        break;
    default:
        break;
    }
}

void entity::UserScene::editStrokeDelete(QUndoStack *stack, entity::Stroke *stroke)
{
    if (!stack) qFatal("editStrokeDelete(): undo stack is NULL, it is not initialized. Editing is not possible.");
    if (!stroke){
        qCritical("editStrokeDelete: stroke is NULL");
        return;
    }
    if (!this->getCanvasCurrent()->getGroupData()->containsDrawable(stroke)){
        qWarning("editStrokeDelete: current canvas does not contain that stroke."
                  "Deletion is not possible.");
        return;
    }
    EditStrokeDeleteCommand* cmd = new EditStrokeDeleteCommand(this, m_canvasCurrent.get(), stroke);
    if (!cmd){
        qCritical("editStrokeDelete: undo/redo command is NULL");
        return;
    }
    stack->push(cmd);
}

bool entity::UserScene::isEmptyScene() const
{
    return this->getNumCanvases()==0? true : false;
}

bool entity::UserScene::clearUserData()
{
    /* clear SG content */
    m_idCanvas=0;
    m_idPhoto=0;
    m_idBookmark=0;
    return m_groupCanvases->removeChildren(0, this->getNumCanvases());
}

bool entity::UserScene::printScene()
{
    for (int i = 0; i < this->getNumCanvases(); ++i){
        Canvas* cnv = this->getCanvas(i);
        if (!cnv) qFatal("printScene canvas is NULL");

        qDebug() << "Canvas name " << cnv->getName().c_str();

        osg::MatrixTransform* t = dynamic_cast<osg::MatrixTransform*>(cnv->getChild(0));
        assert(t == cnv->getTransform());

        osg::Switch* sw = dynamic_cast<osg::Switch*>(t->getChild(0));
        assert(sw == cnv->getSwitch());

//        osg::Geode* data = dynamic_cast<osg::Geode*>(sw->getChild(1));
//        assert(data == cnv->getGroupData());
    }

    return true;
}

void entity::UserScene::updateWidgets()
{
    emit sendRequestUpdate();
}

void entity::UserScene::resetModel(CanvasPhotoWidget *widget)
{
    widget->clear();
    for (int i=0; i<this->getNumCanvases(); ++i){
        entity::Canvas* cnv = this->getCanvas(i);
        if (!cnv) qFatal("UserScene::resetModel canvas is NULL");
        emit this->canvasAdded(cnv->getName().c_str());

        if (cnv == m_canvasCurrent.get())
            emit this->canvasSelectedColor(this->getCanvasIndex(cnv),1);
        else if (cnv == m_canvasPrevious.get())
            emit this->canvasSelectedColor(this->getCanvasIndex(cnv),2);
        else
            emit this->canvasSelectedColor(this->getCanvasIndex(cnv),0);

        /* if canvas has any photos, reset photowidget */
        if (!cnv->getGroupData()) continue;
        for (size_t j=0; j<cnv->getGroupData()->getNumChildren(); ++j){
            entity::Photo* photo = dynamic_cast<entity::Photo*>(cnv->getGroupData()->getChild(j));
            if (!photo) continue;
            emit this->photoAdded(photo->getName(), this->getCanvasIndex(cnv));
        }

        /* set canvas visibility on scene and on GUI */
        if (!cnv->getVisibilityData()){
            cnv->setVisibilityAll(false);
            int row = this->getCanvasIndex(cnv);
            outLogVal("Trying to set up canvas visibility scene and GUI", row);
            emit canvasVisibilitySet(row, false);
        }
    }
}

void entity::UserScene::onItemChanged(QTreeWidgetItem *item, int column)
{
    QTreeWidget* widget = item->treeWidget();
    if (!widget) return;

    /* determine if canvas or photo was changed
     * canvas would not have any parents */
    if (!item->parent()){
        int row = widget->indexOfTopLevelItem(item);
        if (row >= this->getNumCanvases() || row < 0) return;
        entity::Canvas* cnv = this->getCanvasFromIndex(row);
        if (!cnv) qFatal("UserScene::onItemChanged() - canvas is NULL");
        cnv->setName(item->text(column).toStdString());
    }
    /* if photo */
    else{
        QTreeWidgetItem* parent = item->parent();
        if (!parent) return;
        int rowP = widget->indexOfTopLevelItem(parent);
        if (rowP >= this->getNumCanvases() || rowP < 0) return;
        entity::Canvas* canvas = this->getCanvasFromIndex(rowP);
        if (!canvas) qFatal("UserScene::onItemChanged() - canvas is NULL");
        int row = parent->indexOfChild(item);
        if (row >= this->getNumPhotos(canvas) || row < 0) return;

        entity::Photo* photo = this->getPhotoFromIndex(canvas, row);
        if (!photo) qFatal("UserScene::onItemChanged() - photo is NULL");
        photo->setName(item->text(column).toStdString());
        qDebug("photo name changed");
    }
}

/* to selec as current from canvas delegate */
void entity::UserScene::onClicked(const QModelIndex &index)
{
    /* if clicked on parent (canvas):
     * get corresponding canvas ptr
     * make sure you consider bookmark ptr index
     * set that canvas as current */
    if (index.data(cher::DelegateChildRole).toInt() == 1){
        if (index.row()<0)
            return;
        entity::Canvas* cnv = this->getCanvasFromIndex(index.row());
        if (!cnv){
            qWarning("UserScene onClicked: canvas ptr is NULL");
            return;
        }
        qDebug("Changing current canvas from canvas widget");
        this->setCanvasCurrent(cnv);
        this->updateWidgets();
    }
}

/* if clicked on parent (canvas):
 * to select as previous from canvas delegate */
void entity::UserScene::onRightClicked(const QModelIndex &index)
{
    if (index.data(cher::DelegateChildRole).toInt() == 1){
        entity::Canvas* cnv = this->getCanvasFromIndex(index.row());
        if (!cnv){
            qWarning("UserScene onRightClicked: canvas ptr is NULL");
            return;
        }
        qDebug("Changing previous/target canvas from canvas widget");
        if (cnv == m_canvasCurrent.get() || cnv == m_canvasPrevious.get())
            return;
        this->setCanvasPrevious(cnv);
        this->updateWidgets();
    }
}

std::string entity::UserScene::getCanvasName()
{
    return this->getEntityName(cher::NAME_CANVAS, m_idCanvas++);
}

std::string entity::UserScene::getPhotoName()
{
    return this->getEntityName(cher::NAME_PHOTO, m_idPhoto++);
}

std::string entity::UserScene::getBookmarkName()
{
    return this->getEntityName(cher::NAME_BOOKMARK, m_idBookmark++);
}

std::string entity::UserScene::getEntityName(const std::string &name, unsigned int id) const
{
    char buffer[10];

#ifdef Q_OS_WIN
    sprintf_s(buffer, sizeof(buffer), "%d", id);  // especially if msvc < 2015
#else
    snprintf(buffer, sizeof(buffer), "%d", id);
#endif // Q_OS_WIN

    return name + std::string(buffer);//std::to_string(static_cast<long double>(id));
}

void entity::UserScene::strokeStart()
{
    m_canvasCurrent->unselectEntities();
    /* if the canvas is hidden, show it all so that user could see where they sketch */
    if (!m_canvasCurrent->getVisibilityData())
        m_canvasCurrent->setVisibilityAll(true);
    outLogMsg("strokeStart()");
    if (this->strokeValid()){
        outErrMsg("strokeStart(): Cannot start new stroke since the pointer is not NULL");
        return;
    }
    entity::Stroke* stroke = new entity::Stroke();
    m_canvasCurrent->setStrokeCurrent(stroke);
    m_canvasCurrent->getGroupData()->addDrawable(stroke);
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
//    m_canvasCurrent->updateFrame(m_canvasPrevious.get());
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
    m_canvasCurrent->getGroupData()->removeChild(stroke); // remove the "current" copy
    m_canvasCurrent->setStrokeCurrent(false);
    outLogMsg("strokeFinish()");
}

bool entity::UserScene::strokeValid() const
{
    return m_canvasCurrent->getStrokeCurrent();
}

void entity::UserScene::entitiesMoveStart(double u, double v)
{
    /* if the canvas is hidden, show it all so that user could see where they sketch */
    if (!m_canvasCurrent->getVisibilityData())
        m_canvasCurrent->setVisibilityAll(true);
    m_u = u;
    m_v = v;
    m_du = m_dv = 0;
    m_inits = true;
}

void entity::UserScene::entitiesMoveAppend(double u, double v)
{
    double du = u - m_u;
    double dv = v - m_v;

    /* perform delta movement */
    m_canvasCurrent->moveEntitiesSelected(du, dv);
    m_canvasCurrent->updateFrame(m_canvasPrevious.get());

    this->updateWidgets();
    m_du += du;
    m_dv += dv;
    m_u = u;
    m_v = v;
}

void entity::UserScene::entitiesMoveFinish(QUndoStack *stack)
{
    /* move things back so that to perform this operation in undo/redo FW */
    m_canvasCurrent->moveEntitiesSelected(-m_du, -m_dv);

    EditEntitiesMoveCommand* cmd = new EditEntitiesMoveCommand(this,
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

bool entity::UserScene::entitiesSelectedValid() const
{
    return ((m_canvasCurrent->getStrokesSelectedSize() > 0? true : false) && m_inits);
}

void entity::UserScene::entitiesScaleStart(double u, double v)
{
    /* if the canvas is hidden, show it all so that user could see where they sketch */
    if (!m_canvasCurrent->getVisibilityData())
        m_canvasCurrent->setVisibilityAll(true);

    osg::Vec3f center = m_canvasCurrent->getGroupData()->getBoundingBox().center();
    m_u = center.x();
    m_v = center.y();

    m_scaleX = m_scaleY = 1;
    m_du = std::fabs(u-m_u); /* growth/scale in x */
    m_dv = std::fabs(v-m_v); /* growth/scale in y */
    m_inits = true;
}

void entity::UserScene::entitiesScaleAppend(double u, double v)
{
    double sx = 1;
    double sy = 1;
    // make sure it's not smaller than allowed
    if (m_du != 0 /*&& std::fabs(u-m_u) >= 0.1*/)
        sx = std::fabs(u-m_u)/m_du;
    if (m_dv != 0)
        sy = std::fabs(v-m_v)/m_dv;

    m_scaleX *= sx;
    m_scaleY *= sy;
    m_canvasCurrent->scaleEntitiesSelected(sx, sx);
    m_canvasCurrent->updateFrame(m_canvasPrevious.get());

    this->updateWidgets();
    m_du = std::fabs(u-m_u);
    m_dv = std::fabs(v-m_v);
}

void entity::UserScene::entitiesScaleFinish(QUndoStack *stack)
{
    m_canvasCurrent->scaleEntitiesSelected(1/m_scaleX, 1/m_scaleX);

    EditEntitiesScaleCommand* cmd =
            new EditEntitiesScaleCommand(this,
                                         m_canvasCurrent->getStrokesSelected(),
                                         m_canvasCurrent.get(),
                                         m_scaleX, m_scaleX,
                                         m_canvasCurrent->getSelectedEntitiesCenter2D() );
    m_du = m_u = 0;
    m_dv = m_v = 0;
    m_inits = false;
    m_scaleX = m_scaleY = 1;

    if (!cmd){
        outErrMsg("strokeScaleFinish: Could not allocate command");
        return;
    }
    stack->push(cmd);
}

void entity::UserScene::entitiesRotateStart(double u, double v)
{
    /* if the canvas is hidden, show it all so that user could see where they sketch */
    if (!m_canvasCurrent->getVisibilityData())
        m_canvasCurrent->setVisibilityAll(true);

    osg::Vec3f center = m_canvasCurrent->getGroupData()->getBoundingBox().center();
    if (center.z() > cher::EPSILON){
        outLogVec("center", center.x(), center.y(), center.z());
        outErrMsg("entitiesRotateStart: z coordiante is not close to zero");
    }

    m_u = u;
    m_v = v;
    m_du = center.x();
    m_dv = center.y();
    m_rotate = 0;
    m_inits = true;
}

void entity::UserScene::entitiesRotateAppend(double u, double v)
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
    if (theta < -cher::PI || theta > cher::PI)
        theta = 0;

    outLogVal("theta", theta);

    /* now rotate stroke on theta around center of coords [m_du m_dv] */
    m_canvasCurrent->rotateEntitiesSelected(theta);
    m_canvasCurrent->updateFrame(m_canvasPrevious.get());

    m_rotate += theta;
    m_u = u;
    m_v = v;

    this->updateWidgets();
}

void entity::UserScene::entitiesRotateFinish(QUndoStack *stack)
{
    m_canvasCurrent->rotateEntitiesSelected(-m_rotate);

    EditEntitiesRotateCommand* cmd = new EditEntitiesRotateCommand(this,
                                                                 m_canvasCurrent->getStrokesSelected(),
                                                                 m_canvasCurrent.get(),
                                                                 m_rotate,
                                                                   m_canvasCurrent->getSelectedEntitiesCenter2D());
    m_u = m_v = 0;
    m_du = m_dv = 0;
    m_rotate = 0;
    m_inits = false;

    if (!cmd){
        outErrMsg("entitiesRotateFinish: Could not allocate command");
        return;
    }
    stack->push(cmd);
}

void entity::UserScene::eraseStart(entity::Stroke *stroke, osg::Vec3d &hit)
{
    /* if the canvas is hidden, show it all so that user could see where they sketch */
    if (!m_canvasCurrent->getVisibilityData())
        m_canvasCurrent->setVisibilityAll(true);
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
    /* if the canvas is hidden, show it all so that user could see where they sketch */
    if (!m_canvasCurrent->getVisibilityData())
        m_canvasCurrent->setVisibilityAll(true);

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
    /* if the canvas is hidden, show it all so that user could see where they sketch */
    if (!m_canvasCurrent->getVisibilityData())
        m_canvasCurrent->setVisibilityAll(true);

    m_canvasCurrent->unselectAll();

    entity::Canvas* cnv = m_canvasCurrent->clone();
    cnv->setName(this->getCanvasName());
    if (!cnv){
        outErrMsg("canvasCloneStart: could not clone the canvas, ptr is NULL");
        return;
    }
    m_canvasClone = cnv;

    if (!m_groupCanvases->addChild(m_canvasClone.get())){
        outErrMsg("canvasCloneStart: could not add clone as a child");
        return;
    }

    /* have to set the clone as current so that to calculate offset correctly */
    if (!this->setCanvasCurrent(m_canvasClone.get())){
        outErrMsg("canvasCloneStart: could not set clone as current");
        m_groupCanvases->removeChild(m_canvasClone.get());
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
    m_groupCanvases->removeChild(m_canvasClone.get());
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

void entity::UserScene::canvasSeparateStart()
{
    /* if the canvas is hidden, show it all so that user could see where they sketch */
    if (!m_canvasCurrent->getVisibilityData())
        m_canvasCurrent->setVisibilityAll(true);

    entity::Canvas* cnv = m_canvasCurrent->separate();
    cnv->setName(this->getCanvasName());
    if (!cnv){
        outErrMsg("canvasCloneStart: could not clone the canvas, ptr is NULL");
        return;
    }
    m_canvasClone = cnv;

    // now clone contains all the strokes needed
    m_canvasCurrent->unselectAll();

    if (!m_groupCanvases->addChild(m_canvasClone.get())){
        outErrMsg("canvasCloneStart: could not add clone as a child");
        return;
    }

    /* have to set the clone as current so that to calculate offset correctly */
    if (!this->setCanvasCurrent(m_canvasClone.get())){
        outErrMsg("canvasCloneStart: could not set clone as current");
        m_groupCanvases->removeChild(m_canvasClone.get());
        m_canvasClone = 0;
        return;
    }
}

void entity::UserScene::canvasSeparateAppend(const osg::Vec3f &t)
{
    if (!this->canvasCloneValid()){
        outErrMsg("canvasCloneAppend: clone is not valid");
        return;
    }

    /* clone is also current */
    m_canvasCurrent->translate(osg::Matrix::translate(t.x(), t.y(), t.z()));
    this->updateWidgets();
}

void entity::UserScene::canvasSeparateFinish(QUndoStack *stack)
{
    if (!this->canvasCloneValid()){
        outErrMsg("canvasCloneFinish: clone is not valid");
        return;
    }

    this->setCanvasCurrent(m_canvasPrevious.get());

    AddCanvasSeparationCommand* cmd = new AddCanvasSeparationCommand(this, m_canvasCurrent.get(),
                                                                     m_canvasClone.get());

    m_groupCanvases->removeChild(m_canvasClone.get());
    m_canvasClone = 0;
    if (!cmd){
        outErrMsg("canvasCloneFinish: could not allocated AddCanvasCommand");
        return;
    }
    stack->push(cmd);
}

void entity::UserScene::canvasRotateStart()
{
    /* if the canvas is hidden, show it all so that user could see where they sketch */
    if (!m_canvasCurrent->getVisibilityData())
        m_canvasCurrent->setVisibilityAll(true);

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

void entity::UserScene::canvasRotateAppend(const osg::Quat &r, const osg::Vec3f &center3d)
{
    if (!this->canvasEditValid()){
        outErrMsg("canvasRotateAppend: canvas edit mode is not valid");
        return;
    }
    if (!m_canvasCurrent.get()){
        outErrMsg("canvasRotateAppend: canvas is NULL");
        return;
    }

    m_canvasCurrent->rotate(osg::Matrix::rotate(r), center3d);
    m_canvasCurrent->updateFrame(m_canvasPrevious.get());
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
    m_canvasCurrent->rotate(osg::Matrix::rotate(m_deltaR.inverse()), m_canvasCurrent->getCenterMean());
    EditCanvasRotateCommand* cmd = new EditCanvasRotateCommand(this, m_deltaR);
    stack->push(cmd);
    m_deltaR = osg::Quat(0,0,0,1);
}

bool entity::UserScene::addCanvas(entity::Canvas *canvas)
{
    if (!canvas) qFatal("UserScene::addCanvas(Canvas*): canvas is NULL");
    if (!canvas->getGroupData()) qFatal("UserScene::addCanvas() - geodeData is null");

    // gui elements
    emit canvasAdded(canvas->getName());
    //see if any canvas contains any photos, they will be added to canvas widget
    for (int i=0; i<canvas->getNumPhotos(); ++i){
        entity::Photo* photo = canvas->getPhotoFromIndex(i);
        if (!photo) qFatal("UserScene::addCanvas() - photo is null");
        emit this->photoAdded(photo->getName(), this->getCanvasIndex(canvas));
    }

    // scene graph addition
    bool result = m_groupCanvases->addChild(canvas);
    this->setCanvasCurrent(canvas);

    // for each bookmark's state, add data for new canvas
    int index = this->getCanvasIndex(canvas);
    int idx = this->getNumPhotosTill(canvas);
    for (int i=0; i<m_groupBookmarks->getNumBookmarks(); ++i){
        entity::SceneState* state = m_groupBookmarks->getSceneState(i);
        if (!state) qFatal("UserScene::addCanvas(): could not obtain a scene state");

        state->insertDataFlag(index, canvas->getVisibilityAll());
        state->insertToolFlag(index, canvas->getVisibilityFrameInternal());

        // if canvas contains any photos, insert photo transparencies too
        for (int j=0; j<canvas->getNumPhotos(); ++j){
            entity::Photo* photo = canvas->getPhotoFromIndex(j);
            if (!photo) qFatal("UserScene::addCanvas(): photo is NULL");
            state->insertTransparency(idx++, photo->getTransparency());
        }
    }

    // update frame and widget
    canvas->updateFrame(this->getCanvasPrevious());
    bool vis = true;
    emit this->requestSceneToolStatus(vis);
    canvas->setVisibilityFrameInternal(vis);
    this->updateWidgets();

    return result;
}

bool entity::UserScene::removeCanvas(entity::Canvas *canvas)
{
    if (!canvas) qFatal("UserScene::removeCanvas(Canvas*): canvas is NULL");
    if (!canvas->getGroupData()) qFatal("UserScene::removeCanvas() - geodeData is null");

    int index = this->getCanvasIndex(canvas);
    // for each bookmark's state, remove data of the canvas
    for (int i=0; i<m_groupBookmarks->getNumBookmarks(); ++i){
        entity::SceneState* state = m_groupBookmarks->getSceneState(i);
        if (!state) qFatal("UserScene::removeCanvas(): could not obtain a scene state");
        state->eraseDataFlag(index);
        state->eraseToolFlag(index);
//        state->popBackDataFlag();
//        state->popBackToolFlag();

        // if canvas contains any photos, erase data of photo transparencies too
        if (canvas->getNumPhotos() > 0){
            if (canvas->getPhotoFromIndex(0)){
                int start = this->getPhotoIndex(canvas->getPhotoFromIndex(0), canvas);
                state->eraseTransparency(start, canvas->getNumPhotos());
            }
        }
    }

    // make sure current/previous rules hold
    if (canvas == m_canvasCurrent.get())
        this->setCanvasCurrent(m_canvasPrevious.get());
    if (canvas == m_canvasPrevious.get() || m_canvasCurrent.get() == m_canvasPrevious.get()){
        if (this->getNumCanvases() == 2)
            this->setCanvasPrevious(0);
        else {
            for (int i = 0; i < this->getNumCanvases(); ++i){
                entity::Canvas* cnvi = this->getCanvas(i);
                if (cnvi != NULL && cnvi != m_canvasCurrent.get() && cnvi != canvas){
                    this->setCanvasPrevious(cnvi);
                    break;
                }
            } // for
        } // else
    }

    // gui elements
    emit this->canvasRemoved(index);

    // scene graph
    canvas->unselectAll();
    bool result = m_groupCanvases->removeChild(canvas);

    // updates
    if (m_canvasCurrent.get())
        m_canvasCurrent->updateFrame(m_canvasPrevious.get());
    this->updateWidgets();

    return result;
}

//bool entity::UserScene::addPhoto(Canvas *canvas, entity::Photo *photo)
//{
//    if (!canvas) qFatal("UserScene::addPhoto() - canvas is null");
//    if (!canvas->getGroupData()) qFatal("UserScene::addPhoto() - geodeData is null");
//    if (!photo) qFatal("UserScene::addPhoto() - photo is null");

//    // add photo transparecy to each bookmark's scene state
//    int idx = this->getNumPhotosTill(canvas);
//    for (int i=0; i<m_groupBookmarks->getNumBookmarks(); ++i){
//        entity::SceneState* state = m_groupBookmarks->getSceneState(i);
//        if (!state) qFatal("UserScene::addPhoto(): could not obtain a scene state");
//        state->insertTransparency(idx, photo->getTransparency());
//    }

//    // add photo to scene graph
//    bool result = canvas->getGroupData()->addDrawable(photo);

//    // gui elements
//    emit this->photoAdded(photo->getName(), this->getCanvasIndex(canvas));

//    // updates
//    canvas->updateFrame(m_canvasPrevious.get());
//    this->updateWidgets();

//    return result;
//}

//bool entity::UserScene::removePhoto(entity::Canvas *canvas, entity::Photo *photo)
//{
//    if (!canvas) qFatal("UserScene::removePhoto() - canvas is null");
//    if (!canvas->getGroupData()) qFatal("UserScene::removePhoto() - geodeData is null");
//    if (!photo) qFatal("UserScene::removePhoto() - photo is null");

//    // remove photo transparencies from each bookmark's scene state
//    int idx = this->getNumPhotosTill(canvas);
//    for (int i=0; i<m_groupBookmarks->getNumBookmarks(); ++i){
//        entity::SceneState* state = m_groupBookmarks->getSceneState(i);
//        if (!state) qFatal("UserScene::removePhoto(): could not obtain a scene state");
//        state->eraseTransparency(idx, 1);
//    }

//    // remove from gui elements (order is important)
//    emit this->photoRemoved(this->getCanvasIndex(canvas), this->getPhotoIndex(photo, canvas));

//    // remove from scene graph
//    bool result = canvas->getGroupData()->removeDrawable(photo);

//    // make sure it is not a part of selected group, or it will stay within the scene graph
//    canvas->removeEntitySelected(photo);
//    canvas->updateFrame(m_canvasPrevious.get());
//    this->updateWidgets();

//    return result;
//}

bool entity::UserScene::addEntity(entity::Canvas *canvas, Entity2D *entity)
{
    bool result = false;
    if (!canvas || !entity) return result;

    /* scene state update, if needed */
    switch(entity->getEntityType()){
    case cher::ENTITY_PHOTO:{
        /* add photo transparencies to each bookmark scene state */
        int idx = this->getNumPhotosTill(canvas);
        entity::Photo* photo = dynamic_cast<entity::Photo*>(entity);
        if (!photo) return result;
        for (int i=0; i<m_groupBookmarks->getNumBookmarks(); ++i){
            entity::SceneState* state = m_groupBookmarks->getSceneState(i);
            if (!state) return result;
            state->insertTransparency(idx, photo->getTransparency());
        }
        break;
    }
    default:
        break;
    }

    /* add entity to scene graph */
    result = canvas->addEntity(entity);

    /* gui elements, if needed */
    if (result){
        switch(entity->getEntityType()){
        case cher::ENTITY_PHOTO:
            emit this->photoAdded(entity->getName(), this->getCanvasIndex(canvas));
            break;
        default:
            break;
        }
    }

    /* update frame and widget */
    canvas->updateFrame(m_canvasPrevious.get());
    this->updateWidgets();
    return result;
}

bool entity::UserScene::removeEntity(entity::Canvas *canvas, Entity2D *entity)
{
    bool result = false;
    if (!canvas || !entity) return result;

    /* scene state update, if needed */
    /* and remove from gui elements, if needed */
    switch(entity->getEntityType()){
    case cher::ENTITY_PHOTO:{
        /* add photo transparencies to each bookmark scene state */
        int idx = this->getNumPhotosTill(canvas);
        entity::Photo* photo = dynamic_cast<entity::Photo*>(entity);
        if (!photo) return result;
        for (int i=0; i<m_groupBookmarks->getNumBookmarks(); ++i){
            entity::SceneState* state = m_groupBookmarks->getSceneState(i);
            if (!state) return result;
            state->eraseTransparency(idx, photo->getTransparency());
        }
        emit this->photoRemoved(this->getCanvasIndex(canvas), this->getPhotoIndex(photo, canvas));
        break;
    }
    default:
        break;
    }

    /* remove entity from scene graph */
    result = canvas->removeEntity(entity);

    /* make sure it is not a part of selected group, or it will stay within the scene graph */
    canvas->removeEntitySelected(entity);

    /* update frame and widget */
    canvas->updateFrame(m_canvasPrevious.get());
    this->updateWidgets();

    return result;
}

REGISTER_OBJECT_WRAPPER(UserScene_Wrapper
                        , new entity::UserScene
                        , entity::UserScene
                        , "osg::Object osg::Group entity::UserScene")
{
    ADD_OBJECT_SERIALIZER(GroupCanvases, osg::Group, NULL);
    ADD_OBJECT_SERIALIZER(Bookmarks, entity::Bookmarks, NULL);
    ADD_UINT_SERIALIZER(IdCanvas, 0);
    ADD_UINT_SERIALIZER(IdPhoto, 0);
    ADD_UINT_SERIALIZER(IdBookmark, 0);
    ADD_STRING_SERIALIZER(FilePath, "");
}
