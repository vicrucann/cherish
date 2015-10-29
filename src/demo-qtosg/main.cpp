#include <iostream>

#include <QApplication>
//#include "tabletapplication.h"
#include "mainwindow.h"
//#include "osgwidget.h"

int main(int argc, char** argv)
{
    std::cout << "Demo window that uses Qt and OpenSceneGraph together" << std::endl;
    //TabletApplication tapp(argc, argv);
    QApplication qapp(argc, argv);

    MainWindow mwin;
    mwin.show();

    return qapp.exec();
}

