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

#include <osg/MatrixTransform>

#include "MainWindow.h"
#include "GLWidget.h"
#include "ListDelegate.h"
#include "Settings.h"
#include "Data.h"

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
    , m_desktop(0)
    , m_mdiArea(new QMdiArea(this))

    , m_tabWidget(new QTabWidget())
    , m_bookmarkWidget(new BookmarkWidget())
    , m_canvasWidget(new CanvasWidget())

    , m_undoStack(new QUndoStack(this))
    , m_undoView(new QUndoView(m_undoStack))

    , m_menuBar(new QMenuBar(0)) // http://stackoverflow.com/questions/8108729/qmenu-does-not-work-on-mac-qt-creator

    , m_rootScene(new RootScene(m_undoStack))
    , m_viewStack(new QUndoStack(this))
    , m_glWidget(new GLWidget(m_rootScene.get(), m_viewStack, this))
{
    this->setMenuBar(m_menuBar);

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
    m_bookmarkWidget->setItemDelegate(new BookmarkDelegate);
    m_canvasWidget->setItemDelegate(new CanvasDelegate);

    /* undo/redo widget */
    m_undoView->setWindowTitle(tr("Command List"));
    m_undoView->show();
    m_undoView->setAttribute(Qt::WA_QuitOnClose, false);

    /* viewer stack */
    m_viewStack->setUndoLimit(50);

    /* actions, menu, toolbars initialization */
    this->setCentralWidget(m_mdiArea);
    this->initializeActions();
    this->initializeMenus();
    this->initializeToolbars();
    this->initializeCallbacks();

    this->onSketch();
}

MainWindow::~MainWindow(){
    if (m_menuBar)
        delete m_menuBar;
}

void MainWindow::SetDesktopWidget(QDesktopWidget *desktop, dureu::APPMODE mode) {
    m_desktop = desktop;
    QRect availS = m_desktop->availableGeometry();
    QRect fullS = m_desktop->geometry();
    //int nscreen = m_desktop->screenCount();
    double scale = 0.9;
    double scale_inv = 1-scale;
    switch (mode) {
    case dureu::SCREEN_MIN:
        this->showNormal();
        this->move(availS.width()*scale_inv, fullS.height()-availS.height());
        break;
    case dureu::SCREEN_MAX:
        this->showMaximized();
        break;
    case dureu::SCREEN_FULL:
        this->showFullScreen();
        break;
    case dureu::SCREEN_VIRTUAL: // needs testing and fixing
        this->resize(QSize(fullS.width(), fullS.height()));
        break;
    case dureu::SCREEN_DETACHED:
        this->resize(QSize(availS.width()*scale, fullS.height()*scale_inv));
        this->move(availS.width()*scale_inv, fullS.height()-availS.height());
        break;
    default:
        std::cerr << "Application mode not recognized, closing application" << std::endl;
        exit(1);
    }
    std::cout << "Widget width and height: " << this->width() << " " << this->height() << std::endl;
}

void MainWindow::getTabletActivity(bool active){
    emit sendTabletActivity(active);
}

void MainWindow::recievedRequestUpdate()
{
    m_glWidget->update();
}

void MainWindow::recieveAutoSwitchMode(dureu::MOUSE_MODE mode)
{
    switch (mode){
    case dureu::MOUSE_SELECT:
        emit this->onSketch();
        break;
    case dureu::MOUSE_SKETCH:
        emit this->onCameraOrbit();
        break;
    case dureu::MOUSE_ORBIT:
        emit this->onSelect();
        break;
    default:
        emit this->onSelect();
        break;
    }
}

