#include <iostream>

#include <QMdiSubWindow>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QtWidgets>
#include <QObject>
#include <QRect>
#include <QSize>
#include <QDebug>
#include <QMessageBox>
#include <QFileInfo>

#include <osg/MatrixTransform>

#include "MainWindow.h"
#include "GLWidget.h"
#include "ListDelegate.h"
#include "CameraProperties.h"
#include "Settings.h"
#include "Data.h"
#include "Utilities.h"

MainWindow* MainWindow::m_instance = nullptr;

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
    , m_mdiArea(new QMdiArea(this))

    , m_tabWidget(new QTabWidget())
    , m_bookmarkWidget(new BookmarkWidget())
    , m_canvasWidget(new CanvasPhotoWidget())
    , m_photoWidget(new PhotoWidget())
    , m_photoModel(new PhotoModel)

    , m_undoStack(new QUndoStack(this))

    , m_menuBar(new QMenuBar(this))

    , m_rootScene(new RootScene(m_undoStack))
    , m_viewStack(new QUndoStack(this))
    , m_glWidget(new GLWidget(m_rootScene.get(), m_viewStack))
    , m_cameraProperties( new CameraProperties(60.f, this) )
    , m_colorDialog(new QColorDialog(this))
{
    /* singleton check and setup */
    Q_ASSERT_X(m_instance == 0, "MainWindow ctor", "MainWindow is a singleton and cannot be created more than once");
    m_instance = this;

    this->setMenuBar(m_menuBar);
//    this->setIconSize(QSize(cher::APP_MAINWINDOW_ICONSIZE * cher::DPI_SCALING, cher::APP_MAINWINDOW_ICONSIZE * cher::DPI_SCALING));

    /* Create GLWidgets */
    //this->onCreateViewer();
    QMdiSubWindow* subwin = m_mdiArea->addSubWindow(m_glWidget);
    subwin->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    m_glWidget->showMaximized();
    subwin->show();

    /* tab widget with bookmarks, canvases, photos, strokes, annotations */
    QDockWidget* dockwid = new QDockWidget(QString("Lists of entities"));
    this->addDockWidget(Qt::RightDockWidgetArea, dockwid, Qt::Vertical);
    dockwid->setFeatures(QDockWidget::DockWidgetClosable);
    dockwid->setWindowTitle(QString("Lists of entities"));
    //dockwid->hide();

    dockwid->setWidget(m_tabWidget);
    m_tabWidget->setTabPosition(QTabWidget::West);
    m_tabWidget->addTab(m_canvasWidget, Data::controlCanvasesIcon(), QString(""));
    m_tabWidget->addTab(m_bookmarkWidget, Data::controlBookmarksIcon(), QString(""));
    m_tabWidget->addTab(m_photoWidget, Data::controlImagesIcon(), QString(""));

    m_bookmarkWidget->setItemDelegate(new BookmarkDelegate(this));
    m_canvasWidget->setItemDelegate(new CanvasDelegate(this));

    /* viewer stack */
    m_viewStack->setUndoLimit(50);

    /* actions, menu, toolbars initialization */
    this->setCentralWidget(m_mdiArea);
    this->initializeActions();
    this->initializeMenus();
    this->initializeToolbars();
    this->initializeCallbacks();

    /* setup initial mode */
    this->onSketch();

    /* ui elements setup */
//    m_colorDialog->setWindowFlags(Qt::FramelessWindowHint);
    m_colorDialog->setOption(QColorDialog::ShowAlphaChannel);
    m_colorDialog->setCurrentColor(Utilities::getQColor(cher::POLYGON_CLR_NORMALFILL));
//    m_colorDialog->move(this->width(), this->height());

    // test adding second window
//    GLWidget* widget = new GLWidget(m_rootScene, m_viewStack, this);
//    QMdiSubWindow* subwin2 = m_mdiArea->addSubWindow(widget);
//    widget->showMaximized();
//    subwin2->show();
//    m_mdiArea->tileSubWindows();
}

MainWindow::~MainWindow()
{
    m_instance = nullptr;
}

MainWindow &MainWindow::instance()
{
    Q_ASSERT_X(m_instance, "MainWindow::instance", "Singleton not constructed yet");
    return *m_instance;
}

const RootScene *MainWindow::getRootScene() const
{
    return m_rootScene.get();
}

entity::Canvas *MainWindow::getCanvasCurrent() const
{
    if (!m_rootScene->getUserScene())
        return 0;
    return m_rootScene->getUserScene()->getCanvasCurrent();
}

osg::Camera *MainWindow::getCamera() const
{
    if (!m_glWidget) return 0;
    return m_glWidget->getCamera();
}

bool MainWindow::getStrokeFogFactor() const
{
    if (!m_actionStrokeFogFactor) return false;
    return m_actionStrokeFogFactor->isChecked();
}

QPixmap MainWindow::getScreenshot(const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up)
{
    return m_glWidget->getScreenShot(eye, center, up);
}

osg::Vec4f MainWindow::getCurrentColor() const
{
    QColor qc = m_colorDialog->currentColor();
    return Utilities::getOsgColor(qc);
}

void MainWindow::onSetTabletActivity(bool active){
    m_glWidget->setTabletActivity(active);
}

void MainWindow::onRequestUpdate()
{
    m_glWidget->update();
}

void MainWindow::onAutoSwitchMode(cher::MOUSE_MODE mode)
{
    switch (mode){
    case cher::SELECT_ENTITY:
        emit this->onSketch();
        break;
    case cher::PEN_SKETCH:
        emit this->onCameraOrbit();
        break;
    case cher::CAMERA_ORBIT:
        emit this->onSelect();
        break;
    default:
        emit this->onSelect();
        break;
    }
}

void MainWindow::onRequestBookmarkSet(int row)
{
    qDebug("bookmark recieved at MainWindow");
    entity::Bookmarks* bms = m_rootScene->getBookmarksModel();
    osg::Vec3d eye, center, up;
    double fov;
    eye = bms->getEyes()[row];
    center = bms->getCenters()[row];
    up = bms->getUps()[row];
    fov = bms->getFovs()[row];

    const entity::SceneState* state = bms->getSceneState(row);
    if (!state){
        qWarning("onRequestBookmarkSet: state is NULL");
        return;
    }
    this->setSceneState(state);
    m_glWidget->setCameraView(eye, center, up, fov);
}

void MainWindow::onDeleteBookmark(const QModelIndex &index)
{
    const std::string& name = m_rootScene->getBookmarksModel()->getBookmarkName(index.row());
    QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                              tr("Bookmark deletion"),
                                                              QString("Are you sure you want to delete bookmark ") + QString(name.c_str()) + QString("?"),
                                                              QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
        m_rootScene->deleteBookmark(m_bookmarkWidget, index);
}

void MainWindow::onDeleteCanvas(const QModelIndex &index)
{
    entity::Canvas* cnv = m_rootScene->getUserScene()->getCanvasFromIndex(index.row());
    if (!cnv){
        QMessageBox::critical(this, tr("Error"), tr("Could not obtain a canvas pointer"));
        return;
    }
    const std::string& name = cnv->getName();
    QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                              tr("Canvas deletion"),
                                                              QString("Are you sure you want to delete canvas " + QString(name.c_str()) +
                                                              " and all it contains?"),
                                                              QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes)
        m_rootScene->editCanvasDelete(cnv);
}

