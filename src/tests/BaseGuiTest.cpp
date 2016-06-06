#include "BaseGuiTest.h"
#include "RootScene.h"
#include "GLWidget.h"
#include "RootScene.h"

BaseGuiTest::BaseGuiTest(QWidget *parent)
    : MainWindow(parent)
{
}

void BaseGuiTest::init()
{
    this->onNewCanvasStandard();
    QVERIFY(m_rootScene.get());
    QVERIFY(m_rootScene->getUserScene());
    QVERIFY(m_rootScene->getUserScene()->getNumCanvases() == 3);
}

void BaseGuiTest::cleanup()
{
    this->getRootScene()->setSavedToFile(true);
    this->onFileNew();
    QVERIFY(m_rootScene.get());
    QVERIFY(m_rootScene->getUserScene());
    QVERIFY(m_rootScene->getUserScene()->getNumCanvases() == 0);
}
