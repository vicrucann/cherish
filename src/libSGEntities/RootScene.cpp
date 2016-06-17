#include "iostream"
#include <sstream>
#include <stdlib.h>
#include <assert.h>

#include <QtGlobal>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/Registry>

#include "RootScene.h"
#include "Settings.h"
#include "EditEntityCommand.h"

RootScene::RootScene(QUndoStack *undoStack)
    : osg::ProtectedGroup()
    , m_userScene(new entity::UserScene)
    , m_axisTool(new entity::AxisGlobalTool)
    , m_bookmarkTools(new osg::Group)
    , m_undoStack(undoStack)
    , m_saved(false)
    , m_visibilityBookmarkTool(true)
{
    // child #0
    m_userScene->initializeSG();
    if (!this->addChild(m_userScene.get())){
        outErrMsg("RootScene(): could not add user scene as a child");
    }

    // child #1
    if (!this->addChild(m_axisTool)){
        outErrMsg("RootScene(): could not add axes as a child");
    }

    /* child #2 */
    this->addChild(m_bookmarkTools);

    this->setName("RootScene");
}

entity::UserScene*RootScene::getUserScene() const
{
    return m_userScene.get();
}

void RootScene::setFilePath(const std::string& name)
{
    m_userScene->setFilePath(name);
    m_saved = false;
}

bool RootScene::isSetFilePath() const
{
    return m_userScene->isSetFilePath();
}

bool RootScene::isSavedToFile() const
{
    return m_saved;
}

void RootScene::setSavedToFile(bool saved)
{
    m_saved = saved;
}

bool RootScene::isEmptyScene() const
{
    return m_userScene->isEmptyScene();
}

void RootScene::clearUserData()
{
    m_userScene->clearUserData();
    m_buffer.clear();
}

void RootScene::setToolsVisibility(bool vis)
{
    this->setAxesVisibility(vis);
    this->setBookmarkToolVisibility(vis);
    for (int i=0; i<m_userScene->getNumCanvases(); ++i){
        entity::Canvas* cnv = m_userScene->getCanvas(i);
        if (!cnv) qFatal("RootScene::setToolsVisibility() canvas is NULL");
        cnv->setVisibilityFrameInternal(vis);
    }
}

bool RootScene::getToolsVisibility() const
{
    return this->getAxesVisibility();
}

void RootScene::setAxesVisibility(bool vis) {
    m_axisTool->setVisibility(vis);
}

bool RootScene::getAxesVisibility() const{
    return m_axisTool->getVisibility();
}

void RootScene::setCanvasVisibilityAll(entity::Canvas *canvas, bool vis)
{
    if (!canvas) return;
    canvas->setVisibilityAll(vis);
    emit m_userScene->canvasVisibilitySet(m_userScene->getCanvasIndex(canvas), vis);
}

bool RootScene::getCanvasVisibilityAll(entity::Canvas *canvas) const
{
    return canvas->getVisibilityAll();
}

bool RootScene::writeScenetoFile()
{
    bool result = true;
    if (m_userScene->getFilePath() == "") return false;

    /* save current scene state */
    osg::ref_ptr<entity::SceneState> state = new entity::SceneState;
    state->stripDataFrom(this);
    Q_ASSERT(!state->isEmpty());

    /* for each canvas, detach its tools */
    for (int i=0; i<m_userScene->getNumCanvases(); ++i){
        entity::Canvas* canvas = m_userScene->getCanvas(i);
        if (!canvas) continue;
        canvas->detachFrame();
    }

    if (!osgDB::writeNodeFile(*(m_userScene.get()), m_userScene->getFilePath())) result = false;

    /* for each canvas, attach its tools back */
    for (int i=0; i<m_userScene->getNumCanvases(); ++i){
        entity::Canvas* canvas = m_userScene->getCanvas(i);
        if (!canvas) continue;
        if (!canvas->attachFrame()){
            qCritical("RootScene::writeSceneToFile: could not attach the tools back");
            result = false;
        }
    }

    /* re-apply the saved scene state */
    Q_ASSERT(this->setSceneState(state));

    m_saved = result;
    return result;
}

bool RootScene::exportSceneToFile(const std::string &name)
{
    return osgDB::writeNodeFile(*this, name);
}