void MainWindow::onDeletePhoto(const QModelIndex &index)
{
    const QModelIndex parent = index.parent();
    if (!parent.isValid()){
        qWarning("onDeletePhoto: cannot find parent canvas");
        return;
    }
    entity::Canvas* cnv = m_rootScene->getUserScene()->getCanvasFromIndex(parent.row());
    if (!cnv) return;

    entity::Photo* photo = m_rootScene->getUserScene()->getPhoto(cnv, index.row());
    if (!photo) return;

    QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                              tr("Photo deletion"),
                                                              QString("Are you sure you want to delete photo %1?").arg(photo->getName().c_str()),
                                                              QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes)
        m_rootScene->editPhotoDelete(photo, cnv);
}

void MainWindow::onVisibilitySetCanvas(int index)
{
    entity::Canvas* cnv = m_rootScene->getUserScene()->getCanvasFromIndex(index);
    if (!cnv){
        QMessageBox::critical(this, tr("Error"), tr("Could not obtain a canvas pointer"));
        return;
    }
    m_rootScene->setCanvasVisibilityAll(cnv, !cnv->getVisibilityAll());
    this->onRequestUpdate();
}

void MainWindow::onMoveBookmark(const QModelIndex &index)
{
    qDebug("onMoveBookmark: resetting current index");
    m_bookmarkWidget->setCurrentIndex(index);
}

void MainWindow::onBookmarkAddedToWidget(const QModelIndex &, int first, int last)
{
    qDebug("onBookmarkAddedToWidget");
    const std::vector<osg::Vec3d>& centers =    m_rootScene->getUserScene()->getBookmarks()->getCenters();
    const std::vector<osg::Vec3d>& eyes =       m_rootScene->getUserScene()->getBookmarks()->getEyes();
    const std::vector<osg::Vec3d>& ups =        m_rootScene->getUserScene()->getBookmarks()->getUps();
    const std::vector<double>& fovs =           m_rootScene->getUserScene()->getBookmarks()->getFovs();

    int sz = m_rootScene->getUserScene()->getBookmarks()->getNumBookmarks();
    if (first<0 || first>=sz) return;
    if (last<0 || last>=sz) return;
    for (int i=first; i<=last; ++i){
        m_glWidget->setCameraView(eyes[i],centers[i],ups[i],fovs[i]);
        m_rootScene->addBookmarkTool(eyes[i],centers[i],ups[i]);
    }
}

void MainWindow::onBookmarkRemovedFromWidget(const QModelIndex &, int first, int last)
{
    m_rootScene->deleteBookmarkTool(first, last);
}

void MainWindow::onMouseModeSet(cher::MOUSE_MODE mode)
{
    QCursor cur = Utilities::getCursorFromMode(mode);
    if (mode == cher::PEN_POLYGON)
        m_colorDialog->show();
    else
        m_colorDialog->hide();
    if (cur.shape() != Qt::ArrowCursor) m_mdiArea->setCursor(cur);
}

void MainWindow::onPhotoTransparencyPlus(const QModelIndex &index)
{
    const QModelIndex parent = index.parent();
    if (!parent.isValid()){
        qWarning("onPhotoTransparency+: cannot find parent canvas");
        return;
    }
    entity::Canvas* cnv = m_rootScene->getUserScene()->getCanvasFromIndex(parent.row());
    if (!cnv) return;

    entity::Photo* photo = m_rootScene->getUserScene()->getPhoto(cnv, index.row());
    if (!photo) return;

    m_rootScene->getUserScene()->editPhotoTransparency(photo, cnv, photo->getTransparency()+cher::PHOTO_TRANSPARECY_DELTA);
    this->onRequestUpdate();
}

void MainWindow::onPhotoTransparencyMinus(const QModelIndex &index)
{
    const QModelIndex parent = index.parent();
    if (!parent.isValid()){
        qWarning("onPhotoTransparency-: cannot find parent canvas");
        return;
    }
    entity::Canvas* cnv = m_rootScene->getUserScene()->getCanvasFromIndex(parent.row());
    if (!cnv) return;

    entity::Photo* photo = m_rootScene->getUserScene()->getPhoto(cnv, index.row());
    if (!photo) return;

    m_rootScene->getUserScene()->editPhotoTransparency(photo, cnv, photo->getTransparency()-cher::PHOTO_TRANSPARECY_DELTA);
    this->onRequestUpdate();
}

void MainWindow::onPhotoPushed(int parent, int start, int, int destination, int)
{
    entity::Canvas* canvas_old = m_rootScene->getUserScene()->getCanvasFromIndex(parent);
    if (!canvas_old) return;

    entity::Canvas* canvas_new = m_rootScene->getUserScene()->getCanvasFromIndex(destination);
    if (!canvas_new) return;

    entity::Photo* photo = m_rootScene->getUserScene()->getPhoto(canvas_old, start);
    if (!photo) return;

    m_rootScene->editPhotoPush(photo, canvas_old, canvas_new);
}

void MainWindow::onRequestSceneData(entity::SceneState *state)
{
    if (!state){
        qWarning("onRequestSceneData: state is NULL");
        return;
    }
    else
        state->stripDataFrom(m_rootScene.get());
    if (state->isEmpty())
        qDebug("MainWindow: failed to strip data to scene state");
}

void MainWindow::onRequestSceneStateSet(entity::SceneState *state)
{
    this->setSceneState(state);
}

void MainWindow::onRequestSceneToolStatus(bool &visibility)
{
    visibility = m_actionTools->isChecked();
}

void MainWindow::onImportPhoto(const QString &path, const QString &fileName)
{
    QString fullPath = path + "/" + fileName;
    this->importPhoto(fullPath);
}

void MainWindow::onCameraPoseEdited(int index, const osg::Vec3f &eye, const osg::Vec3f &center, const osg::Vec3f &up)
{
    // edit camera pose by editing: bookmark tool position; SceneState data. Update.

}

/* Check whether the current scene is empty or not
 * If not - propose to save changes.
 * Clear the scene graph
*/
void MainWindow::onFileNew()
{
    qDebug("onFileNew called");
    this->onFileClose();

    this->onRequestUpdate();
    this->statusBar()->showMessage(tr("Scene is cleared."));
}

/* Check whether the current scene is empty or not
 * If not - propose to save changes.
 * Load the content of file into scene graph
*/
void MainWindow::onFileOpen()
{
    this->onFileClose();

    QString fname = QFileDialog::getOpenFileName(this, tr("Open a scene from file"),
                                                 QString(), tr("OSG files (*.osg *.osgt)"));
    if (!fname.isEmpty()){
        m_rootScene->setFilePath(fname.toStdString());
        if (!this->loadSceneFromFile()){
            QMessageBox::critical(this, tr("Error"), tr("Could not read from file. See the log for more details."));
            m_rootScene->setFilePath("");
        }
        else
            this->statusBar()->setStatusTip(tr("Scene was successfully read from file"));
    }
}

/* Take content of scene graph
 * If scene graph does not have an associated file name:
 * Open menu and let user to enter the name of the file
 * under which to save the data
 * Write content of scene graph into associated file
 */
