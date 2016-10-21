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
    QVERIFY(m_canvas2->getProgramStroke());
    stroke->initializeProgram(m_canvas2->getProgramStroke());
    qInfo("Check stroke's geometry and other properties");
    QCOMPARE(stroke->getColor(), cher::STROKE_CLR_NORMAL);
    QVERIFY(stroke->getLines());
    QCOMPARE(static_cast<int>(stroke->getLines()->getMode()), GL_LINE_STRIP);
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

    qInfo("Clone stroke by copying the data");
    osg::ref_ptr<entity::Stroke> stroke_clone = new entity::Stroke;
    QVERIFY(stroke_clone.get());
    qInfo("Copy  and re-define cloned stroke as a curve and as a shader");
    QVERIFY(stroke_clone->copyFrom(stroke.get()));
    osg::Vec3Array* verts_clone = static_cast<osg::Vec3Array*>(stroke_clone->getVertexArray());
    QVERIFY(verts_clone);
    QVERIFY(stroke_clone->getLines());
    QVERIFY(stroke->getLines() != stroke_clone->getLines());
    QCOMPARE(static_cast<int>(stroke_clone->getLines()->getMode()), GL_LINES_ADJACENCY_EXT);
    QVERIFY(stroke_clone->getIsCurved());
    QVERIFY(stroke_clone->getIsShadered());

    qInfo("Add cloned stroke as an entity to the scene to replace the phantom");
    QVERIFY(m_canvas2->addEntity(stroke_clone.get()));

    qInfo("Remove phantom stroke and clear the current state");
    QVERIFY(m_canvas2->removeEntity(stroke.get()));
    m_canvas2->setStrokeCurrent(false);
    QVERIFY(!m_canvas2->getStrokeCurrent());

    qInfo("Test cloned stroke's shader parameters");
    QVERIFY(!stroke->getIsCurved());
    QVERIFY(!stroke->getIsShadered());

    qInfo("Shaderize phantom and test against it");
    QVERIFY(stroke->redefineToShape());
    verts =  static_cast<osg::Vec3Array*>(stroke->getVertexArray());
    QVERIFY(verts);
    verts_clone = static_cast<osg::Vec3Array*>(stroke_clone->getVertexArray());
    QVERIFY(verts_clone);
    QCOMPARE(static_cast<int>(stroke->getLines()->getMode()), GL_LINES_ADJACENCY_EXT);
    QVERIFY(stroke->getIsShadered());
    QVERIFY(stroke->getIsCurved());
    QCOMPARE(stroke->getNumPoints(), stroke_clone->getNumPoints());
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
    original->initializeProgram(canvas->getProgramStroke());

    qInfo("Fill-in the stroke and add it to the canvas");
    canvas->setStrokeCurrent(original);
    QVERIFY(canvas->addEntity(original.get()));

    original->appendPoint(0, 0);
    original->appendPoint(1, 0);
    original->appendPoint(1, 1);
    original->appendPoint(0, 1);

    QVERIFY(original->redefineToShape());
    canvas->setStrokeCurrent(false);

    qInfo("Create stroke by copying the original");
    osg::ref_ptr<entity::Stroke> copy = new entity::Stroke;
    QVERIFY(copy->copyFrom(original.get()));
    QVERIFY(copy->getIsShadered());
    QVERIFY(copy->getIsCurved());

    qInfo("Compare the coordinates of the both strokes");
    QCOMPARE(original->getNumPoints(), copy->getNumPoints());
    for (int i=0; i<original->getNumPoints(); ++i){
        auto delta = original->getPoint(i) - copy->getPoint(i);
        QVERIFY( std::fabs(delta.x()) < cher::EPSILON);
        QVERIFY( std::fabs(delta.y()) < cher::EPSILON);
    }

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
    original->initializeProgram(canvas->getProgramStroke());

    qInfo("Fill-in the stroke and add it to the canvas");
    canvas->setStrokeCurrent(original);
    QVERIFY(canvas->addEntity(original.get()));

    original->appendPoint(0, 0);
    original->appendPoint(1, 0);
    original->appendPoint(1, 1);
    original->appendPoint(0, 1);

    qInfo("Shaderize the stroke");

    QVERIFY(original->redefineToShape());
    canvas->setStrokeCurrent(false);

    qInfo("Test stroke parameters");
    QVERIFY(original->getIsCurved());
    QVERIFY(original->getIsShadered());
    int n0 = original->getNumPoints();
    QCOMPARE(static_cast<int>(original->getLines()->getMode()), GL_LINES_ADJACENCY_EXT);
    QCOMPARE(static_cast<int>(canvas->getGeodeStrokes()->getNumChildren()), 1);
    QCOMPARE(canvas->getGeodeStrokes()->getChild(0), original.get());

    qInfo("Write scene to file");
    QString fname = QString("RW_StrokeTest.osgt");
    m_rootScene->setFilePath(fname.toStdString());
    QVERIFY(m_rootScene->writeScenetoFile());
    QVERIFY(m_rootScene->isSavedToFile());

    qInfo("Verify stroke params didnt change after the write");
    QVERIFY(original->getIsCurved());
    QVERIFY(original->getIsShadered());
    QCOMPARE(n0, original->getNumPoints());
    QCOMPARE(static_cast<int>(original->getLines()->getMode()), GL_LINES_ADJACENCY_EXT);
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
    QCOMPARE(saved->getProgram()->getTransform(), canvas->getTransform());
    QVERIFY(saved->getProgram()->getCamera());
    QCOMPARE(saved->getProgram()->getCamera(), m_glWidget->getCamera());
    QCOMPARE(saved->getProgram()->getIsFogged(), this->m_actionStrokeFogFactor->isChecked());

    QVERIFY(saved->getIsCurved());
    QVERIFY(saved->getIsShadered());
    QCOMPARE(saved->getNumPoints(), n0);
    QCOMPARE(static_cast<int>(saved->getLines()->getMode()), GL_LINES_ADJACENCY_EXT);
}