void MainWindow::recieveBookmark(int row)
{
    outLogMsg("bookmark recieved at MainWindow");
    entity::Bookmarks* bms = m_rootScene->getBookmarksModel();
    osg::Vec3d eye, center, up;
    eye = bms->getEyes()[row];
    center = bms->getCenters()[row];
    up = bms->getUps()[row];
    m_glWidget->setCameraView(eye, center, up);

    m_rootScene->updateBookmark(m_bookmarkWidget, row);
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

void MainWindow::onVisibilityCanvas(const QModelIndex &index)
{
    entity::Canvas* cnv = m_rootScene->getUserScene()->getCanvasFromIndex(index.row());
    if (!cnv){
        QMessageBox::critical(this, tr("Error"), tr("Could not obtain a canvas pointer"));
        return;
    }
    cnv->setVisibilityAll(!cnv->getVisibilityData());
    this->recievedRequestUpdate();
}

void MainWindow::onMoveBookmark(const QModelIndex &index)
{
    outLogMsg("onMoveBookmark: resetting current index");
    m_bookmarkWidget->setCurrentIndex(index);
}

/* Create an ordinary single view window on the scene _root
 * To create outside viewer, use:
 * GLWidget* vwid = createViewer(Qt::Window);
*/
/*void MainWindow::onCreateViewer(){
    GLWidget* m_glWidget = createViewer();
    QMdiSubWindow* subwin = m_mdiArea->addSubWindow(m_glWidget);
    subwin->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    m_glWidget->showMaximized();
    subwin->show();
    this->onSketch();
}*/

/* Check whether the current scene is empty or not
 * If not - propose to save changes.
 * Clear the scene graph
*/
void MainWindow::onFileNew()
{
    this->onFileClose();
    m_undoStack->clear();
    m_viewStack->clear();
    m_bookmarkWidget->clear();
    this->recievedRequestUpdate();
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
    if (fname.isEmpty()){
        QMessageBox::critical(this, tr("Error"), tr("Could not obtain a file name"));
        return;
    }
    m_rootScene->setFilePath(fname.toStdString());
    if (!m_rootScene->loadSceneFromFile()){
        QMessageBox::critical(this, tr("Error"), tr("Could not read from file. See the log for more details."));
        m_rootScene->setFilePath("");
    }
    else
        this->statusBar()->setStatusTip(tr("Scene was successfully read from file"));
    m_glWidget->update();
    this->initializeCallbacks();
    m_rootScene->getBookmarksModel()->resetModel(m_bookmarkWidget);

    this->statusBar()->showMessage(tr("Scene loaded."));
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

void MainWindow::onFileImage()
{
    if (m_rootScene->isEmptyScene()){
        QMessageBox::information(this, tr("Scene is empty"), tr("Create a canvas to load an image to"));
        return;
    }
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load an Image File"), QString(),
            tr("Image Files (*.bmp)"));
    if (!fileName.isEmpty()) {
        m_rootScene->addPhoto(fileName.toStdString());
        this->statusBar()->showMessage(tr("Image loaded to current canvas."));
       /* if (!m_rootScene->loadPhotoFromFile(fileName.toStdString())){
            QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
            return;
        }*/
    }
}

void MainWindow::onFileClose()
{
    outLogMsg("onFileClose() called");
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
    this->statusBar()->showMessage(tr("Current project is closed"));
}

void MainWindow::onFileExit()
{
    outLogMsg("onFileExit() called");
    this->onFileClose();
    this->close();
}

void MainWindow::onCut()
{
    this->statusBar()->showMessage(tr("This functionality does not exist yet."));
}

void MainWindow::onCopy()
{
    this->statusBar()->showMessage(tr("This functionality does not exist yet."));
}

void MainWindow::onPaste()
{
    this->statusBar()->showMessage(tr("This functionality does not exist yet."));
}

void MainWindow::onTools()
{
    if (m_actionTools->isChecked()){
        m_rootScene->setToolsVisibility(true);
    }
    else{
        m_rootScene->setToolsVisibility(false);
    }
    this->recievedRequestUpdate();
}

void MainWindow::onCameraOrbit(){
    QCursor* cur = new QCursor(Data::sceneOrbitPixmap(), 0, 0);
    m_mdiArea->setCursor(*cur);
    emit sendMouseMode(dureu::MOUSE_ORBIT);
    this->statusBar()->showMessage(tr("Camera orbit mode."));
}

void MainWindow::onCameraZoom(){
    QCursor* cur = new QCursor(Data::sceneZoomPixmap(), 0, 0);
    m_mdiArea->setCursor(*cur);
    emit sendMouseMode(dureu::MOUSE_ZOOM);
    this->statusBar()->showMessage(tr("Camera zoom mode."));
}

void MainWindow::onCameraPan(){
    QCursor* cur = new QCursor(Data::scenePanPixmap(), 0, 0);
    m_mdiArea->setCursor(*cur);
    emit sendMouseMode(dureu::MOUSE_PAN);
    this->statusBar()->showMessage(tr("Camera pan mode."));
}

void MainWindow::onSelect(){

    QCursor* cur = new QCursor(Data::sceneSelectPixmap(), 0, 0);
    m_mdiArea->setCursor(*cur);
    emit sendMouseMode(dureu::MOUSE_SELECT);
    this->statusBar()->showMessage(tr("To select canvas, click on its pickable; "
                                      "To select strokes within CURRENT canvas, "
                                      "hold <CTRL> and click on stroke to add to selection. "
                                      "Press <CTRL> to deselect all strokes."));
}

void MainWindow::onErase()
{
    QCursor* cur = new QCursor(Data::sceneEraserPixmap(), -1, -1);
    m_mdiArea->setCursor(*cur);
    emit sendMouseMode(dureu::MOUSE_ERASE);
    this->statusBar()->showMessage(tr("This functionality does not exist yet."));
}

void MainWindow::onDelete()
{
    QCursor* cur = new QCursor(Data::editDeleteCursor(), 0, 0);
    m_mdiArea->setCursor(*cur);
    this->statusBar()->showMessage(tr("To delete canvas, click by mouse on pickable; "
                                      "to delete stroke or photo within CURRENT canvas, "
                                      "hold <CTRL> and click on desired entity"));
    emit sendMouseMode(dureu::MOUSE_DELETE);
}

void MainWindow::onSketch()
{
    QCursor* cur = new QCursor(Data::sceneSketchPixmap(), 0, Data::sceneSketchPixmap().height());
    m_mdiArea->setCursor(*cur);
    emit sendMouseMode(dureu::MOUSE_SKETCH);
    this->statusBar()->showMessage(tr("Pressure defines a strokes. Press and drag to create new stroke."
                                      "Release when finished."));
}

void MainWindow::onNewCanvasClone()
{
    m_mdiArea->setCursor(Qt::ArrowCursor);
    emit sendMouseMode(dureu::MOUSE_CANVAS_CLONE);
    this->statusBar()->showMessage(tr("Click to start displaying the position of clonned canvas, drag the new position "
                                      "and release to fixate the position."));
}

void MainWindow::onNewCanvasXY()
{
    m_mdiArea->setCursor(Qt::ArrowCursor);
    m_rootScene->addCanvas(osg::Matrix::identity(), osg::Matrix::translate(0,0,0));
    this->onSketch();
    this->statusBar()->showMessage(tr("New canvas was created."));
}

void MainWindow::onNewCanvasYZ()
{
    m_mdiArea->setCursor(Qt::ArrowCursor);
    m_rootScene->addCanvas(osg::Matrix::rotate(dureu::PI*0.5, 0, -1, 0), osg::Matrix::translate(0,0,0));
    this->onSketch();
    this->statusBar()->showMessage(tr("New canvas was created."));
}

void MainWindow::onNewCanvasXZ()
{
    m_mdiArea->setCursor(Qt::ArrowCursor);
    m_rootScene->addCanvas(osg::Matrix::rotate(dureu::PI*0.5, -1, 0, 0), osg::Matrix::translate(0,0,0));
    this->onSketch();
    this->statusBar()->showMessage(tr("New canvas was created."));
}

void MainWindow::onNewCanvasStandard()
{
    m_rootScene->addCanvas(osg::Matrix::identity(),
                           osg::Matrix::translate(0.f, dureu::CANVAS_MINW*0.5f, 0.f));
    m_rootScene->addCanvas(osg::Matrix::rotate(-dureu::PI*0.5, 0, 1, 0),
                           osg::Matrix::translate(0.f, dureu::CANVAS_MINW*0.5f, 0.f));
    m_rootScene->addCanvas(osg::Matrix::rotate(dureu::PI*0.5, 1, 0, 0),
                           osg::Matrix::translate(0.f, 0.f, 0.f));
    this->onSketch();
    this->statusBar()->showMessage(tr("Set of canvases created."));
}

void MainWindow::onNewCanvasCoaxial()
{
    m_mdiArea->setCursor(Qt::ArrowCursor);
    this->statusBar()->showMessage(tr("This functionality does not exist yet."));
}

void MainWindow::onNewCanvasParallel()
{
    m_mdiArea->setCursor(Qt::ArrowCursor);
    this->statusBar()->showMessage(tr("This functionality does not exist yet."));
}

void MainWindow::onNewCanvasRing()
{
    m_mdiArea->setCursor(Qt::ArrowCursor);
    this->statusBar()->showMessage(tr("This functionality does not exist yet."));
}

void MainWindow::onCanvasOffset()
{
    QCursor* cur = new QCursor(Data::sceneCanvasOffsetCursor(), -1, -1);
    m_mdiArea->setCursor(*cur);
    emit sendMouseMode(dureu::MOUSE_CANVAS_OFFSET);
    this->statusBar()->showMessage(tr("For canvas offset, use mouse and drap and drop to desired position"));
}

void MainWindow::onCanvasRotate()
{
    QCursor* cur = new QCursor(Data::sceneCanvasRotateCursor(), -1, -1);
    m_mdiArea->setCursor(*cur);
    emit sendMouseMode(dureu::MOUSE_CANVAS_ROTATE);
    this->statusBar()->showMessage(tr("To switch the axis of rotation: press 'u' for local X-axis(orange), "
                                      "and 'v' for local Y-axis(yellow)"));
}

void MainWindow::onImageMove()
{
    QCursor* cur = new QCursor(Data::sceneImageMovePixmap(), -1, -1);
    m_mdiArea->setCursor(*cur);
    emit sendMouseMode(dureu::MOUSE_ENTITY_MOVE);
    this->statusBar()->showMessage(tr("To move image, click on it first, and then drag to desired position. "
                                      "To move set of strokes, select them first, then perform move as for image."));
}

void MainWindow::onImageRotate()
{
    QCursor* cur = new QCursor(Data::sceneImageRotatePixmap(), -1, -1);
    m_mdiArea->setCursor(*cur);
    emit sendMouseMode(dureu::MOUSE_ENTITY_ROTATE);
    this->statusBar()->showMessage(tr("To rotate image, click on it and drag it til it rotates to desired angle"
                                      "To rotate set of strokes, select them first, then perform rotate as for image."));
}

void MainWindow::onImageScale()
{
    QCursor* cur = new QCursor(Data::sceneImageScalePixmap(), 0, 0);
    m_mdiArea->setCursor(*cur);
    emit sendMouseMode(dureu::MOUSE_ENTITY_SCALE);
    this->statusBar()->showMessage(tr("To scale image, clock on it, and then drag to get the desired size"
                                      "To scale set of strokes, select them first, then perform scale as for image."));
}

void MainWindow::onImageFlipH()
{
    QCursor* cur = new QCursor(Data::sceneImageFlipHPixmap(), -1, -1);
    m_mdiArea->setCursor(*cur);
    emit sendMouseMode(dureu::MOUSE_PHOTO_FLIPH);
    this->statusBar()->showMessage(tr("To perform a horizontal flip, click on the image to flip."));
}

void MainWindow::onImageFlipV()
{
    QCursor* cur = new QCursor(Data::sceneImageFlipVPixmap(), -1, -1);
    m_mdiArea->setCursor(*cur);
    emit sendMouseMode(dureu::MOUSE_PHOTO_FLIPV);
    this->statusBar()->showMessage(tr("To perform the vertical flip, click on the image to flip"));
}

void MainWindow::onImagePush()
{
    m_mdiArea->setCursor(Qt::ArrowCursor);
    this->statusBar()->showMessage(tr("This functionality does not exist yet."));
}

void MainWindow::onStrokesPush()
{
    m_mdiArea->setCursor(Qt::CrossCursor);
    osg::Camera* camera = m_glWidget->getCamera();
    if (!camera)
    {
        std::cerr << "could not obtain camera" << std::endl;
        return;
    }
    m_rootScene->editStrokesPush(camera);
    this->statusBar()->showMessage(tr("Push is performed on selected strokes from current canvas (pink) to previous canvas (violet)."));
}

void MainWindow::onBookmark()
{
    osg::Vec3d eye,center,up;
    m_glWidget->getCameraView(eye, center, up);
    m_rootScene->addBookmark(m_bookmarkWidget, eye, center, up);
    this->statusBar()->showMessage(tr("Current camera view is saved as a bookmark"));
}

/*GLWidget* MainWindow::createViewer(Qt::WindowFlags f, int viewmode)
{
    GLWidget* vwid = new GLWidget(m_rootScene.get(), this, f);
    QObject::connect(this, SIGNAL(sendTabletActivity(bool)),
                     vwid, SLOT(getTabletActivity(bool)));
    QObject::connect(this, SIGNAL(sendMouseMode(dureu::MOUSE_MODE)),
                     vwid, SLOT(recieveMouseMode(dureu::MOUSE_MODE)));
    return vwid;
}*/

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

    m_actionDelete = new QAction(Data::editDeleteIcon(), tr("&Delete"), this);
    this->connect(m_actionDelete, SIGNAL(triggered(bool)), this, SLOT(onDelete()));
    m_actionDelete->setShortcut(Qt::Key_Delete);

    m_actionTools = new QAction(Data::editSettingsIcon(), tr("&Tools"), this);
    m_actionTools->setCheckable(true);
    m_actionTools->setChecked(true);
    this->connect(m_actionTools, SIGNAL(toggled(bool)), this, SLOT(onTools()));

    // SCENE

    m_actionSketch = new QAction(Data::sceneSketchIcon(), tr("&Sketch"), this);
    this->connect(m_actionSketch, SIGNAL(triggered(bool)), this, SLOT(onSketch()));

    m_actionEraser = new QAction(Data::sceneEraserIcon(), tr("&Eraser"), this);
    this->connect(m_actionEraser, SIGNAL(triggered(bool)), this, SLOT(onErase()));

    m_actionSelect = new QAction(Data::sceneSelectIcon(), tr("S&elect"), this);
    this->connect(m_actionSelect, SIGNAL(triggered(bool)), this, SLOT(onSelect()));

    m_actionOrbit = new QAction(Data::sceneOrbitIcon(), tr("&Orbit"), this);
    this->connect(m_actionOrbit, SIGNAL(triggered(bool)), this, SLOT(onCameraOrbit()));

    m_actionPan = new QAction(Data::scenePanIcon(), tr("&Pan"), this);
    this->connect(m_actionPan, SIGNAL(triggered(bool)), this, SLOT(onCameraPan()));

    m_actionZoom = new QAction(Data::sceneZoomIcon(), tr("&Zoom"), this);
    this->connect(m_actionZoom, SIGNAL(triggered(bool)), this, SLOT(onCameraZoom()));

    m_actionCanvasClone = new QAction(Data::sceneNewCanvasCloneIcon(), tr("Clone Current"), this);
    this->connect(m_actionCanvasClone, SIGNAL(triggered(bool)), this, SLOT(onNewCanvasClone()));

    m_actionCanvasXY = new QAction(Data::sceneNewCanvasXYIcon(), tr("Plane XY"), this);
    this->connect(m_actionCanvasXY, SIGNAL(triggered(bool)), this, SLOT(onNewCanvasXY()));

    m_actionCanvasYZ = new QAction(Data::sceneNewCanvasYZIcon(), tr("Plane YZ"), this);
    this->connect(m_actionCanvasYZ, SIGNAL(triggered(bool)), this, SLOT(onNewCanvasYZ()));

    m_actionCanvasXZ = new QAction(Data::sceneNewCanvasXZIcon(), tr("Plane XZ"), this);
    this->connect(m_actionCanvasXZ, SIGNAL(triggered(bool)), this, SLOT(onNewCanvasXZ()));

    m_actionSetStandard = new QAction(Data::sceneNewCanvasSetStandardIcon(), tr("Standard"), this);
    this->connect(m_actionSetStandard, SIGNAL(triggered(bool)), this, SLOT(onNewCanvasStandard()));

    m_actionSetCoaxial = new QAction(Data::sceneNewCanvasSetCoaxialIcon(), tr("Coaxial"), this);
    this->connect(m_actionSetCoaxial, SIGNAL(triggered(bool)), this, SLOT(onNewCanvasCoaxial()));

    m_actionSetParallel = new QAction(Data::sceneNewCanvasSetParallelIcon(), tr("Parallel"), this);
    this->connect(m_actionSetParallel, SIGNAL(triggered(bool)), this, SLOT(onNewCanvasParallel()));

    m_actionSetRing = new QAction(Data::sceneNewCanvasSetRingIcon(), tr("Ring"), this);
    this->connect(m_actionSetRing, SIGNAL(triggered(bool)), this, SLOT(onNewCanvasRing()));

    m_actionCanvasOffset = new QAction(Data::sceneCanvasOffsetIcon(), tr("Offset Canvas"), this);
    this->connect(m_actionCanvasOffset, SIGNAL(triggered(bool)), this, SLOT(onCanvasOffset()));

    m_actionCanvasRotate = new QAction(Data::sceneCanvasRotateIcon(), tr("Rotate Canvas"), this);
    this->connect(m_actionCanvasRotate, SIGNAL(triggered(bool)), this, SLOT(onCanvasRotate()));

    m_actionImageMove = new QAction(Data::sceneImageMoveIcon(), tr("Move Image"), this);
    this->connect(m_actionImageMove, SIGNAL(triggered(bool)), this, SLOT(onImageMove()));

    m_actionImageRotate = new QAction(Data::sceneImageRotateIcon(), tr("Rotate Image"), this);
    this->connect(m_actionImageRotate, SIGNAL(triggered(bool)), this, SLOT(onImageRotate()));

    m_actionImageScale = new QAction(Data::sceneImageScaleIcon(), tr("Scale Image"), this);
    this->connect(m_actionImageScale, SIGNAL(triggered(bool)), this, SLOT(onImageScale()));

    m_actionImageFlipV = new QAction(Data::sceneImageFlipVIcon(), tr("Flip Image"), this);
    this->connect(m_actionImageFlipV, SIGNAL(triggered(bool)), this, SLOT(onImageFlipV()));

    m_actionImageFlipH = new QAction(Data::sceneImageFlipHIcon(), tr("Flip Image"), this);
    this->connect(m_actionImageFlipH, SIGNAL(triggered(bool)), this, SLOT(onImageFlipH()));

    m_actionImagePush = new QAction(Data::sceneImagePushIcon(), tr("Push Image"), this);
    this->connect(m_actionImagePush, SIGNAL(triggered(bool)), this, SLOT(onImagePush()));

    m_actionStrokesPush = new QAction(Data::scenePushStrokesIcon(), tr("Push Strokes"), this);
    this->connect(m_actionStrokesPush, SIGNAL(triggered(bool)), this, SLOT(onStrokesPush()));

    /* VIEWER actions */
    m_actionPrevView = m_viewStack->createUndoAction(this, tr("Previous view"));
    m_actionPrevView->setIcon(Data::viewerPreviousIcon());

    m_actionNextView = m_viewStack->createRedoAction(this, tr("Next view"));
    m_actionNextView->setIcon(Data::viewerNextIcon());

    m_actionBookmark = new QAction(Data::viewerBookmarkIcon(), tr("Bookmark view"), this);
    this->connect(m_actionBookmark, SIGNAL(triggered(bool)), this, SLOT(onBookmark()));

}