bool RootScene::loadSceneFromFile()
{
    if (!m_undoStack){
        outErrMsg("loadSceneFromFile(): undo stack is NULL. "
                 "Restart the program to ensure undo stack initialization.");
        return false;
    }
    if (m_userScene->getFilePath() == "")
        return false;
    osg::Node* node = osgDB::readNodeFile(m_userScene->getFilePath());
    if (!node){
        outErrMsg("loadSceneFromFile: node is NULL");
        return false;
    }
    qDebug() << "Loaded node, number of children: " << node->asGroup()->getNumChildren();

    osg::ref_ptr<entity::UserScene> newscene = dynamic_cast<entity::UserScene*>(node);
    if (!newscene.get()){
        outErrMsg("loadSceneFromFile: could not load from file, or could not perform the dynamic_cast<osg::Group*>");
        return false;
    }
    qDebug() << "Loaded scene, number of children: " << newscene->getNumChildren();
    qDebug() << "Loaded scene, number of canvases: " << newscene->getNumCanvases();

    /* replace the original */
    if (!this->replaceChild(m_userScene.get(), newscene.get())){
        outErrMsg("loadSceneFromFile: could not replace the original child");
        return false;
    }

    /* update pointer */
    m_userScene = newscene.get();

    /* load the construction tools and set photo textures */
    for (int i=0; i<m_userScene->getNumCanvases(); ++i){
        entity::Canvas* cnv = m_userScene->getCanvas(i);
        if (!cnv) qFatal("RootScene::loadSceneFromFile() canvas is NULL");
        cnv->initializeTools();
        cnv->initializeStateMachine();

        /* photo textures */
        for (size_t i=0; i<cnv->getNumPhotos(); ++i){
            entity::Photo* photo = cnv->getPhoto(i);
            if (!photo) continue;
            photo->getOrCreateStateSet()->setTextureAttributeAndModes(0, photo->getTextureAsAttribute());
        }
    }

    /* update current/previous canvases */
    for (int i=0; i<m_userScene->getNumCanvases(); ++i){
        entity::Canvas* cnv = m_userScene->getCanvas(i);
        if (!cnv) qFatal("RootScene::loadSceneFromFile() canvas is NULL");
        cnv->setColor(cher::CANVAS_CLR_REST);
        m_userScene->setCanvasCurrent(cnv);
    }
    newscene = 0;
    this->printScene();
    m_saved = true;
    return true;
}

int RootScene::getStrokeLevel() const
{
    return m_userScene->getStrokeLevel();
}

int RootScene::getCanvasLevel() const
{
    return m_userScene->getCanvasLevel();
}

int RootScene::getPhotoLevel() const
{
    return m_userScene->getPhotoLevel();
}

void RootScene::addCanvas(const osg::Matrix& R, const osg::Matrix& T)
{
    m_userScene->addCanvas(m_undoStack, R, T);
    m_saved = false;
}

void RootScene::addCanvas(const osg::Vec3f &normal, const osg::Vec3f &center)
{
    m_userScene->addCanvas(m_undoStack, normal, center);
    m_saved = false;
}

void RootScene::addCanvas(const osg::Matrix& R, const osg::Matrix& T, const std::string& name)
{
    m_userScene->addCanvas(m_undoStack, R, T, name);
    m_saved = false;
}

void RootScene::addStroke(float u, float v, cher::EVENT event)
{
    m_userScene->addStroke(m_undoStack, u, v, event);
    m_saved = false;
}

void RootScene::selectAllEntities()
{
    if (this->getCanvasCurrent())
        this->getCanvasCurrent()->selectAllEntities();
}

void RootScene::addPhoto(const std::string& fname)
{
    m_userScene->addPhoto(m_undoStack, fname);
    m_saved = false;
}

void RootScene::addBookmark(BookmarkWidget *widget, const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up, const double &fov)
{
    m_userScene->addBookmark(widget, eye, center, up, fov);
}

void RootScene::addBookmarkTool(const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up)
{
    if (!m_bookmarkTools) {
        outLogMsg("addBookmarkTool: none will be added, ptr is null");
        return;
    }
    entity::BookmarkTool* bt = new entity::BookmarkTool(eye, center, up);
    if (!bt) return;
    if (!m_bookmarkTools->addChild(bt))
        outLogMsg("addBookmarkTool: could not add as child");
    bt->setVisibility(m_visibilityBookmarkTool);

}

void RootScene::updateBookmark(BookmarkWidget *widget, int row)
{
    m_userScene->updateBookmark(widget, row);
}

void RootScene::deleteBookmark(BookmarkWidget *widget, const QModelIndex &index)
{
    m_userScene->deleteBookmark(widget, index);
}

void RootScene::deleteBookmarkTool(int first, int last)
{
    if (first>=int(m_bookmarkTools->getNumChildren()) || last>=int(m_bookmarkTools->getNumChildren())) return;
    m_bookmarkTools->removeChild(first, std::abs(last-first+1));
    outLogMsg("bookmarkTool deleted");
}

