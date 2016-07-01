#include "BaseGuiTest.h"
#include "RootScene.h"
#include "GLWidget.h"
#include "RootScene.h"
#include "ViewerCommand.h"
#include "Manipulator.h"
#include "EventHandler.h"

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
    m_rootScene->setAxesVisibility(true);
    m_rootScene->setBookmarkToolVisibility(true);

    m_scene = m_rootScene->getUserScene();
    QVERIFY(m_scene.get());
    m_canvas0 = m_scene->getCanvas(0);
    QVERIFY(m_canvas0.get());
    QCOMPARE(m_canvas0->getName().c_str(), "Canvas0");
    m_canvas1 = m_scene->getCanvas(1);
    QVERIFY(m_canvas1.get());
    QCOMPARE(m_canvas1->getName().c_str(), "Canvas1");
    m_canvas2 = m_scene->getCanvas(2);
    QVERIFY(m_canvas2.get());
    QCOMPARE(m_canvas2->getName().c_str(), "Canvas2");
}

void BaseGuiTest::cleanup()
{
    m_rootScene->setSavedToFile(true);
    this->onFileNew();
    QVERIFY(m_rootScene.get());
    QVERIFY(m_rootScene->getUserScene());
    QVERIFY(m_rootScene->getUserScene()->getNumCanvases() == 0);
}
