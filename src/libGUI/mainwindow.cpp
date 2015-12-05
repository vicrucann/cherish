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
#include "viewwidget.h"
#include "settings.h"
#include "bookmarkwidget.h"

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags) :
    QMainWindow(parent, flags),
    _desktop(0),
    _mdiArea(new QMdiArea(this)),
    _bookmarks(new BookmarkWidget(this)),
    _tabletActive(false),
    _rootScene(new RootScene()),
    _menuBar(new QMenuBar(0)) // http://stackoverflow.com/questions/8108729/qmenu-does-not-work-on-mac-qt-creator
{
    this->setMenuBar(_menuBar);
    this->onCreateViewer();
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
    menuTM->addAction("Naviagation: rotate", this, SLOT(onMouseRotate()));
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

    this->setCentralWidget(_mdiArea);
}

MainWindow::~MainWindow(){
    if (_menuBar)
        delete _menuBar;
}

void MainWindow::SetDesktopWidget(QDesktopWidget *desktop, dureu::APPMODE mode) {
    _desktop = desktop;
    QRect availS = _desktop->availableGeometry();
    QRect fullS = _desktop->geometry();
    int nscreen = _desktop->screenCount();
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
}

void MainWindow::getTabletActivity(bool active){
    _tabletActive = active;
    emit sendTabletActivity(active);
}

/* Create an ordinary single view window on the scene _root */
void MainWindow::onCreateViewer(){    
    ViewWidget* vwid = createViewer();
    QMdiSubWindow* subwin = _mdiArea->addSubWindow(vwid);
    subwin->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    vwid->showMaximized();
    subwin->show();
}

void MainWindow::onCreateDoubleViewer(){
    ViewWidget* vwid = createViewer(Qt::Widget, 2);
    QMdiSubWindow* subwin = _mdiArea->addSubWindow(vwid);
    subwin->show();
}

