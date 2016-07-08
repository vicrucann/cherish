#include "CanvasTest.h"

#include <math.h>

void CanvasTest::testBasicApi()
{
    qInfo("Test canvas basic api - create new canvas");
    osg::ref_ptr<entity::Canvas> canvas  = new entity::Canvas();
    QVERIFY(canvas);
    qInfo("Test canvas initial parametmers");
    QVERIFY(differenceWithinThreshold(canvas->getCenter(), cher::CENTER));
    QVERIFY(differenceWithinThreshold(canvas->getNormal(), cher::NORMAL));
    QVERIFY(differenceWithinThreshold(canvas->getGlobalAxisU(), osg::Vec3f(1,0,0)));
    QVERIFY(differenceWithinThreshold(canvas->getGlobalAxisV(), osg::Vec3f(0,1,0)));

    qInfo("Test canvas default nodes and their values");
    canvas->initializeSG();
    QCOMPARE(int(canvas->getNodeMask()), int(cher::MASK_CANVAS_IN));

    qInfo("Test matrix transform");
    const osg::MatrixTransform* mt = canvas->getTransform();
    QVERIFY(mt);
    QCOMPARE(mt->getName().c_str(), "Transform");
    QCOMPARE((int)canvas->getNumChildren(), 1);
    QCOMPARE(canvas->getChild(0), mt);

    qInfo("Test switch");
    const osg::Switch* switchC = canvas->getSwitch();
    QVERIFY(switchC);
    QCOMPARE(switchC->getName().c_str(), "Switch");
    QCOMPARE(mt->getChild(0), switchC);
    QCOMPARE((int)switchC->getNumChildren(), 2);

    qInfo("Test group data");
    const osg::Group* groupData = canvas->getGroupData();
    QVERIFY(groupData);
    QCOMPARE(int(groupData->getNodeMask()), int(cher::MASK_CANVASDATA_IN));
    QCOMPARE(switchC->getChild(0), groupData);
    QCOMPARE((int)groupData->getNumChildren(), 2);
    QVERIFY(switchC->getChildValue(groupData));

    qInfo("Test frame tools");
    QCOMPARE(canvas->getColor(), cher::CANVAS_CLR_REST);
    const entity::FrameTool* frameTool = canvas->getToolFrame();
    QVERIFY(frameTool);
    QCOMPARE(switchC->getChild(1), frameTool);
    QVERIFY(switchC->getChildValue(frameTool));

    qInfo("Test group data content");
    const osg::Geode* geodeStrokes = canvas->getGeodeStrokes();
    QVERIFY(geodeStrokes);
    QCOMPARE(groupData->getChild(0), geodeStrokes);
    const osg::Geode* geodePhotos = canvas->getGeodePhotos();
    QVERIFY(geodePhotos);
    QCOMPARE(groupData->getChild(1), geodePhotos);

    qInfo("Test color setting of frame tool");
    canvas->setColor(cher::CANVAS_CLR_CURRENT);
    QCOMPARE(canvas->getColor(), cher::CANVAS_CLR_CURRENT);
    QCOMPARE(frameTool->getColor(), cher::CANVAS_CLR_CURRENT);

    qInfo("Test visibility setting for frame tool");
    canvas->setVisibilityFrameInternal(false);
    QCOMPARE(frameTool->getVisibility(), false );
    QCOMPARE(canvas->getVisibilityFrameInternal(), false);
    QVERIFY(switchC->getChildValue(groupData));
    QVERIFY(switchC->getChildValue(frameTool));
    canvas->setVisibilityFrameInternal(true);
    QCOMPARE(frameTool->getVisibility(), true);
    QCOMPARE(canvas->getVisibilityFrameInternal(), true);

    qInfo("Test visibility setting for all the canvas content");
    canvas->setVisibilityAll(false);
    QCOMPARE(switchC->getChildValue(groupData), false);
    QCOMPARE(switchC->getChildValue(frameTool), false);
    QCOMPARE(canvas->getVisibilityAll(), false);
    QVERIFY(frameTool->getVisibility());
    canvas->setVisibilityAll(true);
    QCOMPARE(switchC->getChildValue(groupData), true);
    QCOMPARE(switchC->getChildValue(frameTool), true);
    QCOMPARE(canvas->getVisibilityAll(), true);
    QVERIFY(frameTool->getVisibility());

    qInfo("Test rotation and translation procedures");

    qInfo("+45 degree rotation along y axis");
    osg::Matrix rotMat = osg::Matrix::rotate(cher::PI/4, osg::Vec3f(0,1,0));
    canvas->rotate(rotMat, cher::CENTER);
    float len = 1.f * std::cos(cher::PI/4);
    QVERIFY(differenceWithinThreshold(canvas->getNormal(), osg::Vec3f(len,0,len)));
    QVERIFY(differenceWithinThreshold(canvas->getGlobalAxisU(), osg::Vec3f(len,0,-len)));
    QVERIFY(differenceWithinThreshold(canvas->getGlobalAxisV(), osg::Vec3f(0,1,0)));

    qInfo("translation along normal");
    osg::Matrix tMat = osg::Matrix::translate(1, 0, 1);
    canvas->translate(tMat);
    QVERIFY(differenceWithinThreshold(canvas->getNormal(), osg::Vec3f(len,0,len)));
    QVERIFY(differenceWithinThreshold(canvas->getGlobalAxisU(), osg::Vec3f(len+1, 0, -len+1)));
    QVERIFY(differenceWithinThreshold(canvas->getGlobalAxisV(), osg::Vec3f(1,1,1)));

    qInfo("+45 degree rotation alont x axis");
    osg::Matrix rotMat2 = osg::Matrix::rotate(cher::PI/4, osg::Vec3f(1,0,0));
    canvas->rotate(rotMat2, canvas->getBoundingBoxCenter3D());


}

