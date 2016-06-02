#ifndef BASETEST_H
#define BASETEST_H

#include <QObject>
#include <QTest>
#include <QtGlobal>

#include "MainWindow.h"
#include "Settings.h"

//#define CHERTEST_MAIN(TestObject) \
//    QT_BEGIN_NAMESPACE \
//    QTEST_ADD_GPU_BLACKLIST_SUPPORT_DEFS \
//    QT_END_NAMESPACE \
//    int main(int argc, char *argv[]) \
//{ \
//    CherishApplication app(argc, argv); \
//    app.setAttribute(Qt::AA_Use96Dpi, true); \
//    QTEST_DISABLE_KEYPAD_NAVIGATION \
//    QTEST_ADD_GPU_BLACKLIST_SUPPORT \
//    TestObject tc; \
//    QTEST_SET_MAIN_SOURCE_PATH \
//    return QTest::qExec(&tc, argc, argv); \
//}

class BaseTest : public QObject
{
    Q_OBJECT
public:
    explicit BaseTest(QObject *parent = 0);

private slots:

protected:
    MainWindow m_window;
};

#endif // BASETEST_H
