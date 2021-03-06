#include "SceneStateTest.h"

#include <osg/ref_ptr>
#include <QModelIndex>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QSignalSpy>

#include "Data.h"
#include "SceneState.h"
#include "TreeWidget.h"
#include "ListWidget.h"
#include "Bookmarks.h"
#include "Photo.h"

void SceneStateTest::testBasicApi()
{
    qInfo("Test scene state strip");
    osg::ref_ptr<entity::SceneState> state_stripped = new entity::SceneState;
    state_stripped->stripDataFrom(m_rootScene);
    QVERIFY(!state_stripped->isEmpty());

    qInfo("Test scene state initialization");
    osg::ref_ptr<entity::SceneState> state_scened = m_rootScene->createSceneState();
    QVERIFY(!state_scened->isEmpty());

    qInfo("Check both method return the same state scenes");
    QCOMPARE(state_stripped->getAxisFlag(), state_scened->getAxisFlag());
    QCOMPARE(state_stripped->getBookmarksFlag(), state_scened->getBookmarksFlag());
    QCOMPARE(state_stripped->getCanvasDataFlags(), state_scened->getCanvasDataFlags());
    QCOMPARE(state_stripped->getCanvasToolFlags(), state_scened->getCanvasToolFlags());
    QCOMPARE(state_stripped->getPhotoTransparencies(), state_scened->getPhotoTransparencies());

    qInfo("Set some flags of scene state and apply it to Root scene");
    state_stripped->setAxisFlag(false);
    state_stripped->setBookmarksFlag(false);
    state_stripped->popBackDataFlag();
    state_stripped->popBackToolFlag();
    state_stripped->popBackDataFlag();
    state_stripped->popBackToolFlag();
    state_stripped->popBackDataFlag();
    state_stripped->popBackToolFlag();

    state_stripped->pushDataFlag(false);
    state_stripped->pushToolFlag(true);

    state_stripped->pushDataFlag(false);
    state_stripped->pushToolFlag(false);

    state_stripped->pushDataFlag(true);
    state_stripped->pushToolFlag(false);

    m_rootScene->setSceneState(state_stripped);

    QCOMPARE(m_rootScene->getAxesVisibility(), false);
    QCOMPARE(m_rootScene->getBookmarkToolVisibility(), false);

    QVERIFY(m_canvas0.get());
    QCOMPARE(m_canvas0->getVisibilityAll(), false);
    QCOMPARE(m_canvas0->getVisibilityFrameInternal(), true);

    QVERIFY(m_canvas1.get());
    QCOMPARE(m_canvas1->getVisibilityAll(), false);
    QCOMPARE(m_canvas1->getVisibilityFrameInternal(), false);

    QVERIFY(m_canvas2.get());
    QCOMPARE(m_canvas2->getVisibilityAll(), true);
    QCOMPARE(m_canvas2->getVisibilityFrameInternal(), false);

    qInfo("Add a photo and check photo transparency is saved to the state");
    m_rootScene->setCanvasCurrent(m_canvas0.get());
    QCOMPARE(m_rootScene->getCanvasCurrent(), m_canvas0.get());
    QString fname1 = "../../samples/ds-32.bmp";
    m_rootScene->addPhoto(fname1.toStdString());
    QCOMPARE(static_cast<int>(m_rootScene->getCanvasCurrent()->getNumChildren()), 1);
    QCOMPARE(static_cast<int>(m_rootScene->getCanvasCurrent()->getNumPhotos()), 1);
    state_stripped->stripDataFrom(m_rootScene);
    QVERIFY(state_stripped.get());
    QVERIFY(!state_stripped->isEmpty());
    QVERIFY( m_scene->getPhoto(m_scene->getCanvasCurrent(), 0) );
    QCOMPARE(m_rootScene->getUserScene()->getPhoto(m_rootScene->getCanvasCurrent(), 0)->getTransparency(), state_stripped->getPhotoTransparencies()[0]);

    qInfo("Add another photo, change its transparency and check if it's saved successfully within scene state");
    m_rootScene->setCanvasCurrent(m_canvas1.get());
    QCOMPARE(m_rootScene->getCanvasCurrent(), m_canvas1.get());
    QString fname2 = "../../samples/test.bmp";
    m_rootScene->addPhoto(fname2.toStdString());
    QCOMPARE(static_cast<int>(m_rootScene->getCanvasCurrent()->getNumChildren()), 1);
    QCOMPARE(static_cast<int>(m_rootScene->getCanvasCurrent()->getNumPhotos()), 1);
    qInfo("Imitate mouse click within CanvasPhotoWidget delegate to trigger clickedTransparencyMinus signal");
    QSignalSpy spy1(m_canvasWidget->getCanvasDelegate(), SIGNAL(clickedTransparencyMinus(QModelIndex)));
    QTreeWidgetItem* itemCa = m_canvasWidget->topLevelItem(m_rootScene->getUserScene()->getCanvasIndex(m_rootScene->getCanvasCurrent()));
    QVERIFY(itemCa);
    QCOMPARE(itemCa->childCount(), 1);
    m_canvasWidget->expandAll();
    QTreeWidgetItem* itemPh = itemCa->child(0);
    QVERIFY(itemPh);
    QRect rect = m_canvasWidget->visualItemRect(itemPh);
    QCOMPARE(cher::DPI_SCALING, 1.);
    QRect rect_button = m_canvasWidget->getCanvasDelegate()->getButtonMinusRect(rect);
    QTest::mouseClick(m_canvasWidget->viewport(), Qt::LeftButton, 0, rect_button.center());
    QCOMPARE(spy1.count(), 1);
    QList<QVariant> args1 = spy1.takeFirst();
    QVERIFY(args1.at(0).type() == QVariant::ModelIndex);
    entity::Photo* photo = m_rootScene->getCanvasCurrent()->getPhoto(0);
    QVERIFY(photo);
    QCOMPARE(photo->getTransparency(), 1-cher::PHOTO_TRANSPARECY_DELTA);
    state_stripped->stripDataFrom(m_rootScene);
    QVERIFY(!state_stripped->isEmpty());
    QCOMPARE(static_cast<int>(state_stripped->getPhotoTransparencies().size()), 2);
    QCOMPARE(photo->getTransparency(), state_stripped->getPhotoTransparencies()[1]);
}