void CanvasTest::testNewXY()
{
    qInfo("Test parameters of canvas created in XY plane");
    QCOMPARE(m_canvasWidget->topLevelItemCount(), 3);
    this->onNewCanvasXY();
    QCOMPARE(m_scene->getNumCanvases(), 4);
    QCOMPARE(m_canvasWidget->topLevelItemCount(), 4);
    entity::Canvas* cnvi = m_scene->getCanvas(3);
    QVERIFY(cnvi);

    qInfo("Test newly created canvas is current");
    QCOMPARE(m_scene->getCanvasCurrent(), cnvi);
    QCOMPARE(cnvi->getName().c_str(), "Canvas3");

    qInfo("Test normal and center parameters");
    QVERIFY(differenceWithinThreshold(cnvi->getCenter(), cher::CENTER));
    QVERIFY(differenceWithinThreshold(cnvi->getNormal(), cher::NORMAL));

    qInfo("Test undo functionality");
    m_undoStack->undo();
    QCOMPARE(m_scene->getNumCanvases(), 3);
    QCOMPARE(m_canvasWidget->topLevelItemCount(), 3);
    QCOMPARE(m_scene->getCanvasCurrent(), m_canvas2.get());

    qInfo("Test redo functionality");
    m_undoStack->redo();
    QCOMPARE(m_scene->getNumCanvases(), 4);
    QCOMPARE(m_canvasWidget->topLevelItemCount(), 4);
    cnvi = m_scene->getCanvas(3);
    QVERIFY(cnvi);
    qInfo("Test current/previous holds");
    QCOMPARE(m_scene->getCanvasCurrent(), cnvi);
    QCOMPARE(cnvi->getName().c_str(), "Canvas3");
    QCOMPARE(m_scene->getCanvasPrevious(), m_canvas2.get());
    qInfo("Test normal and center parameters");
    QVERIFY(differenceWithinThreshold(cnvi->getCenter(), cher::CENTER));
    QVERIFY(differenceWithinThreshold(cnvi->getNormal(), cher::NORMAL));
}

