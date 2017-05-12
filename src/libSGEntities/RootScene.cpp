#include "RootScene.h"

#include "iostream"
#include <sstream>
#include <stdlib.h>

#include <QtGlobal>
#include <QDebug>
#include <QMessageBox>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/ReaderWriter>
#include <osgDB/Registry>
#include <osgDB/Options>

#include "Settings.h"
#include "Utilities.h"
#include "EditEntityCommand.h"
#include "MainWindow.h"

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
        qWarning("RootScene(): could not add user scene as a child");
    }

    // child #1
    if (!this->addChild(m_axisTool)){
        qWarning("RootScene(): could not add axes as a child");
    }

    /* child #2 */
    this->addChild(m_bookmarkTools);

    this->setName("RootScene");
}

entity::UserScene*RootScene::getUserScene() const
{
    return m_userScene.get();
}

QUndoStack *RootScene::getUndoStack() const
{
    return m_undoStack;
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

    if (!osgDB::writeNodeFile(*(m_userScene.get()), m_userScene->getFilePath(), new osgDB::Options("WriteImageHint=IncludeData")))
        result = false;

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
    bool stateset = this->setSceneState(state);
    Q_ASSERT(stateset);

    m_saved = result;
    return result;
}

bool RootScene::exportSceneToFile(const std::string &name)
{
    if (name == "") return false;

    /* save current scene state */
    osg::ref_ptr<entity::SceneState> state = new entity::SceneState;
    state->stripDataFrom(this);
    Q_ASSERT(!state->isEmpty());

    std::vector< osg::ref_ptr<osg::Group> > meshes;
    /* for each canvas, detach its tools */
    for (int i=0; i<m_userScene->getNumCanvases(); ++i){
        entity::Canvas* canvas = m_userScene->getCanvas(i);
        if (!canvas) continue;
        canvas->detachFrame();

        /* attach mesh group */
        meshes.push_back(canvas->attachMeshGroup());

        // for all the strokes within canvas, add the correspondig mesh to the mesh group
        for (unsigned int j=0; j<canvas->getNumStrokes(); ++j){
            entity::Stroke* stroke = canvas->getStroke(j);
            if (!stroke) continue;
            osg::ref_ptr<osg::Node> mesh = stroke->getMeshRepresentation();
            if (!mesh.get()){
                qWarning("Could not obtain mesh represenation from the stroke.");
                continue;
            }
            bool added = meshes.back()->addChild(mesh.get());
            if (!added){
                qWarning("Could not attach mesh to the mesh group.");
                continue;
            }
        }
        for (unsigned int j=0; j<canvas->getNumLineSegments(); ++j){
            entity::LineSegment* segment = canvas->getLineSegment(j);
            if (!segment) continue;
            osg::ref_ptr<osg::Node> mesh = segment->getMeshRepresentation();
            if (!mesh.get()){
                qWarning("Could not obtain mesh represenation from the line segment.");
                continue;
            }
            bool added = meshes.back()->addChild(mesh.get());
            if (!added){
                qWarning("Could not attach mesh to the mesh group.");
                continue;
            }
        }

    }

    Q_CHECK_PTR(m_userScene->getGroupCanvases());
    bool result = osgDB::writeNodeFile(*(m_userScene->getGroupCanvases()), name);

    /* for each canvas, attach its tools back */
    for (int i=0; i<m_userScene->getNumCanvases(); ++i){
        entity::Canvas* canvas = m_userScene->getCanvas(i);
        if (!canvas) continue;
        if (!canvas->attachFrame()){
            qCritical("RootScene::writeSceneToFile: could not attach the tools back");
            result = false;
        }
        /* delete the mesh group */
        if ( !canvas->disattachMeshGroup(meshes.at(i).get()) ){
            qCritical("Failed to remove helper meshes. The scene graph is messed up.");
            return false;
        }

    }

    bool stateset = this->setSceneState(state);
    Q_ASSERT(stateset);

    return result;
}