void MainWindow::onFileSave()
{
    if (!m_rootScene->isSetFilePath()){
        QString fname = QFileDialog::getSaveFileName(this, tr("Saving scene to file"),
                                                     QString(), tr("OSG file (*.osgt)"));
        if (fname.isEmpty()){
            QMessageBox::warning(this, tr("Chosing filename"), tr("No file name is chosen. Changes were not saved."));
            this->statusBar()->showMessage(tr("Scene was not saved to file"));
            return;
        }
        m_rootScene->setFilePath(fname.toStdString());
    }
    if (!m_rootScene->writeScenetoFile()){
        QMessageBox::critical(this, tr("Error"), tr("Could not write scene to file"));
        m_rootScene->setFilePath("");
        this->statusBar()->showMessage(tr("Scene was not saved to file"));
        return;
    }
    this->statusBar()->showMessage(tr("Scene was successfully saved to file"));
}

/* Take content of scene graph
 * Open menu and let user to enter the name of the file
 * under which to save the data
 * Write content of scene graph into associated file
*/
void MainWindow::onFileSaveAs()
{
    this->m_rootScene->setFilePath("");
    this->onFileSave();
}

void MainWindow::onFileExport()
{
    QString fname = QFileDialog::getSaveFileName(this, tr("Exporting file"), QString(), tr("File formats (*.osgt *.obj *.3ds)"));
    if (fname.isEmpty()){
        QMessageBox::warning(this, tr("Chosing filename"), tr("No file name is chosen. File was not exported."));
        this->statusBar()->showMessage(tr("Scene was not exported."));
        return;
    }
    if (!m_rootScene->exportSceneToFile(fname.toStdString())){
        QMessageBox::critical(this, tr("Error"), tr("Could not export scene to file"));
        this->statusBar()->showMessage(tr("Scene was not exported to file"));
        return;
    }
    this->statusBar()->showMessage(tr("Scene was successfully exported."));
}

void MainWindow::onFileImage()
{
    if (m_rootScene->isEmptyScene()){
        QMessageBox::information(this, tr("Scene is empty"), tr("Create a canvas to load an image to"));
        return;
    }
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load an Image File"), QString(),
            tr("Image Files (*.bmp)"));

    this->importPhoto(fileName);
}

void MainWindow::onFilePhotoBase()
{
    QString directory = QFileDialog::getExistingDirectory(this, tr("Chose photo base directory"), QString(),
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (directory.isEmpty()){
        QMessageBox::information(this, tr("Photo base directory"), tr("No directory was chosen"));
        return;
    }

    m_photoModel->setRootPath(directory);
    m_photoWidget->setModel(m_photoModel);
//    m_photoWidget->setRootIndex(m_photoModel->index(directory));

    m_tabWidget->setCurrentWidget(m_photoWidget);

    this->statusBar()->showMessage(tr("Start dragging photos one-by-one to the current canvas to perfrom a photo import to the scene."));
}

void MainWindow::onFileClose()
{
    qDebug("onFileClose() called");
    if (!m_rootScene->isSavedToFile() && !m_rootScene->isEmptyScene()){
        QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                                  tr("Closing the current project"),
                                                                  tr("Do you want to save changes?"),
                                                                  QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes)
            this->onFileSave();
        if (!m_rootScene->isSavedToFile() && reply==QMessageBox::Yes)
            return;
    }
    m_rootScene->clearUserData();

    m_undoStack->clear();
    m_viewStack->clear();
    m_bookmarkWidget->model()->removeRows(0,m_bookmarkWidget->count());
//    m_canvasWidget->model()->removeRows(0, m_canvasWidget->count());
    m_canvasWidget->model()->removeRows(0, m_canvasWidget->topLevelItemCount());

    this->statusBar()->showMessage(tr("Current project is closed"));
}

void MainWindow::onFileExit()
{
    qDebug("onFileExit() called");
    this->onFileClose();
    this->close();
}

void MainWindow::onCut()
{
    m_rootScene->cutToBuffer();
    this->statusBar()->showMessage(tr("Cutted selected strokes to buffer."));
}

void MainWindow::onCopy()
{
    m_rootScene->copyToBuffer();
    this->statusBar()->showMessage(tr("Copied selected strokes to the buffer."));
}

void MainWindow::onPaste()
{
    m_rootScene->pasteFromBuffer();
    this->statusBar()->showMessage(tr("Pasted buffer content into current canvas."));
}

void MainWindow::onTools()
{
    qDebug() << "Tools: change status called to " << m_actionTools->isChecked();
    m_rootScene->setToolsVisibility(m_actionTools->isChecked()? true : false);
    this->onRequestUpdate();
}

void MainWindow::onCameraOrbit(){
    m_glWidget->setMouseMode(cher::CAMERA_ORBIT);
}

void MainWindow::onCameraZoom(){
    m_glWidget->setMouseMode(cher::CAMERA_ZOOM);
}

void MainWindow::onCameraPan(){
    m_glWidget->setMouseMode(cher::CAMERA_PAN);
}

void MainWindow::onCameraAperture()
{
    m_cameraProperties->show();
//    CameraProperties properties(this);
    //    properties.exec();
}

void MainWindow::onHomeView()
{
    m_glWidget->onHome();
}

void MainWindow::onViewAllCanvas()
{
    entity::Canvas* canvas = m_rootScene->getCanvasCurrent();
    if (!canvas) return;
    osg::Vec3f new_eye, new_center, new_up;
    new_up = canvas->getGlobalAxisV();
    float delta = 1;
    osg::BoundingBox bb = canvas->getBoundingBox();
    Q_ASSERT(bb.valid());
    Utilities::getGlobalFromLocal(bb.center(), canvas->getTransform()->getMatrix(), new_center);
//    new_center = bb.center();
    /* delta = BB_max * tan(FOV/2) */
    Q_ASSERT(m_cameraProperties->getFOV() > 0);
    auto bb_dx = std::fabs(bb.xMax()-bb.xMin());
    auto bb_dy = std::fabs(bb.yMax()-bb.yMin());
    auto half_max = std::max(bb_dx, bb_dy) * 0.5f;
    delta = std::fabs(half_max * std::tan(m_cameraProperties->getFOV() * 0.5));
    new_eye = new_center + canvas->getNormal() * delta;

    m_glWidget->setCameraView(new_eye, new_center, new_up, m_cameraProperties->getFOV());
}

void MainWindow::onSelect(){
    m_glWidget->setMouseMode(cher::SELECT_ENTITY);
    this->onRequestUpdate();
}

void MainWindow::onSelect3d()
{
    m_glWidget->setMouseMode(cher::SELECT_CANVAS);
    this->onRequestUpdate();
}

void MainWindow::onErase()
{
    m_glWidget->setMouseMode(cher::PEN_ERASE);
    this->onRequestUpdate();
}

void MainWindow::onDelete()
{
    m_glWidget->setMouseMode(cher::PEN_DELETE);
    this->onRequestUpdate();
}

void MainWindow::onSketch()
{
    m_glWidget->setMouseMode(cher::PEN_SKETCH);
    this->onRequestUpdate();
}

void MainWindow::onPolygon()
{
    m_glWidget->setMouseMode(cher::PEN_POLYGON);
    this->onRequestUpdate();
}

