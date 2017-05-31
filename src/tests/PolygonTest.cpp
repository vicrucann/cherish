#include "PolygonTest.h"

#include "Polygon.h"

void PolygonTest::testAddPolygon()
{
    qInfo("Simulate polygon creation by repeating steps from UserScene::addPolygon");

    qInfo("Create new stroke");
    osg::ref_ptr<entity::Polygon> entity = new entity::Polygon;
    QVERIFY(entity.get());
    QVERIFY(m_canvas2->getProgramPolygon());
    entity->initializeProgram(m_canvas2->getProgramPolygon());
    qInfo("Check polygon's geometry and other properties");
    QVERIFY(entity->getLines());
    QCOMPARE(static_cast<int>(entity->getLines()->getMode()), GL_LINE_STRIP);
    QCOMPARE(entity->getColorBinding(), osg::Geometry::BIND_PER_VERTEX);

    qInfo("Add a phantom to the current canvas");
    m_canvas2->setEntityCurrent(entity.get());
    QVERIFY(m_canvas2->addEntity(entity.get()));
    QCOMPARE(m_canvas2->getEntityCurrent<entity::Polygon>(), entity.get());
    QVERIFY(m_canvas2->getGeodePolygons());
    QCOMPARE(m_canvas2->getGeodePolygons()->getChild(0), entity.get());

    qInfo("Append some points to the polygon");
    entity->appendPoint(0,0);
    entity->appendPoint(1,0);
    entity->appendPoint(1,1);
    entity->appendPoint(0,1);
    QVERIFY(entity->getVertexArray());
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(entity->getVertexArray());
    QVERIFY(verts);
    QCOMPARE( static_cast<int>(verts->size()), 4);

    qInfo("Clone polygon by copying the data");
    osg::ref_ptr<entity::Polygon> entity_clone = new entity::Polygon;
    QVERIFY(entity_clone.get());
    qInfo("Copy  and re-define cloned polygon as a curve and as a shader");
    QVERIFY(entity_clone->copyFrom(entity.get()));
    osg::Vec3Array* verts_clone = static_cast<osg::Vec3Array*>(entity_clone->getVertexArray());
    QVERIFY(verts_clone);
    QVERIFY(entity_clone->getLines());
    QVERIFY(entity->getLines() != entity_clone->getLines());
    QCOMPARE(static_cast<int>(entity_clone->getLines()->getMode()), GL_POLYGON);
    QVERIFY(entity_clone->getIsShadered());

    qInfo("Add cloned polygon as an entity to the scene to replace the phantom");
    QVERIFY(m_canvas2->addEntity(entity_clone.get()));

    qInfo("Remove phantom polygon and clear the current state");
    QVERIFY(m_canvas2->removeEntity(entity.get()));
    m_canvas2->setEntityCurrent(false);
    QVERIFY(!m_canvas2->getEntityCurrent<entity::Polygon>());

    qInfo("Test cloned polygon's shader parameters");
    QVERIFY(!entity->getIsShadered());

    qInfo("Test canvas scene state for only 1 child");
    QVERIFY(m_canvas2->getGeodePolygons()->getChild(0));
    QCOMPARE(m_canvas2->getGeodePolygons()->getChild(0), entity_clone.get());
    QVERIFY(!m_canvas2->getGeodePolygons()->containsDrawable(entity));

}

void PolygonTest::testCloneShaderedPolygon()
{
    entity::Canvas* canvas = m_scene->getCanvasCurrent();
    QVERIFY(canvas);

    qInfo("Create new polygon");
    osg::ref_ptr<entity::Polygon> original = new entity::Polygon;
    QVERIFY(original.get());
    original->initializeProgram(canvas->getProgramPolygon());

    qInfo("Fill-in the polygon and add it to the canvas");
    canvas->setEntityCurrent(original);
    QVERIFY(canvas->addEntity(original.get()));

    original->appendPoint(0, 0);
    original->appendPoint(1, 0);
    original->appendPoint(1, 1);
    original->appendPoint(0, 1);

    QVERIFY(original->redefineToShape());
    canvas->setEntityCurrent(false);

    qInfo("Create polygon by copying the original");
    osg::ref_ptr<entity::Polygon> copy = new entity::Polygon;
    QVERIFY(copy->copyFrom(original.get()));
    QVERIFY(copy->getIsShadered());

    qInfo("Compare the coordinates of the both polygons");
    QCOMPARE(original->getNumPoints(), copy->getNumPoints());
    for (int i=0; i<original->getNumPoints(); ++i){
        auto delta = original->getPoint(i) - copy->getPoint(i);
        QVERIFY( std::fabs(delta.x()) < cher::EPSILON);
        QVERIFY( std::fabs(delta.y()) < cher::EPSILON);
    }

    qInfo("Delta move the copy polygon");
    copy->moveDelta(-0.2, -0.2);
    copy->setColor(solarized::cyan);

    qInfo("Add the copy polygon to the scene");
    QVERIFY(canvas->addEntity(copy));

    QCOMPARE(static_cast<int>(canvas->getGeodePolygons()->getNumChildren()), 2);
}

