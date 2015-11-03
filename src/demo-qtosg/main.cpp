#include <iostream>

#include <QObject>
#include <QApplication>
#include "tabletapplication.h"
#include "mainwindow.h"


int main(int argc, char** argv){
    std::cout << "Demo window that uses Qt and OpenSceneGraph together" << std::endl;
    //QApplication qapp(argc, argv);
    TabletApplication tapp(argc, argv);

    MainWindow mwin;
    QObject::connect(&tapp, SIGNAL(sendTabletDevice(bool)),
                     &mwin, SLOT(getTabletDevice(bool)));
    mwin.show();

    return tapp.exec();
}