void MainWindow::onNewCanvasClone()
{
    m_glWidget->setMouseMode(cher::CREATE_CANVASCLONE);
    this->onRequestUpdate();
}

void MainWindow::onNewCanvasXY()
{
    m_rootScene->addCanvas(osg::Matrix::identity(), osg::Matrix::translate(0,0,0));
    this->onSketch();
    this->statusBar()->showMessage(tr("New canvas was created."));
    this->onRequestUpdate();
}

void MainWindow::onNewCanvasYZ()
{
    m_rootScene->addCanvas(osg::Matrix::rotate(cher::PI*0.5, 0, 1, 0), osg::Matrix::translate(0,0,0));
    this->onSketch();
    this->statusBar()->showMessage(tr("New canvas was created."));
    this->onRequestUpdate();
}

void MainWindow::onNewCanvasXZ()
{
    m_rootScene->addCanvas(osg::Matrix::rotate(cher::PI*0.5, 1, 0, 0), osg::Matrix::translate(0,0,0));
    this->onSketch();
    this->statusBar()->showMessage(tr("New canvas was created."));
    this->onRequestUpdate();
}

void MainWindow::onNewCanvasOrtho()
{
    entity::Canvas* canvas = m_rootScene->getCanvasCurrent();
    if (!canvas) return;
    osg::Vec3f center_glo = canvas->getEntitiesSelectedCenter3D();
    osg::Vec3f normal = canvas->getGlobalAxisV();
    m_rootScene->addCanvas(normal, center_glo);

    this->onSketch();
    this->statusBar()->showMessage(tr("New canvas perpendicular to previous was created"));
    this->onRequestUpdate();
}

void MainWindow::onNewCanvasSeparate()
{
    m_glWidget->setMouseMode(cher::CREATE_CANVASSEPARATE);
    this->onRequestUpdate();
}

void MainWindow::onNewCanvasStandard()
{
    m_rootScene->addCanvas(osg::Matrix::identity(),
                           osg::Matrix::translate(0.f, cher::CANVAS_MINW*0.5f, 0.f));
    m_rootScene->addCanvas(osg::Matrix::rotate(-cher::PI*0.5, 0, 1, 0),
                           osg::Matrix::translate(0.f, cher::CANVAS_MINW*0.5f, 0.f));
    m_rootScene->addCanvas(osg::Matrix::rotate(cher::PI*0.5, 1, 0, 0),
                           osg::Matrix::translate(0.f, 0.f, 0.f));
    this->onSketch();
    this->statusBar()->showMessage(tr("Set of canvases created."));
    this->onRequestUpdate();
}

void MainWindow::onNewCanvasCoaxial()
{
    this->statusBar()->showMessage(tr("This functionality does not exist yet."));
}

void MainWindow::onNewCanvasParallel()
{
    this->statusBar()->showMessage(tr("This functionality does not exist yet."));
}

void MainWindow::onNewCanvasRing()
{
    this->statusBar()->showMessage(tr("This functionality does not exist yet."));
}

void MainWindow::onCanvasEdit()
{
    m_glWidget->setMouseMode(cher::MOUSE_CANVAS);
    this->onRequestUpdate();
}

void MainWindow::onImageMove()
{
    m_glWidget->setMouseMode(cher::ENTITY_MOVE);
}

void MainWindow::onImageRotate()
{
    m_glWidget->setMouseMode(cher::ENTITY_ROTATE);
}

void MainWindow::onImageScale()
{
    m_glWidget->setMouseMode(cher::ENTITY_SCALE);
}

void MainWindow::onImageFlipH()
{
    m_glWidget->setMouseMode(cher::ENTITY_FLIPH);
}

void MainWindow::onImageFlipV()
{
    m_glWidget->setMouseMode(cher::ENTITY_FLIPV);
}

void MainWindow::onStrokesPush()
{
    osg::Camera* camera = m_glWidget->getCamera();
    if (!camera)
    {
        qWarning( "could not obtain camera");
        return;
    }
    m_rootScene->editStrokesPush(camera);
    this->statusBar()->showMessage(tr("Push is performed on selected strokes from current canvas (pink) to previous canvas (violet)."));
    this->onSelect();
}

void MainWindow::onBookmark()
{
    osg::Vec3d eye,center,up;
    double fov;
    m_glWidget->getCameraView(eye, center, up, fov);
    m_rootScene->addBookmark(m_bookmarkWidget, eye, center, up, fov);
    this->statusBar()->showMessage(tr("Current camera view is saved as a bookmark."));
}

void MainWindow::onBookmarkNew()
{
    /* create initial camera position, add it to scene graph and tools */
    osg::Vec3d eye, center, up;
    double fov;
    m_glWidget->getCameraView(eye, center, up, fov);
    m_rootScene->addBookmark(m_bookmarkWidget, eye, center, up, fov);

    /* create corresponding SVM data connected to the bookmark */
    m_rootScene->addSVMData();

    /* set mouse in SVM mode.
     * The camera position will be updated from EventHandler. */
    m_glWidget->setMouseMode(cher::SVM_IDLE);

    this->statusBar()->showMessage(tr("New camera view added through SVM method."));
}

void MainWindow::onBookmarkEdit()
{

}

void MainWindow::onStrokeFogFactor()
{
    bool factor = m_actionStrokeFogFactor->isChecked()? true : false;
    for (int i=0; i<m_rootScene->getUserScene()->getNumCanvases(); ++i){
        entity::Canvas* cnv = m_rootScene->getUserScene()->getCanvas(i);
        if (!cnv) {
            qWarning("Canvas is NULL");
            continue;
        }

        for (unsigned int j=0; j<cnv->getNumStrokes(); ++j){
            entity::Stroke* s = cnv->getStroke(j);
            if (!s){
                qWarning("Stroke is NULL");
                continue;
            }
            if (s->getProgram())
                s->getProgram()->updateIsFogged(factor);
        }
        for (unsigned int j=0; j<cnv->getNumPolygons(); ++j){
            entity::Polygon* p = cnv->getPolygon(j);
            if (!p){
                qWarning("Polygon is NULL");
                continue;
            }
            if (p->getProgram())
                p->getProgram()->updateIsFogged(factor);
        }
    }
}

