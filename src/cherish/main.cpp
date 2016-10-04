#include <iostream>
#include <algorithm>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#include <QObject>
#include <QDesktopWidget>
#include <QtGlobal>

#include "CherishApplication.h"
#include "MainWindow.h"
#include "CMainWindow.h"
#include "Settings.h"
#include "Data.h"


/*! \mainpage Cultural Heritage data representation by means of image manipualtion and 3D sketching
 *
 * \section Description
 *
 * This documentation site covers both API and user documentation.
 *
 * For the API, go to "Classes".
 *
 * For user documentation files, go to "Related Pages".
 *
 * The codes are separated into different folders: cherish, libGUI, libSGControls, libSGEntities and tests.
 *
 * "cherish" folder contains main.cpp, as well as settings, utility functions and image data.
 *
 * "libGUI" contains all the elements related to user interface and its elements.
 *
 * "libSGControls" contains codes such as event handling, manipulators and anything related to undo/redo framework.
 *
 * "libSGEntities" contains geometry-based and group-based OpenSceneGraph elements that are elements of a scene graph.
 *
 * "tests" contains batteries of different basic unit tests.
 *
 */

int main(int argc, char** argv)
{
    qInfo("Starting cherish: 3d representation of historical sites...");

#ifdef Q_OS_WIN
    /* use automatic scaling */
    SetProcessDPIAware();
#else
    /* try to use the hand */
#endif // Q_OS_WIN


#if QT_VERSION >= QT_VERSION_CHECK(5,6,0)
    /* For more info see: http://doc.qt.io/qt-5/highdpi.html */
    QApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
#else
    qputenv("QT_DEVICE_PIXEL_RATIO", QByteArray("1"));
#endif // QT_VERSION

    /* application */
    CherishApplication cherish_app(argc, argv);
    cherish_app.setWindowIcon(Data::appIcon());

    /* main window */
    MainWindow mwin;
    mwin.move(0,0); // if multi-monitor system, always open in main display
    mwin.setWindowState(Qt::WindowMaximized);
    QObject::connect(&cherish_app, SIGNAL(setTabletActivity(bool)), &mwin, SLOT(onSetTabletActivity(bool)));
    mwin.show();

    return cherish_app.exec();
}