void SceneStateTest::testBookmarkTaken()
{
    qInfo("Set 0-th canvas as invisible");
    int index = 0;
    QVERIFY(m_canvas0.get());
    QTreeWidgetItem* item = m_canvasWidget->topLevelItem(index);
    QVERIFY(item);

    qInfo("Check it is visible in scene graph");
    QVERIFY(m_canvas0->getVisibilityAll() == true);
    qInfo("Check GUI icon corresponds to visibility");
    QVERIFY(item->data(0, cher::DelegateVisibilityRole).toBool() == false);

    qInfo("swap the visibility to the opposite");
    this->onVisibilitySetCanvas(index);
    qInfo("check it is invisible in scene graph");
    QVERIFY(m_canvas0->getVisibilityAll() == false);
    qInfo("check GUI icon corresponds to invisibility");
    QVERIFY(item->data(0, cher::DelegateVisibilityRole).toBool() == true);
    qInfo("intial scene graph settings");
    QVERIFY(m_rootScene->getAxesVisibility() == true);
    QVERIFY(m_rootScene->getBookmarkToolVisibility() == true);

    qInfo("get current scene state");
    osg::ref_ptr<entity::SceneState> state_old = m_rootScene->createSceneState();
    QVERIFY(state_old.get());
    QVERIFY(!state_old->isEmpty());
    QVERIFY(state_old->getAxisFlag() == true);
    QVERIFY(state_old->getBookmarksFlag() == true);
    QVERIFY(state_old->getCanvasDataFlags()[0] == false);
    QVERIFY(state_old->getCanvasDataFlags()[1] == true);
    QVERIFY(state_old->getCanvasDataFlags()[2] == true);
    QVERIFY(state_old->getCanvasToolFlags()[0] == true);
    QVERIFY(state_old->getCanvasToolFlags()[1] == true);
    QVERIFY(state_old->getCanvasToolFlags()[2] == true);

    qInfo("spy on emitted signals");
    QSignalSpy spy1(m_rootScene->getUserScene()->getBookmarks(), SIGNAL(requestSceneData(entity::SceneState*)));

    qInfo("take a bookmark");
    this->onBookmark();

    qInfo("check if signal was emitted correctly");
    QCOMPARE(spy1.count(), 1);

    qInfo("check if bookmark was added successfully within the scene graph");
    const entity::Bookmarks* bookmarks = m_rootScene->getUserScene()->getBookmarks();
    QVERIFY(bookmarks != NULL);
    QVERIFY(bookmarks->getNumChildren() == 1);
    QVERIFY(bookmarks->getCenters().size() == 1);
    QVERIFY(bookmarks->getEyes().size() == 1);
    QVERIFY(bookmarks->getUps().size() == 1);
    QVERIFY(bookmarks->getNames().size() == 1);
    QVERIFY(bookmarks->getFovs().size() == 1);
    qInfo("within gui");
    QVERIFY(m_bookmarkWidget->count() == 1);

    qInfo("make sure scene state was reset correctly");
    osg::ref_ptr<entity::SceneState> state_new = m_rootScene->createSceneState();
    QVERIFY(m_rootScene->getAxesVisibility() == true);
    QVERIFY(m_rootScene->getBookmarkToolVisibility() == true);
    QVERIFY(m_canvas0->getVisibilityAll() == false);
    QVERIFY(item->data(0, cher::DelegateVisibilityRole).toBool() == true);

    QVERIFY(!state_new->isEmpty());
    QVERIFY(state_new->getAxisFlag() == true);
    QVERIFY(state_new->getBookmarksFlag() == true);
    QVERIFY(state_new->getCanvasDataFlags()[0] == false);
    QVERIFY(state_new->getCanvasDataFlags()[1] == true);
    QVERIFY(state_new->getCanvasDataFlags()[2] == true);
    QVERIFY(state_new->getCanvasToolFlags()[0] == true);
    QVERIFY(state_new->getCanvasToolFlags()[1] == true);
    QVERIFY(state_new->getCanvasToolFlags()[2] == true);
}