bool RootScene::loadSceneFromFile()
{
    if (!m_undoStack){
        qWarning("loadSceneFromFile(): undo stack is NULL. "
                 "Restart the program to ensure undo stack initialization.");
        return false;
    }
    if (m_userScene->getFilePath() == "")
        return false;
    osg::Node* node = osgDB::readNodeFile(m_userScene->getFilePath());
    if (!node){
        qWarning("loadSceneFromFile: node is NULL");
        return false;
    }

    osg::ref_ptr<entity::UserScene> newscene = dynamic_cast<entity::UserScene*>(node);
    if (!newscene.get()){
        qWarning("loadSceneFromFile: could not load from file, or could not perform the dynamic_cast<osg::Group*>");
        return false;
    }
    qDebug() << "Loaded scene, number of children: " << newscene->getNumChildren();
    qDebug() << "Loaded scene, number of canvases: " << newscene->getNumCanvases();

    /* replace the original */
    if (!this->replaceChild(m_userScene.get(), newscene.get())){
        qWarning("loadSceneFromFile: could not replace the original child");
        return false;
    }

    /* update pointer */
    m_userScene = newscene.get();

    /* load the construction tools, set photo textures */
    for (int i=0; i<m_userScene->getNumCanvases(); ++i){
        entity::Canvas* cnv = m_userScene->getCanvas(i);
        if (!cnv) qFatal("RootScene::loadSceneFromFile() canvas is NULL");
        cnv->initializeTools();
        cnv->initializeStateMachine();
        cnv->initializeMasks();
        cnv->initializeProgramStroke();
        cnv->initializeProgramPolygon();
        cnv->initializeProgramLineSegment();

        /* photo textures */
        for (size_t j=0; j<cnv->getNumPhotos(); ++j){
            entity::Photo* photo = cnv->getPhoto(j);
            if (!photo) continue;
            photo->getOrCreateStateSet()->setTextureAttributeAndModes(0, photo->getTextureAsAttribute());
        }

        /* stroke curved */
        for (size_t k=0; k<cnv->getNumStrokes(); ++k){
            entity::Stroke* stroke = cnv->getStroke(k);
            if (!stroke) {
                qWarning("Could not read stroke");
                continue;
            }
            stroke->initializeProgram(cnv->getProgramStroke());
            if (!stroke->redefineToShape(cnv->getTransform()))
                qWarning("Could not redefine stroke as curve");
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

void RootScene::addPolygon(float u, float v, cher::EVENT event)
{
    m_userScene->addPolygon(m_undoStack, u, v, event);
    m_saved = false;
}

void RootScene::addLineSegment(float u, float v, cher::EVENT event)
{
    m_userScene->addLineSegment(m_undoStack, u, v, event);
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

bool RootScene::addSVMData()
{
    /* extract the lastly added bookmark (entity::SceneState) */
    Q_CHECK_PTR(m_userScene->getBookmarks());
    entity::SceneState* ss = m_userScene->getBookmarksModel()->getLastSceneState();
    if (!ss) return false;

    /* take current and previous canvases to create SVMData */
    entity::Canvas* wall = m_userScene->getCanvasCurrent();
    entity::Canvas* floor = m_userScene->getCanvasPrevious();
    if (!wall || !floor) return false;

    /* Add the svm data with the given canvas parameters */
    bool added = ss->addSVMData(wall->getMatrix(), floor->getMatrix());
    return added;
}

bool RootScene::addPhotoScaleData()
{
    entity::Canvas* model = m_userScene->getCanvasCurrent();
    entity::Canvas* image = m_userScene->getCanvasPrevious();
    if (!model || !image) return false;

    osg::ref_ptr<entity::SVMData> svm = new entity::SVMData;
    Q_CHECK_PTR(svm.get());
    svm->setNodeMask(cher::MASK_SVMDATA_IN);
    svm->setTransformWall(image->getMatrix());
    svm->setTransformFloor(model->getMatrix());
    return this->addChild(svm.get());
}

bool RootScene::hidePhotoScaleData()
{
    bool hidden = false;
    for (unsigned int i=0; i<this->getNumChildren(); ++i){
        entity::SVMData* svm = dynamic_cast<entity::SVMData*>( this->getChild(i));
        if (!svm) continue;
        svm->setVisibility(false);
        hidden = true;
    }
    return hidden;
}

bool RootScene::removePhotoScaleData()
{
    bool removed = false;
    for (unsigned int i=0; i<this->getNumChildren(); ++i){
        entity::SVMData* svm = dynamic_cast<entity::SVMData*>( this->getChild(i));
        if (!svm) continue;
        removed = this->removeChild(svm);
        break;
    }
    return removed;
}

const entity::SVMData *RootScene::getPhotoScaleData() const
{
    for (unsigned int i=0; i<this->getNumChildren(); ++i){
        const entity::SVMData* svm = dynamic_cast<const entity::SVMData*>( this->getChild(i));
        if (!svm) continue;
        return svm;
    }
    return nullptr;
}

bool RootScene::addCamPoseData()
{
    Q_CHECK_PTR(m_userScene->getBookmarks());
    entity::SceneState* ss = m_userScene->getBookmarksModel()->getLastSceneState();
    if (!ss) return false;

    /* take current canvas transform to create CamPoseData */
    entity::Canvas* cnv = m_userScene->getCanvasCurrent();
    if (!cnv) return false;

    /* add campose data with the given params */
    double fov = MainWindow::instance().getFOV2();
    bool added = ss->addCamPoseData(cnv->getMatrix(), fov);
    return added;
}

void RootScene::hideAndUpdateSVMData()
{
    Q_CHECK_PTR(m_userScene->getBookmarks());
    int num = m_userScene->getBookmarks()->getNumBookmarks();
    for (int i=0; i<num; ++i){
        entity::SceneState* ss = m_userScene->getBookmarksModel()->getSceneState(i);
        entity::SVMData* svm = ss->getSVMData();
        if (!svm) continue;
        bool vis = svm->getVisibility();
        if (!vis) continue;
        /* if the wire is present and about to be hidden, update the corresponding camera position. */
//        osg::Matrixd projection;
//        bool projected = Utilities::getProjectionMatrix(svm, projection);
//        if (!projected){
//            qWarning("Could not obtain projection matrix, the bookmark will not be updated");
//            continue;
//        }
        // apply the projection for the "image" canvas

        // re-calculate the camera position

        osg::Vec3f eye, center, up;
//        bool success = Utilities::getCameraPosition(svm, eye, center, up);
//        if (!success){
//            qWarning("Could not obtain camera pose from the given Homography");
//            continue;
//        }
        // edit camera pose by editing: bookmark tool position; Bookmarks data.
        entity::Bookmarks* bms = m_userScene->getBookmarksModel();
        if (!bms){
            qWarning("Could not exatract bookmarks pointer for editing");
            continue;
        }
        double fov = MainWindow::instance().getFOV2();
        bool edited = bms->editBookmarkPose(i, eye, center, up, fov);
        if (!edited){
            qWarning("Could not edit the bookmark position");
            continue;
        }

        entity::BookmarkTool* bt = this->getBookmarkTool(i);
        if (!bt){
            qWarning("Could not extract bookmark tool pointer");
            continue;
        }
        bt->setPose(eye, center, up);

        // trigger update
        emit m_userScene->updateWidgets();

        /* hide the wires */
        svm->setVisibility(false);
    }
}

void RootScene::hideAndUpdateCamPoseData()
{
    Q_CHECK_PTR(m_userScene->getBookmarks());
    int num = m_userScene->getBookmarks()->getNumBookmarks();
    for (int i=0; i<num; ++i){
        entity::SceneState* ss = m_userScene->getBookmarksModel()->getSceneState(i);
        entity::CamPoseData* cam = ss->getCamPoseData();
        if (!cam) continue;
        bool vis = cam->getVisibility();
        if (!vis) continue;
        // get new camera pose
        double fov2;
        osg::Vec3f eye,center,up;
        cam->getCamera(eye,center, up, fov2);
        // edit camera pose by editing: current camera pose, bookmark tool position; Bookmarks data.
        entity::Bookmarks* bms = m_userScene->getBookmarksModel();
        if (!bms){
            qWarning("Could not exatract bookmarks pointer for editing");
            continue;
        }
        // reset the current view
        MainWindow::instance().setCameraView(eye, center, up, fov2);
        // edit bookmark position
        bool edited = bms->editBookmarkPose(i, eye, center, up, fov2);
        if (!edited){
            qWarning("Could not edit the bookmark position");
            continue;
        }
        // edit phisical tool position.
        entity::BookmarkTool* bt = this->getBookmarkTool(i);
        if (!bt){
            qWarning("Could not extract bookmark tool pointer");
            continue;
        }
        bt->setPose(eye, center, up);
        // trigger update
        emit m_userScene->updateWidgets();

        /* hide the wires */
        cam->setVisibility(false);

        // request screenshot update
        BookmarkWidget* widget = MainWindow::instance().getBookmarkWidget();
        this->updateBookmark(widget, i);

        // suggest to insert new canvas in the FOV of the new camera pose.
        // In this canvas an image will be placed.
        emit m_userScene->requestCanvasCreate(eye, center, up);
        break;
    }
}

entity::SVMData *RootScene::getSVMDataCurrent() const
{
    Q_CHECK_PTR(m_userScene->getBookmarks());
    int num = m_userScene->getBookmarks()->getNumBookmarks();
    entity::SVMData* result = nullptr;
    for (int i=0; i<num; ++i){
        entity::SceneState* ss = m_userScene->getBookmarksModel()->getSceneState(i);
        if (!ss) return 0;
        entity::SVMData* svm = ss->getSVMData();
        if (!svm) continue;
        bool vis = svm->getVisibility();
        if (!vis) continue;
        result = svm;
    }
    return result;
}

entity::CamPoseData *RootScene::getCamPoseDataCurrent() const
{
    Q_CHECK_PTR(m_userScene->getBookmarks());
    int num = m_userScene->getBookmarks()->getNumBookmarks();
    entity::CamPoseData* result = nullptr;
    for (int i=0; i<num; ++i){
        entity::SceneState* ss = m_userScene->getBookmarksModel()->getSceneState(i);
        if (!ss) return nullptr;
        entity::CamPoseData* cam = ss->getCamPoseData();
        if (!cam) continue;
        bool vis = cam->getVisibility();
        if (!vis) continue;
        result = cam;
    }
    return result;
}

void RootScene::addBookmarkTool(const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up)
{
    if (!m_bookmarkTools) {
        qDebug("addBookmarkTool: none will be added, ptr is null");
        return;
    }
    entity::BookmarkTool* bt = new entity::BookmarkTool(eye, center, up);
    if (!bt) return;
    if (!m_bookmarkTools->addChild(bt))
        qDebug("addBookmarkTool: could not add as child");
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
    qDebug("bookmarkTool deleted");
}

void RootScene::resetBookmarks(BookmarkWidget *widget)
{
    entity::Bookmarks* bms = this->getBookmarksModel();
    if (!bms){
        qWarning("resetBookmarkTools: could not obtain bookmarks data");
        return;
    }
    bms->resetModel(widget);
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
                qWarning("Re-setting bookmark tool visibility");
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

void RootScene::setAllCanvases(bool enable)
{
    m_userScene->setAllCanvases(enable);
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

void RootScene::editPolygonDelete(entity::Polygon *poly, entity::Canvas *canvas)
{
    m_userScene->editPolygonDelete(m_undoStack, poly, canvas);
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
    entity::Canvas* canvas = m_userScene->getCanvasCurrent();
    if (!canvas) return;

    const std::vector<entity::Entity2D*>& selected = canvas->getEntitiesSelected();
    if (selected.size()==0) return;

    for (size_t i=0; i<selected.size(); ++i){
        const entity::Stroke& copy = dynamic_cast<const entity::Stroke&>( *selected.at(i));

        entity::Stroke* stroke = new entity::Stroke;
        if (!stroke) {
            qWarning("Could not allocated stroke");
            continue;
        }
        stroke->copyFrom(&copy);
        stroke->redefineToShape(copy.getProgram()->getTransform());
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

    fur::EditCutCommand* cmd = new fur::EditCutCommand(m_userScene.get(), this->getCanvasCurrent(),
                                             this->getCanvasCurrent()->getEntitiesSelected(),
                                             m_buffer);
    if (!cmd) return;
    m_undoStack->push(cmd);
}

void RootScene::pasteFromBuffer()
{
    if (m_buffer.size()==0) return;

    fur::EditPasteCommand* cmd = new fur::EditPasteCommand(m_userScene.get(), this->getCanvasCurrent(), m_buffer);
    if (!cmd) return;
    m_undoStack->push(cmd);
    m_saved = false;
}

const std::vector<osg::ref_ptr<entity::Entity2D> > &RootScene::getBuffer() const
{
    return m_buffer;
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

entity::BookmarkTool *RootScene::getBookmarkTool(int index)
{
    return dynamic_cast<entity::BookmarkTool*>(m_bookmarkTools->getChild(index));
}



