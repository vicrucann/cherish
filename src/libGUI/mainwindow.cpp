#include <iostream>

#include <QMdiSubWindow>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QtWidgets>
#include <QObject>
#include <QRect>
#include <QSize>

#include "mainwindow.h"
//#include "viewwidget.h"
#include "glwidget.h"
#include "settings.h"
#include "bookmarkwidget.h"
#include "listwidget.h"
#include "canvasinfomodel.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
    , _desktop(0)
    , _mdiArea(new QMdiArea(this))
    , _bookmarks(new BookmarkWidget(this))
    , _tabletActive(false)
    , m_UndoStack(new QUndoStack(this))
    , m_UndoView(new QUndoView(m_UndoStack))
    , _rootScene(new RootScene(m_UndoStack))
    , _menuBar(new QMenuBar(0)) // http://stackoverflow.com/questions/8108729/qmenu-does-not-work-on-mac-qt-creator
{
    this->setMenuBar(_menuBar);
    this->onCreateViewer();

    m_UndoView->setWindowTitle(tr("Command List"));
    m_UndoView->show();
    m_UndoView->setAttribute(Qt::WA_QuitOnClose, false);

    //_rootScene->setUndoStack(m_UndoStack);

    //this->addDockWidget(Qt::LeftDockWidgetArea, _bookmarks);

    //QMenuBar* menuBar = this->menuBar();
    /*QMenu* menuTest = _menuBar->addMenu("Test");

    menuTest->addAction("Add scene double viewer", this, SLOT(onCreateDoubleViewer()));
    menuTest->addAction("Add outisde viewer", this, SLOT(onCreateOutsideViewer()));
    menuTest->addAction("Load cow to the scene",  this, SLOT(onLoadCow()));
    menuTest->addAction("Stylus draw ON", this, SLOT(onSetStylusSketchON()));
    menuTest->addAction("Stylus draw OFF", this, SLOT(onSetStylusSketchOFF()));
    menuTest->addAction("Global axes ON", this, SLOT(onSetGloAxesON()));
    menuTest->addAction("Global axes OFF", this, SLOT(onSetGloAxesOFF()));

    QMenu* menuTC = _menuBar->addMenu("TestModels");
    menuTC->addAction("Delete canvas 3", this, SLOT(onDeleteCanvas()));
    menuTC->addAction("Delete cow.osg", this, SLOT(onDeleteCow()));
    menuTC->addAction("Increase size of current canvas", this, SLOT(onChangeSizeCanvas()));

    QMenu* menuTM = _menuBar->addMenu("TestMouse");
    menuTM->addAction("Naviagation: rotate", this, SLOT(onMouseOrbit()));
    menuTM->addAction("Navigation: zoom", this, SLOT(onMouseZoom()));
    menuTM->addAction("Naviagation: pan", this, SLOT(onMousePan()));
    menuTM->addAction("Mouse: pick", this, SLOT(onMousePick()));
    menuTM->addAction("Mouse: erase", this, SLOT(onMouseErase()));
    menuTM->addAction("Mouse: draw", this, SLOT(onMouseSketch()));*/

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createLayerManager();
    createBookMark();

    this->setCentralWidget(_mdiArea);
    //this->centralWidget()->setMouseTracking(true);
    // it is to show how to add a photo to a current canvas
    // start with extenstions *.bmp and *.rgb
    // for other file formats, OSG would need corresponding plugins
    _rootScene->loadPhotoFromFile("../../samples/ds-32.bmp");
}

MainWindow::~MainWindow(){
    if (_menuBar)
        delete _menuBar;
}

void MainWindow::SetDesktopWidget(QDesktopWidget *desktop, dureu::APPMODE mode) {
    _desktop = desktop;
    QRect availS = _desktop->availableGeometry();
    QRect fullS = _desktop->geometry();
    //int nscreen = _desktop->screenCount();
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
    _tabletActive = active;
    emit sendTabletActivity(active);
}