void SceneStateTest::testBookmarkClickedOn()
{
    int index = 0;

    QTreeWidgetItem* item = m_canvasWidget->topLevelItem(index);
    QVERIFY(item);

    this->onVisibilitySetCanvas(index);
    QVERIFY(m_canvas0->getVisibilityAll() == false);
    QVERIFY(item->data(0, cher::DelegateVisibilityRole).toBool() == true);
    QVERIFY(m_rootScene->getAxesVisibility() == true);
    QVERIFY(m_rootScene->getBookmarkToolVisibility() == true);

    this->onBookmark();

    QVERIFY(m_rootScene->getAxesVisibility() == true);
    QVERIFY(m_rootScene->getBookmarkToolVisibility() == true);
    QVERIFY(m_canvas0->getVisibilityAll() == false);

    /* reverse the canvas visibility, again */
    this->onVisibilitySetCanvas(index);
    QVERIFY(m_canvas0->getVisibilityAll() == true);
    QVERIFY(item->data(0, cher::DelegateVisibilityRole).toBool() == false);

    /* set up a signal spy to make sure necessary signal is emitted */
    entity::Bookmarks* bookmarks = m_rootScene->getBookmarksModel();
    QVERIFY(bookmarks != NULL);
    QSignalSpy spy1(m_bookmarkWidget, SIGNAL(clicked(QModelIndex)));
    QSignalSpy spy2(bookmarks, SIGNAL(requestBookmarkSet(int)));
    QListWidgetItem* itemBM = m_bookmarkWidget->item(0);
    QVERIFY(itemBM);
    QRect rect = m_bookmarkWidget->visualItemRect(itemBM);

    /* imitate the click on bookmark */
    QTest::mouseClick(m_bookmarkWidget->viewport(), Qt::LeftButton, 0, rect.center());
    /* check for presense of emitted signal */
    QCOMPARE(spy1.count(), 1);
    QList<QVariant> args1 = spy1.takeFirst();
    QVERIFY(args1.at(0).type() == QVariant::ModelIndex);

    QCOMPARE(spy2.count(), 1);
    QList<QVariant> args2 = spy2.takeFirst();
    QVERIFY(args2.at(0).type() == QVariant::Int);

    /* see if scene state was set correctly as in the bookmark */
    QVERIFY(m_canvas0->getVisibilityAll() == false);
    QVERIFY(item->data(0, cher::DelegateVisibilityRole).toBool() == true);
    QVERIFY(m_rootScene->getAxesVisibility() == true);
    QVERIFY(m_rootScene->getBookmarkToolVisibility() == true);
}

