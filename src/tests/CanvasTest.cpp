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
    this->testOrthogonality(canvas);

    qInfo("Test canvas default nodes and their values");
    canvas->initializeSG();
    this->testStructure(canvas);

    qInfo("Test color setting of frame tool");
    canvas->setColor(cher::CANVAS_CLR_CURRENT);
    QCOMPARE(canvas->getColor(), cher::CANVAS_CLR_CURRENT);
    QVERIFY(canvas->getToolFrame());
    QCOMPARE(canvas->getToolFrame()->getColor(), cher::CANVAS_CLR_CURRENT);

    qInfo("Test rotation and translation procedures");

    qInfo("+45 degree rotation along y axis");
    osg::Matrix rotMat = osg::Matrix::rotate(cher::PI/4, osg::Vec3f(0,1,0));
    canvas->rotate(rotMat, cher::CENTER);
    float len = 1.f * std::cos(cher::PI/4);
    QVERIFY(differenceWithinThreshold(canvas->getNormal(), osg::Vec3f(len,0,len)));
    QVERIFY(differenceWithinThreshold(canvas->getGlobalAxisU(), osg::Vec3f(len,0,-len)));
    QVERIFY(differenceWithinThreshold(canvas->getGlobalAxisV(), osg::Vec3f(0,1,0)));
    this->testOrthogonality(canvas);

    qInfo("translation along normal");
    osg::Matrix tMat = osg::Matrix::translate(1, 0, 1);
    canvas->translate(tMat);
    QVERIFY(differenceWithinThreshold(canvas->getNormal(), osg::Vec3f(len,0,len)));
    QVERIFY(differenceWithinThreshold(canvas->getGlobalAxisU(), osg::Vec3f(len,0,-len)));
    QVERIFY(differenceWithinThreshold(canvas->getGlobalAxisV(), osg::Vec3f(0,1,0)));

    qInfo("Test for orthogonality between normal and axis UV");
    this->testOrthogonality(canvas);
}

void CanvasTest::testReadWrite()
{
    qInfo("Tests for read and write functionality");
    qInfo("Test initial structure");
    QCOMPARE(static_cast<int>(m_scene->getNumCanvases()), 3);
    QCOMPARE(m_canvas0.get(), m_scene->getCanvas(0));
    QCOMPARE(m_canvas1.get(), m_scene->getCanvas(1));
    QCOMPARE(m_canvas2.get(), m_scene->getCanvas(2));
    QCOMPARE(m_canvasWidget->topLevelItemCount(), 3);
    this->testStructure(m_canvas0.get());

    qInfo("Write scene to file");
    QString fname_canvas = QString("RW_CanvasTest.osgt");
    m_rootScene->setFilePath(fname_canvas.toStdString());
    QVERIFY(m_rootScene->writeScenetoFile());
    QVERIFY(m_rootScene->isSavedToFile());

    qInfo("Clear scene");
    this->onFileClose();
    QVERIFY(m_rootScene->isEmptyScene());

    qInfo("Re-open the saved file");
    m_rootScene->setFilePath(fname_canvas.toStdString());
    QVERIFY(m_rootScene->isSetFilePath());
    QVERIFY(this->loadSceneFromFile());
    m_scene = m_rootScene->getUserScene();
    QVERIFY(m_scene.get());

    qInfo("Test an arbitrary canvas structure");
    QCOMPARE(m_scene->getNumCanvases(), 3);
    m_canvas0 = m_scene->getCanvas(0);
    QVERIFY(m_canvas0.get());
    QCOMPARE(m_canvasWidget->topLevelItemCount(), 3);
    this->testStructure(m_canvas0.get());
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
    QCOMPARE(static_cast<int>(m_scene->getNumCanvases()), 4);
    entity::Canvas* canvas = m_scene->getCanvas(3);
    QVERIFY(canvas);
    QCOMPARE(m_scene->getCanvasCurrent(), canvas);
    QVERIFY(differenceWithinThreshold( canvas->getCenter(), cher::CENTER));
    QVERIFY(differenceWithinThreshold( canvas->getNormal(), cher::NORMAL));
    QVERIFY(differenceWithinThreshold(canvas->getGlobalAxisU(), osg::Vec3f(1, 0, 0)));
    QVERIFY(differenceWithinThreshold(canvas->getGlobalAxisV(), osg::Vec3f(0, 1, 0)));

    qInfo("Rotate the canvas +45 degrees around Y axis");
    osg::Matrix rotMat = osg::Matrix::rotate(cher::PI/4, osg::Vec3f(0,1,0));
    canvas->rotate(rotMat, cher::CENTER);
    this->testOrthogonality(canvas);
    float len = 1.f * std::cos(cher::PI/4);
    QVERIFY(differenceWithinThreshold(canvas->getCenter(), cher::CENTER));
    QVERIFY(differenceWithinThreshold(canvas->getNormal(), osg::Vec3f(len,0,len)));
    QVERIFY(differenceWithinThreshold(canvas->getGlobalAxisU(), osg::Vec3f(len,0,-len)));
    QVERIFY(differenceWithinThreshold(canvas->getGlobalAxisV(), osg::Vec3f(0,1,0)));

    qInfo("Translate canvas along its normal");
    osg::Matrix tMat = osg::Matrix::translate(1,0,1);
    canvas->translate(tMat);
    this->testOrthogonality(canvas);
    QVERIFY(differenceWithinThreshold(canvas->getCenter(), osg::Vec3f(1,0,1)));
    QVERIFY(differenceWithinThreshold(canvas->getNormal(), osg::Vec3f(len,0,len)));
    QVERIFY(differenceWithinThreshold(canvas->getGlobalAxisU(), osg::Vec3f(len,0,-len)));
    QVERIFY(differenceWithinThreshold(canvas->getGlobalAxisV(), osg::Vec3f(0,1,0)));

    qInfo("Rotate the canvas +30 degrees around X axis");
    osg::Matrix rotMat2 = osg::Matrix::rotate(cher::PI/6, osg::Vec3f(1,0,0));
    canvas->rotate(rotMat2, canvas->getCenter());
    this->testOrthogonality(canvas);
    QVERIFY(differenceWithinThreshold(canvas->getCenter(), osg::Vec3f(1,0,1)));
    QCOMPARE(m_scene->getCanvasCurrent(), canvas);

    qInfo("Create new canvas orthogonal to the current one");
    this->onNewCanvasOrtho();
    QCOMPARE(static_cast<int>(m_scene->getNumCanvases()), 5);
    entity::Canvas* canvas2 = m_scene->getCanvas(4);
    QVERIFY(canvas2);

    QVERIFY(differenceWithinThreshold(canvas2->getCenter(), osg::Vec3f(1,0,1)));
    QVERIFY(differenceWithinThreshold(canvas2->getNormal(), canvas->getGlobalAxisV()));

    qInfo("Test for orthogonality of the cloned canvas");
    this->testOrthogonality(canvas2);
}

