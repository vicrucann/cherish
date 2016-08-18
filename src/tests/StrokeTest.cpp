#include "StrokeTest.h"

#include <QSignalSpy>

#include <osg/ref_ptr>
#include <osg/Geometry>
#include <osg/Camera>
#include <osg/Program>

#include "Stroke.h"

void StrokeTest::testAddStroke()
{
    qInfo("Simulate stroke creation by repeating steps from UserScene::addStroke()");
    QCOMPARE(m_canvas2.get(), m_scene->getCanvasCurrent());
    QCOMPARE(static_cast<int>(m_canvas2->getEntitiesSelectedSize()), 0);
    qInfo("Make sure canvas is not hidden");
    QVERIFY(m_canvas2->getVisibilityAll());

    qInfo("Create new stroke");
    osg::ref_ptr<entity::Stroke> stroke = new entity::Stroke;
    QVERIFY(stroke.get());
    qInfo("Check stroke's geometry and other properties");
    QCOMPARE(stroke->getColor(), cher::STROKE_CLR_NORMAL);
    QVERIFY(stroke->getLines());
    QCOMPARE(static_cast<int>(stroke->getLines()->getMode()), GL_LINE_STRIP_ADJACENCY);
    QCOMPARE(stroke->getColorBinding(), osg::Geometry::BIND_OVERALL);

    qInfo("Add a phantom to the current canvas");
    m_canvas2->setStrokeCurrent(stroke.get());
    QVERIFY(m_canvas2->addEntity(stroke.get()));
    QCOMPARE(m_canvas2->getStrokeCurrent(), stroke.get());
    QVERIFY(m_canvas2->getGeodeStrokes());
    QCOMPARE(m_canvas2->getGeodeStrokes()->getChild(0), stroke.get());

    qInfo("Append some points to the stroke");
    stroke->appendPoint(0,0);
    stroke->appendPoint(1,0);
    stroke->appendPoint(1,1);
    stroke->appendPoint(0,1);
    stroke->appendPoint(0.5, 1.5);
    stroke->appendPoint(0.5, 2);
    QVERIFY(stroke->getVertexArray());
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(stroke->getVertexArray());
    QVERIFY(verts);
    QCOMPARE( static_cast<int>(verts->size()), 6);

    qInfo("Test stroke's length");
    QCOMPARE(stroke->getLength(), 2.f);
    QVERIFY(stroke->isLengthy());

    qInfo("Extract camera to do shader re-definition");
    osg::Camera* camera = NULL;
    QVERIFY(!camera);

    QSignalSpy spy_camera(m_scene.get(), SIGNAL(requestCamera(osg::Camera*&)));
    emit m_scene->requestCamera(camera);
    QCOMPARE(spy_camera.count(), 1);
    QVERIFY(camera);

    qInfo("Clone stroke by copying the data");
    osg::ref_ptr<entity::Stroke> stroke_clone = new entity::Stroke;
    QVERIFY(stroke_clone.get());
    QVERIFY(stroke_clone->copyFrom(stroke.get()));
    QCOMPARE(stroke_clone->getLength(), stroke->getLength());
    osg::Vec3Array* verts_clone = static_cast<osg::Vec3Array*>(stroke_clone->getVertexArray());
    QVERIFY(verts_clone);
    QCOMPARE(verts->size(), verts_clone->size());
    for (unsigned int i=0; i<verts->size(); ++i){
        QCOMPARE((*verts)[i], (*verts_clone)[i]);
    }
    QVERIFY(stroke_clone->getLines());
    QVERIFY(stroke->getLines() != stroke_clone->getLines());
    QCOMPARE(static_cast<int>(stroke_clone->getLines()->getMode()), GL_LINE_STRIP_ADJACENCY);

    qInfo("Re-define cloned stroke as a shader");
    QVERIFY(stroke_clone->redefineToShader(camera));

    qInfo("Add cloned stroke as an entity to the scene to replace the phantom");
    QVERIFY(m_canvas2->addEntity(stroke_clone.get()));

    qInfo("Remove phantom stroke and clear the current state");
    QVERIFY(m_canvas2->removeEntity(stroke.get()));
    m_canvas2->setStrokeCurrent(false);
    QVERIFY(!m_canvas2->getStrokeCurrent());

    qInfo("Test cloned stroke's shader parameters");
    QVERIFY(stroke_clone->isShadered());
    QVERIFY(!stroke->isShadered());
    verts_clone = static_cast<osg::Vec3Array*>(stroke_clone->getVertexArray());
    QVERIFY(verts_clone);
    QCOMPARE(static_cast<int>(verts_clone->size()), 20);
    QCOMPARE(static_cast<int>(stroke_clone->getLines()->getMode()), GL_LINES_ADJACENCY_EXT);

    qInfo("Shaderize phantom and test against it");
    QVERIFY(stroke->redefineToShader(camera));
    verts =  static_cast<osg::Vec3Array*>(stroke->getVertexArray());
    QVERIFY(verts);
    QCOMPARE(static_cast<int>(verts->size()), 20);
    QCOMPARE(static_cast<int>(stroke->getLines()->getMode()), GL_LINES_ADJACENCY_EXT);
    for (unsigned int i=0; i<verts->size(); ++i){
        QCOMPARE((*verts)[i], (*verts_clone)[i]);
    }
    QCOMPARE(stroke->getColor(), stroke_clone->getColor());

    qInfo("Test coordinates of new vertices");
//    QCOMPARE((*verts_clone)[0], osg::Vec3f(-1.f, 0.f, 0.f));
    QCOMPARE((*verts_clone)[1], osg::Vec3f(0.f, 0.f, 0.f));
    QCOMPARE((*verts_clone)[2], osg::Vec3f(1.f, 0.f, 0.f));
    QCOMPARE((*verts_clone)[3], osg::Vec3f(1.f, 1.f, 0.f));

    QCOMPARE((*verts_clone)[4], osg::Vec3f(0.f, 0.f, 0.f));
    QCOMPARE((*verts_clone)[5], osg::Vec3f(1.f, 0.f, 0.f));
    QCOMPARE((*verts_clone)[6], osg::Vec3f(1.f, 1.f, 0.f));
    QCOMPARE((*verts_clone)[7], osg::Vec3f(0.f, 1.f, 0.f));

    QCOMPARE((*verts_clone)[8], osg::Vec3f(1.f, 0.f, 0.f));
    QCOMPARE((*verts_clone)[9], osg::Vec3f(1.f, 1.f, 0.f));
    QCOMPARE((*verts_clone)[10], osg::Vec3f(0.f, 1.f, 0.f));
    QCOMPARE((*verts_clone)[11], osg::Vec3f(0.5f, 1.5f, 0.f));

    QCOMPARE((*verts_clone)[12], osg::Vec3f(1.f, 1.f, 0.f));
    QCOMPARE((*verts_clone)[13], osg::Vec3f(0.f, 1.f, 0.f));
    QCOMPARE((*verts_clone)[14], osg::Vec3f(0.5f, 1.5f, 0.f));
    QCOMPARE((*verts_clone)[15], osg::Vec3f(0.5f, 2.f, 0.f));

    QCOMPARE((*verts_clone)[16], osg::Vec3f(0.f, 1.f, 0.f));
    QCOMPARE((*verts_clone)[17], osg::Vec3f(0.5f, 1.5f, 0.f));
    QCOMPARE((*verts_clone)[18], osg::Vec3f(0.5f, 2.0f, 0.f));
//    QCOMPARE((*verts_clone)[19], osg::Vec3f(0.5f, 3.f, 0.f));

    qInfo("Test canvas scene state for only 1 child");
    QVERIFY(m_canvas2->getGeodeStrokes()->getChild(0));
    QCOMPARE(m_canvas2->getGeodeStrokes()->getChild(0), stroke_clone.get());
    QVERIFY(!m_canvas2->getGeodeStrokes()->containsDrawable(stroke));
}

