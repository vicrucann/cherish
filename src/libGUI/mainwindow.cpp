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

#include "mainwindow.h"
#include "glwidget.h"
#include "settings.h"
#include "Data.h"

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
    , m_desktop(0)
    , m_mdiArea(new QMdiArea(this))
    , m_undoStack(new QUndoStack(this))
    , m_undoView(new QUndoView(m_undoStack))
    , m_menuBar(new QMenuBar(0)) // http://stackoverflow.com/questions/8108729/qmenu-does-not-work-on-mac-qt-creator
    , m_rootScene(new RootScene(m_undoStack))

    , m_actionNewFile(new QAction(Data::fileNewSceneIcon(), tr("New..."), this))
    , m_actionClose(new QAction(Data::fileCloseIcon(), tr("Close"), this))
    , m_actionExit(new QAction(Data::fileExitIcon(), tr("Exit"), this))
    , m_actionImportImage(new QAction(Data::fileExitIcon(), tr("Import Image..."), this))
    , m_actionOpenFile(new QAction(Data::fileOpenIcon(), tr("Open..."), this))
    , m_actionSaveFile(new QAction(Data::fileSaveIcon(), tr("Save..."), this))
    // (new QAction(), tr(), this)
{

    this->setMenuBar(m_menuBar);
    this->onCreateViewer();

    m_undoView->setWindowTitle(tr("Command List"));
    m_undoView->show();
    m_undoView->setAttribute(Qt::WA_QuitOnClose, false);

    this->setCentralWidget(m_mdiArea);
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

/* Create an ordinary single view window on the scene _root
 * To create outside viewer, use:
 * GLWidget* vwid = createViewer(Qt::Window);
*/
void MainWindow::onCreateViewer(){
    GLWidget* vwid = createViewer();
    QMdiSubWindow* subwin = m_mdiArea->addSubWindow(vwid);
    subwin->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    vwid->showMaximized();
    subwin->show();
}

void MainWindow::onMouseOrbit(){
    emit sendMouseMode(dureu::MOUSE_ROTATE);
}

void MainWindow::onMouseZoom(){
    emit sendMouseMode(dureu::MOUSE_ZOOM);
}

void MainWindow::onMousePan(){
    emit sendMouseMode(dureu::MOUSE_PAN);
}

void MainWindow::onMousePick(){
    emit sendMouseMode(dureu::MOUSE_PICK);
}

void MainWindow::onMouseErase()
{
    emit sendMouseMode(dureu::MOUSE_ERASE);
}

void MainWindow::onMouseDelete()
{
    emit sendMouseMode(dureu::MOUSE_DELETE);
}

void MainWindow::onMouseSketch()
{
    emit sendMouseMode(dureu::MOUSE_SKETCH);
}

void MainWindow::onMouseOffset()
{
    emit sendMouseMode(dureu::MOUSE_EDIT_OFFSET);
}

void MainWindow::onMouseRotate()
{
    emit sendMouseMode(dureu::MOUSE_EDIT_ROTATE);
}

void MainWindow::onMouseMove()
{
    emit sendMouseMode(dureu::MOUSE_EDIT_MOVE);
}

void MainWindow::onFileImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load an Image File"), QString(),
            tr("Image Files (*.bmp)"));
    if (!fileName.isEmpty()) {
        if (!m_rootScene->loadPhotoFromFile(fileName.toStdString())){
            QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
            return;
        }
    }
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

void MainWindow::createActions()
{

    loadImageAction = new QAction(QIcon(":/load_image.png"), tr("Load Image"), this);
    loadImageAction->setStatusTip(tr("Load an image"));
    connect(loadImageAction, SIGNAL(triggered()), this, SLOT(onFileImage()));

    m_ActionUndo = m_undoStack->createUndoAction(this, tr("&Undo"));
    m_ActionUndo->setIcon(QIcon(":/undo.png"));
    m_ActionUndo->setShortcuts(QKeySequence::Undo);
    m_ActionUndo->setStatusTip(tr("Undo"));

    m_ActionRedo = m_undoStack->createRedoAction(this, tr("&Redo"));
    m_ActionRedo->setIcon(QIcon(":/redo.png"));
    m_ActionRedo->setShortcuts(QKeySequence::Redo);
    m_ActionRedo->setStatusTip(tr("Redo"));

    _mActionSketch = new QAction(QIcon(":/pencil.png"), tr("Sketch"), this);
    _mActionSketch->setStatusTip(tr("Sketch"));
    this->connect(_mActionSketch, SIGNAL(triggered()), this, SLOT(onMouseSketch()));

    _mActionOrbit = new QAction(QIcon(":/orbit.png"),tr("&orbit"), this);
    _mActionOrbit->setStatusTip(tr("orbit"));
    this->connect(_mActionOrbit, SIGNAL(triggered()), this, SLOT(onMouseOrbit()));

    _mActionPan = new QAction(QIcon(":/pan.png"),tr("&Pan"), this);
    _mActionPan->setStatusTip(tr("Pan"));
    this->connect(_mActionPan, SIGNAL(triggered()), this, SLOT(onMousePan()));

    _mActionZoom = new QAction(QIcon(":/zoom.png"),tr("&Zoom"), this);
    _mActionZoom->setStatusTip(tr("Zoom"));
    this->connect(_mActionZoom, SIGNAL(triggered()), this, SLOT(onMouseZoom()));

    _mActionSelect = new QAction(QIcon(":/select.png"),tr("&Select"), this);
    _mActionSelect->setStatusTip(tr("Select"));
    this->connect(_mActionSelect, SIGNAL(triggered()), this, SLOT(onMousePick()));

    _mActionEraser = new QAction(QIcon(":/eraser.png"),tr("Eraser"), this);
    _mActionEraser->setStatusTip(tr("Eraser"));
    this->connect(_mActionEraser, SIGNAL(triggered(bool)), this, SLOT(onMouseErase()));

    _mActionDelete = new QAction(QIcon(":/delete.png"),tr("Delete"), this);
    _mActionDelete->setStatusTip(tr("Delete"));
    this->connect(_mActionDelete, SIGNAL(triggered(bool)), this, SLOT(onMouseDelete()));

    _mActionMove = new QAction(QIcon(":/move.png"),tr("&Move"), this);
    _mActionMove->setStatusTip(tr("Move"));
    this->connect(_mActionMove, SIGNAL(triggered(bool)), this, SLOT(onMouseMove()));

    _mActionPushStrokes = new QAction(tr("&Push Strokes"), this);
    _mActionPushStrokes->setStatusTip(tr("Push Strokes"));
    this->connect(_mActionPushStrokes, SIGNAL(triggered(bool)), this, SLOT(onMousePushStrokes()));

    _mActionRotate = new QAction(QIcon(":/rotate.png"),tr("&Rotate"), this);
    _mActionRotate->setStatusTip(tr("Rotate"));
    this->connect(_mActionRotate, SIGNAL(triggered(bool)), this, SLOT(onMouseRotate()));

    _mActionRotateImage = new QAction(tr("&Rotate Image"), this);
    _mActionRotateImage->setStatusTip(tr("Rotate Image"));
    this->connect(_mActionRotateImage, SIGNAL(triggered(bool)), this, SLOT(onMouseRotateImage()));

    _mActionOffset = new QAction(QIcon(":/offset.png"),tr("&Offset"), this);
    _mActionOffset->setStatusTip(tr("Offset"));
    this->connect(_mActionOffset, SIGNAL(triggered(bool)), this, SLOT(onMouseOffset()));
}
