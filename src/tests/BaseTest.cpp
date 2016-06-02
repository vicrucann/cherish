#include "BaseTest.h"
#include "RootScene.h"
#include "GLWidget.h"

BaseTest::BaseTest(QObject *parent)
    : QObject(parent)
//    , m_window(new MainWindow)
{
}

void BaseTest::init()
{
//    m_window->onNewCanvasStandard();
}

void BaseTest::cleanup()
{
//    m_window->getRootScene()->setSavedToFile(true);
    //    m_window->onFileNew();
}

void BaseTest::trial()
{
    qDebug() << "Start trial test";
    MainWindow window;
//    window.onNewCanvasStandard();
//    window.getRootScene()->setSavedToFile(true);
//    window.onFileNew();
}

QTEST_MAIN(BaseTest)
#include "BaseTest.moc"
