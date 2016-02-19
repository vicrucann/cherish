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
#include "glwidget.h"
#include "Settings.h"
#include "Data.h"

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
    , m_desktop(0)
    , m_mdiArea(new QMdiArea(this))
    , m_undoStack(new QUndoStack(this))
    , m_undoView(new QUndoView(m_undoStack))
    , m_menuBar(new QMenuBar(0)) // http://stackoverflow.com/questions/8108729/qmenu-does-not-work-on-mac-qt-creator
    , m_rootScene(new RootScene(m_undoStack))
    , m_glWidget(new GLWidget(m_rootScene.get(), this))
{
    this->setMenuBar(m_menuBar);

    /* Create GLWidgets */
    //this->onCreateViewer();
    QObject::connect(this, SIGNAL(sendTabletActivity(bool)), m_glWidget, SLOT(getTabletActivity(bool)));
    QObject::connect(this, SIGNAL(sendMouseMode(dureu::MOUSE_MODE)), m_glWidget, SLOT(recieveMouseMode(dureu::MOUSE_MODE)));
    QMdiSubWindow* subwin = m_mdiArea->addSubWindow(m_glWidget);
    subwin->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    m_glWidget->showMaximized();
    subwin->show();

    /* connect MainWindow with UserScene*/
    entity::UserScene* scene = m_rootScene->getUserScene();
    if (!scene){
        outErrMsg("MainWindow ctor: UserScene is NULL");
        this->close();
    }
    QObject::connect(scene, SIGNAL(sendRequestUpdate()), this, SLOT(recievedRequestUpdate()));

    /* undo/redo widget */
    m_undoView->setWindowTitle(tr("Command List"));
    m_undoView->show();
    m_undoView->setAttribute(Qt::WA_QuitOnClose, false);

    /* actions, menu, toolbars initialization */
    this->setCentralWidget(m_mdiArea);
    this->initializeActions();
    this->initializeMenus();
    this->initializeToolbars();

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

/* Create an ordinary single view window on the scene _root
 * To create outside viewer, use:
 * GLWidget* vwid = createViewer(Qt::Window);
*/
void MainWindow::onCreateViewer(){
    GLWidget* m_glWidget = createViewer();
    QMdiSubWindow* subwin = m_mdiArea->addSubWindow(m_glWidget);
    subwin->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    m_glWidget->showMaximized();
    subwin->show();
    this->onSketch();
}

/* Check whether the current scene is empty or not
 * If not - propose to save changes.
 * Clear the scene graph
*/
void MainWindow::onFileNew()
{
    this->onFileClose();
    m_undoStack->clear();
    this->recievedRequestUpdate();
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
            this->statusBar()->showMessage(tr("Scene was not saved to file"), 2000);
            return;
        }
        m_rootScene->setFilePath(fname.toStdString());
    }
    if (!m_rootScene->writeScenetoFile()){
        QMessageBox::critical(this, tr("Error"), tr("Could not write scene to file"));
        m_rootScene->setFilePath("");
        this->statusBar()->showMessage(tr("Scene was not saved to file"), 2000);
        return;
    }
    this->statusBar()->showMessage(tr("Scene was successfully saved to file"), 2000);
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
    this->statusBar()->showMessage(tr("Current project is closed"), 2000);
}

void MainWindow::onFileExit()
{
    outLogMsg("onFileExit() called");
    this->onFileClose();
    this->close();
}

void MainWindow::onCut()
{

}

void MainWindow::onCopy()
{

}

void MainWindow::onPaste()
{

}

void MainWindow::onCameraOrbit(){
    QCursor* cur = new QCursor(Data::sceneOrbitPixmap(), 0, 0);
    m_mdiArea->setCursor(*cur);
    emit sendMouseMode(dureu::MOUSE_ORBIT);
}

void MainWindow::onCameraZoom(){
    QCursor* cur = new QCursor(Data::sceneZoomPixmap(), 0, 0);
    m_mdiArea->setCursor(*cur);
    emit sendMouseMode(dureu::MOUSE_ZOOM);
}

void MainWindow::onCameraPan(){
    QCursor* cur = new QCursor(Data::scenePanPixmap(), 0, 0);
    m_mdiArea->setCursor(*cur);
    emit sendMouseMode(dureu::MOUSE_PAN);
}

void MainWindow::onSelect(){

    QCursor* cur = new QCursor(Data::sceneSelectPixmap(), 0, 0);
    m_mdiArea->setCursor(*cur);
    emit sendMouseMode(dureu::MOUSE_SELECT);
}

void MainWindow::onErase()
{
    QCursor* cur = new QCursor(Data::sceneEraserPixmap(), -1, -1);
    m_mdiArea->setCursor(*cur);
    emit sendMouseMode(dureu::MOUSE_ERASE);
}

void MainWindow::onDelete()
{
    QCursor* cur = new QCursor(Data::editDeleteCursor(), 0, 0);
    m_mdiArea->setCursor(*cur);
    this->statusBar()->showMessage(tr("To delete canvas, click by mouse on pickable; "
                                      "to delete stroke or photo within CURRENT canvas, "
                                      "hold <CTRL> and click on desired entity"), 20000);
    emit sendMouseMode(dureu::MOUSE_DELETE);
}

