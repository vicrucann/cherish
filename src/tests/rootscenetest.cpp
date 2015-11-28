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

void RootSceneTest::addDeleteCanvas(){
    std::cout << "TEST: add default +Canvas3" << std::endl;
    Canvas* cnv = _RS->addCanvas();
    QVERIFY(_RS->getMaxCanvasId() == 4);
    QVERIFY(cnv->getName() == "Canvas3");
    QVERIFY(cnv->getVisibility() == 1);
    QVERIFY(cnv->getTransform()->getMatrix().isIdentity());
    QVERIFY(cnv == _RS->getCanvasCurrent());
    QVERIFY(_RS->getCanvas(3) == _RS->getCanvasCurrent());
    QVERIFY(_RS->getCanvas(0) == _RS->getCanvasPrevious());
    assertConstants();

    std::cout << "TEST: delete by name -Canvas3" << std::endl;
    QVERIFY(_RS->deleteCanvas("Canvas3"));
    QVERIFY(_RS->getCanvas(0) == _RS->getCanvasCurrent());
    QVERIFY(_RS->getCanvas(1) == _RS->getCanvasPrevious());
    assertConstants();

    std::cout << "TEST: add as a copy +Canvas4" << std::endl;
    _RS->addCanvas(_RS->getCanvas(0));
    QVERIFY(_RS->getMaxCanvasId() == 5);
    QVERIFY(_RS->getCanvas(4) == _RS->getCanvasCurrent());
    QVERIFY(_RS->getCanvas(0) == _RS->getCanvasPrevious());
    assertConstants();

    std::cout << "TEST: setting current and previous to Canvas0 and Canvas1"<<std::endl;
    QVERIFY(_RS->setCanvasCurrent(_RS->getCanvas(0)));
    QVERIFY(_RS->getCanvas(0) == _RS->getCanvasCurrent());
    QVERIFY(_RS->getCanvas(4) == _RS->getCanvasPrevious());
    QVERIFY(_RS->setCanvasPrevious(_RS->getCanvas(1)));
    QVERIFY(_RS->getCanvas(1) == _RS->getCanvasPrevious());
    QVERIFY(_RS->getCanvas(0) == _RS->getCanvasCurrent());
    assertConstants();

    std::cout << "TEST: delete by id, delete current -Canvas0" << std::endl;
    QVERIFY(_RS->deleteCanvas(0));
    QVERIFY(_RS->getCanvas(1) == _RS->getCanvasCurrent());
    QVERIFY(_RS->getCanvas(2) == _RS->getCanvasPrevious());
    assertConstants();

    // delete by pointer, delete previous
}

void RootSceneTest::setNameUserScene()
{
    // edit the name of user scene
    // or impossibility to edit it
    // since it will not be displayed anyway
}

void RootSceneTest::assertConstants(){
    QVERIFY(_RS->getCanvasCurrent() != NULL);
    QVERIFY(_RS->getCanvasPrevious() != NULL);
    QVERIFY(_RS->getCanvasCurrent() != _RS->getCanvasPrevious());
    QVERIFY(_RS->getCanvasCurrent()->getColor() == dureu::CANVAS_CLR_CURRENT);
    QVERIFY(_RS->getCanvasPrevious()->getColor() == dureu::CANVAS_CLR_PREVIOUS);

    QVERIFY(_RS->getNumChildren() == 3);
    QVERIFY(_RS->getNameUserScene() == "UserScene");
}

QTEST_MAIN(RootSceneTest)
#include "rootscenetest.moc"