void MainWindow::initializeActions()
{
    // FILE
    m_actionNewFile = new QAction(Data::fileNewSceneIcon(), tr("&New..."), this);
    this->connect(m_actionNewFile, SIGNAL(triggered(bool)), this, SLOT(onFileNew()));
    m_actionNewFile->setShortcut(tr("Ctrl+N"));

    m_actionClose = new QAction(Data::fileCloseIcon(), tr("&Close"), this);
    this->connect(m_actionClose, SIGNAL(triggered(bool)), this, SLOT(onFileClose()));
    m_actionClose->setShortcut(tr("Ctrl+W"));

    m_actionExit = new QAction(Data::fileExitIcon(), tr("&Exit"), this);
    this->connect(m_actionExit, SIGNAL(triggered(bool)), this, SLOT(onFileExit()));
    m_actionExit->setShortcut(tr("Ctrl+Q"));

    m_actionImportImage = new QAction(Data::fileImageIcon(), tr("Import &Image..."), this);
    this->connect(m_actionImportImage, SIGNAL(triggered(bool)), this, SLOT(onFileImage()));
    m_actionImportImage->setShortcut(tr("Ctrl+I"));

    m_actionOpenFile = new QAction(Data::fileOpenIcon(), tr("&Open..."), this);
    this->connect(m_actionOpenFile, SIGNAL(triggered(bool)), this, SLOT(onFileOpen()));
    m_actionOpenFile->setShortcut(tr("Ctrl+O"));

    m_actionSaveFile = new QAction(Data::fileSaveIcon(), tr("&Save..."), this);
    this->connect(m_actionSaveFile, SIGNAL(triggered(bool)), this, SLOT(onFileSave()));
    m_actionSaveFile->setShortcut(tr("Ctrl+S"));

    m_actionSaveAsFile = new QAction(tr("Save as..."), this);
    this->connect(m_actionSaveAsFile, SIGNAL(triggered(bool)), this, SLOT(onFileSaveAs()));

    m_actionExportAs = new QAction(Data::fileExportIcon(), tr("Export as..."), this);
    this->connect(m_actionExportAs, SIGNAL(triggered(bool)), this, SLOT(onFileExport()));

    m_actionPhotoBase = new QAction(Data::controlImagesIcon(), tr("Chose folder with photo base..."), this);
    this->connect(m_actionPhotoBase, SIGNAL(triggered(bool)), this, SLOT(onFilePhotoBase()));

    // EDIT

    m_actionUndo = m_undoStack->createUndoAction(this, tr("&Undo"));
    m_actionUndo->setIcon(Data::editUndoIcon());
    m_actionUndo->setShortcut(QKeySequence::Undo);

    m_actionRedo = m_undoStack->createRedoAction(this, tr("&Redo"));
    m_actionRedo->setIcon(Data::editRedoIcon());
    m_actionRedo->setShortcut(tr("Ctrl+R"));

    m_actionCut = new QAction(Data::editCutIcon(), tr("&Cut"), this);
    this->connect(m_actionCut, SIGNAL(triggered(bool)), this, SLOT(onCut()));
    m_actionCut->setShortcut(tr("Ctrl+X"));

    m_actionCopy = new QAction(Data::editCopyIcon(), tr("C&opy"), this);
    this->connect(m_actionCopy, SIGNAL(triggered(bool)), this, SLOT(onCopy()));
    m_actionCopy->setShortcut(tr("Ctrl+C"));

    m_actionPaste = new QAction(Data::editPasteIcon(), tr("&Paste"), this);
    this->connect(m_actionPaste, SIGNAL(triggered(bool)), this, SLOT(onPaste()));
    m_actionPaste->setShortcut(tr("Ctrl+V"));

//    m_actionDelete = new QAction(Data::editDeleteIcon(), tr("&Delete"), this);
//    this->connect(m_actionDelete, SIGNAL(triggered(bool)), this, SLOT(onDelete()));
//    m_actionDelete->setShortcut(Qt::Key_Delete);

    m_actionTools = new QAction(Data::editSettingsIcon(), tr("&Tools"), this);
    m_actionTools->setCheckable(true);
    m_actionTools->setChecked(true);
    this->connect(m_actionTools, SIGNAL(toggled(bool)), this, SLOT(onTools()));

    /* CAMERA */

    m_actionOrbit = new QAction(Data::sceneOrbitIcon(), tr("&Orbit"), this);
    this->connect(m_actionOrbit, SIGNAL(triggered(bool)), this, SLOT(onCameraOrbit()));

    m_actionPan = new QAction(Data::scenePanIcon(), tr("&Pan"), this);
    this->connect(m_actionPan, SIGNAL(triggered(bool)), this, SLOT(onCameraPan()));

    m_actionZoom = new QAction(Data::sceneZoomIcon(), tr("&Zoom"), this);
    this->connect(m_actionZoom, SIGNAL(triggered(bool)), this, SLOT(onCameraZoom()));

    m_actionPrevView = m_viewStack->createUndoAction(this, tr("Previous view"));
    m_actionPrevView->setIcon(Data::viewerPreviousIcon());

    m_actionNextView = m_viewStack->createRedoAction(this, tr("Next view"));
    m_actionNextView->setIcon(Data::viewerNextIcon());

    m_actionBookmark = new QAction(Data::viewerBookmarkIcon(), tr("Bookmark view"), this);
    this->connect(m_actionBookmark, SIGNAL(triggered(bool)), this, SLOT(onBookmark()));

    m_actionBookmarkNew = new QAction(Data::viewerBookmarkNewIcon(), tr("Create new bookmark..."), this);
    this->connect(m_actionBookmarkNew, SIGNAL(triggered(bool)), this, SLOT(onBookmarkNew()));

    m_actionBookmarkEdit = new QAction(Data::viewerBookmarkEditIcon(), tr("Edit existing bookmark"), this);
    this->connect(m_actionBookmarkEdit, SIGNAL(triggered(bool)), this, SLOT(onBookmarkEdit()));

    m_actionCameraSettings = new QAction(Data::cameraApertureIcon(), tr("Camera settings"), this);
    this->connect(m_actionCameraSettings, SIGNAL(triggered(bool)), this, SLOT(onCameraAperture()));

    m_actionHomeView = new QAction(Data::viewerHomeIcon(), tr("Home view"), this);
    this->connect(m_actionHomeView, SIGNAL(triggered(bool)), this, SLOT(onHomeView()));

    m_actionViewAllCanvas = new QAction(Data::viewerAllCanvas(), tr("Look at canvas"), this);
    this->connect(m_actionViewAllCanvas, SIGNAL(triggered(bool)), this, SLOT(onViewAllCanvas()));

    // SCENE

    m_actionSketch = new QAction(Data::sceneSketchIcon(), tr("&Sketch"), this);
    this->connect(m_actionSketch, SIGNAL(triggered(bool)), this, SLOT(onSketch()));

    m_actionEraser = new QAction(Data::sceneEraserIcon(), tr("&Deleter"), this);
    this->connect(m_actionEraser, SIGNAL(triggered(bool)), this, SLOT(onDelete()));
    m_actionEraser->setShortcut(Qt::Key_Delete);

    m_actionSelect = new QAction(Data::sceneSelectIcon(), tr("S&elect 2D entity"), this);
    this->connect(m_actionSelect, SIGNAL(triggered(bool)), this, SLOT(onSelect()));

    m_actionSelect3d = new QAction(Data::sceneSelect3DIcon(), tr("Select &canvas"), this);
    this->connect(m_actionSelect3d, SIGNAL(triggered(bool)), this, SLOT(onSelect3d()));

    m_actionPolygon = new QAction(Data::scenePolygonIcon(), tr("Draw polygon"), this);
    this->connect(m_actionPolygon, SIGNAL(triggered(bool)), this, SLOT(onPolygon()));

    m_actionCanvasClone = new QAction(Data::sceneNewCanvasCloneIcon(), tr("Clone Current"), this);
    this->connect(m_actionCanvasClone, SIGNAL(triggered(bool)), this, SLOT(onNewCanvasClone()));

    m_actionCanvasOrtho = new QAction(Data::sceneNewCanvasOrthoIcon(), tr("Perpendicular to current"), this);
    this->connect(m_actionCanvasOrtho, SIGNAL(triggered(bool)), this, SLOT(onNewCanvasOrtho()));

    m_actionCanvasSeparate = new QAction(Data::sceneNewCanvasSeparateIcon(), tr("Separate selected strokes"), this);
    this->connect(m_actionCanvasSeparate, SIGNAL(triggered(bool)), this, SLOT(onNewCanvasSeparate()));

    m_actionCanvasXY = new QAction(Data::sceneNewCanvasXYIcon(), tr("Plane XY"), this);
    this->connect(m_actionCanvasXY, SIGNAL(triggered(bool)), this, SLOT(onNewCanvasXY()));

    m_actionCanvasYZ = new QAction(Data::sceneNewCanvasYZIcon(), tr("Plane YZ"), this);
    this->connect(m_actionCanvasYZ, SIGNAL(triggered(bool)), this, SLOT(onNewCanvasYZ()));

    m_actionCanvasXZ = new QAction(Data::sceneNewCanvasXZIcon(), tr("Plane XZ"), this);
    this->connect(m_actionCanvasXZ, SIGNAL(triggered(bool)), this, SLOT(onNewCanvasXZ()));

    m_actionSetStandard = new QAction(Data::sceneNewCanvasSetStandardIcon(), tr("Standard"), this);
    this->connect(m_actionSetStandard, SIGNAL(triggered(bool)), this, SLOT(onNewCanvasStandard()));

    m_actionCanvasEdit = new QAction(Data::sceneCanvasEditIcon(), tr("Edit canvas location"), this);
    this->connect(m_actionCanvasEdit, SIGNAL(triggered(bool)), this, SLOT(onCanvasEdit()));

    m_actionStrokesPush = new QAction(Data::scenePushStrokesIcon(), tr("Push Strokes"), this);
    this->connect(m_actionStrokesPush, SIGNAL(triggered(bool)), this, SLOT(onStrokesPush()));

    // OPTIONS
    m_actionStrokeFogFactor = new QAction(Data::viewerIsoIcon(), tr("Fog factor"), this);
    m_actionStrokeFogFactor->setCheckable(true);
    m_actionStrokeFogFactor->setChecked(false);
    this->connect(m_actionStrokeFogFactor, SIGNAL(toggled(bool)), this, SLOT(onStrokeFogFactor()));

}