void MainWindow::onSketch()
{
    QCursor* cur = new QCursor(Data::sceneSketchPixmap(), 0, Data::sceneSketchPixmap().height());
    m_mdiArea->setCursor(*cur);
    emit sendMouseMode(dureu::MOUSE_SKETCH);
}

void MainWindow::onNewCanvasClone()
{
    m_mdiArea->setCursor(Qt::ArrowCursor);
    emit sendMouseMode(dureu::MOUSE_CANVAS_CLONE);
    this->statusBar()->showMessage(tr("Click to start displaying the position of clonned canvas, drag the new position "
                                      "and release to fixate the position."), 20000);
}

void MainWindow::onNewCanvasXY()
{
    m_mdiArea->setCursor(Qt::ArrowCursor);
    m_rootScene->addCanvas(osg::Matrix::identity(), osg::Matrix::translate(0,0,0));
    this->onSketch();
}

void MainWindow::onNewCanvasYZ()
{
    m_mdiArea->setCursor(Qt::ArrowCursor);
    m_rootScene->addCanvas(osg::Matrix::rotate(dureu::PI*0.5, 0, -1, 0), osg::Matrix::translate(0,0,0));
    this->onSketch();
}

void MainWindow::onNewCanvasXZ()
{
    m_mdiArea->setCursor(Qt::ArrowCursor);
    m_rootScene->addCanvas(osg::Matrix::rotate(dureu::PI*0.5, -1, 0, 0), osg::Matrix::translate(0,0,0));
    this->onSketch();
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
}

void MainWindow::onNewCanvasCoaxial()
{
    m_mdiArea->setCursor(Qt::ArrowCursor);
}

void MainWindow::onNewCanvasParallel()
{
    m_mdiArea->setCursor(Qt::ArrowCursor);
}

void MainWindow::onNewCanvasRing()
{
    m_mdiArea->setCursor(Qt::ArrowCursor);
}

void MainWindow::onCanvasOffset()
{
    QCursor* cur = new QCursor(Data::sceneCanvasOffsetCursor(), -1, -1);
    m_mdiArea->setCursor(*cur);
    emit sendMouseMode(dureu::MOUSE_CANVAS_OFFSET);
}

void MainWindow::onCanvasRotate()
{
    QCursor* cur = new QCursor(Data::sceneCanvasRotateCursor(), -1, -1);
    m_mdiArea->setCursor(*cur);
    emit sendMouseMode(dureu::MOUSE_CANVAS_ROTATE);
    this->statusBar()->showMessage(tr("To switch the axis of rotation: press 'u' for local X-axis, "
                                      "and 'v' for local Y-axis"), 20000);
}

void MainWindow::onImageMove()
{
    QCursor* cur = new QCursor(Data::sceneImageMovePixmap(), -1, -1);
    m_mdiArea->setCursor(*cur);
    emit sendMouseMode(dureu::MOUSE_ENTITY_MOVE);
}

void MainWindow::onImageRotate()
{
    QCursor* cur = new QCursor(Data::sceneImageRotatePixmap(), -1, -1);
    m_mdiArea->setCursor(*cur);
    emit sendMouseMode(dureu::MOUSE_ENTITY_ROTATE);
}

void MainWindow::onImageScale()
{
    QCursor* cur = new QCursor(Data::sceneImageScalePixmap(), 0, 0);
    m_mdiArea->setCursor(*cur);
    emit sendMouseMode(dureu::MOUSE_ENTITY_SCALE);
}

void MainWindow::onImageFlipH()
{
    QCursor* cur = new QCursor(Data::sceneImageFlipHPixmap(), -1, -1);
    m_mdiArea->setCursor(*cur);
    emit sendMouseMode(dureu::MOUSE_PHOTO_FLIPH);
}

void MainWindow::onImageFlipV()
{
    QCursor* cur = new QCursor(Data::sceneImageFlipVPixmap(), -1, -1);
    m_mdiArea->setCursor(*cur);
    emit sendMouseMode(dureu::MOUSE_PHOTO_FLIPV);
}

void MainWindow::onImagePush()
{
    m_mdiArea->setCursor(Qt::ArrowCursor);
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
}

GLWidget* MainWindow::createViewer(Qt::WindowFlags f, int viewmode)
{
    GLWidget* vwid = new GLWidget(m_rootScene.get(), this, f);
    QObject::connect(this, SIGNAL(sendTabletActivity(bool)),
                     vwid, SLOT(getTabletActivity(bool)));
    QObject::connect(this, SIGNAL(sendMouseMode(dureu::MOUSE_MODE)),
                     vwid, SLOT(recieveMouseMode(dureu::MOUSE_MODE)));
    return vwid;
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

    // Camera navigation
    QToolBar* tbCamera = this->addToolBar(tr("Camera"));
    tbCamera->addAction(m_actionOrbit);
    tbCamera->addAction(m_actionZoom);
    tbCamera->addAction(m_actionPan);

    // Input
    QToolBar* tbInput = this->addToolBar(tr("Input"));
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
    QToolBar* tbEntity = this->addToolBar(tr("Edit entity"));
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
}