void PolygonTest::testReadWrite()
{
    entity::Canvas* canvas = m_scene->getCanvasCurrent();
    QVERIFY(canvas);

    qInfo("Create a polygon to save to file");
    osg::ref_ptr<entity::Polygon> original = new entity::Polygon;
    QVERIFY(original.get());
    original->initializeProgram(canvas->getProgramPolygon());

    qInfo("Fill-in the polygon and add it to the canvas");
    canvas->setEntityCurrent(original);
    QVERIFY(canvas->addEntity(original.get()));

    original->appendPoint(0, 0);
    original->appendPoint(1, 0);
    original->appendPoint(1, 1);
    original->appendPoint(0, 1);

    qInfo("Shaderize the polygon");

    QVERIFY(original->redefineToShape());
    canvas->setEntityCurrent(false);

    qInfo("Test polygon parameters");
    QVERIFY(original->getIsShadered());
    int n0 = original->getNumPoints();
    QCOMPARE(static_cast<int>(original->getLines()->getMode()), GL_POLYGON);
    QCOMPARE(static_cast<int>(canvas->getGeodePolygons()->getNumChildren()), 1);
    QCOMPARE(canvas->getGeodePolygons()->getChild(0), original.get());

    qInfo("Write scene to file");
    QString fname = QString("RW_PolygonTest.osgt");
    m_rootScene->setFilePath(fname.toStdString());
    QVERIFY(m_rootScene->writeScenetoFile());
    QVERIFY(m_rootScene->isSavedToFile());

    qInfo("Verify polygon params didnt change after the write");
    QVERIFY(original->getIsShadered());
    QCOMPARE(n0, original->getNumPoints());
    QCOMPARE(static_cast<int>(original->getLines()->getMode()), GL_POLYGON);
    QCOMPARE(static_cast<int>(canvas->getGeodePolygons()->getNumChildren()), 1);
    QCOMPARE(canvas->getGeodePolygons()->getChild(0), original.get());

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

    qInfo("Test polygon structure and its relation for canvas");
    QCOMPARE(static_cast<int>(canvas->getGeodePolygons()->getNumChildren()), 1);
    const entity::Polygon* saved = dynamic_cast<const entity::Polygon*>(canvas->getGeodePolygons()->getChild(0));
    QVERIFY(saved);

    QVERIFY(saved->getProgram());
    QCOMPARE(static_cast<int>(saved->getProgram()->getNumShaders()), 2);
    QCOMPARE(saved->getProgram()->getTransform(), canvas->getTransform());
    QVERIFY(saved->getProgram()->getCamera());
    QCOMPARE(saved->getProgram()->getCamera(), m_glWidget->getCamera());
    QCOMPARE(saved->getProgram()->getIsFogged(), this->m_actionStrokeFogFactor->isChecked());

    QVERIFY(saved->getIsShadered());
    QCOMPARE(saved->getNumPoints(), n0);
    QCOMPARE(static_cast<int>(saved->getLines()->getMode()), GL_POLYGON);
}

