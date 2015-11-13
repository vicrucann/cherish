#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    int mode = 0;
    QDesktopWidget* d = a.desktop();

    MainWindow w;
    w.setDesktopWidget(d, mode);
    w.show();

    return a.exec();
}
