#ifndef BASEGUITEST_H
#define BASEGUITEST_H

#include <QObject>
#include <QTest>
#include <QtGlobal>
#include <QScopedPointer>

#include "MainWindow.h"
#include "CherishApplication.h"
#include "Settings.h"

class MainWindow;

/*! \class BaseGuiTest
 * \brief This class is used for all testing that involved using MainWindow, e.g., signal/slots,
 * composite widget functionality such as CanvasPhotoWidget and corresponding scene graph data.
 *
 * Inherit this class when want to test functionalities of MainWindow. For every test case,
 * private slots BaseTest::init() and BaseTest::cleanup() are performed before and after the
 * test. For more info on this, refer to Qt documentation: http://doc.qt.io/qt-4.8/qtestlib-manual.html .
*/
class BaseGuiTest : public MainWindow
{
    Q_OBJECT
public:
    explicit BaseGuiTest(QWidget *parent = 0);

private slots:
    /*! Slot is called before each test functions is executed. */
    void init();

    /*! Slot is called after every test function is executed. */
    void cleanup();
};

#endif // BASETEST_H