void MainWindow::initializeMenus()
{
    // FILE
    QMenu* menuFile = m_menuBar->addMenu(tr("&File"));
    menuFile->addAction(m_actionNewFile);
    menuFile->addAction(m_actionOpenFile);
    menuFile->addSeparator();
    menuFile->addAction(m_actionSaveFile);
    menuFile->addAction(m_actionSaveAsFile);
    menuFile->addAction(m_actionExportAs);
    menuFile->addSeparator();
    menuFile->addAction(m_actionImportImage);
    menuFile->addAction(m_actionPhotoBase);
    menuFile->addSeparator();
    menuFile->addAction(m_actionClose);
    menuFile->addAction(m_actionExit);

    // EDIT
    QMenu* menuEdit = m_menuBar->addMenu(tr("&Edit"));
    menuEdit->addAction(m_actionUndo);
    menuEdit->addAction(m_actionRedo);
    menuEdit->addSeparator();
    menuEdit->addAction(m_actionCut);
    menuEdit->addAction(m_actionCopy);
    menuEdit->addAction(m_actionPaste);
//    menuEdit->addAction(m_actionDelete);

    /* CAMERA */
    QMenu* menuCamera = m_menuBar->addMenu(tr("Camera"));
    menuCamera->addAction(m_actionOrbit);
    menuCamera->addAction(m_actionPan);
    menuCamera->addAction(m_actionZoom);
    menuCamera->addSeparator();
    menuCamera->addAction(m_actionHomeView);
    menuCamera->addAction(m_actionViewAllCanvas);
    menuCamera->addAction(m_actionPrevView);
    menuCamera->addAction(m_actionNextView);
    menuCamera->addSeparator();
    menuCamera->addAction(m_actionBookmark);
    menuCamera->addAction(m_actionBookmarkNew);
    menuCamera->addAction(m_actionBookmarkEdit);
    menuCamera->addSeparator();
    menuCamera->addAction(m_actionCameraSettings);

    /* SCENE */
    QMenu* menuScene = m_menuBar->addMenu(tr("&Scene"));
    menuScene->addAction(m_actionSelect);
    menuScene->addAction(m_actionSelect3d);
    menuScene->addAction(m_actionSketch);
    menuScene->addAction(m_actionPolygon);
    menuScene->addAction(m_actionEraser);
    menuScene->addAction(m_actionCanvasEdit);
    menuScene->addSeparator();
    QMenu* submenuCanvas = menuScene->addMenu("New Canvas");
    submenuCanvas->setIcon(Data::sceneNewCanvasIcon());
    submenuCanvas->addAction(m_actionCanvasClone);
    submenuCanvas->addAction(m_actionCanvasXY);
    submenuCanvas->addAction(m_actionCanvasYZ);
    submenuCanvas->addAction(m_actionCanvasXZ);
    submenuCanvas->addAction(m_actionCanvasOrtho);
    submenuCanvas->addAction(m_actionCanvasSeparate);
    QMenu* submenuSet = menuScene->addMenu("New Canvas Set");
    submenuSet->setIcon(Data::sceneNewCanvasSetIcon());
    submenuSet->addAction(m_actionSetStandard);

    menuScene->addSeparator();
//    QMenu* submenuEC = menuScene->addMenu("Edit Canvas");
//    submenuEC->addAction(m_actionCanvasOffset);
//    submenuEC->addAction(m_actionCanvasRotate);
//    QMenu* submenuEI = menuScene->addMenu("Edit Image");
//    submenuEI->addAction(m_actionImageMove);
//    submenuEI->addAction(m_actionImageRotate);
//    submenuEI->addAction(m_actionImageScale);
//    submenuEI->addAction(m_actionImageFlipH);
//    submenuEI->addAction(m_actionImageFlipV);
    QMenu* submenuES = menuScene->addMenu("Edit Strokes");
    submenuES->addAction(m_actionStrokesPush);

    /* OPTIONS */
    QMenu* menuOptions = m_menuBar->addMenu(tr("&Options"));
    menuOptions->addAction(m_actionTools);
    QMenu* submenuVisuals = menuOptions->addMenu("Visuals");
    submenuVisuals->setIcon(Data::optionsVisibilityIcon());
    submenuVisuals->addAction(m_actionStrokeFogFactor);

}

