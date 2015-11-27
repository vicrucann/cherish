#include "rootscenetest.h"
#include "canvas.h"
#include "settings.h"

#include <QtTest>

RootSceneTest::RootSceneTest():
    _RS(new RootScene)
{
}

void RootSceneTest::initialValues(){
    QVERIFY(_RS->getNameUserScene() == "UserScene");
    // initial number of canvases
    QVERIFY(_RS->getMaxCanvasId() == 3);
    // direct children: axes, hud and userScene
    QVERIFY(_RS->getNumChildren() == 3);
    QVERIFY(_RS->getCanvasCurrent() == _RS->getCanvas(0));
    QVERIFY(_RS->getCanvasPrevious() == _RS->getCanvas(1));
}

void RootSceneTest::addDeleteCanvas(){
    // add default
    Canvas* cnv = _RS->addCanvas();
    QVERIFY(_RS->getMaxCanvasId() == 4);
    QVERIFY(_RS->getNumChildren() == 3);
    QVERIFY(cnv->getName() == "Canvas3");
    QVERIFY(cnv->getColor() == dureu::CANVAS_CLR_CURRENT);
    QVERIFY(cnv->getVisibility() == 1);
    QVERIFY(cnv->getTransform()->getMatrix().isIdentity());

    // delete by name
    QVERIFY(_RS->deleteCanvas("Canvas3"));

    // add as a copy
    cnv = _RS->addCanvas(_RS->getCanvas(0));
    QVERIFY(cnv->getColor() == dureu::CANVAS_CLR_CURRENT);
    QVERIFY(_RS->getCanvas(0)->getColor() == dureu::CANVAS_CLR_PREVIOUS);
    QVERIFY(cnv->getName() == "Canvas4");
    QVERIFY(_RS->getMaxCanvasId() == 5);
    QVERIFY(_RS->getNumChildren() == 3);

    // delete by id
    QVERIFY(_RS->deleteCanvas(4));
}

void RootSceneTest::setNameUserScene()
{
    // edit the name of user scene
    // or impossibility to edit it
    // since it will not be displayed anyway
}

QTEST_MAIN(RootSceneTest)
#include "rootscenetest.moc"