void StrokeTest::testCloneShaderedStroke()
{
    entity::Canvas* canvas = m_scene->getCanvasCurrent();
    QVERIFY(canvas);

    qInfo("Create new stroke");
    osg::ref_ptr<entity::Stroke> original = new entity::Stroke;
    QVERIFY(original.get());

    qInfo("Fill-in the stroke and add it to the canvas");
    canvas->setStrokeCurrent(original);
    QVERIFY(canvas->addEntity(original.get()));

    original->appendPoint(0, 0);
    original->appendPoint(1, 0);
    original->appendPoint(1, 1);
    original->appendPoint(0, 1);

    osg::Camera* camera = NULL;
    emit m_scene->requestCamera(camera);
    QVERIFY(camera);

    QVERIFY(original->redefineToShader(camera));
    canvas->setStrokeCurrent(false);

    qInfo("Create stroke by copying the original");
    osg::ref_ptr<entity::Stroke> copy = new entity::Stroke;
    QVERIFY(copy->copyFrom(original.get()));
    QVERIFY(copy->redefineToShader(original->getCamera()));

    qInfo("Delta move the copy stroke");
    copy->moveDelta(-0.2, -0.2);
    copy->setColor(solarized::cyan);

    qInfo("Add the copy stroke to the scene");
    QVERIFY(canvas->addEntity(copy));

    QCOMPARE(static_cast<int>(canvas->getGeodeStrokes()->getNumChildren()), 2);

//    canvas->updateFrame(m_scene->getCanvasPrevious());
//    QTest::qWait(3000);
}