void RootScene::resetBookmarks(BookmarkWidget *widget)
{
    entity::Bookmarks* bms = this->getBookmarksModel();
    if (!bms){
        outErrMsg("resetBookmarkTools: could not obtain bookmarks data");
        return;
    }
    bms->resetModel(widget);

//    const std::vector<osg::Vec3d>& eyes = bms->getEyes();
//    const std::vector<osg::Vec3d>& centers = bms->getCenters();
//    const std::vector<osg::Vec3d>& ups = bms->getUps();
//    for (size_t i=0; i < bms->getNames().size(); ++i){
//        m_bookmarkTools->addChild(new entity::BookmarkTool(eyes[i], centers[i], ups[i]));
//    }
}

void RootScene::setBookmarkToolVisibility(bool vis)
{
    m_visibilityBookmarkTool = vis;
    for (size_t i=0; i<m_bookmarkTools->getNumChildren(); i++){
        entity::BookmarkTool* bt = dynamic_cast<entity::BookmarkTool*>(m_bookmarkTools->getChild(i));
        if (bt)
            bt->setVisibility(m_visibilityBookmarkTool);
    }
}

bool RootScene::getBookmarkToolVisibility() const
{
    bool result = m_visibilityBookmarkTool;
    for (size_t i=0; i<m_bookmarkTools->getNumChildren(); ++i){
        entity::BookmarkTool* bt = dynamic_cast<entity::BookmarkTool*>(m_bookmarkTools->getChild(i));
        if (bt){
            if (bt->getVisibility() != m_visibilityBookmarkTool){
                outErrMsg("Re-setting bookmark tool visibility");
                bt->setVisibility(m_visibilityBookmarkTool);
            }
        }
    }
    return result;
}

void RootScene::eraseStroke(entity::Stroke *stroke, int first, int last, cher::EVENT event)
{
    m_userScene->eraseStroke(m_undoStack, stroke, first, last, event);
    m_saved = false;
}

bool RootScene::setCanvasCurrent(entity::Canvas* cnv)
{
    return m_userScene->setCanvasCurrent(cnv);
}

bool RootScene::setCanvasPrevious(entity::Canvas* cnv)
{
    return m_userScene->setCanvasPrevious(cnv);
}

void RootScene::setCanvasesButCurrent(bool enable)
{
    m_userScene->setCanvasesButCurrent(enable);
}

entity::Canvas* RootScene::getCanvasCurrent() const
{
    return m_userScene->getCanvasCurrent();
}

entity::Canvas* RootScene::getCanvasPrevious() const
{
    return m_userScene->getCanvasPrevious();
}

entity::Bookmarks *RootScene::getBookmarksModel() const
{
    return m_userScene->getBookmarksModel();
}

void RootScene::editCanvasOffset(const osg::Vec3f& translate, cher::EVENT event)
{
    m_userScene->editCanvasOffset(m_undoStack, translate, event);
    m_saved = false;
}

void RootScene::editCanvasRotate(const osg::Quat &rotation, const osg::Vec3f &center3d, cher::EVENT event)
{
    m_userScene->editCanvasRotate(m_undoStack, rotation, center3d, event);
    m_saved = false;
}

void RootScene::editCanvasClone(const osg::Vec3f &translate, cher::EVENT event)
{
    m_userScene->editCanvasClone(m_undoStack, translate, event);
    m_saved = false;
}

void RootScene::editCanvasSeparate(const osg::Vec3f &translate, cher::EVENT event)
{
    m_userScene->editCanvasSeparate(m_undoStack, translate, event);
    m_saved = false;
}

void RootScene::editCanvasDelete(entity::Canvas *canvas)
{
    m_userScene->editCanvasDelete(m_undoStack, canvas);
}

void RootScene::editPhotoDelete(entity::Photo *photo, entity::Canvas *canvas)
{
    m_userScene->editPhotoDelete(m_undoStack, photo, canvas);
    m_saved = false;
}

void RootScene::editPhotoPush(entity::Photo *photo, entity::Canvas *source, entity::Canvas *destination)
{
    m_userScene->editPhotoPush(m_undoStack, photo, source, destination);
    m_saved = false;
}

void RootScene::editStrokesPush(osg::Camera* camera)
{
    m_userScene->editStrokesPush(m_undoStack, camera);
    m_saved = false;
}

void RootScene::editStrokesMove(double u, double v, cher::EVENT event)
{
    m_userScene->editStrokesMove(m_undoStack, u, v, event);
    m_saved = false;
}

void RootScene::editStrokesScale(double u, double v, cher::EVENT event)
{
    m_userScene->editStrokesScale(m_undoStack, u, v, event);
    m_saved = false;
}

