#include <assert.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QRect>
#include <QSize>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _mode(1)
{
    ui->setupUi(this);
    this->DesktopSetup();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::DesktopSetup(){
    /// Size, num of screens and whether they are configured as separate
    /// or single virtual desktops.
    /// Use this variable to place tooltips, menus and dialog boxes on the correct
    /// screen for their parent or app widgets.
    /// Use this class within app to obtain info: window positions, placement of child
    /// widget and dialogs on a particular screen.
    QRect mainSS = _desktop->availableGeometry(_desktop->primaryScreen());
    QRect SS = _desktop->geometry();
    int nscreen = _desktop->screenCount();
    double scale = 0.9;
    double scale_inv = 1-scale;
    if (_mode == 0){
        this->resize(QSize(mainSS.width()*scale, mainSS.height()*scale));
        this->move(mainSS.width()*scale_inv, SS.height()-mainSS.height());
    }
}

void MainWindow::setDesktopWidget(QDesktopWidget *desktop, int mode){
    _desktop = desktop;
    _mode = mode;
    this->DesktopSetup();
}