void CanvasTest::testNewYZ()
{
    qInfo("Test parameters of canvas created in YZ plane");
    QCOMPARE(m_canvasWidget->topLevelItemCount(), 3);
    this->onNewCanvasYZ();
    QCOMPARE(m_scene->getNumCanvases(), 4);
    QCOMPARE(m_canvasWidget->topLevelItemCount(), 4);
    entity::Canvas* cnvi = m_scene->getCanvas(3);
    QVERIFY(cnvi);

    qInfo("Test newly created canvas is current");
    QCOMPARE(m_scene->getCanvasCurrent(), cnvi);
    QCOMPARE(cnvi->getName().c_str(), "Canvas3");

    qInfo("Test normal and center parameters");
    QVERIFY(differenceWithinThreshold(cnvi->getCenter(), cher::CENTER));
    QVERIFY(differenceWithinThreshold(cnvi->getNormal(), osg::Vec3f(1.f, 0.f, 0.f)));
}

void CanvasTest::testNewXZ()
{
    qInfo("Test parameters of canvas created in XZ plane");
    QCOMPARE(m_canvasWidget->topLevelItemCount(), 3);
    this->onNewCanvasXZ();
    QCOMPARE(m_scene->getNumCanvases(), 4);
    QCOMPARE(m_canvasWidget->topLevelItemCount(), 4);
    entity::Canvas* cnvi = m_scene->getCanvas(3);
    QVERIFY(cnvi);

    qInfo("Test newly created canvas is current");
    QCOMPARE(m_scene->getCanvasCurrent(), cnvi);
    QCOMPARE(cnvi->getName().c_str(), "Canvas3");

    qInfo("Test normal and center parameters");
    QVERIFY(differenceWithinThreshold(cnvi->getCenter(), cher::CENTER));
    QVERIFY(differenceWithinThreshold(cnvi->getNormal(), osg::Vec3f(0.f, -1.f, 0.f)));
}

void CanvasTest::testCloneOrtho()
{
    qInfo("Test clone canvas orthogonal to the current one");
    QVERIFY(m_canvas2.get());
    m_scene->setCanvasCurrent(m_canvas2.get());
    QCOMPARE(m_scene->getCanvasCurrent(), m_canvas2.get());

    qInfo("Test current canvas for initial parameters: center and normal");
    QCOMPARE(m_canvas2->getCenter(), cher::CENTER);
    QVERIFY(differenceWithinThreshold(m_canvas2->getNormal(), osg::Vec3f(0.f, -1.f, 0.f)));

    qInfo("Test cloning with non-default initial parameters");
    qInfo("Create canvas in XY plane and test its parameters");
    this->onNewCanvasXY();
    QCOMPARE((int)m_scene->getNumCanvases(), 4);
    entity::Canvas* canvas = m_scene->getCanvas(3);
    QVERIFY(canvas);
    QCOMPARE(m_scene->getCanvasCurrent(), canvas);
    QVERIFY(differenceWithinThreshold( canvas->getCenter(), cher::CENTER));
    QVERIFY(differenceWithinThreshold( canvas->getNormal(), cher::NORMAL));
    QVERIFY(differenceWithinThreshold(canvas->getGlobalAxisU(), osg::Vec3f(1.f, 0, 0)));
    QVERIFY(differenceWithinThreshold(canvas->getGlobalAxisV(), osg::Vec3f(0.f,1.f,0.f)));

    qInfo("Rotate and translate the canvas and test its parameters");
    osg::Matrix rotMat = osg::Matrix::rotate(cher::PI/4, osg::Vec3f(0,1,0));
    canvas->rotate(rotMat, cher::CENTER);

}

bool CanvasTest::differenceWithinThreshold(const osg::Vec3f &X, const osg::Vec3f &Y)
{
    osg::Vec3f diff = X-Y;
    return (std::fabs(diff.x())<=cher::EPSILON &&
            std::fabs(diff.y())<=cher::EPSILON &&
            std::fabs(diff.z())<cher::EPSILON );
}

QTEST_MAIN(CanvasTest)
#include "CanvasTest.moc"