void RootScene::editStrokesRotate(double u, double v, cher::EVENT event)
{
    m_userScene->editStrokesRotate(m_undoStack, u, v, event);
    m_saved = false;
}

void RootScene::editStrokeDelete(entity::Stroke *stroke)
{
    m_userScene->editStrokeDelete(m_undoStack, stroke);
    m_saved = false;
}

/* copies the selected strokes into buffer, makes a deep copy */
void RootScene::copyToBuffer()
{
    m_buffer.clear();
    if (!m_userScene->getCanvasCurrent()) return;

    const std::vector<entity::Entity2D*>& selected = m_userScene->getCanvasCurrent()->getEntitiesSelected();
    if (selected.size()==0) return;

    for (size_t i=0; i<selected.size(); ++i){
        const entity::Stroke& copy = dynamic_cast<const entity::Stroke&>( *selected.at(i));
        entity::Stroke* stroke = new entity::Stroke(copy, osg::CopyOp::DEEP_COPY_ALL);
        if (!stroke) continue;
        m_buffer.push_back(stroke);
    }
}

/* also removes the selected strokes from current canvas */
void RootScene::cutToBuffer()
{
    m_buffer.clear();
    if (!m_userScene->getCanvasCurrent()) return;
    const std::vector<entity::Entity2D*>& selected = m_userScene->getCanvasCurrent()->getEntitiesSelected();
    if (selected.size()==0) return;

    EditCutCommand* cmd = new EditCutCommand(m_userScene.get(), this->getCanvasCurrent(),
                                             this->getCanvasCurrent()->getEntitiesSelected(),
                                             m_buffer);
    if (!cmd) return;
    m_undoStack->push(cmd);
}

void RootScene::pasteFromBuffer()
{
    if (m_buffer.size()==0) return;

    EditPasteCommand* cmd = new EditPasteCommand(m_userScene.get(), this->getCanvasCurrent(), m_buffer);
    if (!cmd) return;
    m_undoStack->push(cmd);
    m_saved = false;
}

entity::SceneState *RootScene::createSceneState() const
{
    osg::ref_ptr<entity::SceneState> state = new entity::SceneState;
    if (!state.get()) return 0;

    state->clear();
    state->setAxisFlag(this->getAxesVisibility());
    state->setBookmarksFlag(this->getBookmarkToolVisibility());

    size_t sz = m_userScene->getNumCanvases();
    for (size_t i=0; i<sz; ++i){
        entity::Canvas* cnv = m_userScene->getCanvas(i);
        if (!cnv) continue;
        state->pushDataFlag(cnv->getVisibilityAll());
        state->pushToolFlag(cnv->getVisibilityFrameInternal());
        for (size_t j=0; j<cnv->getNumPhotos(); ++j){
            entity::Photo* photo = cnv->getPhoto(j);
            if (!photo) continue;
            state->pushTransparency(photo->getTransparency());
        }
    }
    return state.release();
}

bool RootScene::setSceneState(const entity::SceneState *state)
{
    if (!state) return false;

    this->setAxesVisibility(state->getAxisFlag());
    this->setBookmarkToolVisibility(state->getBookmarksFlag());

    if (state->isEmpty()) return false;

    const std::vector<bool>& cdf = state->getCanvasDataFlags();
    const std::vector<bool>& ctf = state->getCanvasToolFlags();
    const std::vector<float>& pt = state->getPhotoTransparencies();
    size_t sz = m_userScene->getNumCanvases();
    size_t szP = m_userScene->getNumPhotos();

    if (cdf.size() != ctf.size()) return false;
    if (cdf.size() != sz || pt.size() != szP) return false;

    int idx = 0;
    for (size_t i=0; i<sz; ++i){
        entity::Canvas* cnv = m_userScene->getCanvas(i);
        if (!cnv) continue;
        cnv->setVisibilityAll(cdf[i]);
        emit m_userScene->canvasVisibilitySet(m_userScene->getCanvasIndex(cnv) , cdf[i]);
        cnv->setVisibilityFrameInternal(ctf[i]);
        for (size_t j=0; j<cnv->getNumPhotos(); ++j){
            entity::Photo* photo = cnv->getPhoto(j);
            if (!photo) continue;
            photo->setTransparency(pt[idx]);
            idx++;
        }
    }

    return true;
}

RootScene::~RootScene()
{

}

void RootScene::printScene()
{
    outLogVal("RootScene #children", this->getNumChildren());
    for (unsigned int i=0; i<this->getNumChildren(); ++i){
        outLogVal("Child", this->getChild(i)->getName().c_str());
    }
    outLogMsg("For each canvas");
    m_userScene->printScene();
}




