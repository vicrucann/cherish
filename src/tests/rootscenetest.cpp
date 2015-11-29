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

    std::cout << "TEST: delete by pointer, delete previous, -Canvas2" << std::endl;
    cnv = _RS->getCanvas(2);
    QVERIFY(_RS->deleteCanvas(cnv));
    QVERIFY(_RS->getCanvas(1) == _RS->getCanvasCurrent());
    QVERIFY(_RS->getCanvas(4) == _RS->getCanvasPrevious());
    assertConstants();

    std::cout << "TEST: delete current, one previous left, -Canvas1" << std::endl;
    QVERIFY(_RS->deleteCanvas(1));
    QVERIFY(_RS->getCanvas(4) == _RS->getCanvasCurrent());
    QVERIFY(NULL == _RS->getCanvasPrevious());

    std::cout << "TEST: delete last current, -Canvas4" << std::endl;
    QVERIFY(_RS->deleteCanvas(4));
    QVERIFY(NULL == _RS->getCanvasCurrent());
    QVERIFY(NULL == _RS->getCanvasPrevious());

    std::cout << "TEST: create two new canvases, +Canvas5, +Canvas6" << std::endl;
    QVERIFY(_RS->addCanvas());
    QVERIFY(_RS->getCanvas(5) == _RS->getCanvasCurrent());
    QVERIFY(NULL == _RS->getCanvasPrevious());
    QVERIFY(_RS->addCanvas());
    QVERIFY(_RS->getCanvas(5) == _RS->getCanvasPrevious());
    QVERIFY(_RS->getCanvas(6) == _RS->getCanvasCurrent());
    assertConstants();

    std::cout << "TEST: delete previous, last current, -Canvas5" << std::endl;
    QVERIFY(_RS->deleteCanvas(5));
    QVERIFY(_RS->getCanvas(6) == _RS->getCanvasCurrent());
    QVERIFY(NULL == _RS->getCanvasPrevious());
}

void RootSceneTest::addDeleteNode()
{
    std::cout << "TEST: load cow model" << std::endl;
    osg::Node* node = _RS->loadSceneFromFile("../../samples/cow.osgt");
    QVERIFY(_RS->getNode("Entity0") == node);
    QVERIFY(_RS->getNode(0) == node);

    std::cout << "TEST: delete cow mode" << std::endl;
    QVERIFY(_RS->deleteNode("Entity0"));
    QVERIFY(_RS->getMaxNodeId() == 1);
}

void RootSceneTest::renameCanvas()
{
    QVERIFY(_RS->addCanvas());
    QVERIFY(_RS->getCanvas(7) == _RS->getCanvasCurrent());
    QVERIFY(_RS->getCanvas(6) == _RS->getCanvasPrevious());

    QVERIFY(_RS->setCanvasName(_RS->getCanvasCurrent(), "CanvasMy"));
    QVERIFY(_RS->getCanvasCurrent()->getName() == "CanvasMy");
    QVERIFY(_RS->getCanvas("CanvasMy") == _RS->getCanvasCurrent());
    assertConstants();

    QVERIFY(_RS->setCanvasName(_RS->getCanvasCurrent(), "Canvas7"));
    QVERIFY(_RS->getCanvasCurrent()->getName() == "Canvas7");
    QVERIFY(_RS->getCanvas("Canvas7") == _RS->getCanvasCurrent());
    assertConstants();

    QVERIFY(!_RS->setCanvasName(_RS->getCanvasCurrent(), "Canvas6"));
    assertConstants();
}

void RootSceneTest::renameNode() {
    osg::Node* node = _RS->loadSceneFromFile("../../samples/cow.osgt");
    QVERIFY(_RS->getNode("Entity1") == node);
    QVERIFY(_RS->getNode(1) == node);

    QVERIFY(_RS->setNodeName(node, "EntityMy"));
    QVERIFY(node->getName() == "EntityMy");

    QVERIFY(_RS->deleteNode(node));
}

void RootSceneTest::setNameUserScene(){
    _RS->setName("SceneMy");
    QVERIFY(_RS->getName() == "SceneMy");
    _RS->setName("UserScene");
    assertConstants();
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