void StrokeTest::testReadWrite()
{
    entity::Canvas* canvas = m_scene->getCanvasCurrent();
    QVERIFY(canvas);

    qInfo("Create a stroke to save to file");
    osg::ref_ptr<entity::Stroke> original = new entity::Stroke;
    QVERIFY(original.get());

    qInfo("Fill-in the stroke and add it to the canvas");
    canvas->setStrokeCurrent(original);
    QVERIFY(canvas->addEntity(original.get()));

    original->appendPoint(0, 0);
    original->appendPoint(1, 0);
    original->appendPoint(1, 1);
    original->appendPoint(0, 1);

    qInfo("Shaderize the stroke");
    osg::Camera* camera = NULL;
    emit m_scene->requestCamera(camera);
    QVERIFY(camera);

    QVERIFY(original->redefineToShader(camera));
    canvas->setStrokeCurrent(false);

    qInfo("Test stroke parameters");
    QVERIFY(original->isShadered());
    QCOMPARE(original->getNumPoints(), 12);
    QCOMPARE(static_cast<int>(original->getLines()->getMode()), GL_LINES_ADJACENCY_EXT);
    QVERIFY(original->getCamera());
    QCOMPARE(static_cast<int>(canvas->getGeodeStrokes()->getNumChildren()), 1);
    QCOMPARE(canvas->getGeodeStrokes()->getChild(0), original.get());

    qInfo("Write scene to file");
    QString fname = QString("RW_StrokeTest.osgt");
    m_rootScene->setFilePath(fname.toStdString());
    QVERIFY(m_rootScene->writeScenetoFile());
    QVERIFY(m_rootScene->isSavedToFile());

    qInfo("Verify stroke params didnt change after the write");
    QVERIFY(original->isShadered());
    QCOMPARE(original->getNumPoints(), 12);
    QCOMPARE(static_cast<int>(original->getLines()->getMode()), GL_LINES_ADJACENCY_EXT);
    QVERIFY(original->getCamera());
    QCOMPARE(static_cast<int>(canvas->getGeodeStrokes()->getNumChildren()), 1);
    QCOMPARE(canvas->getGeodeStrokes()->getChild(0), original.get());

    qInfo("Clear scene");
    this->onFileClose();
    QVERIFY(m_rootScene->isEmptyScene());

    qInfo("Re-open the saved file");
    m_rootScene->setFilePath(fname.toStdString());
    QVERIFY(m_rootScene->isSetFilePath());
    QVERIFY(this->loadSceneFromFile());
    m_scene = m_rootScene->getUserScene();
    QVERIFY(m_scene.get());

    qInfo("Get the canvas pointer");
    QCOMPARE(static_cast<int>(m_scene->getNumCanvases()), 3);
    canvas = m_scene->getCanvas(2);
    QVERIFY(canvas);

    qInfo("Test stroke structure and its relation for canvas");
    QCOMPARE(static_cast<int>(canvas->getGeodeStrokes()->getNumChildren()), 1);
    const entity::Stroke* saved = dynamic_cast<const entity::Stroke*>(canvas->getGeodeStrokes()->getChild(0));
    QVERIFY(saved);

    QVERIFY(saved->getProgram());
    QCOMPARE(static_cast<int>(saved->getProgram()->getNumShaders()), 3);

    QVERIFY(saved->isShadered());
    QCOMPARE(saved->getNumPoints(), 12);
    QCOMPARE(static_cast<int>(saved->getLines()->getMode()), GL_LINES_ADJACENCY_EXT);
    QVERIFY(saved->getCamera());
    QCOMPARE(saved->getCamera(), m_glWidget->getCamera());

}

QTEST_MAIN(StrokeTest)
#include "StrokeTest.moc"