void SceneStateTest::testReadWrite()
{
    /* set 0th canvas invisible */
    int index = 0;
    QSignalSpy spy_visibility(m_canvasWidget->getCanvasDelegate(), SIGNAL(clickedVisibilitySet(int)));
    QTreeWidgetItem* item_canvas = m_canvasWidget->topLevelItem(index);
    QVERIFY(item_canvas);
    QRect rect_canvas = m_canvasWidget->visualItemRect(item_canvas);
    QRect rect_visibility = m_canvasWidget->getCanvasDelegate()->getButtonVisibilityRect(rect_canvas);
    QTest::mouseClick(m_canvasWidget->viewport(), Qt::LeftButton, 0, rect_visibility.center());
    QCOMPARE(spy_visibility.count(), 1);
    QVERIFY(!m_canvas0->getVisibilityAll());

    /* take bookmark with this settings */
    QSignalSpy spy_bookmark(m_rootScene->getUserScene()->getBookmarks(), SIGNAL(requestSceneData(entity::SceneState*)));
    this->onBookmark();
    QCOMPARE(spy_bookmark.count(), 1);
    const entity::Bookmarks* bookmarks = m_rootScene->getUserScene()->getBookmarks();
    QVERIFY(bookmarks);
    QCOMPARE(static_cast<int>(bookmarks->getNumChildren()), 1);
    QCOMPARE(m_bookmarkWidget->count(), 1);
    QCOMPARE(static_cast<int>(bookmarks->getSceneState(0)->getCanvasDataFlags().size()), 3);
    QCOMPARE(bookmarks->getSceneState(0)->getCanvasDataFlags()[0], false);
    QCOMPARE(bookmarks->getSceneState(0)->getCanvasDataFlags()[1], true);
    QCOMPARE(bookmarks->getSceneState(0)->getCanvasDataFlags()[2], true);

    /* save scene as a file */
    QString filename = "RW_SceneStateTest.osgt";
    m_rootScene->setFilePath(filename.toStdString());
    QVERIFY(m_rootScene->writeScenetoFile());
    QVERIFY(m_rootScene->isSavedToFile());

    /* clear scene */
    this->onFileClose();

    /* open the saved file */
    m_rootScene->setFilePath(filename.toStdString());
    QVERIFY(m_rootScene->isSetFilePath());
    QVERIFY(this->loadSceneFromFile());
    QCOMPARE(static_cast<int>(m_rootScene->getUserScene()->getNumCanvases()), 3);
    QVERIFY(m_rootScene->getUserScene()->getCanvas(index));
    QVERIFY(!m_rootScene->getUserScene()->getCanvas(index)->getVisibilityAll());

    /* check the scene state */
    QVERIFY(m_rootScene->getBookmarksModel());
    QCOMPARE(static_cast<int>(m_rootScene->getBookmarksModel()->getNumChildren()), 1);
    QCOMPARE(m_bookmarkWidget->count(), 1);
    entity::SceneState* state = m_rootScene->getBookmarksModel()->getSceneState(0);
    QVERIFY(state);
    QCOMPARE(static_cast<int>(state->getCanvasDataFlags().size()), 3);
    QCOMPARE(state->getCanvasDataFlags()[0], false);
    QCOMPARE(state->getCanvasDataFlags()[1], true);
    QCOMPARE(state->getCanvasDataFlags()[2], true);
}