void MainWindow::initializeMenus()
{
    // FILE
    QMenu* menuFile = m_menuBar->addMenu(tr("&File"));
    menuFile->addAction(m_actionNewFile);
    menuFile->addAction(m_actionOpenFile);
    menuFile->addSeparator();
    menuFile->addAction(m_actionSaveFile);
    // "Save As" here
    menuFile->addSeparator();
    menuFile->addAction(m_actionImportImage);
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
    menuEdit->addAction(m_actionDelete);

    // SCENE
    QMenu* menuScene = m_menuBar->addMenu(tr("&Scene"));
    menuScene->addAction(m_actionSelect);
    menuScene->addAction(m_actionSketch);
    menuScene->addAction(m_actionEraser);
    menuScene->addSeparator();
    QMenu* submenuCamera = menuScene->addMenu("Camera Navigation");
    submenuCamera->addAction(m_actionOrbit);
    submenuCamera->addAction(m_actionPan);
    submenuCamera->addAction(m_actionZoom);
    menuScene->addSeparator();
    QMenu* submenuCanvas = menuScene->addMenu("New Canvas");
    submenuCanvas->setIcon(Data::sceneNewCanvasIcon());
    submenuCanvas->addAction(m_actionCanvasClone);
    submenuCanvas->addAction(m_actionCanvasXY);
    submenuCanvas->addAction(m_actionCanvasYZ);
    submenuCanvas->addAction(m_actionCanvasXZ);
    QMenu* submenuSet = menuScene->addMenu("New Canvas Set");
    submenuSet->setIcon(Data::sceneNewCanvasSetIcon());
    submenuSet->addAction(m_actionSetStandard);
    submenuSet->addAction(m_actionSetCoaxial);
    submenuSet->addAction(m_actionSetParallel);
    submenuSet->addAction(m_actionSetRing);
    menuScene->addSeparator();
    QMenu* submenuEC = menuScene->addMenu("Edit Canvas");
    submenuEC->addAction(m_actionCanvasOffset);
    submenuEC->addAction(m_actionCanvasRotate);
    QMenu* submenuEI = menuScene->addMenu("Edit Image");
    submenuEI->addAction(m_actionImageMove);
    submenuEI->addAction(m_actionImageRotate);
    submenuEI->addAction(m_actionImageScale);
    submenuEI->addAction(m_actionImageFlipH);
    submenuEI->addAction(m_actionImageFlipV);
    submenuEI->addAction(m_actionImagePush);
    QMenu* submenuES = menuScene->addMenu("Edit Strokes");
    submenuES->addAction(m_actionStrokesPush);
}

