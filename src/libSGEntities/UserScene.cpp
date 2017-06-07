#include "UserScene.h"

#include <QDebug>
#include <QtGlobal>

#include "Settings.h"
#include "Utilities.h"
#include "AddEntityCommand.h"
#include "EditEntityCommand.h"
#include "FindNodeVisitor.h"
#include "MainWindow.h"

#include <osgDB/WriteFile>
#include <osgDB/ReadFile>

entity::UserScene::UserScene()
    : osg::ProtectedGroup()
    , m_groupCanvases(new osg::Group)
    , m_groupBookmarks(new entity::Bookmarks)
    , m_canvasCurrent(NULL)
    , m_canvasPrevious(NULL)
    , m_canvasClone(NULL)
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
    : osg::ProtectedGroup(scene, copyop)
    , m_groupCanvases(scene.m_groupCanvases)
    , m_groupBookmarks(scene.m_groupBookmarks)
    , m_canvasCurrent(scene.m_canvasCurrent)
    , m_canvasPrevious(scene.m_canvasPrevious)
    , m_canvasClone(NULL)
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

#ifndef DOXYGEN_SHOULD_SKIP_THIS
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
#endif // DOXYGEN_SHOULD_SKIP_THIS

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
        qWarning("addCanvas(): undo stack is NULL, Canvas will not be added. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }
    fur::AddCanvasCommand* cmd = new fur::AddCanvasCommand(this, normal, center,
                                                 getEntityName(cher::NAME_CANVAS, m_idCanvas++));
    if (!cmd){
        qWarning("addCanvas: cmd is NULL");
        return;
    }
    stack->push(cmd);
}