void SceneStateTest::testAddCanvas()
{
    QCOMPARE(static_cast<int>(m_rootScene->getUserScene()->getNumCanvases()), 3);
    /* set 0-th canvas as invisible */
    int index = 0;
    this->onVisibilitySetCanvas(index);
    QVERIFY(m_canvas0->getVisibilityAll() == false);

    /* take a bookmark */
    this->onBookmark();
    const entity::SceneState* state = m_rootScene->getUserScene()->getBookmarks()->getSceneState(0);
    QVERIFY(state);
    QCOMPARE(static_cast<int>(state->getCanvasDataFlags().size()), 3);
    QCOMPARE(static_cast<int>(state->getCanvasToolFlags().size()), 3);
    QCOMPARE(state->getCanvasDataFlags()[0], false);
    QCOMPARE(state->getCanvasToolFlags()[0], true);

    /* add new canvas to the scene */
    m_rootScene->addCanvas(osg::Vec3f(1,0,0), osg::Vec3f(1,1,1));
    QCOMPARE(static_cast<int>(m_rootScene->getUserScene()->getNumCanvases()), 4);
    QCOMPARE(static_cast<int>(state->getCanvasDataFlags().size()), 4);
    QCOMPARE(static_cast<int>(state->getCanvasToolFlags().size()), 4);
    QCOMPARE(state->getCanvasDataFlags()[0], false);
    QCOMPARE(state->getCanvasToolFlags()[0], true);
    QCOMPARE(state->getCanvasDataFlags()[3], true);
    QCOMPARE(state->getCanvasToolFlags()[3], true);

    /* do undo */
    m_undoStack->undo();
    QCOMPARE(static_cast<int>(m_rootScene->getUserScene()->getNumCanvases()), 3);
    QCOMPARE(static_cast<int>(state->getCanvasDataFlags().size()), 3);
    QCOMPARE(static_cast<int>(state->getCanvasToolFlags().size()), 3);
    QCOMPARE(state->getCanvasDataFlags()[0], false);
    QCOMPARE(state->getCanvasToolFlags()[0], true);

    /* do redo */
    m_undoStack->redo();
    QCOMPARE(static_cast<int>(m_rootScene->getUserScene()->getNumCanvases()), 4);
    QCOMPARE(static_cast<int>(state->getCanvasDataFlags().size()), 4);
    QCOMPARE(static_cast<int>(state->getCanvasToolFlags().size()), 4);
    QCOMPARE(state->getCanvasDataFlags()[0], false);
    QCOMPARE(state->getCanvasToolFlags()[0], true);
    QCOMPARE(state->getCanvasDataFlags()[3], true);
    QCOMPARE(state->getCanvasToolFlags()[3], true);

    /* check setting scene state does not fail */
    QVERIFY(m_rootScene->setSceneState(state));
}

