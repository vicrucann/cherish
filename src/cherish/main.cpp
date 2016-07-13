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
#include "Settings.h"
#include "Data.h"


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

    CherishApplication cherish_app(argc, argv);
    cherish_app.setWindowIcon(Data::appIcon());
    MainWindow mwin;
    mwin.setWindowState(Qt::WindowMaximized);
    QObject::connect(&cherish_app, SIGNAL(setTabletActivity(bool)), &mwin, SLOT(onSetTabletActivity(bool)));
    mwin.show();

    return cherish_app.exec();
}

