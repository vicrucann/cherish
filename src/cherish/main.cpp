#include <iostream>
#include <algorithm>

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

namespace cher{
double DPI_SCALING;
}

void setScalingGuiMacro()
{
    int dpiX = QApplication::desktop()->physicalDpiX();
    int dpiY = QApplication::desktop()->physicalDpiY();
    int dpi = std::min(dpiX, dpiY);

    double scale = 1.;
    if (dpi <= 96)                  scale = 1.;
    else if (dpi>96 && dpi<=120)    scale = 1.25;
    else if (dpi>120 && dpi<=144)   scale = 1.5;
    else if (dpi>144 && dpi<=192)   scale = 2.0;
    else if (dpi>192 && dpi<=240)   scale = 2.5;
    else if (dpi>240 && dpi<=288)   scale = 3.0;
    else if (dpi>288 && dpi<=384)   scale = 4.0;
    else if (dpi>383 && dpi<=480)   scale = 5.0;
    else if (dpi>480)               scale = 6.0;

    cher::DPI_SCALING = scale;
}

int main(int argc, char** argv)
{
    outLogMsg("Starting cherish: 3d representation of historical sites...");

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
    setScalingGuiMacro();
    cherish_app.setWindowIcon(Data::appIcon());
    QDesktopWidget* desktop = cherish_app.desktop();
    MainWindow mwin;
    mwin.SetDesktopWidget(desktop, cher::SCREEN_MAX); // pass the desktop geometry and configuration
    QObject::connect(&cherish_app, SIGNAL(sendTabletActivity(bool)), &mwin, SLOT(getTabletActivity(bool)));
    mwin.show();

    return cherish_app.exec();
}