void MainWindow::initializeToolbars()
{
    // File
    QToolBar* tbFile = this->addToolBar(tr("File"));
    tbFile->addAction(m_actionNewFile);
    tbFile->addAction(m_actionOpenFile);
    tbFile->addAction(m_actionSaveFile);
    tbFile->addAction(m_actionImportImage);

    // Edit
    QToolBar* tbEdit = this->addToolBar(tr("Edit"));
    tbEdit->addAction(m_actionUndo);
    tbEdit->addAction(m_actionRedo);
    tbEdit->addSeparator();
    tbEdit->addAction(m_actionCut);
    tbEdit->addAction(m_actionCopy);
    tbEdit->addAction(m_actionPaste);
    tbEdit->addAction(m_actionDelete);
    tbEdit->addAction(m_actionTools);

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
    tbInput->addAction(m_actionSketch);
    tbInput->addAction(m_actionEraser);

    QMenu* menuNewCanvas = new QMenu();
    menuNewCanvas->addAction(m_actionCanvasXY);
    menuNewCanvas->addAction(m_actionCanvasYZ);
    menuNewCanvas->addAction(m_actionCanvasXZ);
    menuNewCanvas->addAction(m_actionCanvasClone);
    QToolButton* tbNewCanvas = new QToolButton();
    tbNewCanvas->setIcon(Data::sceneNewCanvasIcon());
    tbNewCanvas->setMenu(menuNewCanvas);
    tbNewCanvas->setPopupMode(QToolButton::InstantPopup);
    QWidgetAction* waNewCanvas = new QWidgetAction(this);
    waNewCanvas->setDefaultWidget(tbNewCanvas);

    QMenu* menuNewCanvasSet = new QMenu();
    menuNewCanvasSet->addAction(m_actionSetStandard);
    menuNewCanvasSet->addAction(m_actionSetCoaxial);
    menuNewCanvasSet->addAction(m_actionSetParallel);
    menuNewCanvasSet->addAction(m_actionSetRing);
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
    //QToolBar* tbEntity = this->addToolBar(tr("Edit entity"));
    this->addToolBar(Qt::LeftToolBarArea, tbEntity);
    tbEntity->addAction(m_actionCanvasOffset);
    tbEntity->addAction(m_actionCanvasRotate);
    tbEntity->addSeparator();
    tbEntity->addAction(m_actionImageMove);
    tbEntity->addAction(m_actionImageRotate);
    tbEntity->addAction(m_actionImageScale);
    tbEntity->addAction(m_actionImageFlipH);
    tbEntity->addAction(m_actionImageFlipV);
    tbEntity->addAction(m_actionImagePush);
    tbEntity->addSeparator();
    tbEntity->addAction(m_actionStrokesPush);

    /* VIEWER bar */
    QToolBar* tbViewer = new QToolBar(tr("Viewer"));
    this->addToolBar(Qt::BottomToolBarArea, tbViewer);
    tbViewer->addAction(m_actionPrevView);
    tbViewer->addAction(m_actionNextView);
    tbViewer->addAction(m_actionBookmark);
}

