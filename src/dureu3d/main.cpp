#include <iostream>
#include "dureuapplication.h"
#include "mainwindow.h"
#include "settings.h"
#include <QObject>
#include <QDesktopWidget>

int main(int argc, char** argv)
{
    osg::notify(osg::NOTICE) << "Dura-Europus In Situ: 3D reconstruction of historical sites." << std::endl;
    osg::notify(osg::NOTICE) << "Starting application..." << std::endl;

    DureuApplication dura_app(argc, argv);
    QDesktopWidget* desktop = dura_app.desktop();
    MainWindow mwin;
    mwin.SetDesktopWidget(desktop, dureu::SCREEN_MIN); // pass the desktop geometry and configuration
    QObject::connect(&dura_app, SIGNAL(sendTabletActivity(bool)),
                     &mwin, SLOT(getTabletActivity(bool)));
    mwin.show();

    return dura_app.exec();
}

