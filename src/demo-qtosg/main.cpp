#include <iostream>

#include "tabletapplication.h"
#include "mainwindow.h"
//#include "osgwidget.h"

int main(int argc, char** argv)
{
    std::cout << "Demo window that uses Qt and OpenSceneGraph together" << std::endl;
    TabletApplication tapp(argc, argv);

    OSGWidget osgwid = new OSGWidget;
    MainWindow mwin;
    mwin.show();
    //tapp.setOSGWidget(osgwid);

    return tapp.exec();
}