/* signal slot connections must be established in two cases:
 * in MainWindow ctor;
 * on when scene is loaded from file
 */
void MainWindow::initializeCallbacks()
{
    /* connect MainWindow with GLWidget */
    QObject::connect(this, SIGNAL(sendTabletActivity(bool)),
                     m_glWidget, SLOT(getTabletActivity(bool)),
                     Qt::UniqueConnection);

    QObject::connect(this, SIGNAL(sendMouseMode(dureu::MOUSE_MODE)),
                     m_glWidget, SLOT(recieveMouseMode(dureu::MOUSE_MODE)),
                     Qt::UniqueConnection);

    QObject::connect(m_glWidget, SIGNAL(sendAutoSwitchMode(dureu::MOUSE_MODE)),
                     this, SLOT(recieveAutoSwitchMode(dureu::MOUSE_MODE)),
                     Qt::UniqueConnection);

    /* connect MainWindow with UserScene */
    QObject::connect(m_rootScene->getUserScene(), SIGNAL(sendRequestUpdate()),
                     this, SLOT(recievedRequestUpdate()),
                     Qt::UniqueConnection);

    /* bookmark widget data */
    QObject::connect(m_bookmarkWidget, SIGNAL(clicked(QModelIndex)),
                     m_rootScene->getBookmarksModel(), SLOT(onClicked(QModelIndex)),
                     Qt::UniqueConnection);

    QObject::connect(m_rootScene->getBookmarksModel(), SIGNAL(sendBookmark(int)),
                     this, SLOT(recieveBookmark(int)),
                     Qt::UniqueConnection);

    QObject::connect(m_bookmarkWidget->getBookmarkDelegate(), SIGNAL(clickedDelete(QModelIndex)),
                     this, SLOT(onDeleteBookmark(QModelIndex)),
                     Qt::UniqueConnection);

    QObject::connect(m_bookmarkWidget->getBookmarkDelegate(), SIGNAL(clickedMove(QModelIndex)),
                     this, SLOT(onMoveBookmark(QModelIndex)),
                     Qt::UniqueConnection);

    QObject::connect(m_bookmarkWidget, SIGNAL(itemChanged(QListWidgetItem*)), m_rootScene->getBookmarksModel(), SLOT(onItemChanged(QListWidgetItem*)) ,
                     Qt::UniqueConnection);

    QObject::connect(m_bookmarkWidget->model(), SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),
                     m_rootScene->getBookmarksModel(), SLOT(onRowsMoved(QModelIndex,int,int,QModelIndex,int)),
                     Qt::UniqueConnection);

    QObject::connect(m_bookmarkWidget->model(), SIGNAL(rowsRemoved(QModelIndex,int,int)),
                     m_rootScene->getBookmarksModel(), SLOT(onRowsRemoved(QModelIndex,int,int)),
                     Qt::UniqueConnection);

    QObject::connect(m_rootScene->getBookmarksModel(), SIGNAL(requestScreenshot(QPixmap&,osg::Vec3d,osg::Vec3d,osg::Vec3d)),
                     m_glWidget, SLOT(onRequestScreenshot(QPixmap&,osg::Vec3d,osg::Vec3d,osg::Vec3d)),
                     Qt::UniqueConnection);

    /* canvas widget area */
    QObject::connect(m_rootScene->getUserScene(), SIGNAL(canvasAdded(std::string)),
                     m_canvasWidget, SLOT(onCanvasAdded(std::string)), Qt::UniqueConnection);

    QObject::connect(m_rootScene->getUserScene(), SIGNAL(canvasSelectedColor(int,int)),
                     m_canvasWidget, SLOT(onCanvasSelectedColor(int,int)),
                     Qt::UniqueConnection);

    QObject::connect(m_rootScene->getUserScene(), SIGNAL(canvasRemoved(int)),
                     m_canvasWidget, SLOT(onCanvasRemoved(int)),
                     Qt::UniqueConnection);

    QObject::connect(m_canvasWidget, SIGNAL(clicked(QModelIndex)),
                     m_rootScene->getUserScene(), SLOT(onClicked(QModelIndex)),
                     Qt::UniqueConnection);

    QObject::connect(m_canvasWidget, SIGNAL(rightClicked(QModelIndex)),
                     m_rootScene->getUserScene(), SLOT(onRightClicked(QModelIndex)),
                     Qt::UniqueConnection);

    QObject::connect(m_canvasWidget->getCanvasDelegate(), SIGNAL(clickedDelete(QModelIndex)),
                     this, SLOT(onDeleteCanvas(QModelIndex)),
                     Qt::UniqueConnection);

    QObject::connect(m_canvasWidget->getCanvasDelegate(), SIGNAL(clickedVisibility(QModelIndex)),
                     this, SLOT(onVisibilityCanvas(QModelIndex)),
                     Qt::UniqueConnection);
}