void MainWindow::initializeToolbars()
{
    // File
    QToolBar* tbFile = this->addToolBar(tr("File"));
    tbFile->addAction(m_actionNewFile);
    tbFile->addAction(m_actionOpenFile);
    tbFile->addAction(m_actionSaveFile);
    tbFile->addAction(m_actionImportImage);
    tbFile->addAction(m_actionPhotoBase);

    // Edit
    QToolBar* tbEdit = this->addToolBar(tr("Edit"));
    tbEdit->addAction(m_actionUndo);
    tbEdit->addAction(m_actionRedo);
    tbEdit->addSeparator();
    tbEdit->addAction(m_actionCut);
    tbEdit->addAction(m_actionCopy);
    tbEdit->addAction(m_actionPaste);

    // Camera navigation
    QToolBar* tbCamera = new QToolBar(tr("Camera"));
    //QToolBar* tbCamera = this->addToolBar(tr("Camera"));
    this->addToolBar(Qt::TopToolBarArea, tbCamera);
    tbCamera->addAction(m_actionOrbit);
    tbCamera->addAction(m_actionZoom);
    tbCamera->addAction(m_actionPan);

    // Input
    QToolBar* tbInput = new QToolBar(tr("Input"));
    this->addToolBar(Qt::LeftToolBarArea, tbInput);
    //QToolBar* tbInput = this->addToolBar(tr("Input"));
    tbInput->addAction(m_actionSelect);
    tbInput->addAction(m_actionSelect3d);
    tbInput->addAction(m_actionSketch);
    tbInput->addAction(m_actionPolygon);
    tbInput->addAction(m_actionEraser);
    tbInput->addAction(m_actionCanvasEdit);

    QMenu* menuNewCanvas = new QMenu(this);
    menuNewCanvas->addAction(m_actionCanvasXY);
    menuNewCanvas->addAction(m_actionCanvasYZ);
    menuNewCanvas->addAction(m_actionCanvasXZ);
    menuNewCanvas->addAction(m_actionCanvasClone);
    menuNewCanvas->addAction(m_actionCanvasOrtho);
    menuNewCanvas->addAction(m_actionCanvasSeparate);
    QToolButton* tbNewCanvas = new QToolButton();
    tbNewCanvas->setIcon(Data::sceneNewCanvasIcon());
    tbNewCanvas->setMenu(menuNewCanvas);
    tbNewCanvas->setPopupMode(QToolButton::InstantPopup);
    QWidgetAction* waNewCanvas = new QWidgetAction(this);
    waNewCanvas->setDefaultWidget(tbNewCanvas);

    QMenu* menuNewCanvasSet = new QMenu(this);
    menuNewCanvasSet->addAction(m_actionSetStandard);
    QToolButton* tbNewCanvasSet = new QToolButton();
    tbNewCanvasSet->setIcon(Data::sceneNewCanvasSetIcon());
    tbNewCanvasSet->setMenu(menuNewCanvasSet);
    tbNewCanvasSet->setPopupMode(QToolButton::InstantPopup);
    QWidgetAction* waNewCanvasSet = new QWidgetAction(this);
    waNewCanvasSet->setDefaultWidget(tbNewCanvasSet);

    tbInput->addAction(waNewCanvas);
    tbInput->addAction(waNewCanvasSet);

    // Edit entity
    QToolBar* tbEntity = new QToolBar(tr("Edit entity"));
    this->addToolBar(Qt::LeftToolBarArea, tbEntity);
    tbEntity->addAction(m_actionStrokesPush);

    /* VIEWER bar */
    QToolBar* tbViewer = new QToolBar(tr("Viewer"));
    this->addToolBar(Qt::BottomToolBarArea, tbViewer);
    tbViewer->addAction(m_actionHomeView);
    tbViewer->addAction(m_actionViewAllCanvas);
    tbViewer->addAction(m_actionPrevView);
    tbViewer->addAction(m_actionNextView);
    tbViewer->addAction(m_actionBookmark);
    tbViewer->addAction(m_actionBookmarkNew);
    tbViewer->addAction(m_actionCameraSettings);

    /* OPTIONS bar */
    QToolBar* tbOptions = new QToolBar(tr("Options"));
    this->addToolBar(Qt::TopToolBarArea, tbOptions);
    tbOptions->addAction(m_actionTools);

    QMenu* menuVisuals = new QMenu(this);
    menuVisuals->addAction(m_actionStrokeFogFactor);
    QToolButton* tbVisuals = new QToolButton();
    tbVisuals->setIcon(Data::optionsVisibilityIcon());
    tbVisuals->setMenu(menuVisuals);
    tbVisuals->setPopupMode(QToolButton::InstantPopup);
    QWidgetAction* waVisuals = new QWidgetAction(this);
    waVisuals->setDefaultWidget(tbVisuals);

    tbOptions->addAction(waVisuals);

}

/* signal slot connections must be established in two cases:
 * in MainWindow ctor;
 * on when scene is loaded from file
 */
