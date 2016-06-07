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
    osg::ref_ptr<entity::SceneState> state_stripped = new entity::SceneState;
    state_stripped->stripDataFrom(m_rootScene);
    QVERIFY(!state_stripped->isEmpty());

    osg::ref_ptr<entity::SceneState> state_scened = m_rootScene->createSceneState();
    QVERIFY(!state_scened->isEmpty());

    /* check both method return the same state scenes */
    QCOMPARE(state_stripped->getAxisFlag(), state_scened->getAxisFlag());
    QCOMPARE(state_stripped->getBookmarksFlag(), state_scened->getBookmarksFlag());
    QCOMPARE(state_stripped->getCanvasDataFlags(), state_scened->getCanvasDataFlags());
    QCOMPARE(state_stripped->getCanvasToolFlags(), state_scened->getCanvasToolFlags());
    QCOMPARE(state_stripped->getPhotoTransparencies(), state_scened->getPhotoTransparencies());

    /* set some flags of scene state and apply it to Root scene */
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

    QVERIFY(m_rootScene->getUserScene()->getCanvas(0));
    QCOMPARE(m_rootScene->getUserScene()->getCanvas(0)->getVisibilityAll(), false);
    QCOMPARE(m_rootScene->getUserScene()->getCanvas(0)->getVisibilityData(), false);
    QCOMPARE(m_rootScene->getUserScene()->getCanvas(0)->getVisibilityFrame(), false);
    QCOMPARE(m_rootScene->getUserScene()->getCanvas(0)->getVisibilityFrameInternal(), true);

    QVERIFY(m_rootScene->getUserScene()->getCanvas(1));
    QCOMPARE(m_rootScene->getUserScene()->getCanvas(1)->getVisibilityAll(), false);
    QCOMPARE(m_rootScene->getUserScene()->getCanvas(1)->getVisibilityData(), false);
    QCOMPARE(m_rootScene->getUserScene()->getCanvas(1)->getVisibilityFrame(), false);
    QCOMPARE(m_rootScene->getUserScene()->getCanvas(1)->getVisibilityFrameInternal(), false);

    QVERIFY(m_rootScene->getUserScene()->getCanvas(2));
    QCOMPARE(m_rootScene->getUserScene()->getCanvas(2)->getVisibilityAll(), true);
    QCOMPARE(m_rootScene->getUserScene()->getCanvas(2)->getVisibilityData(), true);
    QCOMPARE(m_rootScene->getUserScene()->getCanvas(2)->getVisibilityFrame(), true);
    QCOMPARE(m_rootScene->getUserScene()->getCanvas(2)->getVisibilityFrameInternal(), false);

    /* add a photo and check photo transparency is saved to the state */
    m_rootScene->setCanvasCurrent(m_rootScene->getUserScene()->getCanvas(0));
    QCOMPARE(m_rootScene->getCanvasCurrent(), m_rootScene->getUserScene()->getCanvas(0));
    QString fname1 = "../../samples/ds-32.bmp";
    m_rootScene->addPhoto(fname1.toStdString());
    QCOMPARE((int)m_rootScene->getCanvasCurrent()->getNumChildren(), 1);
    QCOMPARE(m_rootScene->getCanvasCurrent()->getNumPhotos(), 1);
    state_stripped->stripDataFrom(m_rootScene);
    QVERIFY(!state_stripped->isEmpty());
    QCOMPARE(m_rootScene->getUserScene()->getPhotoFromIndex(m_rootScene->getCanvasCurrent(), 0)->getTransparency(), state_stripped->getPhotoTransparencies()[0]);

    /* add another photo, change its transparency and check if it's saved successfully within scene state */
    m_rootScene->setCanvasCurrent(m_rootScene->getUserScene()->getCanvas(1));
    QCOMPARE(m_rootScene->getCanvasCurrent(), m_rootScene->getUserScene()->getCanvas(1));
    QString fname2 = "../../samples/test.bmp";
    m_rootScene->addPhoto(fname2.toStdString());
    QCOMPARE((int)m_rootScene->getCanvasCurrent()->getNumChildren(), 1);
    QCOMPARE(m_rootScene->getCanvasCurrent()->getNumPhotos(), 1);
    /* imitate mouse click within CanvasPhotoWidget delegate to trigger clickedTransparencyMinus signal */
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
    entity::Photo* photo = m_rootScene->getCanvasCurrent()->getPhotoFromIndex(0);
    QVERIFY(photo);
    QCOMPARE(photo->getTransparency(), 1-cher::PHOTO_TRANSPARECY_DELTA);
    state_stripped->stripDataFrom(m_rootScene);
    QVERIFY(!state_stripped->isEmpty());
    QCOMPARE((int)state_stripped->getPhotoTransparencies().size(), 2);
    QCOMPARE(photo->getTransparency(), state_stripped->getPhotoTransparencies()[1]);
}