void SceneStateTest::testAddPhoto()
{
    QCOMPARE(static_cast<int>(m_rootScene->getUserScene()->getNumCanvases()), 3);
    /* set 0-th canvas as invisible */
    int index = 0;
    this->onVisibilitySetCanvas(index);
    QVERIFY(m_canvas0->getVisibilityAll() == false);

    /* take a bookmark */
    this->onBookmark();
    const entity::SceneState* state = m_rootScene->getUserScene()->getBookmarks()->getSceneState(0);
    QVERIFY(state);
    QCOMPARE(static_cast<int>(state->getCanvasDataFlags().size()), 3);
    QCOMPARE(state->getCanvasDataFlags()[0], false);
    QCOMPARE(state->getCanvasToolFlags()[0], true);

    /* add photo to 1st canvas */
    m_rootScene->setCanvasCurrent(m_canvas1.get());
    QCOMPARE(m_rootScene->getCanvasCurrent(), m_canvas1.get());
    QString filename1 = "../../samples/ds-32.bmp";
    m_rootScene->addPhoto(filename1.toStdString());
    QCOMPARE(static_cast<int>(m_canvas1->getNumPhotos()), 1);

    /* make sure the transparency is a part of scene states */
    QCOMPARE(static_cast<int>(state->getPhotoTransparencies().size()), 1);
    QCOMPARE(state->getPhotoTransparencies()[0], 1.);

    /* add another photo to second canvas */
    m_rootScene->setCanvasCurrent(m_canvas2.get());
    QCOMPARE(m_rootScene->getCanvasCurrent(), m_canvas2.get());
    QString filename2 = "../../samples/test.bmp";
    m_rootScene->addPhoto(filename2.toStdString());
    QCOMPARE(static_cast<int>(m_canvas2->getNumPhotos()), 1);

    /* make sure the transparency is a part of scene states */
    QCOMPARE(static_cast<int>(state->getPhotoTransparencies().size()), 2);
    QCOMPARE(state->getPhotoTransparencies()[1], 1.);

    /* check setting scene state does not fail */
    QVERIFY(m_rootScene->setSceneState(state));

    /* perform undo 2 times */
    m_undoStack->undo();
    m_undoStack->undo();
    QCOMPARE(static_cast<int>( state->getPhotoTransparencies().size()), 0);

    /* check setting scene state does not fail */
    QVERIFY(m_rootScene->setSceneState(state));

    /* perform a redo */
    m_undoStack->redo();
    QCOMPARE(static_cast<int>( state->getPhotoTransparencies().size()), 1);

    /* check setting scene state does not fail */
    QVERIFY(m_rootScene->setSceneState(state));
}

