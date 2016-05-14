#include <iostream>
#include "DureuApplication.h"
#include "MainWindow.h"
#include "Settings.h"
#include "Data.h"
#include <QObject>
#include <QDesktopWidget>

/*! \mainpage Cultural Heritage data representation by means of image manipualtion and 3D sketching
 *
 * \section Description
 *
 * This is the API documentation site.
 *
 * For user documentation, go this link
 *
 */

int main(int argc, char** argv)
{
    osg::setNotifyLevel(osg::NOTICE);
    noticeMsg("Dura-Europus In Situ: 3D reconstruction of historical sites.");
    noticeMsg("Starting application...");

    DureuApplication dura_app(argc, argv);
    dura_app.setWindowIcon(Data::appIcon());
    QDesktopWidget* desktop = dura_app.desktop();
    MainWindow mwin;
    mwin.SetDesktopWidget(desktop, cher::SCREEN_MIN); // pass the desktop geometry and configuration
    QObject::connect(&dura_app, SIGNAL(sendTabletActivity(bool)),
                     &mwin, SLOT(getTabletActivity(bool)));
    mwin.show();

    return dura_app.exec();
}

