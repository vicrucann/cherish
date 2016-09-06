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
    QCOMPARE(stroke->getColorBinding(), osg::Geometry::BIND_PER_VERTEX);

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
    QVERIFY(stroke->getVertexArray());
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(stroke->getVertexArray());
    QVERIFY(verts);
    QCOMPARE( static_cast<int>(verts->size()), 4);

    qInfo("Test stroke's length");
    QCOMPARE(stroke->getLength(), 1.f);
    QVERIFY(stroke->isLengthy());

    qInfo("Extract camera to do shader re-definition");
    osg::Camera* camera = NULL;
    QVERIFY(!camera);
    emit m_scene->requestCamera(camera);
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

    qInfo("Re-define cloned stroke as a curve and as a shader");
    QVERIFY(stroke_clone->redefineToCurve());
    QVERIFY(stroke_clone->redefineToShader(camera, m_scene->getCanvasCurrent()->getTransform()));

    qInfo("Add cloned stroke as an entity to the scene to replace the phantom");
    QVERIFY(m_canvas2->addEntity(stroke_clone.get()));

    qInfo("Remove phantom stroke and clear the current state");
    QVERIFY(m_canvas2->removeEntity(stroke.get()));
    m_canvas2->setStrokeCurrent(false);
    QVERIFY(!m_canvas2->getStrokeCurrent());

    qInfo("Test cloned stroke's shader parameters");
    QVERIFY(stroke_clone->getIsCurved());
    QVERIFY(stroke_clone->isShadered());
    QVERIFY(!stroke->getIsCurved());
    QVERIFY(!stroke->isShadered());
    QCOMPARE(static_cast<int>(stroke_clone->getLines()->getMode()), GL_LINES_ADJACENCY_EXT);

    qInfo("Shaderize phantom and test against it");
    QVERIFY(stroke->redefineToCurve());
    QVERIFY(stroke->redefineToShader(camera, m_canvas2->getTransform()));
    verts =  static_cast<osg::Vec3Array*>(stroke->getVertexArray());
    QVERIFY(verts);
    verts_clone = static_cast<osg::Vec3Array*>(stroke_clone->getVertexArray());
    QVERIFY(verts_clone);
    QCOMPARE(static_cast<int>(stroke->getLines()->getMode()), GL_LINES_ADJACENCY_EXT);
    for (unsigned int i=0; i<verts->size(); ++i){
        auto delta = verts->at(i) - verts_clone->at(i);
        QVERIFY( std::fabs(delta.x()) < cher::EPSILON);
        QVERIFY( std::fabs(delta.y()) < cher::EPSILON);
        QVERIFY( std::fabs(delta.z()) < cher::EPSILON);
    }
    QCOMPARE(stroke->getColor(), stroke_clone->getColor());

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

    QVERIFY(original->redefineToCurve());
    QVERIFY(original->redefineToShader(camera, canvas->getTransform()));
    canvas->setStrokeCurrent(false);

    qInfo("Create stroke by copying the original");
    osg::ref_ptr<entity::Stroke> copy = new entity::Stroke;
    QVERIFY(copy->copyFrom(original.get()));
    QVERIFY(copy->redefineToCurve());
    QVERIFY(copy->redefineToShader(original->getCamera(), canvas->getTransform()));

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

    QVERIFY(original->redefineToCurve());
    QVERIFY(original->redefineToShader(camera, canvas->getTransform()));
    canvas->setStrokeCurrent(false);

    qInfo("Test stroke parameters");
    QVERIFY(original->getIsCurved());
    QVERIFY(original->isShadered());
    int n0 = original->getNumPoints();
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
    QVERIFY(original->getIsCurved());
    QVERIFY(original->isShadered());
    QCOMPARE(n0, original->getNumPoints());
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

    QVERIFY(saved->getIsCurved());
    QVERIFY(saved->isShadered());
    QCOMPARE(saved->getNumPoints(), n0);
    QCOMPARE(static_cast<int>(saved->getLines()->getMode()), GL_LINES_ADJACENCY_EXT);
    QVERIFY(saved->getCamera());
    QCOMPARE(saved->getCamera(), m_glWidget->getCamera());

}

void StrokeTest::testCopyPaste()
{
    entity::Canvas* canvas = m_scene->getCanvasCurrent();
    QVERIFY(canvas);

    qInfo("Test copy-cut-paste work well for stroke");
    qInfo("Create a stroke");
    osg::ref_ptr<entity::Stroke> original = new entity::Stroke;
    QVERIFY(original.get());

    qInfo("Feed some points to the empty stroke");
    canvas->setStrokeCurrent(original.get());
    QVERIFY(canvas->addEntity(original.get()));

    original->appendPoint(0, 0);
    original->appendPoint(0.2, 0.2);
    original->appendPoint(0.4, 0.4);
    original->appendPoint(0.8, 0.9);

    osg::Camera* camera = NULL;
    emit m_scene->requestCamera(camera);
    QVERIFY(camera);

    qInfo("Shaderize the stroke");
    QVERIFY(original->redefineToCurve());
    QVERIFY(original->redefineToShader(camera, canvas->getTransform()));
    canvas->setStrokeCurrent(false);

    qInfo("Test stroke parameters");
    QVERIFY(original->getIsCurved());
    QVERIFY(original->isShadered());
    QCOMPARE(static_cast<int>(original->getLines()->getMode()), GL_LINES_ADJACENCY_EXT);
    QCOMPARE(static_cast<int>(canvas->getGeodeStrokes()->getNumChildren()), 1);
    QCOMPARE(canvas->getGeodeStrokes()->getChild(0), original.get());

    qInfo("Perform stroke selection");
    canvas->addEntitySelected(original.get());
    const auto& entities = canvas->getEntitiesSelected();
    QVERIFY(entities.size() == 1);
    QCOMPARE(entities.at(0), original.get());

    qInfo("Copy the selected stroke to the buffer");
    this->m_rootScene->copyToBuffer();

    qInfo("Test buffer content");
    auto buffer = m_rootScene->getBuffer();
    QVERIFY(buffer.size() == 1);
    auto b0 = buffer.at(0);
    entity::Stroke* buffered = dynamic_cast<entity::Stroke*>(b0.get());
    QVERIFY(buffered);
    QCOMPARE(original->getNumPoints(), buffered->getNumPoints());
    for (auto i=0; i<original->getNumPoints(); ++i){
        auto o = original->getPoint(i);
        auto b = buffered->getPoint(i);
        auto delta = o - b;
        QVERIFY(std::fabs(delta.x()) < cher::EPSILON);
        QVERIFY(std::fabs(delta.y()) < cher::EPSILON);
    }

    qInfo("Perform the paste");
    m_rootScene->pasteFromBuffer();
    QVERIFY(canvas->getNumStrokes() == 2);
    qInfo("Test canvas content and stroke coordinates");
    entity::Stroke* pasted = canvas->getStroke(1);
    QVERIFY(pasted);
    QCOMPARE(original->getNumPoints(), pasted->getNumPoints());
    for (auto i=0; i<original->getNumPoints(); ++i){
        auto o = original->getPoint(i);
        auto p = pasted->getPoint(i);
        auto p0 = p - osg::Vec2f(0.2, 0.2);
        auto delta = o-p0;
        QVERIFY(std::fabs(delta.x()) < cher::EPSILON);
        QVERIFY(std::fabs(delta.y()) < cher::EPSILON);
    }
}

QTEST_MAIN(StrokeTest)
#include "StrokeTest.moc"