void SceneStateTest::testDeleteCanvas()
{
    /* set 0-th canvas as invisible */
    this->onVisibilitySetCanvas(0);
    QVERIFY(m_canvas0->getVisibilityAll() == false);

    /* add photo to 0th canvas */
    m_rootScene->setCanvasCurrent(m_canvas0.get());
    QCOMPARE(m_rootScene->getCanvasCurrent(), m_canvas0.get());
    QString filename0 = "../../samples/ds-32.bmp";
    m_rootScene->addPhoto(filename0.toStdString());
    QCOMPARE(static_cast<int>(m_canvas0->getNumPhotos()), 1);

    /* take a bookmark */
    this->onBookmark();
    const entity::SceneState* state = m_rootScene->getUserScene()->getBookmarks()->getSceneState(0);
    QVERIFY(state);
    QCOMPARE(static_cast<int>(state->getCanvasDataFlags().size()), 3);
    QCOMPARE(static_cast<int>(state->getCanvasToolFlags().size()), 3);
    QCOMPARE(static_cast<int>(state->getPhotoTransparencies().size()), 1);
    QCOMPARE(state->getCanvasDataFlags()[0], false);
    QCOMPARE(state->getCanvasToolFlags()[0], true);

    /* add another photo to the same canvas */
    QCOMPARE(m_rootScene->getCanvasCurrent(), m_canvas0.get());
    QString filename1 = "../../samples/test.bmp";
    m_rootScene->addPhoto(filename1.toStdString());
    QCOMPARE(static_cast<int>(m_canvas0->getNumPhotos()), 2);
    QCOMPARE(static_cast<int>(state->getPhotoTransparencies().size()), 2);
    /* check setting scene state does not fail */
    QVERIFY(m_rootScene->setSceneState(state));

    /* delete 0th canvas from scene */
    m_rootScene->editCanvasDelete(m_canvas0.get());
    QCOMPARE(static_cast<int>(m_rootScene->getUserScene()->getNumCanvases()), 2);
    QCOMPARE(static_cast<int>(state->getCanvasDataFlags().size()), 2);
    QCOMPARE(static_cast<int>( state->getCanvasToolFlags().size()), 2);
    QCOMPARE(static_cast<int>(state->getPhotoTransparencies().size()), 0);
    QCOMPARE(static_cast<int>(m_rootScene->getUserScene()->getNumPhotos()), 0);
    /* check setting scene state does not fail */
    QVERIFY(m_rootScene->setSceneState(state));

    /* perform undo */
    m_undoStack->undo();
    QCOMPARE(static_cast<int>(state->getCanvasDataFlags().size()), 3);
    QCOMPARE(static_cast<int>(state->getCanvasToolFlags().size()), 3);
    QCOMPARE(static_cast<int>(state->getPhotoTransparencies().size()), 2);
    /* check setting scene state does not fail */
    QVERIFY(m_rootScene->setSceneState(state));

    /* perform redo */
    m_undoStack->redo();
    QCOMPARE(static_cast<int>(m_rootScene->getUserScene()->getNumCanvases()), 2);
    QCOMPARE(static_cast<int>(state->getCanvasDataFlags().size()), 2);
    QCOMPARE(static_cast<int>( state->getCanvasToolFlags().size()), 2);
    QCOMPARE(static_cast<int>(state->getPhotoTransparencies().size()), 0);
    QVERIFY(m_rootScene->setSceneState(state));
}