bool CanvasTest::differenceWithinThreshold(const osg::Vec3f &X, const osg::Vec3f &Y)
{
    osg::Vec3f diff = X-Y;
    return (std::fabs(diff.x())<=cher::EPSILON &&
            std::fabs(diff.y())<=cher::EPSILON &&
            std::fabs(diff.z())<cher::EPSILON );
}

void CanvasTest::testOrthogonality(entity::Canvas *canvas)
{
    QVERIFY(canvas);
    qInfo("Test canvas local axis and normal are orthogonal");
    float dotUN = std::fabs(canvas->getNormal() * canvas->getGlobalAxisU());
    float dotVN = std::fabs(canvas->getNormal() * canvas->getGlobalAxisV());
    float dotUV = std::fabs(canvas->getGlobalAxisU() * canvas->getGlobalAxisV());
    QVERIFY(dotUN<cher::EPSILON);
    QVERIFY(dotVN<cher::EPSILON);
    QVERIFY(dotUV<cher::EPSILON);
}

void CanvasTest::testStructure(entity::Canvas *canvas)
{
    QVERIFY(canvas);
    QCOMPARE(int(canvas->getNodeMask()), int(cher::MASK_CANVAS_IN));

    qInfo("Test matrix transform");
    const osg::MatrixTransform* mt = canvas->getTransform();
    QVERIFY(mt);
    QCOMPARE(mt->getName().c_str(), "Transform");
    QCOMPARE(static_cast<int>(canvas->getNumChildren()), 1);
    QCOMPARE(canvas->getChild(0), mt);

    qInfo("Test switch");
    const osg::Switch* switchC = canvas->getSwitch();
    QVERIFY(switchC);
    QCOMPARE(switchC->getName().c_str(), "Switch");
    QCOMPARE(mt->getChild(0), switchC);
    QCOMPARE(static_cast<int>(switchC->getNumChildren()), 2);

    qInfo("Test group data");
    const osg::Group* groupData = canvas->getGroupData();
    QVERIFY(groupData);
    QCOMPARE(int(groupData->getNodeMask()), int(cher::MASK_CANVASDATA_IN));
    QCOMPARE(switchC->getChild(0), groupData);
    QCOMPARE(static_cast<int>(groupData->getNumChildren()), 2);
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
}

QTEST_MAIN(CanvasTest)
#include "CanvasTest.moc"
