#include "rootscenetest.h"
#include "canvas.h"
#include "settings.h"

#include <QtTest>

RootSceneTest::RootSceneTest():
    _RS(new RootScene)
{
}

void RootSceneTest::initialValues(){
    assertConstants();
    QVERIFY(_RS->getNumChildren() == 3);
    QVERIFY(_RS->getCanvasCurrent() == _RS->getCanvas(0));
    QVERIFY(_RS->getCanvasPrevious() == _RS->getCanvas(1));
}

void RootSceneTest::userSceneSetGet()
{

}

void RootSceneTest::axesVisibility(){
    QVERIFY(_RS->getAxesVisibility());
    _RS->setAxesVisibility(false);
    QVERIFY(!_RS->getAxesVisibility());
    _RS->setAxesVisibility(true);
    assertConstants();
}

void RootSceneTest::axesSetGet()
{

}

void RootSceneTest::observerGet()
{
    QVERIFY(_RS->getSceneObserver()->getName() == "SceneObserver");
    assertConstants();
}

void RootSceneTest::hudGetSet()
{

}

void RootSceneTest::addDeleteCanvas(){

}

void RootSceneTest::addDeleteNode()
{

}

void RootSceneTest::renameCanvas()
{

}

void RootSceneTest::renameNode() {

}

void RootSceneTest::setNameUserScene(){
    _RS->setNameUserScene("SceneMy");
    QVERIFY(_RS->getNameUserScene() == "SceneMy");
    _RS->setNameUserScene("UserScene");
    assertConstants();
}

void RootSceneTest::assertConstants(){
    QVERIFY(_RS->getCanvasCurrent() != NULL);
    QVERIFY(_RS->getCanvasPrevious() != NULL);
    QVERIFY(_RS->getCanvasCurrent() != _RS->getCanvasPrevious());
    QVERIFY(_RS->getCanvasCurrent()->getColor() == dureu::CANVAS_CLR_CURRENT);
    QVERIFY(_RS->getCanvasPrevious()->getColor() == dureu::CANVAS_CLR_PREVIOUS);

    QVERIFY(_RS->getName() == "RootScene");
    QVERIFY(_RS->getNumChildren() == 3);
    QVERIFY(_RS->getNameUserScene() == "UserScene");
    QVERIFY(_RS->getUserScene() == _RS->getChild(0));
    QVERIFY(_RS->getAxesVisibility());
    QVERIFY(_RS->getAxes() == _RS->getChild(1));
    QVERIFY(_RS->getHudCamera()->getName() == "HUDCamera");
    QVERIFY(_RS->getHudCameraVisibility());
}

QTEST_MAIN(RootSceneTest)
#include "rootscenetest.moc"