void SceneStateTest::testBookmarkTaken()
{
    /* set 0-th canvas as invisible */
    int index = 0;
    entity::Canvas* canvas = m_rootScene->getUserScene()->getCanvas(index);
    QVERIFY(canvas != 0);
    QTreeWidgetItem* item = m_canvasWidget->topLevelItem(index);
    QVERIFY(item);

    /* check it is visible in scene graph */
    QVERIFY(canvas->getVisibilityAll() == true);
    /* check GUI icon corresponds to visibility */
    QVERIFY(item->data(0, cher::DelegateVisibilityRole).toBool() == false);

    /* swap the visibility to the opposite */
    this->onVisibilitySetCanvas(index);
    /* check it is invisible in scene graph */
    QVERIFY(canvas->getVisibilityAll() == false);
    /* check GUI icon corresponds to invisibility */
    QVERIFY(item->data(0, cher::DelegateVisibilityRole).toBool() == true);
    /* intial scene graph settings */
    QVERIFY(m_rootScene->getAxesVisibility() == true);
    QVERIFY(m_rootScene->getBookmarkToolVisibility() == true);

    /* get current scene state */
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

    /* spy on emitted signals */
    QSignalSpy spy1(m_rootScene->getUserScene()->getBookmarks(), SIGNAL(requestSceneData(entity::SceneState*)));

    /* take a bookmark */
    this->onBookmark();

    /* check if signal was emitted correctly */
    QCOMPARE(spy1.count(), 1);

    /* check if bookmark was added successfully */
    /* within the scene graph */
    const entity::Bookmarks* bookmarks = m_rootScene->getUserScene()->getBookmarks();
    QVERIFY(bookmarks != NULL);
    QVERIFY(bookmarks->getNumChildren() == 1);
    QVERIFY(bookmarks->getCenters().size() == 1);
    QVERIFY(bookmarks->getEyes().size() == 1);
    QVERIFY(bookmarks->getUps().size() == 1);
    QVERIFY(bookmarks->getNames().size() == 1);
    QVERIFY(bookmarks->getFovs().size() == 1);
    /* within gui */
    QVERIFY(m_bookmarkWidget->count() == 1);

    /* make sure scene state was reset correctly */
    osg::ref_ptr<entity::SceneState> state_new = m_rootScene->createSceneState();
    QVERIFY(m_rootScene->getAxesVisibility() == true);
    QVERIFY(m_rootScene->getBookmarkToolVisibility() == true);
    QVERIFY(canvas->getVisibilityAll() == false);
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
    entity::Canvas* canvas = m_rootScene->getUserScene()->getCanvas(index);
    QVERIFY(canvas != 0);

    QTreeWidgetItem* item = m_canvasWidget->topLevelItem(index);
    QVERIFY(item);

    this->onVisibilitySetCanvas(index);
    QVERIFY(canvas->getVisibilityAll() == false);
    QVERIFY(item->data(0, cher::DelegateVisibilityRole).toBool() == true);
    QVERIFY(m_rootScene->getAxesVisibility() == true);
    QVERIFY(m_rootScene->getBookmarkToolVisibility() == true);

    this->onBookmark();

    QVERIFY(m_rootScene->getAxesVisibility() == true);
    QVERIFY(m_rootScene->getBookmarkToolVisibility() == true);
    QVERIFY(canvas->getVisibilityAll() == false);

    /* reverse the canvas visibility, again */
    this->onVisibilitySetCanvas(index);
    QVERIFY(canvas->getVisibilityAll() == true);
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
    QVERIFY(canvas->getVisibilityAll() == false);
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
    entity::Canvas* canvas = m_rootScene->getUserScene()->getCanvas(index);
    QVERIFY(canvas);
    QVERIFY(!canvas->getVisibilityAll());

    /* take bookmark with this settings */
    QSignalSpy spy_bookmark(m_rootScene->getUserScene()->getBookmarks(), SIGNAL(requestSceneData(entity::SceneState*)));
    this->onBookmark();
    QCOMPARE(spy_bookmark.count(), 1);
    const entity::Bookmarks* bookmarks = m_rootScene->getUserScene()->getBookmarks();
    QVERIFY(bookmarks);
    QCOMPARE((int)bookmarks->getNumChildren(), 1);
    QCOMPARE(m_bookmarkWidget->count(), 1);
    QCOMPARE((int)bookmarks->getSceneState(0)->getCanvasDataFlags().size(), 3);
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
    QVERIFY(m_rootScene->loadSceneFromFile());
    this->initializeCallbacks();
    m_rootScene->resetBookmarks(m_bookmarkWidget);
    m_rootScene->getUserScene()->resetModel(m_canvasWidget);
    QVERIFY(!m_rootScene->getUserScene()->getCanvas(index)->getVisibilityAll());

    /* check the scene state */
    QVERIFY(m_rootScene->getBookmarksModel());
    QCOMPARE((int)m_rootScene->getBookmarksModel()->getNumChildren(), 1);
    QCOMPARE(m_bookmarkWidget->count(), 1);
    entity::SceneState* state = m_rootScene->getBookmarksModel()->getSceneState(0);
    QVERIFY(state);
    QCOMPARE((int)state->getCanvasDataFlags().size(), 3);
    QCOMPARE(state->getCanvasDataFlags()[0], false);
    QCOMPARE(state->getCanvasDataFlags()[1], true);
    QCOMPARE(state->getCanvasDataFlags()[2], true);
}

QTEST_MAIN(SceneStateTest)
#include "SceneStateTest.moc"