void StrokeTest::testCopyPaste()
{
    entity::Canvas* canvas = m_scene->getCanvasCurrent();
    QVERIFY(canvas);

    qInfo("Test copy-cut-paste work well for stroke");
    qInfo("Create a stroke");
    osg::ref_ptr<entity::Stroke> original = new entity::Stroke;
    QVERIFY(original.get());
    original->initializeProgram(canvas->getProgramStroke());

    qInfo("Feed some points to the empty stroke");
    canvas->setStrokeCurrent(original.get());
    QVERIFY(canvas->addEntity(original.get()));

    original->appendPoint(0, 0);
    original->appendPoint(0.2, 0.2);
    original->appendPoint(0.4, 0.4);
    original->appendPoint(0.8, 0.9);

    qInfo("Shaderize the stroke");
    QVERIFY(original->redefineToShape());
    canvas->setStrokeCurrent(false);

    qInfo("Test stroke parameters");
    QVERIFY(original->getIsCurved());
    QVERIFY(original->getIsShadered());
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
    qInfo("Test buffered parameters");
    QVERIFY(buffered->getProgram());
    QCOMPARE(buffered->getProgram()->getCamera(), m_glWidget->getCamera());
    QCOMPARE(buffered->getProgram()->getTransform(), canvas->getTransform());
    QCOMPARE(static_cast<int>(buffered->getProgram()->getNumShaders()), 3);
    QVERIFY(buffered->getIsShadered());
    QVERIFY(buffered->getIsCurved());

    qInfo("Perform the paste to the same canvas");
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
    QVERIFY(pasted->getIsCurved());
    QVERIFY(pasted->getIsShadered());

    qInfo("Perform the paste to different canvas");
    m_scene->setCanvasCurrent(m_canvas0.get());
    m_rootScene->pasteFromBuffer();

    qInfo("Test parameters of the second pasted stroke");
    QVERIFY(m_canvas0->getNumStrokes() == 1);
    entity::Stroke* pasted2 = m_canvas0->getStroke(0);
    QVERIFY(pasted2);
    QCOMPARE(pasted2->getNumPoints(), original->getNumPoints());
    QVERIFY(pasted->getIsShadered());
    QVERIFY(pasted->getIsCurved());
    QCOMPARE(static_cast<int>(pasted2->getLines()->getMode()), GL_LINES_ADJACENCY_EXT);

    qInfo("Test the pasted-2 program settings");
    QVERIFY(pasted2->getProgram());
    QCOMPARE(pasted2->getProgram()->getCamera(), m_glWidget->getCamera());
    QCOMPARE(pasted2->getProgram()->getTransform(), m_canvas0->getTransform());
    QCOMPARE(pasted2->getProgram()->getIsFogged(), this->m_actionStrokeFogFactor->isChecked());
}

void StrokeTest::testFogSwitch()
{
    qInfo("Add couple strokes to the scene");
    osg::ref_ptr<entity::Stroke> s1 = new entity::Stroke;
    QVERIFY(m_canvas0->addEntity(s1.get()));
    s1->initializeProgram(m_canvas2->getProgramStroke());
    s1->appendPoint(0, 0);
    s1->appendPoint(0.2, 0.2);
    s1->appendPoint(0.4, 0.4);
    s1->appendPoint(0.8, 0.9);

    qInfo("Shaderize the stroke");
    QVERIFY(s1->redefineToShape(m_canvas0->getTransform()));
    QVERIFY(s1->getProgram());
    bool f0 = this->m_actionStrokeFogFactor->isChecked();
    QCOMPARE(s1->getProgram()->getIsFogged(), this->m_actionStrokeFogFactor->isChecked());

    qInfo("Add second stroke to another canvas");
    osg::ref_ptr<entity::Stroke> s2 = new entity::Stroke;
    QVERIFY(m_canvas1->addEntity(s2.get()));
    s2->initializeProgram(m_canvas2->getProgramStroke());
    s2->appendPoint(0, 0);
    s2->appendPoint(0.2, 0.2);
    s2->appendPoint(0.4, 0.4);
    s2->appendPoint(0.8, 0.9);

    qInfo("Shaderize the stroke");
    QVERIFY(s2->redefineToShape(m_canvas1->getTransform()));
    QVERIFY(s2->getProgram());
    QCOMPARE(s2->getProgram()->getIsFogged(), this->m_actionStrokeFogFactor->isChecked());

    qInfo("Switch the fog factor");
    this->m_actionStrokeFogFactor->setChecked(true);
    this->onStrokeFogFactor();
    QVERIFY(this->m_actionStrokeFogFactor->isChecked() != f0);

    qInfo("Test stroke parameters changed as well");
    QCOMPARE(s1->getProgram()->getIsFogged(), this->m_actionStrokeFogFactor->isChecked());
    QCOMPARE(s2->getProgram()->getIsFogged(), this->m_actionStrokeFogFactor->isChecked());
}

QTEST_MAIN(StrokeTest)
#include "StrokeTest.moc"