/* Create an ordinary single view window on the scene _root */
void MainWindow::onCreateViewer(){
    GLWidget* vwid = createViewer();
    //ViewWidget* vwid = createViewer();
    QMdiSubWindow* subwin = _mdiArea->addSubWindow(vwid);
    subwin->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    vwid->showMaximized();
    subwin->show();
}

void MainWindow::onCreateDoubleViewer(){
    GLWidget* vwid = createViewer(Qt::Widget, 2);
    QMdiSubWindow* subwin = _mdiArea->addSubWindow(vwid);
    subwin->show();
}

void MainWindow::onCreateOutsideViewer(){
    GLWidget* vwid = createViewer(Qt::Window);
    vwid->show();
}

void MainWindow::onLoadCow(){
    bool success = _rootScene->loadSceneFromFile("../../samples/cow.osgt");
    if (!success){
        std::cerr << "The filename provided was not correct, or the model could not be read" << std::endl;
    }
}

void MainWindow::onSetStylusSketchON(){
    emit sendStylusSketchStatus(true);
}

void MainWindow::onSetStylusSketchOFF(){
    emit sendStylusSketchStatus(false);
}

void MainWindow::onSetGloAxesON() {
    _rootScene->setAxesVisibility(true);
}

void MainWindow::onSetGloAxesOFF() {
    _rootScene->setAxesVisibility(false);
}

void MainWindow::onDeleteCanvas()
{
    // this is just to show how a certain canvas can be deleted
    _rootScene->deleteCanvas(2); // we delete a canvas with id "3"
}

void MainWindow::onDeleteCow() {
    _rootScene->deleteNode("cow.node");
}

void MainWindow::onChangeSizeCanvas()
{
    //_rootScene->getCanvasCurrent()->testWidthPlus();
}

void MainWindow::onMouseOrbit(){
    emit sendMouseMode(dureu::MOUSE_ROTATE);
    m_currentCursor = new QCursor(QPixmap(":/orbit_icon.png"),-1,-1);
    setCursor(*m_currentCursor);
}

void MainWindow::onMouseZoom(){
    emit sendMouseMode(dureu::MOUSE_ZOOM);
    m_currentCursor = new QCursor(QPixmap(":/zoom_icon.png"),-1,-1);
    setCursor(*m_currentCursor);
}

void MainWindow::onMousePan(){
    emit sendMouseMode(dureu::MOUSE_PAN);
    m_currentCursor = new QCursor(QPixmap(":/pan_icon.png"),-1,-1);
    setCursor(*m_currentCursor);
}

void MainWindow::onMousePick(){
    emit sendMouseMode(dureu::MOUSE_PICK);
    m_currentCursor = new QCursor(QPixmap(":/select_icon.png"),-1,-1);
    setCursor(*m_currentCursor);
}

void MainWindow::onMouseErase()
{
    emit sendMouseMode(dureu::MOUSE_ERASE);
    m_currentCursor = new QCursor(QPixmap(":/eraser_icon.png"),-1,-1);
    setCursor(*m_currentCursor);
}

void MainWindow::onMouseDelete()
{
    emit sendMouseMode(dureu::MOUSE_DELETE);
    m_currentCursor = new QCursor(QPixmap(":/delete_icon.png"),-1,-1);
    setCursor(*m_currentCursor);
}

void MainWindow::onMouseSketch()
{
    emit sendMouseMode(dureu::MOUSE_SKETCH);
    // We recommend using 32 x 32 cursors, because this size is supported on all platforms.
    m_currentCursor = new QCursor(QPixmap(":/stylus_icon.png"),-1,-1);
    setCursor(*m_currentCursor);
    //setCursor(*myCursor);
    //don't forget to change m_currentCursor to (Qt::ArrowCursor) in offMouseSketch()
}

void MainWindow::onMouseOffset()
{
    emit sendMouseMode(dureu::MOUSE_EDIT_OFFSET);
    m_currentCursor = new QCursor(QPixmap(":/offset_icon.png"),-1,-1);
    setCursor(*m_currentCursor);
}

