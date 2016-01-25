#include <iostream>
#include "DureuApplication.h"
#include "MainWindow.h"
#include "settings.h"
#include <QObject>
#include <QDesktopWidget>

int main(int argc, char** argv)
{
    osg::setNotifyLevel(osg::NOTICE);
    noticeMsg("Dura-Europus In Situ: 3D reconstruction of historical sites.");
    noticeMsg("Starting application...");

    DureuApplication dura_app(argc, argv);
    QDesktopWidget* desktop = dura_app.desktop();
    MainWindow mwin;
    mwin.SetDesktopWidget(desktop, dureu::SCREEN_MIN); // pass the desktop geometry and configuration
    QObject::connect(&dura_app, SIGNAL(sendTabletActivity(bool)),
                     &mwin, SLOT(getTabletActivity(bool)));
    mwin.show();

    return dura_app.exec();
}