void SceneStateTest::testDeletePhoto()
{
    m_rootScene->setCanvasCurrent(m_canvas0.get());
    QCOMPARE(m_rootScene->getCanvasCurrent(), m_canvas0.get());

    /* add photo to 0th canvas */
    m_rootScene->setCanvasCurrent(m_canvas0.get());
    QCOMPARE(m_rootScene->getCanvasCurrent(), m_canvas0.get());
    QString filename0 = "../../samples/ds-32.bmp";
    m_rootScene->addPhoto(filename0.toStdString());
    QCOMPARE(static_cast<int>(m_canvas0->getNumPhotos()), 1);

    /* take a bookmark */
    this->onBookmark();
    const entity::SceneState* state = m_rootScene->getUserScene()->getBookmarks()->getSceneState(0);
    QVERIFY(state);
    QCOMPARE(static_cast<int>(state->getCanvasDataFlags().size()), 3);
    QCOMPARE(static_cast<int>(state->getCanvasToolFlags().size()), 3);
    QCOMPARE(static_cast<int>(state->getPhotoTransparencies().size()), 1);
    QCOMPARE(state->getCanvasDataFlags()[0], true);
    QCOMPARE(state->getCanvasToolFlags()[0], true);

    /* add another photo to the same canvas */
    QCOMPARE(m_rootScene->getCanvasCurrent(), m_canvas0.get());
    QString filename1 = "../../samples/test.bmp";
    m_rootScene->addPhoto(filename1.toStdString());
    QCOMPARE(static_cast<int>(m_canvas0->getNumPhotos()), 2);
    QCOMPARE(static_cast<int>(state->getPhotoTransparencies().size()), 2);
    /* check setting scene state does not fail */
    QVERIFY(m_rootScene->setSceneState(state));

    /* imitate mouse click within CanvasPhotoWidget delegate to trigger clickedTransparencyMinus signal */
    QSignalSpy spy_minus(m_canvasWidget->getCanvasDelegate(), SIGNAL(clickedTransparencyMinus(QModelIndex)));
    QTreeWidgetItem* item = m_canvasWidget->topLevelItem(m_rootScene->getUserScene()->getCanvasIndex(m_canvas0.get()));
    QVERIFY(item);
    QCOMPARE(item->childCount(), 2);
    m_canvasWidget->expandAll();
    QTreeWidgetItem* child = item->child(1);
    QVERIFY(child);
    QCOMPARE(child->text(0), QString("Photo1"));
    QRect rect = m_canvasWidget->visualItemRect(child);
    QCOMPARE(cher::DPI_SCALING, 1.);
    QRect rect_button = m_canvasWidget->getCanvasDelegate()->getButtonMinusRect(rect);
    QTest::mouseClick(m_canvasWidget->viewport(), Qt::LeftButton, 0, rect_button.center());
    QCOMPARE(spy_minus.count(), 1);
    QList<QVariant> args = spy_minus.takeFirst();
    QVERIFY(args.at(0).type() == QVariant::ModelIndex);

    QCOMPARE(static_cast<int>(state->getPhotoTransparencies().size()), 2);
    entity::Photo* photo0 = m_canvas0->getPhoto(0);
    QVERIFY(photo0);
    QCOMPARE(photo0->getTransparency(), 1.);
    QCOMPARE(photo0->getTransparency(), state->getPhotoTransparencies()[0]);
    entity::Photo* photo1 = m_canvas0->getPhoto(1);
    QVERIFY(photo1);
    QCOMPARE(photo1->getTransparency(), 1-cher::PHOTO_TRANSPARECY_DELTA);
    QCOMPARE(photo1->getTransparency(), state->getPhotoTransparencies()[1]);

    /* delete 0th photo, with name ds-32.bmp */
    QSignalSpy spy_delete0(m_rootScene->getUserScene(), SIGNAL(photoRemoved(int,int)));
    m_rootScene->editPhotoDelete(photo0, m_canvas0.get());
    QCOMPARE(spy_delete0.count(), 1);
    QCOMPARE(static_cast<int>(state->getPhotoTransparencies().size()), 1);
    QVERIFY(photo1);
    QCOMPARE(photo1->getTransparency(), state->getPhotoTransparencies()[0]);
    QVERIFY(item);
    QCOMPARE(item->childCount(), 1);

    /* delete 1st photo, with name test.bmp */
    QSignalSpy spy_delete1(m_rootScene->getUserScene(), SIGNAL(photoRemoved(int,int)));
    m_rootScene->editPhotoDelete(photo1, m_canvas0.get());
    QCOMPARE(spy_delete1.count(), 1);
    QCOMPARE(static_cast<int>(state->getPhotoTransparencies().size()), 0);
    QVERIFY(item);
    QCOMPARE(item->childCount(), 0);

    /* perform undo (return 1st photo with name test.bmp) */
    m_undoStack->undo();
    QCOMPARE(static_cast<int>(m_scene->getNumPhotos()), 1);
    photo1 = m_scene->getPhoto(m_canvas0.get(), 0);
    QVERIFY(photo1);
    QCOMPARE(photo1->getName().c_str(), "Photo1");
    QCOMPARE(photo1->getTransparency(), 1-cher::PHOTO_TRANSPARECY_DELTA);
    QVERIFY(item);
    QCOMPARE(item->childCount(), 1);
    QVERIFY(state);
    QCOMPARE(static_cast<int>(state->getPhotoTransparencies().size()), 1);
    QCOMPARE(state->getPhotoTransparencies()[0], photo1->getTransparency());
    child = item->child(0);
    QVERIFY(child);
    QCOMPARE(child->text(0), QString("Photo1"));

}

QTEST_MAIN(SceneStateTest)
#include "SceneStateTest.moc"