void MainWindow::onMouseRotate()
{
    emit sendMouseMode(dureu::MOUSE_EDIT_ROTATE);
    m_currentCursor = new QCursor(QPixmap(":/rotate_icon.png"), -1, -1);
    setCursor(*m_currentCursor);
}

void MainWindow::onMouseMove()
{
    emit sendMouseMode(dureu::MOUSE_EDIT_MOVE);
    m_currentCursor = new QCursor(QPixmap(":/move_icon.png"), -1, -1);
    setCursor(*m_currentCursor);
}

GLWidget* MainWindow::createViewer(Qt::WindowFlags f, int viewmode)
{
    GLWidget* vwid = new GLWidget(_rootScene.get(), this, f);
    QObject::connect(this, SIGNAL(sendTabletActivity(bool)),
                     vwid, SLOT(getTabletActivity(bool)));
    QObject::connect(this, SIGNAL(sendStylusSketchStatus(bool)),
                     vwid, SLOT(getStylusSketchStatus(bool)) );
    QObject::connect(this, SIGNAL(sendMouseMode(dureu::MOUSE_MODE)),
                     vwid, SLOT(recieveMouseMode(dureu::MOUSE_MODE)));
    return vwid;
}

void MainWindow::createActions()
{
    newAct = new QAction(QIcon(":/new.png"), tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(QIcon(":/open.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    loadImageAction = new QAction(QIcon(":/load_image.png"), tr("Load Image"), this);
    loadImageAction->setStatusTip(tr("Load an image"));
    connect(loadImageAction, SIGNAL(triggered()), this, SLOT(loadImage()));

    saveAct = new QAction(QIcon(":/save.png"), tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    print = new QAction(tr("&Print..."), this);
    print->setShortcuts(QKeySequence::Print);
    print->setStatusTip(tr("Print..."));

    //m_ActionUndo = new QAction(QIcon(":/undo.png"), tr("&Undo"), this);
    m_ActionUndo = m_UndoStack->createUndoAction(this, tr("&Undo"));
    m_ActionUndo->setIcon(QIcon(":/undo.png"));
    m_ActionUndo->setShortcuts(QKeySequence::Undo);
    m_ActionUndo->setStatusTip(tr("Undo"));

    //m_ActionRedo = new QAction(QIcon(":/redo.png"), tr("&Redo"), this);
    m_ActionRedo = m_UndoStack->createRedoAction(this, tr("&Redo"));
    m_ActionRedo->setIcon(QIcon(":/redo.png"));
    m_ActionRedo->setShortcuts(QKeySequence::Redo);
    m_ActionRedo->setStatusTip(tr("Redo"));

    cutAct = new QAction(QIcon(":/cut.png"), tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    //connect(cutAct, SIGNAL(triggered()), textEdit, SLOT(cut()));

    copyAct = new QAction(QIcon(":/copy.png"), tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    //connect(copyAct, SIGNAL(triggered()), textEdit, SLOT(copy()));

    pasteAct = new QAction(QIcon(":/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    //connect(pasteAct, SIGNAL(triggered()), textEdit, SLOT(paste()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    contactUs = new QAction(tr("Contact Us"), this);
    contactUs->setStatusTip(tr("Show the contact info"));
    connect(contactUs, SIGNAL(triggered()), qApp, SLOT(contactInfo()));

    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    //connect(textEdit, SIGNAL(copyAvailable(bool)),
    //        cutAct, SLOT(setEnabled(bool)));
    //connect(textEdit, SIGNAL(copyAvailable(bool)),
     //       copyAct, SLOT(setEnabled(bool)));


    toolPalsandBox = new QAction(tr("SandBox"), this);
    toolPalsandBox->setStatusTip(tr("SandBox tool palettes"));

    axes = new QAction(tr("Axes"), this);
    axes->setStatusTip(tr("Show axes"));

    _mActionSketch = new QAction(QIcon(":/pencil.png"), tr("Sketch"), this);
    _mActionSketch->setStatusTip(tr("Sketch"));
    this->connect(_mActionSketch, SIGNAL(triggered()), this, SLOT(onMouseSketch()));

    line = new QAction(QIcon(":/line.png"),tr("Line"), this);
    line->setStatusTip(tr("Line"));

    arc = new QAction(QIcon(":/arc.png"),tr("Arc"), this);
    arc->setStatusTip(tr("Arc"));

    rectangle = new QAction(QIcon(":/rectangle.png"),tr("Rectangle"), this);
    rectangle->setStatusTip(tr("Rectangle"));

    previousCam = new QAction(tr("Previous"), this);
    previousCam->setStatusTip(tr("Go back to previous cam position"));

    _mActionOrbit = new QAction(QIcon(":/orbit.png"),tr("&orbit"), this);
    _mActionOrbit->setStatusTip(tr("orbit"));
    this->connect(_mActionOrbit, SIGNAL(triggered()), this, SLOT(onMouseOrbit()));

    _mActionPan = new QAction(QIcon(":/pan.png"),tr("&Pan"), this);
    _mActionPan->setStatusTip(tr("Pan"));
    this->connect(_mActionPan, SIGNAL(triggered()), this, SLOT(onMousePan()));

    _mActionZoom = new QAction(QIcon(":/zoom.png"),tr("&Zoom"), this);
    _mActionZoom->setStatusTip(tr("Zoom"));
    this->connect(_mActionZoom, SIGNAL(triggered()), this, SLOT(onMouseZoom()));

    zoomWindow = new QAction(tr("&Zoom Window"), this);
    zoomWindow->setStatusTip(tr("Zoom Window"));

    zoomExtents = new QAction(QIcon(":/zoomExtents.png"),tr("&Zoom Extents"), this);
    zoomExtents->setStatusTip(tr("Zoom Extents"));

    _mActionSelect = new QAction(QIcon(":/select.png"),tr("&Select"), this);
    _mActionSelect->setStatusTip(tr("Select"));
    this->connect(_mActionSelect, SIGNAL(triggered()), this, SLOT(onMousePick()));

    _mActionSelectPlane = new QAction(tr("&SelectPlane"), this);
    _mActionSelectPlane->setStatusTip(tr("SelectPlane"));
    this->connect(_mActionSelectPlane, SIGNAL(triggered()), this, SLOT(onMouseSelectPlane()));

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


    toolScale = new QAction(QIcon(":/scale.png"),tr("&Scale"), this);
    toolScale->setStatusTip(tr("Scale"));

    _mActionOffset = new QAction(QIcon(":/offset.png"),tr("&Offset"), this);
    _mActionOffset->setStatusTip(tr("Offset"));
    this->connect(_mActionOffset, SIGNAL(triggered(bool)), this, SLOT(onMouseOffset()));

    layers = new QAction(QIcon(":/layersb.png"),tr("Layers"), this);
    layers->setStatusTip(tr("Layers"));

    allStyle = new QAction(tr("allStyle"), this);
    allStyle->setStatusTip(tr("allStyle"));

    strokesStyle = new QAction(tr("strokesStyle"), this);
    strokesStyle->setStatusTip(tr("strokesStyle"));

    imagesStyle =new QAction(tr("imagesStyle"), this);
    imagesStyle->setStatusTip(tr("imagesStyle"));

    isoView = new QAction(tr("isoView"), this);
    isoView->setStatusTip(tr("isoView"));

    topView = new QAction(tr("topView"), this);
    topView->setStatusTip(tr("topView"));

    frontView = new QAction(tr("frontView"), this);
    frontView->setStatusTip(tr("frontView"));

    rightView = new QAction(tr("rightView"), this);
    rightView->setStatusTip(tr("rightView"));

    backView = new QAction(tr("backView"), this);
    backView->setStatusTip(tr("backView"));

    leftView = new QAction(tr("leftView"), this);
    leftView->setStatusTip(tr("leftView"));

    _mActionStack = new QAction(tr("Stack"), this);
    _mActionStack->setStatusTip(tr("Stack"));

    _mActionCircle = new QAction(tr("Circle"), this);
    _mActionCircle->setStatusTip(tr("Circle"));

    _mActionClone = new QAction(tr("Clone"), this);
    _mActionClone->setStatusTip(tr("Clone"));

    _mActionXY = new QAction(tr("XY"), this);
    _mActionXY->setStatusTip(tr("XY"));

    _mActionXZ = new QAction(tr("XZ"), this);
    _mActionXZ->setStatusTip(tr("XZ"));

    _mActionYZ = new QAction(tr("YZ"), this);
    _mActionYZ->setStatusTip(tr("YZ"));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addAction(exitAct);
    fileMenu->addSeparator();
    //fileMenu->addAction(print);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(m_ActionUndo);
    editMenu->addAction(m_ActionRedo);
    editMenu->addSeparator();
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);

    //menuBar()->addSeparator();
    viewMenu = menuBar()->addMenu(tr("&View"));
    toolPalettes  = viewMenu->addMenu(tr("Tool Palettes"));
    toolPalettes->addAction(toolPalsandBox);
    viewMenu->addSeparator();
    viewMenu->addAction(axes);

    drawMenu = menuBar()->addMenu(tr("&Draw"));
    _mSketchMenu = drawMenu->addMenu(tr("Sketching"));
    _mSketchMenu->addAction(_mActionSketch);
    lines = drawMenu->addMenu(tr("Lines"));
    lines->addAction(line);
    arcs = drawMenu->addMenu(tr("Arcs"));
    arcs->addAction(arc);
    shapes = drawMenu->addMenu(tr("Shapes"));
    shapes->addAction(rectangle);

    cameraMenu = menuBar()->addMenu(tr("&Camera"));
    cameraMenu->addAction(previousCam);
    cameraMenu->addSeparator();
    cameraMenu->addAction(_mActionOrbit);
    cameraMenu->addAction(_mActionPan);
    cameraMenu->addAction(_mActionZoom);
    //cameraMenu->addAction(zoomWindow);
    //cameraMenu->addAction(zoomExtents);


    toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(_mActionSelect);
    toolsMenu->addAction(_mActionEraser);
    toolsMenu->addSeparator();
    //toolsMenu->addAction(toolMove);
    toolsMenu->addAction(_mActionRotate);
    toolsMenu->addAction(toolScale);
    //toolsMenu->addSeparator();
    toolsMenu->addAction(_mActionOffset);

    windowMenu = menuBar()->addMenu(tr("&Window"));
    windowMenu->addAction(layers);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(contactUs);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(loadImageAction);
    fileToolBar->addAction(saveAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);

    drawToolBar = addToolBar(tr("Draw"));
    drawToolBar->addAction(_mActionSketch);
    drawToolBar->addAction(line);
    //drawToolBar->addAction(arc);
    //drawToolBar->addAction(rectangle);

    cameraToolBar = addToolBar(tr("Camera"));
    cameraToolBar->addAction(_mActionOrbit);
    cameraToolBar->addAction(_mActionPan);
    cameraToolBar->addAction(_mActionZoom);
    //cameraToolBar->addAction(zoomExtents);

    toolsToolBar = addToolBar(tr("Tools"));

    newCanvasMenu = new QMenu();
    newCanvasMenu->addAction(_mActionStack);
    newCanvasMenu->addAction(_mActionCircle);
    newCanvasMenu->addAction(_mActionClone);
    newCanvasMenu->addAction(_mActionXY);
    newCanvasMenu->addAction(_mActionXZ);
    newCanvasMenu->addAction(_mActionYZ);
    newCanvas = new QToolButton();
    newCanvas->setIcon(QIcon(":/new_canvas.png"));
    newCanvas->setMenu(newCanvasMenu);
    newCanvas->setPopupMode(QToolButton::InstantPopup);
    newCanvasAction = new QWidgetAction(this);
    newCanvasAction->setDefaultWidget(newCanvas);
    toolsToolBar->addAction(newCanvasAction);

    selectMenu = new QMenu();
    selectMenu->addAction(_mActionSelect);
    selectMenu->addAction(_mActionSelectPlane);
    select = new QToolButton();
    select->setIcon(QIcon(":/select.png"));
    select->setMenu(selectMenu);
    select->setPopupMode(QToolButton::InstantPopup);
    selectAction = new QWidgetAction(this);
    selectAction->setDefaultWidget(select);
    toolsToolBar->addAction(selectAction);

    toolsToolBar->addAction(_mActionEraser);
    toolsToolBar->addAction(_mActionDelete);
    toolsToolBar->addAction(_mActionOffset);
    toolsToolBar->addAction(_mActionMove);
    toolsToolBar->addAction(_mActionPushStrokes);
    toolsToolBar->addAction(_mActionRotate);
    toolsToolBar->addAction(_mActionRotateImage);
    toolsToolBar->addAction(toolScale);

    //styleToolBar = addToolBar(tr("Style"));
    //styleToolBar->addAction(allStyle);
    //styleToolBar->addAction(strokesStyle);
   // styleToolBar->addAction(imagesStyle);

    viewToolBar = addToolBar(tr("View"));
    viewToolBar->addAction(isoView);
    viewToolBar->addAction(topView);
    viewToolBar->addAction(frontView);
    viewToolBar->addAction(rightView);
    viewToolBar->addAction(backView);
    viewToolBar->addAction(leftView);

    windowToolBar = addToolBar(tr("Window"));
    windowToolBar->addAction(layers);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createLayerManager()
{
    tableView = new QTableView();
    tableView->setShowGrid(false);
    tableView->verticalHeader()->hide();
    tableView->setFrameShape(QFrame::NoFrame);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->horizontalHeader()->setStretchLastSection(true);

    CanvasInfoModel *myModel = new CanvasInfoModel(2,1,this);   //total column 2
    tableView->setModel(myModel); //set QTableView'model to bestudentInfoModel
    CanvasInfo cInfo;
    cInfo.SetCanName("Canvas 1");
    //add four cans
    myModel->AddCanvasInfo(cInfo);
    myModel->AddCanvasInfo(cInfo);
    myModel->AddCanvasInfo(cInfo);
    myModel->AddCanvasInfo(cInfo);
    QDockWidget *dock = new QDockWidget(tr("Layers"), this);
    dock->setAllowedAreas(Qt::RightDockWidgetArea);
    dock->setWidget(tableView);
    addDockWidget(Qt::RightDockWidgetArea, dock);
}


void MainWindow::createBookMark()
{
    QDockWidget *dock = new QDockWidget(tr("Bookmarks"), this);
    dock->setAllowedAreas(Qt::BottomDockWidgetArea);
    m_lw = new ListWidget(dock);
    dock->setWidget(m_lw);
    addDockWidget(Qt::BottomDockWidgetArea, dock);
}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if(object==_mdiArea && (event->type()==QEvent::Enter || event->type()==QEvent::Leave)){
        if(event->type()==QEvent::Enter){
            this->setCursor(*m_currentCursor);
            qDebug()<<"enter"<<this->mapFromGlobal(QCursor::pos());
        }
        else
        {
            this->setCursor(Qt::ArrowCursor);
            qDebug()<<"leave"<<this->mapFromGlobal(QCursor::pos());
        }
    }
}

void MainWindow::loadImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load an Image File"), QString(),
            tr("Image Files (*.bmp)"));

    if (!fileName.isEmpty()) {
        _rootScene->loadPhotoFromFile(fileName.toStdString());
        /*recommend change loadPhotoFromFile into a bool function
          if (something wrong) {
            QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
            return;
        }*/
    }

}