void entity::UserScene::addCanvas(QUndoStack* stack, const osg::Matrix& R, const osg::Matrix& T, const std::string& name)
{
    if (!stack){
        qWarning("addCanvas(): undo stack is NULL, Canvas will not be added. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }
    fur::AddCanvasCommand* cmd = new fur::AddCanvasCommand(this, R, T, name);
    if (!cmd){
        qWarning("addCanvas: cmd is NULL");
        return;
    }
    stack->push(cmd);

}

void entity::UserScene::addStroke(QUndoStack* stack, float u, float v, cher::EVENT event)
{
    if (!stack){
        qWarning("addStroke(): undo stack is NULL, it is not initialized. "
                 "Sketching is not possible. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }
    switch (event){
    case cher::EVENT_OFF:
        qDebug("EVENT_OFF");
        this->strokeFinish(stack);
        break;
    case cher::EVENT_PRESSED:
        qDebug("EVENT_PRESSED");
        this->strokeStart();
        this->strokeAppend(u, v);
        break;
    case cher::EVENT_DRAGGED:
        if (!this->strokeValid())
            this->strokeStart();
        this->strokeAppend(u, v);
        break;
    case cher::EVENT_RELEASED:
        qDebug("EVENT_RELEASED");
        if (!this->strokeValid())
            break;
        this->strokeAppend(u, v);
        this->strokeFinish(stack);
        break;
    default:
        break;
    }
}

void entity::UserScene::addPolygon(QUndoStack *stack, float u, float v, cher::EVENT event)
{
    if (!stack){
        qWarning("addPolygon(): undo stack is NULL, it is not initialized. "
                 "Sketching is not possible. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }

    switch(event){
    case cher::EVENT_OFF:
        qDebug("polygon_OFF");
        this->polygonFinish(stack);
        break;
    case cher::EVENT_PRESSED:
        qDebug("polygon_PRESSED");
        if (!this->polygonValid())
            this->polygonStart();
        this->polygonAppend(u, v, stack);
        break;
    case cher::EVENT_DRAGGED:
    case cher::EVENT_RELEASED:
        if (!this->polygonValid()) break;
        this->polygonEdit(u, v);
        break;
    default:
        break;
    }
}

void entity::UserScene::addLineSegment(QUndoStack *stack, float u, float v, cher::EVENT event)
{
    if (!stack){
        qWarning("addPolygon(): undo stack is NULL, it is not initialized. "
                 "Sketching is not possible. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }

    switch (event) {
    case cher::EVENT_OFF:
        qDebug("linesegment OFF");
        this->linesegmentFinish(stack);
        break;
    case cher::EVENT_PRESSED:
        qDebug("linesegment PRESSED");
        if (!this->linesegmentValid())
            this->linesegmentStart();
        this->linesegmentAppend(u, v, stack);
        break;
    case cher::EVENT_DRAGGED:
    case cher::EVENT_RELEASED:
        if (!this->linesegmentValid()) break;
        this->linesegmentEdit(u, v);
        break;
    default:
        break;
    }
}

void entity::UserScene::addPhoto(QUndoStack* stack, const std::string& fname)
{
    qDebug("loadPhotoFromFile()");
    if (!stack){
        qWarning("addPhoto(): undo stack is NULL, Canvas will not be added. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }
    fur::AddPhotoCommand* cmd = new fur::AddPhotoCommand(this, fname, this->getPhotoName());
    if (!cmd){
        qWarning("addPhoto(): could not allocate fur::AddPhotoCommand.");
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
        qWarning("eraseStroke(): undo stack is NULL, it is not initialized. "
                 "Erase is not possible. "
                 "Restart the program to ensure undo stack initialization.");
        return;
    }

//    stroke->removePoints(first, last);

    switch (event){
    case cher::EVENT_OFF:
        qDebug("EVENT_OFF");
        break;
    case cher::EVENT_PRESSED:
        qDebug("EVENT_PRESSED");
        break;
    case cher::EVENT_DRAGGED:
        break;
    case cher::EVENT_RELEASED:
        qDebug("EVENT_RELEASED");
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

    // make sure current canvas is not in editable mode
    if (m_canvasCurrent.get())
        m_canvasCurrent->setFrameEditable(false);

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
        qWarning( "setCanvasCurrent(): The input canvas pointer is NULL, no current canvas is assigned" );
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

void entity::UserScene::setAllCanvases(bool enabled)
{
    for (int i=0; i<this->getNumCanvases(); ++i){
        entity::Canvas* cnv = this->getCanvas(i);
        if (!cnv) continue;
        if (enabled)    cnv->setNodeMask(cher::MASK_CANVAS_IN);
        else            cnv->setNodeMask(cher::MASK_CANVAS_OUT);
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
    for (size_t i=0; i< canvas->getNumPhotos(); ++i){
        entity::Photo* pi = canvas->getPhoto(i);
        if (!pi) qFatal("UserScene::getPhotoIndex() failed");
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

entity::Photo *entity::UserScene::getPhoto(Canvas *canvas, int row)
{
    if (!canvas) return NULL;
    return canvas->getPhoto(row);
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
        qDebug("event off for canvas offset");
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
        qDebug("event release for canvas offset");
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
        qDebug("editCanvasRotate: OFF");
        this->canvasRotateFinish(stack);
        break;
    case cher::EVENT_PRESSED:
        qDebug("editCanvasRotate: pressed");
        this->canvasRotateStart();
        this->canvasRotateAppend(rotation, center3d);
        break;
    case cher::EVENT_DRAGGED:
        if (!this->canvasEditValid())
            this->canvasRotateStart();
        this->canvasRotateAppend(rotation, center3d);
        break;
    case cher::EVENT_RELEASED:
        qDebug("editCanvasRotate: release");
        if (!this->canvasEditValid())
            break;
        qDebug("editCanvasRotate: released");
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

    fur::EditCanvasDeleteCommand* cmd = new fur::EditCanvasDeleteCommand(this, canvas);
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

    if (!canvas->containsEntity(photo)){
        qCritical("editPhotoDelete: current canvas does not contain that photo."
                  "Deletion is not possible.");
        return;
    }

    fur::EditPhotoDeleteCommand* cmd = new fur::EditPhotoDeleteCommand(this, canvas, photo);
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


    if (!source->containsEntity(photo)){
        qCritical("editPhotoDelete: source canvas does not contain that photo."
                          "Scene movement is not possible.");
                return;
    }

    fur::EditPhotoPushCommand* cmd = new fur::EditPhotoPushCommand(this, source, destination, photo);
    if (!cmd){
        qCritical("editPhotoPush: undo/redo command is NULL");
        return;}
    stack->push(cmd);
}

void entity::UserScene::editPhotoTransparency(entity::Photo *photo, entity::Canvas *canvas, float t)
{
    if (!photo || !canvas) return;
    if (!canvas->containsEntity(photo)) {
        qWarning("editPhotoTransparency: canvas does not contains the photo");
        return;
    }
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

// TODO: replace all the geometries by Entity2D type, e.g. editEntityDelete(), EditEntityDeleteCommand etc.
void entity::UserScene::editPolygonDelete(QUndoStack *stack, entity::Polygon *poly, entity::Canvas *canvas)
{
    if (!stack) qFatal("editPhotoDelete(): undo stack is NULL, it is not initialized. Editing is not possible.");
    if (!poly){
        qCritical("editPolygonDelete: poly is NULL");
        return;
    }
    if (!canvas->containsEntity(poly)){
        qCritical("editPolygonDelete: current canvas does not contain that polygon."
                  "Deletion is not possible.");
        return;
    }

    fur::EditEntityDeleteCommand* cmd = new fur::EditEntityDeleteCommand(this, canvas, poly);
    if (!cmd){
        qCritical("editPolygonDelete: undo/redo command is NULL");
        return;
    }
    stack->push(cmd);
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

    const std::vector<entity::Entity2D*>& strokes = m_canvasCurrent->getEntitiesSelected();

    if (strokes.size() == 0) return;

//    if (!Utilities::areStrokesProjectable(strokes, m_canvasCurrent.get(), m_canvasPrevious.get(), camera)){
//        qWarning("Strokes are not pushable under this point of view. Try to change camera position.");
//        return;
//    }

    fur::EditStrokesPushCommand* cmd = new fur::EditStrokesPushCommand(this, strokes,
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
    if (!m_canvasCurrent->containsEntity(stroke)){
        qWarning("editStrokeDelete: current canvas does not contain that stroke."
                  "Deletion is not possible.");
        return;
    }
    fur::EditStrokeDeleteCommand* cmd = new fur::EditStrokeDeleteCommand(this, m_canvasCurrent.get(), stroke);
    if (!cmd){
        qCritical("editStrokeDelete: undo/redo command is NULL");
        return;
    }
    stack->push(cmd);
}

void entity::UserScene::editEntity2DDelete(QUndoStack *stack, entity::Entity2D *entity)
{
    if (!stack) qFatal("editStrokeDelete(): undo stack is NULL, it is not initialized. Editing is not possible.");
    if (!entity){
        qCritical("editStrokeDelete: stroke is NULL");
        return;
    }
    if (!m_canvasCurrent->containsEntity(entity)){
        qWarning("editStrokeDelete: current canvas does not contain that stroke."
                  "Deletion is not possible.");
        return;
    }
    fur::EditEntityDeleteCommand* cmd = new fur::EditEntityDeleteCommand(this, m_canvasCurrent.get(), entity);
    if (!cmd){
        qCritical("editStrokeDelete: undo/redo command is NULL");
        return;
    }
    stack->push(cmd);
}

void entity::UserScene::editSelectedEntitiesDelete(QUndoStack *stack)
{
    entity::Canvas* canvas = this->getCanvasCurrent();
    if (!canvas) return;

    if (!stack) qFatal("editSelectedEntities(): undo stack is null.");

    std::vector<osg::ref_ptr<entity::Entity2D>> entities;
    const std::vector<entity::Entity2D*>& selected = canvas->getEntitiesSelected();
    for (auto* entity : selected)
        entities.push_back(entity);
    if (entities.size() == 0) return;
    canvas->unselectAll();
    fur::EditSelectedEntitiesDeleteCommand* cmd = new fur::EditSelectedEntitiesDeleteCommand(this,
                                                                                             m_canvasCurrent.get(),
                                                                                             entities);
    if (!cmd){
        qCritical("editSelectedEntitiesDelete(): undo/redo command is null");
        return;
    }
    stack->push(cmd);

}

bool entity::UserScene::isEntityCurrent() const
{
    return this->strokeValid() || this->polygonValid() || this->canvasEditValid() || canvasCloneValid();
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
        for (size_t j=0; j<cnv->getNumPhotos(); ++j){
            entity::Photo* photo = cnv->getPhoto(j);
            if (!photo) continue;
            emit this->photoAdded(photo->getName(), this->getCanvasIndex(cnv));
        }

        /* set canvas visibility on scene and on GUI */
        if (!cnv->getVisibilityAll()){
            cnv->setVisibilityAll(false);
            int row = this->getCanvasIndex(cnv);
            qDebug() << "Trying to set up canvas visibility scene and GUI " << row;
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

        entity::Photo* photo = this->getPhoto(canvas, row);
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

#ifndef DOXYGEN_SHOULD_SKIP_THIS

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
    if (!m_canvasCurrent->getVisibilityAll())
        m_canvasCurrent->setVisibilityAll(true);
    qDebug("strokeStart()");
    if (this->strokeValid()){
        qWarning("strokeStart(): Cannot start new stroke since the pointer is not NULL");
        return;
    }
    entity::Stroke* stroke = new entity::Stroke();
    stroke->initializeProgram(m_canvasCurrent->getProgramStroke());
    m_canvasCurrent->setStrokeCurrent(stroke);
    m_canvasCurrent->addEntity(stroke);
}

void entity::UserScene::strokeAppend(float u, float v)
{
    if (this->strokeValid()){
        entity::Stroke* stroke = m_canvasCurrent->getStrokeCurrent();
        stroke->appendPoint(u, v);
        this->updateWidgets();
    }
    else
        qWarning("strokeAppend: pointer is NULL");
}

/* if command is still a valid pointer,
   if stroke is long enough to be kept,
   clone the fur::AddStrokeCommand and push the cloned instance to stack
   set the shared pointer to zero and return*/
void entity::UserScene::strokeFinish(QUndoStack* stack)
{
    if (!m_canvasCurrent.get()) return;
//    m_canvasCurrent->updateFrame(m_canvasPrevious.get());
    entity::Stroke* stroke = m_canvasCurrent->getStrokeCurrent();
    if (this->strokeValid()){
//        if (stroke->isLengthy())
        {
            osg::ref_ptr<entity::Stroke> stroke_clone = new entity::Stroke;
            Q_ASSERT(stroke_clone.get());
            if (stroke_clone->copyFrom(stroke)){
                stroke_clone->redefineToShape();
                Q_ASSERT(stroke_clone->getIsCurved());
                fur::AddStrokeCommand* cmd = new fur::AddStrokeCommand(this, stroke_clone);
                stack->push(cmd);

            }
        }
    }
    else{
        qInfo("strokeFinish(): stroke pointer is NULL, impossible to finish the stroke");
        return;
    }
    m_canvasCurrent->removeEntity(stroke); // remove the "current" copy
    m_canvasCurrent->setStrokeCurrent(false);
    qDebug("strokeFinish()");
}

bool entity::UserScene::strokeValid() const
{
    if (!m_canvasCurrent) return false;
    return m_canvasCurrent->getStrokeCurrent();
}

void entity::UserScene::polygonStart()
{
    m_canvasCurrent->unselectEntities();
    /* if the canvas is hidden, show it all so that user could see where they sketch */
    if (!m_canvasCurrent->getVisibilityAll())
        m_canvasCurrent->setVisibilityAll(true);
    qDebug("Polygone start");
    if (this->polygonValid()){
        qWarning("polygonStart(): Cannot start new polygon since the pointer is not NULL");
        return;
    }
    entity::Polygon* poly = new entity::Polygon;
    poly->initializeProgram(m_canvasCurrent->getProgramPolygon());
    m_canvasCurrent->setPolygonCurrent(poly);
    m_canvasCurrent->addEntity(poly);
//    poly->redefineToShape();

    osg::Vec4f color = MainWindow::instance().getCurrentColor();
    poly->setColor(color);

    this->updateWidgets();
}

void entity::UserScene::polygonAppend(float u, float v, QUndoStack *stack)
{
    if (this->polygonValid()){
        entity::Polygon* poly = m_canvasCurrent->getPolygonCurrent();
        Q_CHECK_PTR(poly);
        float dist = 10.f;
        if (poly->getNumPoints() >= 3){
            osg::Vec2f p0 = poly->getPoint(0);
            dist = std::sqrt((p0.x()-u)*(p0.x()-u) + (p0.y()-v)*(p0.y()-v));
        }

        /* close the polygon if the last point is close to the first point */
        if (dist <= cher::POLYGON_PROXIMITY_THRESHOLD){
            // remove the last point since it's "edit" point
            poly->removeLastPoint();
            this->polygonFinish(stack);
            return;
        }

        /* otherwise, append it */
        qDebug("Appending point to a polygon");
        /* if more than 2 points, redefine to shape */
        if (poly->getNumPoints() >= 2)
            poly->redefineToShape();
        if (poly->getNumPoints() == 0)
            poly->appendPoint(u, v);
        poly->appendPoint(u, v); // so that we see the continuation of the polygon in edit mode
        this->updateWidgets();
    }
    else
        qWarning("polygonAppend: pointer is NUL");
}

void entity::UserScene::polygonEdit(float u, float v)
{
    if (this->polygonValid()){
        entity::Polygon* poly = m_canvasCurrent->getPolygonCurrent();
        poly->editLastPoint(u, v);
        this->updateWidgets();
    }
    else
        qWarning("polygonEdit: pointer is NULL");
}

void entity::UserScene::polygonFinish(QUndoStack *stack)
{
    if (!m_canvasCurrent.get()) return;
    qDebug("redefining polygon");
    entity::Polygon* poly = m_canvasCurrent->getPolygonCurrent();
    if (this->polygonValid()){
        osg::ref_ptr<entity::Polygon> poly_clone = new entity::Polygon;
        Q_CHECK_PTR(poly_clone);
        if (poly_clone->copyFrom(poly)){
            poly_clone->redefineToShape();
            fur::AddPolygonCommand* cmd = new fur::AddPolygonCommand(this, poly_clone);
            Q_CHECK_PTR(cmd);
            stack->push(cmd);
        }
    }
    else {
        qInfo("polygonFinish(): polygon pointer is NULL, impossible to finish the polygon");
        return;
    }
    m_canvasCurrent->removeEntity(poly);
    m_canvasCurrent->setPolygonCurrent(false);
    qDebug("Polygon finish");
}

bool entity::UserScene::polygonValid() const
{
    if (!m_canvasCurrent) {
        qCritical("There is no current canvas on the scene");
        return false;
    }
    return m_canvasCurrent->getPolygonCurrent();
}

void entity::UserScene::linesegmentStart()
{
    m_canvasCurrent->unselectEntities();
    /* if the canvas is hidden, show it all so that user could see where they sketch */
    if (!m_canvasCurrent->getVisibilityAll())
        m_canvasCurrent->setVisibilityAll(true);
    qDebug("Linesegment start");
    if (this->linesegmentValid()){
        qWarning("linesegmentStart(): Cannot start new linesegment since the pointer is not NULL.");
        return;
    }
    entity::LineSegment* segment = new entity::LineSegment;
    segment->initializeProgram(m_canvasCurrent->getProgramLineSegment(), GL_LINES);
    m_canvasCurrent->setEntityCurrent(segment);
    m_canvasCurrent->addEntity(segment);

    this->updateWidgets();
}

void entity::UserScene::linesegmentAppend(float u, float v, QUndoStack *stack)
{
    if (this->linesegmentValid()){
        entity::LineSegment* segment = m_canvasCurrent->getEntityCurrent<entity::LineSegment>();
        // do not add any points, if there is already at least 2 points
        if (segment->getNumPoints() >= 2){
            this->linesegmentFinish(stack);
            return;
        }
        Q_CHECK_PTR(segment);
        segment->appendPoint(u,v);
        segment->appendPoint(u,v);
        this->updateWidgets();
    }
}

void entity::UserScene::linesegmentEdit(float u, float v)
{
    if (this->linesegmentValid()){
        entity::LineSegment* segment = m_canvasCurrent->getEntityCurrent<entity::LineSegment>();
        Q_CHECK_PTR(segment);
        segment->editLastPoint(u, v);
        this->updateWidgets();
    }
    else
        qWarning("segmentEdit(): pointer is NULL");
}

void entity::UserScene::linesegmentFinish(QUndoStack *stack)
{
    if (!m_canvasCurrent.get()) return;
    entity::LineSegment* segment = m_canvasCurrent->getEntityCurrent<entity::LineSegment>();
    if (this->linesegmentValid()){
        osg::ref_ptr<entity::LineSegment> segment_clone = new entity::LineSegment;
        Q_CHECK_PTR(segment_clone.get());
        if (segment_clone->copyFrom(segment)){
            segment_clone->redefineToShape();
            // TODO: replace add stroke, photo and polygon with add entity command
            fur::AddEntityCommand* cmd = new fur::AddEntityCommand(this, segment_clone);
            Q_CHECK_PTR(cmd);
            stack->push(cmd);
        }
    }
    else{
        qInfo("linesegment finish(): pointer is NULL, impossible to finish the segment");
        return;
    }
    m_canvasCurrent->removeEntity(segment);
    m_canvasCurrent->setEntityCurrent(false);
    qDebug("line segement finished OK.");
}

bool entity::UserScene::linesegmentValid() const
{
    if (!m_canvasCurrent){
        return false;
    }
    return m_canvasCurrent->getEntityCurrent<entity::LineSegment>();
}

void entity::UserScene::entitiesMoveStart(double u, double v)
{
    /* if the canvas is hidden, show it all so that user could see where they sketch */
    if (!m_canvasCurrent->getVisibilityAll())
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
    if (!m_canvasCurrent.get()) return;
    /* move things back so that to perform this operation in undo/redo FW */
    m_canvasCurrent->moveEntitiesSelected(-m_du, -m_dv);

    fur::EditEntitiesMoveCommand* cmd = new fur::EditEntitiesMoveCommand(this,
                                                             m_canvasCurrent->getEntitiesSelected(),
                                                             m_canvasCurrent.get(),
                                                             m_du, m_dv);
    if (!cmd){
        qWarning("strokeMoveFinish: Could not allocate command");
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
    return ((m_canvasCurrent->getEntitiesSelectedSize() > 0? true : false) && m_inits);
}

void entity::UserScene::entitiesScaleStart(double u, double v)
{
    /* if the canvas is hidden, show it all so that user could see where they sketch */
    if (!m_canvasCurrent->getVisibilityAll())
        m_canvasCurrent->setVisibilityAll(true);

    osg::Vec3f center = m_canvasCurrent->getBoundingBoxCenter2D();
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
    if (!m_canvasCurrent.get()) return;
    m_canvasCurrent->scaleEntitiesSelected(1/m_scaleX, 1/m_scaleX);

    fur::EditEntitiesScaleCommand* cmd =
            new fur::EditEntitiesScaleCommand(this,
                                         m_canvasCurrent->getEntitiesSelected(),
                                         m_canvasCurrent.get(),
                                         m_scaleX, m_scaleX,
                                         m_canvasCurrent->getEntitiesSelectedCenter2D() );
    m_du = m_u = 0;
    m_dv = m_v = 0;
    m_inits = false;
    m_scaleX = m_scaleY = 1;

    if (!cmd){
        qWarning("strokeScaleFinish: Could not allocate command");
        return;
    }
    stack->push(cmd);
}

void entity::UserScene::entitiesRotateStart(double u, double v)
{
    /* if the canvas is hidden, show it all so that user could see where they sketch */
    if (!m_canvasCurrent->getVisibilityAll())
        m_canvasCurrent->setVisibilityAll(true);

    osg::Vec3f center = m_canvasCurrent->getBoundingBoxCenter2D();
    if (center.z() > cher::EPSILON){
        outLogVec("center", center.x(), center.y(), center.z());
        qWarning("entitiesRotateStart: z coordiante is not close to zero");
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

    qDebug() << "theta " << theta;

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
    if (!m_canvasCurrent.get()) return;
    m_canvasCurrent->rotateEntitiesSelected(-m_rotate);

    fur::EditEntitiesRotateCommand* cmd = new fur::EditEntitiesRotateCommand(this,
                                                                 m_canvasCurrent->getEntitiesSelected(),
                                                                 m_canvasCurrent.get(),
                                                                 m_rotate,
                                                                   m_canvasCurrent->getEntitiesSelectedCenter2D());
    m_u = m_v = 0;
    m_du = m_dv = 0;
    m_rotate = 0;
    m_inits = false;

    if (!cmd){
        qWarning("entitiesRotateFinish: Could not allocate command");
        return;
    }
    stack->push(cmd);
}

void entity::UserScene::eraseStart(entity::Stroke *stroke, osg::Vec3d &hit)
{
    /* if the canvas is hidden, show it all so that user could see where they sketch */
    if (!m_canvasCurrent->getVisibilityAll())
        m_canvasCurrent->setVisibilityAll(true);
}

void entity::UserScene::eraseAppend(entity::Stroke *stroke, osg::Vec3d &hit)
{

}

void entity::UserScene::eraseFinish(QUndoStack *stack, entity::Stroke *stroke)
{
    if (!this->eraseValid(stroke)){
        qWarning("eraseFinish: stroke ptr is NULL, impossible to finish erase");
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
    if (!m_canvasCurrent->getVisibilityAll())
        m_canvasCurrent->setVisibilityAll(true);

    if (this->canvasEditValid()){
        qWarning("CanvasOffsetStart: cannot start editing since the canvas is already in edit mode");
        return;
    }
    m_canvasCurrent->setModeEdit(true);
    m_deltaT = osg::Vec3f(0.f,0.f,0.f);
}

void entity::UserScene::canvasOffsetAppend(const osg::Vec3f &t)
{
    if (!this->canvasEditValid()){
        qWarning("canvasOffsetAppend: canvas edit mode is not valid");
        return;
    }
    m_canvasCurrent->translate(osg::Matrix::translate(t.x(), t.y(), t.z()));
    m_deltaT = m_deltaT + t;
    this->updateWidgets();
}

void entity::UserScene::canvasOffsetFinish(QUndoStack *stack)
{
    if (!m_canvasCurrent.get()) return;
    if (!this->canvasEditValid()){
        m_canvasCurrent->setFrameEditable(false);
        return;
    }
    m_canvasCurrent->setModeEdit(false);
    m_canvasCurrent->translate(osg::Matrix::translate(-m_deltaT.x(), -m_deltaT.y(), -m_deltaT.z()));
    qDebug("canvas offset command created");
    fur::EditCanvasOffsetCommand* cmd = new fur::EditCanvasOffsetCommand(this, m_deltaT);
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
    if (!m_canvasCurrent->getVisibilityAll())
        m_canvasCurrent->setVisibilityAll(true);

    m_canvasCurrent->unselectAll();
    entity::Canvas* remain_prev = m_canvasPrevious.get();

    entity::Canvas* cnv = m_canvasCurrent->clone();
    if (!cnv){
        qWarning("canvasCloneStart: could not clone the canvas, ptr is NULL");
        return;
    }
    cnv->setName(this->getCanvasName());
    m_canvasClone = cnv;

    if (!m_groupCanvases->addChild(m_canvasClone.get())){
        qWarning("canvasCloneStart: could not add clone as a child");
        return;
    }

    /* have to set the clone as current so that to calculate offset correctly */
    if (!this->setCanvasCurrent(m_canvasClone.get())){
        qWarning("canvasCloneStart: could not set clone as current");
        m_groupCanvases->removeChild(m_canvasClone.get());
        m_canvasClone = 0;
        return;
    }
    this->setCanvasPrevious(remain_prev);
}

void entity::UserScene::canvasCloneAppend(const osg::Vec3f &t)
{
    if (!this->canvasCloneValid()){
        qWarning("canvasCloneAppend: clone is not valid");
        return;
    }

    /* clone is also current */
    m_canvasCurrent->translate(osg::Matrix::translate(t.x(), t.y(), t.z()));
    this->updateWidgets();
}

void entity::UserScene::canvasCloneFinish(QUndoStack *stack)
{
    if (!this->canvasCloneValid()){
        qWarning("canvasCloneFinish: clone is not valid");
        return;
    }

    this->setCanvasCurrent(m_canvasPrevious.get());

    fur::AddCanvasCommand* cmd = new fur::AddCanvasCommand(this, *(m_canvasClone.get()));
    m_groupCanvases->removeChild(m_canvasClone.get());
    m_canvasClone = 0;
    if (!cmd){
        qWarning("canvasCloneFinish: could not allocated AddCanvasCommand");
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
    if (!m_canvasCurrent->getVisibilityAll())
        m_canvasCurrent->setVisibilityAll(true);

    entity::Canvas* cnv = m_canvasCurrent->separate();
    if (!cnv){
        qWarning("canvasCloneStart: could not clone the canvas, ptr is NULL");
        return;
    }
    cnv->setName(this->getCanvasName());
    m_canvasClone = cnv;

    // now clone contains all the strokes needed
    m_canvasCurrent->unselectAll();

    if (!m_groupCanvases->addChild(m_canvasClone.get())){
        qWarning("canvasCloneStart: could not add clone as a child");
        return;
    }

    /* have to set the clone as current so that to calculate offset correctly */
    if (!this->setCanvasCurrent(m_canvasClone.get())){
        qWarning("canvasCloneStart: could not set clone as current");
        m_groupCanvases->removeChild(m_canvasClone.get());
        m_canvasClone = 0;
        return;
    }
}

void entity::UserScene::canvasSeparateAppend(const osg::Vec3f &t)
{
    if (!this->canvasCloneValid()){
        qWarning("canvasCloneAppend: clone is not valid");
        return;
    }

    /* clone is also current */
    m_canvasCurrent->translate(osg::Matrix::translate(t.x(), t.y(), t.z()));
    this->updateWidgets();
}

void entity::UserScene::canvasSeparateFinish(QUndoStack *stack)
{
    if (!m_canvasCurrent.get()) return;
    if (!this->canvasCloneValid()){
        qWarning("canvasCloneFinish: clone is not valid");
        return;
    }

    this->setCanvasCurrent(m_canvasPrevious.get());

    fur::AddCanvasSeparationCommand* cmd = new fur::AddCanvasSeparationCommand(this, m_canvasCurrent.get(),
                                                                     m_canvasClone.get());

    m_groupCanvases->removeChild(m_canvasClone.get());
    m_canvasClone = 0;
    if (!cmd){
        qWarning("canvasCloneFinish: could not allocated AddCanvasCommand");
        return;
    }
    stack->push(cmd);
}

void entity::UserScene::canvasRotateStart()
{
    /* if the canvas is hidden, show it all so that user could see where they sketch */
    if (!m_canvasCurrent->getVisibilityAll())
        m_canvasCurrent->setVisibilityAll(true);

    if (this->canvasEditValid()){
        qWarning("CanvasRotateStart: cannot start editing since the canvas is already in edit mode");
        return;
    }
    if (!m_canvasCurrent.get()){
        qWarning("canvasRotateStart: current canvas is NULL");
        return;
    }
    m_canvasCurrent->setModeEdit(true);
    m_deltaR = osg::Quat();
}

void entity::UserScene::canvasRotateAppend(const osg::Quat &r, const osg::Vec3f &center3d)
{
    if (!this->canvasEditValid()){
        qWarning("canvasRotateAppend: canvas edit mode is not valid");
        return;
    }
    if (!m_canvasCurrent.get()){
        qWarning("canvasRotateAppend: canvas is NULL");
        return;
    }

    m_canvasCurrent->rotate(osg::Matrix::rotate(r), center3d);
    m_canvasCurrent->updateFrame(m_canvasPrevious.get());
    m_deltaR = r * m_deltaR;
    this->updateWidgets();
}

void entity::UserScene::canvasRotateFinish(QUndoStack *stack)
{
    if (!m_canvasCurrent.get()) return;
    if (!this->canvasEditValid()){
        qWarning("canvasRotateFinish: no canvas in edit mode, impossible to finish offset mode");
        return;
    }
    m_canvasCurrent->setModeEdit(false);
    m_canvasCurrent->rotate(osg::Matrix::rotate(m_deltaR.inverse()), m_canvasCurrent->getBoundingBoxCenter3D());
    fur::EditCanvasRotateCommand* cmd = new fur::EditCanvasRotateCommand(this, m_deltaR);
    stack->push(cmd);
    m_deltaR = osg::Quat(0,0,0,1);
}

bool entity::UserScene::addCanvas(entity::Canvas *canvas)
{
    if (!canvas) qFatal("UserScene::addCanvas(Canvas*): canvas is NULL");

    // gui elements
    emit canvasAdded(canvas->getName());
    //see if any canvas contains any photos, they will be added to canvas widget
    for (size_t i=0; i<canvas->getNumPhotos(); ++i){
        entity::Photo* photo = canvas->getPhoto(i);
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
        for (size_t j=0; j<canvas->getNumPhotos(); ++j){
            entity::Photo* photo = canvas->getPhoto(j);
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
            if (canvas->getPhoto(0)){
                int start = this->getPhotoIndex(canvas->getPhoto(0), canvas);
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
            if (i<m_groupBookmarks->getNumBookmarks()-1)
                state->insertTransparency(idx, 0.f);
            else
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
            state->eraseTransparency(idx, 1);
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

#endif // DOXYGEN_SHOULD_SKIP_THIS

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
