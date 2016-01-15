#include "rootscenetest.h"
#include "Canvas.h"
#include "settings.h"

#include <QtTest>

RootSceneTest::RootSceneTest()
    //:
    //_RS(new RootScene)
{
}

void RootSceneTest::initialValues(){
    assertConstants();
    QVERIFY(_RS->getNumChildren() == 3);
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

}

void RootSceneTest::assertConstants(){
   // QVERIFY(_RS->getCanvasCurrent()->getColor() == dureu::CANVAS_CLR_CURRENT);
    //QVERIFY(_RS->getCanvasPrevious()->getColor() == dureu::CANVAS_CLR_PREVIOUS);

    QVERIFY(_RS->getName() == "RootScene");
    QVERIFY(_RS->getNumChildren() == 3);
    QVERIFY(_RS->getAxesVisibility());
    QVERIFY(_RS->getAxes() == _RS->getChild(1));
    QVERIFY(_RS->getHudCamera()->getName() == "HUDCamera");
    QVERIFY(_RS->getHudCameraVisibility());
}

QTEST_MAIN(RootSceneTest)
#include "rootscenetest.moc"