void MainWindow::initializeCallbacks()
{
    /* connect MainWindow with GLWidget */
    QObject::connect(m_glWidget, SIGNAL(mouseModeSet(cher::MOUSE_MODE)),
                     this, SLOT(onMouseModeSet(cher::MOUSE_MODE)),
                     Qt::UniqueConnection);

    QObject::connect(m_glWidget, SIGNAL(autoSwitchMode(cher::MOUSE_MODE)),
                     this, SLOT(onAutoSwitchMode(cher::MOUSE_MODE)),
                     Qt::UniqueConnection);

    QObject::connect(m_glWidget, SIGNAL(importPhoto(QString,QString)),
                     this, SLOT(onImportPhoto(QString,QString)),
                     Qt::UniqueConnection);

    /* connect MainWindow with UserScene */
    QObject::connect(m_rootScene->getUserScene(), SIGNAL(sendRequestUpdate()),
                     this, SLOT(onRequestUpdate()),
                     Qt::UniqueConnection);

    QObject::connect(m_rootScene->getUserScene(), SIGNAL(requestSceneToolStatus(bool&)),
                     this, SLOT(onRequestSceneToolStatus(bool&)),
                     Qt::UniqueConnection);

//    QObject::connect(m_rootScene->getUserScene(), SIGNAL(cameraPoseEdited(int,osg::Vec3f,osg::Vec3f,osg::Vec3f)),
//                     this, SLOT(onCameraPoseEdited(int,osg::Vec3f,osg::Vec3f,osg::Vec3f)),
//                     Qt::UniqueConnection);

    /* bookmark widget data */
    QObject::connect(m_bookmarkWidget, SIGNAL(clicked(QModelIndex)),
                     m_rootScene->getBookmarksModel(), SLOT(onClicked(QModelIndex)),
                     Qt::UniqueConnection);

    QObject::connect(m_rootScene->getBookmarksModel(), SIGNAL(requestBookmarkSet(int)),
                     this, SLOT(onRequestBookmarkSet(int)),
                     Qt::UniqueConnection);

    QObject::connect(m_bookmarkWidget->model(), SIGNAL(rowsInserted(QModelIndex,int,int)),
                     this, SLOT(onBookmarkAddedToWidget(QModelIndex,int,int)),
                     Qt::UniqueConnection);

    /* for bookmark scene graph representation */
    QObject::connect(m_bookmarkWidget->model(), SIGNAL(rowsRemoved(QModelIndex,int,int)),
                     this, SLOT(onBookmarkRemovedFromWidget(QModelIndex,int,int)),
                     Qt::UniqueConnection);

    /* for bookmark data of std::vectors */
    QObject::connect(m_bookmarkWidget->model(), SIGNAL(rowsRemoved(QModelIndex,int,int)),
                     m_rootScene->getBookmarksModel(), SLOT(onRowsRemoved(QModelIndex,int,int)),
                     Qt::UniqueConnection);

    QObject::connect(m_bookmarkWidget->getBookmarkDelegate(), SIGNAL(clickedDelete(QModelIndex)),
                     this, SLOT(onDeleteBookmark(QModelIndex)),
                     Qt::UniqueConnection);

    QObject::connect(m_bookmarkWidget->getBookmarkDelegate(), SIGNAL(clickedMove(QModelIndex)),
                     this, SLOT(onMoveBookmark(QModelIndex)),
                     Qt::UniqueConnection);

    QObject::connect(m_bookmarkWidget, SIGNAL(itemChanged(QListWidgetItem*)),
                     m_rootScene->getBookmarksModel(), SLOT(onItemChanged(QListWidgetItem*)) ,
                     Qt::UniqueConnection);

    QObject::connect(m_bookmarkWidget->model(), SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),
                     m_rootScene->getBookmarksModel(), SLOT(onRowsMoved(QModelIndex,int,int,QModelIndex,int)),
                     Qt::UniqueConnection);

    QObject::connect(m_rootScene->getBookmarksModel(), SIGNAL(requestSceneData(entity::SceneState*)),
                     this, SLOT(onRequestSceneData(entity::SceneState*)),
                     Qt::UniqueConnection);

    QObject::connect(m_rootScene->getBookmarksModel(), SIGNAL(requestSceneStateSet(entity::SceneState*)),
                     this, SLOT(onRequestSceneStateSet(entity::SceneState*)),
                     Qt::UniqueConnection);

    /* canvas widget area */
    QObject::connect(m_rootScene->getUserScene(), SIGNAL(canvasAdded(std::string)),
                     m_canvasWidget, SLOT(onCanvasAdded(std::string)),
                     Qt::UniqueConnection);

    QObject::connect(m_rootScene->getUserScene(), SIGNAL(photoAdded(std::string,int)),
                     m_canvasWidget, SLOT(onPhotoAdded(std::string,int)),
                     Qt::UniqueConnection);

    QObject::connect(m_rootScene->getUserScene(), SIGNAL(canvasSelectedColor(int,int)),
                     m_canvasWidget, SLOT(onCanvasSelectedColor(int,int)),
                     Qt::UniqueConnection);

    /* canvasRemoved and deleteCanvas - are two different slots because
     * we use undo/redo framework to invoke the second signal */
    QObject::connect(m_rootScene->getUserScene(), SIGNAL(canvasRemoved(int)),
                     m_canvasWidget, SLOT(onCanvasRemoved(int)),
                     Qt::UniqueConnection);

    QObject::connect(m_rootScene->getUserScene(), SIGNAL(photoRemoved(int,int)),
                     m_canvasWidget, SLOT(onPhotoRemoved(int,int)),
                     Qt::UniqueConnection);

    QObject::connect(m_canvasWidget, SIGNAL(clicked(QModelIndex)),
                     m_rootScene->getUserScene(), SLOT(onClicked(QModelIndex)),
                     Qt::UniqueConnection);

    QObject::connect(m_canvasWidget, SIGNAL(rightClicked(QModelIndex)),
                     m_rootScene->getUserScene(), SLOT(onRightClicked(QModelIndex)),
                     Qt::UniqueConnection);

    QObject::connect(m_canvasWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
                     m_rootScene->getUserScene(), SLOT(onItemChanged(QTreeWidgetItem*,int)),
                     Qt::UniqueConnection);

    QObject::connect(m_canvasWidget->getCanvasDelegate(), SIGNAL(clickedDelete(QModelIndex)),
                     this, SLOT(onDeleteCanvas(QModelIndex)),
                     Qt::UniqueConnection);

    QObject::connect(m_canvasWidget->getCanvasDelegate(), SIGNAL(clickedDeletePhoto(QModelIndex)),
                     this, SLOT(onDeletePhoto(QModelIndex)),
                     Qt::UniqueConnection);

    QObject::connect(m_canvasWidget->getCanvasDelegate(), SIGNAL(clickedVisibilitySet(int)),
                     this, SLOT(onVisibilitySetCanvas(int)),
                     Qt::UniqueConnection);

    QObject::connect(m_rootScene->getUserScene(), SIGNAL(canvasVisibilitySet(int,bool)),
                     m_canvasWidget, SLOT(onCanvasVisibilitySet(int,bool)),
                     Qt::UniqueConnection);

    QObject::connect(m_canvasWidget->getCanvasDelegate(), SIGNAL(clickedTransparencyPlus(QModelIndex)),
                     this, SLOT(onPhotoTransparencyPlus(QModelIndex)),
                     Qt::UniqueConnection);

    QObject::connect(m_canvasWidget->getCanvasDelegate(), SIGNAL(clickedTransparencyMinus(QModelIndex)),
                     this, SLOT(onPhotoTransparencyMinus(QModelIndex)),
                     Qt::UniqueConnection);

//    QObject::connect(m_canvasWidget, SIGNAL(photoDraggedAndDropped(int,int,int,int,int)),
//                     this, SLOT(onPhotoPushed(int,int,int,int,int)),
//                     Qt::UniqueConnection);


    /* Camera properties UI form */
    // connect camera widget slider with GLWidget osg::Camera variable
    QObject::connect(m_cameraProperties, SIGNAL(fovChangedBySlider(double)),
                     m_glWidget, SLOT(onFOVChangedSlider(double)),
                     Qt::UniqueConnection);

    QObject::connect(m_cameraProperties, SIGNAL(orthoChecked(bool)),
                     m_glWidget, SLOT(onOrthoSet(bool)),
                     Qt::UniqueConnection);

    QObject::connect(m_glWidget, SIGNAL(FOVSet(double)),
                     m_cameraProperties, SLOT(onFOVSet(double)),
                     Qt::UniqueConnection);

}

bool MainWindow::loadSceneFromFile()
{
    if (!m_rootScene->isSetFilePath()) return false;
    if (!m_rootScene->loadSceneFromFile()) return false;
    m_glWidget->update();
    this->initializeCallbacks();

    m_rootScene->resetBookmarks(m_bookmarkWidget);
    if (!m_rootScene->getUserScene()) return false;
    m_rootScene->getUserScene()->resetModel(m_canvasWidget);

    return true;
}

bool MainWindow::importPhoto(QString &fileName)
{
    if (fileName.isEmpty()){
        QMessageBox::critical(this, tr("Import Photo"), tr("Path is empty. "
                                                           "No import will be performed."));
        return false;
    }

    QFileInfo checkFile(fileName);
    if (!checkFile.exists() || !checkFile.isFile()){
        QMessageBox::critical(this, tr("Import Photo"), tr("No file exists under the given path."
                                                           " No import will be performed."));
        return false;
    }

    if (!m_rootScene.get()){
        QMessageBox::critical(this, tr("Import Photo"), tr("The scene is NULL. It is advised to restart. "
                                                          "No import will be performed."));
        return false;
    }

    if (m_rootScene->isEmptyScene()){
        QMessageBox::warning(this, tr("Import Photo"), tr("There are no canvases on the scene. Add a canvas first. "
                                                          "No import will be performed."));
        return false;
    }

    m_rootScene->addPhoto(fileName.toStdString());
    this->statusBar()->showMessage(tr("Image loaded to current canvas."));
    return true;
}

void MainWindow::setSceneState(const entity::SceneState *state)
{
    if (!state) throw std::runtime_error("setSceneState(): state is NULL");
    m_rootScene->setSceneState(state);
    m_actionTools->setChecked(state->getAxisFlag());

    // we only want to keep original screenshot
//    m_rootScene->updateBookmark(m_bookmarkWidget, row);
}
