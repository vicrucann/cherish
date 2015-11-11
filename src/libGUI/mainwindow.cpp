#include <QMdiSubWindow>
#include <QMenuBar>
#include <QObject>

#include "mainwindow.h"
#include "viewwidget.h"
#include "settings.h"

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags) :
    QMainWindow(parent, flags),
    _mdiArea(new QMdiArea(this)),
    _tabletActive(false),
    _rootScene(new RootScene())
{
    QMenuBar* menuBar = this->menuBar();
    QMenu* menu = menuBar->addMenu("Test");

    menu->addAction("Add scene viewer", this, SLOT(onCreateViewer()));

    this->setCentralWidget(_mdiArea);
}

MainWindow::~MainWindow(){}

void MainWindow::getTabletActivity(bool active){
    _tabletActive = active;
    emit sendTabletActivity(active);
}

/* Create an ordinary single view window on the scene _root */
void MainWindow::onCreateViewer(){    
    ViewWidget* vwid = new ViewWidget(_rootScene, this);
    QObject::connect(this, SIGNAL(sendTabletActivity(bool)),
                     vwid, SLOT(getTabletActivity(bool)));
    QMdiSubWindow* subwin = _mdiArea->addSubWindow(vwid);
    subwin->show();
}