void PolygonTest::testCopyPaste()
{
    entity::Canvas* canvas = m_scene->getCanvasCurrent();
    QVERIFY(canvas);

    qInfo("Test copy-cut-paste work well for polygon");
    qInfo("Create a polygon");
    osg::ref_ptr<entity::Polygon> original = new entity::Polygon;
    QVERIFY(original.get());
    original->initializeProgram(canvas->getProgramPolygon());

    qInfo("Feed some points to the empty polygon");
    canvas->setEntityCurrent(original.get());
    QVERIFY(canvas->addEntity(original.get()));

    original->appendPoint(0, 0);
    original->appendPoint(0.2, 0.2);
    original->appendPoint(0.4, 0.4);
    original->appendPoint(0.8, 0.9);

    qInfo("Shaderize");
    QVERIFY(original->redefineToShape());
    canvas->setEntityCurrent(false);

    qInfo("Test parameters");
    QVERIFY(original->getIsShadered());
    QCOMPARE(static_cast<int>(original->getLines()->getMode()), GL_POLYGON);
    QCOMPARE(static_cast<int>(canvas->getGeodePolygons()->getNumChildren()), 1);
    QCOMPARE(canvas->getGeodePolygons()->getChild(0), original.get());

    qInfo("Perform selection");
    canvas->addEntitySelected(original.get());
    const auto& entities = canvas->getEntitiesSelected();
    QVERIFY(entities.size() == 1);
    QCOMPARE(entities.at(0), original.get());

    qInfo("Copy the selected to the buffer");
    this->m_rootScene->copyToBuffer();

    qInfo("Test buffer content");
    auto buffer = m_rootScene->getBuffer();
    QVERIFY(buffer.size() == 1);
    auto b0 = buffer.at(0);
    entity::Polygon* buffered = dynamic_cast<entity::Polygon*>(b0.get());
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
    QCOMPARE(static_cast<int>(buffered->getProgram()->getNumShaders()), 2);
    QVERIFY(buffered->getIsShadered());

    qInfo("Perform the paste to the same canvas");
    m_rootScene->pasteFromBuffer();
    QVERIFY(canvas->getNumPolygons() == 2);
    qInfo("Test canvas content and coordinates");
    entity::Polygon* pasted = canvas->getPolygon(1);
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
    QVERIFY(pasted->getIsShadered());

    qInfo("Perform the paste to different canvas");
    m_scene->setCanvasCurrent(m_canvas0.get());
    m_rootScene->pasteFromBuffer();

    qInfo("Test parameters of the second pasted");
    QVERIFY(m_canvas0->getNumPolygons() == 1);
    entity::Polygon* pasted2 = m_canvas0->getPolygon(0);
    QVERIFY(pasted2);
    QCOMPARE(pasted2->getNumPoints(), original->getNumPoints());
    QVERIFY(pasted->getIsShadered());
    QCOMPARE(static_cast<int>(pasted2->getLines()->getMode()), GL_POLYGON);

    qInfo("Test the pasted-2 program settings");
    QVERIFY(pasted2->getProgram());
    QCOMPARE(pasted2->getProgram()->getCamera(), m_glWidget->getCamera());
    QCOMPARE(pasted2->getProgram()->getTransform(), m_canvas0->getTransform());
    QCOMPARE(pasted2->getProgram()->getIsFogged(), this->m_actionStrokeFogFactor->isChecked());
}

void PolygonTest::testFogSwitch()
{
    qInfo("Add couple entities to the scene");
    osg::ref_ptr<entity::Polygon> s1 = new entity::Polygon;
    QVERIFY(m_canvas0->addEntity(s1.get()));
    s1->initializeProgram(m_canvas2->getProgramPolygon());
    s1->appendPoint(0, 0);
    s1->appendPoint(0.2, 0.2);
    s1->appendPoint(0.4, 0.4);
    s1->appendPoint(0.8, 0.9);

    qInfo("Shaderize");
    QVERIFY(s1->redefineToShape(m_canvas0->getTransform()));
    QVERIFY(s1->getProgram());
    bool f0 = this->m_actionStrokeFogFactor->isChecked();
    QCOMPARE(s1->getProgram()->getIsFogged(), this->m_actionStrokeFogFactor->isChecked());

    qInfo("Add second to another canvas");
    osg::ref_ptr<entity::Polygon> s2 = new entity::Polygon;
    QVERIFY(m_canvas1->addEntity(s2.get()));
    s2->initializeProgram(m_canvas2->getProgramPolygon());
    s2->appendPoint(0, 0);
    s2->appendPoint(0.2, 0.2);
    s2->appendPoint(0.4, 0.4);
    s2->appendPoint(0.8, 0.9);

    qInfo("Shaderize");
    QVERIFY(s2->redefineToShape(m_canvas1->getTransform()));
    QVERIFY(s2->getProgram());
    QCOMPARE(s2->getProgram()->getIsFogged(), this->m_actionStrokeFogFactor->isChecked());

    qInfo("Switch the fog factor");
    this->m_actionStrokeFogFactor->setChecked(true);
    this->onStrokeFogFactor();
    QVERIFY(this->m_actionStrokeFogFactor->isChecked() != f0);

    qInfo("Test parameters changed as well");
    QCOMPARE(s1->getProgram()->getIsFogged(), this->m_actionStrokeFogFactor->isChecked());
    QCOMPARE(s2->getProgram()->getIsFogged(), this->m_actionStrokeFogFactor->isChecked());
}

QTEST_MAIN(PolygonTest)
#include "PolygonTest.moc"