void MainWindow::onCreateOutsideViewer(){
    ViewWidget* vwid = createViewer(Qt::Window);
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

void MainWindow::onMouseRotate(){
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

void MainWindow::onMouseSketch()
{
    emit sendMouseMode(dureu::MOUSE_SKETCH);
}

ViewWidget* MainWindow::createViewer(Qt::WindowFlags f, int viewmode)
{
    ViewWidget* vwid = new ViewWidget(_rootScene.get(), this, f, viewmode);
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
    newAct = new QAction(QIcon(":new.png"), tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(QIcon(":/open.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

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

    undo = new QAction(QIcon(":/undo.png"), tr("&Undo"), this);
    undo->setShortcuts(QKeySequence::Undo);
    undo->setStatusTip(tr("Undo"));

    redo = new QAction(QIcon(":/redo.png"), tr("&Redo"), this);
    redo->setShortcuts(QKeySequence::Redo);
    redo->setStatusTip(tr("Redo"));

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

    _mSketch = new QAction(QIcon(":/line.png"), tr("Sketch"), this);
    _mSketch->setStatusTip(tr("Sketch"));
    this->connect(_mSketch, SIGNAL(triggered()), this, SLOT(onMouseSketch()));

    line = new QAction(QIcon(":/line.png"),tr("Line"), this);
    line->setStatusTip(tr("Line"));

    arc = new QAction(QIcon(":/arc.png"),tr("Arc"), this);
    arc->setStatusTip(tr("Arc"));

    rectangle = new QAction(QIcon(":/rectangle.png"),tr("Rectangle"), this);
    rectangle->setStatusTip(tr("Rectangle"));

    previousCam = new QAction(tr("Previous"), this);
    previousCam->setStatusTip(tr("Go back to previous cam position"));

    _mOrbit = new QAction(QIcon(":/orbit.png"),tr("&orbit"), this);
    _mOrbit->setStatusTip(tr("orbit"));
    this->connect(_mOrbit, SIGNAL(triggered()), this, SLOT(onMouseRotate()));

    _mPan = new QAction(QIcon(":/pan.png"),tr("&Pan"), this);
    _mPan->setStatusTip(tr("Pan"));
    this->connect(_mPan, SIGNAL(triggered()), this, SLOT(onMousePan()));

    _mZoom = new QAction(QIcon(":/zoom.png"),tr("&Zoom"), this);
    _mZoom->setStatusTip(tr("Zoom"));
    this->connect(_mZoom, SIGNAL(triggered()), this, SLOT(onMouseZoom()));

    zoomWindow = new QAction(tr("&Zoom Window"), this);
    zoomWindow->setStatusTip(tr("Zoom Window"));

    zoomExtents = new QAction(QIcon(":/zoomExtents.png"),tr("&Zoom Extents"), this);
    zoomExtents->setStatusTip(tr("Zoom Extents"));

    _mSelect = new QAction(QIcon(":/select.png"),tr("&Select"), this);
    _mSelect->setStatusTip(tr("Select"));
    this->connect(_mSelect, SIGNAL(triggered()), this, SLOT(onMousePick()));

    _mEraser = new QAction(QIcon(":/eraser.png"),tr("Eraser"), this);
    _mEraser->setStatusTip(tr("Eraser"));
    this->connect(_mEraser, SIGNAL(triggered(bool)), this, SLOT(onMouseErase()));

    toolMove = new QAction(QIcon(":/move.png"),tr("&Move"), this);
    toolMove->setStatusTip(tr("Move"));

    toolRotate = new QAction(QIcon(":/rotate.png"),tr("&Rotate"), this);
    toolRotate->setStatusTip(tr("Rotate"));

    toolScale = new QAction(QIcon(":/scale.png"),tr("&Scale"), this);
    toolScale->setStatusTip(tr("Scale"));

    toolOffset = new QAction(QIcon(":/offset.png"),tr("&Offset"), this);
    toolOffset->setStatusTip(tr("Offset"));

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
    fileMenu->addAction(print);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undo);
    editMenu->addAction(redo);
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
    _mSketchMenu->addAction(_mSketch);
    lines = drawMenu->addMenu(tr("Lines"));
    lines->addAction(line);
    arcs = drawMenu->addMenu(tr("Arcs"));
    arcs->addAction(arc);
    shapes = drawMenu->addMenu(tr("Shapes"));
    shapes->addAction(rectangle);

    cameraMenu = menuBar()->addMenu(tr("&Camera"));
    cameraMenu->addAction(previousCam);
    cameraMenu->addSeparator();
    cameraMenu->addAction(_mOrbit);
    cameraMenu->addAction(_mPan);
    cameraMenu->addAction(_mZoom);
    cameraMenu->addAction(zoomWindow);
    cameraMenu->addAction(zoomExtents);


    toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(_mSelect);
    toolsMenu->addAction(_mEraser);
    toolsMenu->addSeparator();
    toolsMenu->addAction(toolMove);
    toolsMenu->addAction(toolRotate);
    toolsMenu->addAction(toolScale);
    toolsMenu->addSeparator();
    toolsMenu->addAction(toolOffset);

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
    fileToolBar->addAction(saveAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);

    toolsToolBar = addToolBar(tr("Tools"));
    toolsToolBar->addAction(_mSelect);
    toolsToolBar->addAction(_mEraser);
    toolsToolBar->addAction(toolOffset);
    toolsToolBar->addAction(toolMove);
    toolsToolBar->addAction(toolRotate);
    toolsToolBar->addAction(toolScale);

    drawToolBar = addToolBar(tr("Draw"));
    drawToolBar->addAction(_mSketch);
    drawToolBar->addAction(line);
    drawToolBar->addAction(arc);
    drawToolBar->addAction(rectangle);

    cameraToolBar = addToolBar(tr("Camera"));
    cameraToolBar->addAction(_mOrbit);
    cameraToolBar->addAction(_mPan);
    cameraToolBar->addAction(_mZoom);
    cameraToolBar->addAction(zoomExtents);

    styleToolBar = addToolBar(tr("Style"));
    styleToolBar->addAction(allStyle);
    styleToolBar->addAction(strokesStyle);
    styleToolBar->addAction(imagesStyle);

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
    QDockWidget *dock = new QDockWidget(tr("Layers"), this);
    dock->setAllowedAreas(Qt::RightDockWidgetArea);

    m_TableHeader<<"Name"<<"Visible"<<"Color";

    m_pTableWidget = new QTableWidget(dock);
    m_pTableWidget->setColumnCount(3);
    m_pTableWidget->setRowCount(2);
    m_pTableWidget->horizontalHeader()->setSectionsClickable(false);
    m_pTableWidget->setHorizontalHeaderLabels(m_TableHeader);
    m_pTableWidget->horizontalHeader()->setStretchLastSection(true); //extend to the full width
    m_pTableWidget->horizontalHeader()->setFixedHeight(30);

    m_pTableWidget->verticalHeader()->setDefaultSectionSize(30); //height of each line
    m_pTableWidget->setFrameShape(QFrame::NoFrame); //no frame
    m_pTableWidget->setShowGrid(false); //no grid
    m_pTableWidget->verticalHeader()->setVisible(false);


    m_pTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_pTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pTableWidget->setStyleSheet("QTableView {selection-background-color: lightgrey;}");
    m_pTableWidget->setGeometry(QApplication::desktop()->screenGeometry());



    //replace this part with a loop using parameters passed in
    //insert data
    //use it in a loop to insert new rows
    //int row_count = m_pTableWidget->rowCount(); //get the number of rows
    //m_pTableWidget->insertRow(row_count); //insert a new row
    QTableWidgetItem* item0 = new QTableWidgetItem("canvas 1");
    item0->setTextAlignment( Qt::AlignCenter );
    m_pTableWidget->setItem(0, 0, item0);//row,column,item
    QTableWidgetItem* item2 = new QTableWidgetItem("Red");
    item2->setTextAlignment( Qt::AlignCenter );
    m_pTableWidget->setItem(0, 2, item2);

    //checkBox  signals:   stateChanged()  isChecked()
    //http://doc.qt.io/qt-5/qcheckbox.html
    QWidget *item1 = new QWidget();
    QCheckBox *pCheckBox = new QCheckBox();
    QHBoxLayout *pLayout = new QHBoxLayout(item1);
    pLayout->addWidget(pCheckBox);
    pLayout->setAlignment(Qt::AlignCenter);
    pLayout->setContentsMargins(0,0,0,0);
    item1->setLayout(pLayout);
    m_pTableWidget->setCellWidget(0,1,item1);//row,col,item
    //connect(pCheckBox, SIGNAL(isChecked()), this, SLOT(myChecked(int 1)));//row1

    m_pTableWidget->resizeColumnsToContents();
    m_pTableWidget->resizeRowsToContents();
    //single click signal
    //connect(m_pTableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(myCellClicked(int, int)));//row,col
    dock->setWidget(m_pTableWidget);
    addDockWidget(Qt::RightDockWidgetArea, dock);
}
