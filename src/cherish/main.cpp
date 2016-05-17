#include <iostream>
#include "CherishApplication.h"
#include "MainWindow.h"
#include "Settings.h"
#include "Data.h"
#include <QObject>
#include <QDesktopWidget>

/*! \mainpage Cultural Heritage data representation by means of image manipualtion and 3D sketching
 *
 * \section Description
 *
 * This documentation site convers both API and user documentation.
 *
 * For the API, go to "Classes".
 *
 * For user documentation files, go to "Related Pages".
 *
 */

int main(int argc, char** argv)
{
    outLogMsg("Starting cherish: 3d representation of historical sites...");

    CherishApplication cherish_app(argc, argv);
    cherish_app.setWindowIcon(Data::appIcon());
    QDesktopWidget* desktop = cherish_app.desktop();
    MainWindow mwin;
    mwin.SetDesktopWidget(desktop, cher::SCREEN_MIN); // pass the desktop geometry and configuration
    QObject::connect(&cherish_app, SIGNAL(sendTabletActivity(bool)),
                     &mwin, SLOT(getTabletActivity(bool)));
    mwin.show();

    return cherish_app.exec();
}

