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


/*! \mainpage Cherish - Tool for Cultural Heritage Data Representation Image and Sketching Manipulation.
 *
 * \section Description
 *
 * This documentation site covers the cherish API mainly.
 *
 * To start with API, you may browse the content and hierarchy of `Classes`. The codes are separated into different folders: `cherish`, `libGUI`, `libSGControls`,
 * `libSGEntities`, `libNumerics` and `tests`.
 *
 * * `cherish` folder contains `main.cpp`, as well as settings, utility functions, shader and image data.
 * * `libGUI` contains classes related to user interface, widgets and its content.
 * * `libSGControls` is responsible for event handling, mouse manipulators,anything related to undo/redo framework, shader programs, geometry intersectors, callbacks.
 * * `libSGEntities` contains geometry-based and group-based OpenSceneGraph elements that are elements of a scene graph.
 * * `libNumerics` contain outside numerical libraries and codes, e.g. curve fitting algorithm.
 * * `tests` contains batteries of different basic unit tests.
 *
 * Some other useful pages:
 * * [Cherish scene graph](https://codedocs.xyz/vicrucann/cherish/md_docs_scene.html)
 * * [Cherish README](https://codedocs.xyz/vicrucann/cherish/md_README.html)
 * * [Cherish user site](http://vicrucann.github.io/cherish/) (is not up to date currently)
 * * [Cherish license file](https://github.com/vicrucann/cherish/blob/develop/LICENSE)
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

    /* if file is being opened by providing it as an argument */
    if (argc == 2){
        qInfo("Trying to open the passed file...");
        std::string s(argv[1]);
        QString fname(s.c_str());
        qInfo() << "File=" << fname;
        mwin.openFile(fname);
    }

    return cherish_app.exec();
}

