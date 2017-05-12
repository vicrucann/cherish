#include "UserSceneTest.h"


void UserSceneTest::testWriteReadCanvases()
{
    /* test RW for canvases */
    QCOMPARE(static_cast<int>(m_scene->getNumCanvases()), 3);
    QCOMPARE(static_cast<int>(m_scene->getNumChildren()), 2);
    QVERIFY(m_scene->getChild(1));
    QCOMPARE(m_scene->getGroupCanvases(), m_scene->getChild(1));
    QVERIFY(m_scene->getChild(0));
    QCOMPARE(m_scene->getBookmarks(), m_scene->getChild(0));
    QCOMPARE(m_canvas0.get(), m_scene->getCanvas(0));
    QCOMPARE(m_canvas1.get(), m_scene->getCanvas(1));
    QCOMPARE(m_canvas2.get(), m_scene->getCanvas(2));
    QCOMPARE(m_canvasWidget->topLevelItemCount(), 3);

    /* write scene to file */
    QString fname_canvas = QString("RW_UserSceneTest_canvases.osgt");
    m_rootScene->setFilePath(fname_canvas.toStdString());
    QVERIFY(m_rootScene->writeScenetoFile());
    QVERIFY(m_rootScene->isSavedToFile());
    /* clear scene */
    this->onFileClose();
    /* re-open the saved file */
    m_rootScene->setFilePath(fname_canvas.toStdString());
    QVERIFY(m_rootScene->isSetFilePath());
    QVERIFY(this->loadSceneFromFile());
    m_scene = m_rootScene->getUserScene();
    QVERIFY(m_scene.get());

    /* test the scene graph */
    QCOMPARE(static_cast<int>(m_scene->getNumCanvases()), 3);
    m_canvas0 = m_scene->getCanvas(0);
    QVERIFY(m_canvas0.get());
    m_canvas1 = m_scene->getCanvas(1);
    QVERIFY(m_canvas1.get());
    m_canvas2 = m_scene->getCanvas(2);
    QVERIFY(m_canvas2.get());
    QCOMPARE(static_cast<int>(m_scene->getNumChildren()), 2);
    QVERIFY(m_scene->getChild(0));
    QCOMPARE(m_scene->getBookmarks(), m_scene->getChild(0));
    QVERIFY(m_scene->getChild(1));
    QCOMPARE(m_scene->getGroupCanvases(), m_scene->getChild(1));
    m_scene->setCanvasCurrent(m_canvas1.get());
    m_scene->setCanvasCurrent(m_canvas0.get());
    QCOMPARE(m_scene->getCanvasCurrent(), m_canvas0.get());
    QCOMPARE(m_scene->getCanvasPrevious(), m_canvas1.get());
    QCOMPARE(m_canvasWidget->topLevelItemCount(), 3);

    /* test scene graph of a canvas0 */
    QCOMPARE(static_cast<int>(m_canvas0->getNumChildren()), 1);
    QVERIFY(m_canvas0->getTransform());
    QVERIFY(m_canvas0->getChild(0));
    QCOMPARE(m_canvas0->getChild(0), m_canvas0->getTransform());
    QCOMPARE(static_cast<int>(m_canvas0->getTransform()->getNumChildren()), 1);
    QVERIFY(m_canvas0->getSwitch());
    QVERIFY(m_canvas0->getTransform()->getChild(0));
    QCOMPARE(m_canvas0->getSwitch(), m_canvas0->getTransform()->getChild(0));
    QCOMPARE(static_cast<int>(m_canvas0->getSwitch()->getNumChildren()), 2);
    QVERIFY(m_canvas0->getSwitch()->getChild(0));
    QCOMPARE(m_canvas0->getSwitch()->getChild(0), m_canvas0->getGroupData());
    QVERIFY(m_canvas0->getSwitch()->getChild(1));
    QCOMPARE(m_canvas0->getSwitch()->getChild(1), m_canvas0->getToolFrame());
    QCOMPARE(static_cast<int>(m_canvas0->getGroupData()->getNumChildren()), 4);
    QVERIFY(m_canvas0->getGroupData());
    QVERIFY(m_canvas0->getGroupData()->getChild(0));
    QCOMPARE(m_canvas0->getGroupData()->getChild(0), m_canvas0->getGeodeStrokes());
    QVERIFY(m_canvas0->getGroupData()->getChild(1));
    QCOMPARE(m_canvas0->getGroupData()->getChild(1), m_canvas0->getGeodePhotos());
}

void UserSceneTest::testWriteReadBookmarks()
{
    /* add a photo to canvas */
    m_scene->setCanvasCurrent(m_canvas0.get());
    QString filename_photo = "../../samples/ds-32.bmp";
    m_rootScene->addPhoto(filename_photo.toStdString());
    QCOMPARE(static_cast<int>(m_canvas0->getNumPhotos()), 1);

    /* take bookmark */
    this->onBookmark();
    entity::Bookmarks* bookmarks = m_scene->getBookmarksModel();
    QVERIFY(bookmarks);
    QCOMPARE(static_cast<int>(bookmarks->getNumChildren()), 1);
    QCOMPARE(m_bookmarkWidget->count(), 1);
    entity::SceneState* state = bookmarks->getSceneState(0);
    QVERIFY(state);
    QCOMPARE(static_cast<int>(state->getCanvasDataFlags().size()), 3);
    QCOMPARE(static_cast<int>(state->getCanvasToolFlags().size()), 3);
    QCOMPARE(static_cast<int>(state->getPhotoTransparencies().size()), 1);

    /* save scene as file */
    QString filename = "RW_UserSceneTest_bookmarks.osgt";
    m_rootScene->setFilePath(filename.toStdString());
    QVERIFY(m_rootScene->writeScenetoFile());
    QVERIFY(m_rootScene->isSavedToFile());
    /* clear scene */
    this->onFileClose();

    /* open the saved file */
    m_rootScene->setFilePath(filename.toStdString());
    QVERIFY(m_rootScene->isSetFilePath());
    QVERIFY(this->loadSceneFromFile());
    m_scene = m_rootScene->getUserScene();
    QVERIFY(m_scene.get());
    QCOMPARE(static_cast<int>(m_scene->getNumCanvases()), 3);
    QCOMPARE(static_cast<int>(m_scene->getNumPhotos()), 1);

    /* test if bookmarks data loaded correctly */
    bookmarks = m_scene->getBookmarksModel();
    QVERIFY(bookmarks);
    QCOMPARE(static_cast<int>(bookmarks->getNumChildren()), 1);
    QCOMPARE(m_bookmarkWidget->count(), 1);
    state = bookmarks->getSceneState(0);
    QVERIFY(state);
    QCOMPARE(static_cast<int>(state->getCanvasDataFlags().size()), 3);
    QCOMPARE(static_cast<int>(state->getCanvasToolFlags().size()), 3);
    QCOMPARE(static_cast<int>(state->getPhotoTransparencies().size()), 1);
    QCOMPARE(m_bookmarkWidget->count(), 1);
}

QTEST_MAIN(UserSceneTest)
#include "UserSceneTest.moc"
