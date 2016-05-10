#include "iostream"
#include <sstream>
#include <stdlib.h>
#include <assert.h>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/Registry>

#include "RootScene.h"
#include "Settings.h"
#include "EditEntityCommand.h"

RootScene::RootScene(QUndoStack *undoStack)
    : osg::Group()
    , m_userScene(new entity::UserScene)
    , m_axisTool(new entity::AxisGlobalTool)
    , m_bookmarkGroup(new osg::Group)
    , m_undoStack(undoStack)
    , m_saved(false)
{
    // child #0
    if (!this->addChild(m_userScene.get())){
        outErrMsg("RootScene(): could not add user scene as a child");
        fatalMsg("RootScene(): could not add user scene as a child");
    }

    // child #1
    if (!this->addChild(m_axisTool)){
        outErrMsg("RootScene(): could not add axes as a child");
        fatalMsg("RootScene(): could not add axes as a child");
    }

    /* child #2 */
    this->addChild(m_bookmarkGroup);

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
    for (unsigned int i=0; i<m_userScene->getNumChildren(); ++i){
        entity::Canvas* cnv = m_userScene->getCanvas(i);
        if (!cnv)
            continue;
        cnv->setVisibilityFrame(vis);
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

bool RootScene::writeScenetoFile()
{
    if (m_userScene->getFilePath() == "")
        return false;
    if (osgDB::writeNodeFile(*(m_userScene.get()), m_userScene->getFilePath())){
        m_saved = true;
        return true;
    }
    return false;
}

bool RootScene::exportSceneToFile(const std::string &name)
{
    return osgDB::writeNodeFile(*this, name);
}

bool RootScene::loadSceneFromFile()
{
    if (!m_undoStack){
        fatalMsg("loadSceneFromFile(): undo stack is NULL. "
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
    osg::ref_ptr<entity::UserScene> newscene = dynamic_cast<entity::UserScene*>(node);
    if (!newscene.get()){
        outErrMsg("loadSceneFromFile: could not load from file, or could not perform the dynamic_cast<osg::Group*>");
        return false;
    }

    /* replace the original */
    if (!this->replaceChild(m_userScene.get(), newscene.get())){
        outErrMsg("loadSceneFromFile: could not replace the original child");
        return false;
    }

    /* update pointer */
    m_userScene = newscene.get();

    /* load the construction tools and set photo textures */
    for (unsigned int i=0; i<m_userScene->getNumChildren(); ++i){
        entity::Canvas* cnv = m_userScene->getCanvas(i);
        if (!cnv){
            continue;
        }
        cnv->initializeTools();

        /* photo textures */
        for (size_t i=0; i<cnv->getGeodeData()->getNumChildren(); ++i){
            entity::Photo* photo = dynamic_cast<entity::Photo*>(cnv->getGeodeData()->getChild(i));
            if (!photo) continue;
            photo->getOrCreateStateSet()->setTextureAttributeAndModes(0, photo->getTextureAsAttribute());
        }
    }

    /* update current/previous canvases */
    for (unsigned int i=0; i<m_userScene->getNumChildren(); ++i){
        entity::Canvas* cnv = m_userScene->getCanvas(i);
        if (!cnv){
            continue;
        }
        cnv->setColor(dureu::CANVAS_CLR_REST);
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

void RootScene::addStroke(float u, float v, dureu::EVENT event)
{
    m_userScene->addStroke(m_undoStack, u, v, event);
    m_saved = false;
}

void RootScene::selectAllStrokes()
{
    if (this->getCanvasCurrent())
        this->getCanvasCurrent()->selectAllStrokes();
}

void RootScene::addPhoto(const std::string& fname)
{
    m_userScene->addPhoto(m_undoStack, fname);
    m_saved = false;
}

void RootScene::addBookmark(BookmarkWidget *widget, const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up)
{
    m_userScene->addBookmark(widget, eye, center, up);
}

void RootScene::addBookmarkTool(const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up)
{
    if (!m_bookmarkGroup) {
        outLogMsg("addBookmarkTool: none will be added, ptr is null");
        return;
    }
    if (!m_bookmarkGroup->addChild(new entity::BookmarkTool(eye, center, up)))
        outLogMsg("addBookmarkTool: could not add as child");
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
    if (first>=int(m_bookmarkGroup->getNumChildren()) || last>=int(m_bookmarkGroup->getNumChildren())) return;
    m_bookmarkGroup->removeChild(first, std::abs(last-first+1));
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
//        m_bookmarkGroup->addChild(new entity::BookmarkTool(eyes[i], centers[i], ups[i]));
//    }
}

void RootScene::setBookmarkToolVisibility(bool vis)
{
    // TODO
    for (size_t i=0; i<m_bookmarkGroup->getNumChildren(); i++){
        entity::BookmarkTool* bt = dynamic_cast<entity::BookmarkTool*>(m_bookmarkGroup->getChild(i));
        if (bt)
            bt->setVisibility(vis);
    }
}

void RootScene::eraseStroke(entity::Stroke *stroke, int first, int last, dureu::EVENT event)
{
    m_userScene->eraseStroke(m_undoStack, stroke, first, last, event);
    m_saved = false;
}

void RootScene::setCanvasSelected(entity::Canvas *cnv)
{
    m_userScene->setCanvasSelected(cnv);
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

entity::Canvas *RootScene::getCanvasSelected() const
{
    return m_userScene->getCanvasSelected();
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

void RootScene::editCanvasOffset(const osg::Vec3f& translate, dureu::EVENT event)
{
    m_userScene->editCanvasOffset(m_undoStack, translate, event);
    m_saved = false;
}

void RootScene::editCanvasRotate(const osg::Quat &rotation, const osg::Vec3f &center3d, dureu::EVENT event)
{
    m_userScene->editCanvasRotate(m_undoStack, rotation, center3d, event);
    m_saved = false;
}

void RootScene::editCanvasClone(const osg::Vec3f &translate, dureu::EVENT event)
{
    m_userScene->editCanvasClone(m_undoStack, translate, event);
    m_saved = false;
}

void RootScene::editCanvasSeparate(const osg::Vec3f &translate, dureu::EVENT event)
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

void RootScene::editStrokesMove(double u, double v, dureu::EVENT event)
{
    m_userScene->editStrokesMove(m_undoStack, u, v, event);
    m_saved = false;
}

void RootScene::editStrokesScale(double u, double v, dureu::EVENT event)
{
    m_userScene->editStrokesScale(m_undoStack, u, v, event);
    m_saved = false;
}

void RootScene::editStrokesRotate(double u, double v, dureu::EVENT event)
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

    const std::vector<entity::Entity2D*>& selected = m_userScene->getCanvasCurrent()->getStrokesSelected();
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
    const std::vector<entity::Entity2D*>& selected = m_userScene->getCanvasCurrent()->getStrokesSelected();
    if (selected.size()==0) return;

    EditCutCommand* cmd = new EditCutCommand(m_userScene.get(), this->getCanvasCurrent(),
                                             this->getCanvasCurrent()->getStrokesSelected(),
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

RootScene::~RootScene()
{

}

void RootScene::printScene()
{
    outLogVal("RootScene #children", this->getNumChildren());
    for (unsigned int i=0; i<this->getNumChildren(); ++i){
        outLogVal("Child", this->getChild(i)->getName());
    }
    outLogMsg("For each canvas");
    m_userScene->printScene();
}




