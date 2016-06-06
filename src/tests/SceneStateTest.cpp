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
    osg::ref_ptr<entity::SceneState> state_old = m_rootScene->getSceneState();
    QVERIFY(state_old.get());
    QVERIFY(state_old->isEmpty() == false);
    QVERIFY(state_old->getAxisFlag() == true);
    QVERIFY(state_old->getBookmarksFlag() == true);
    QVERIFY(state_old->getCanvasDataFlags()[0] == false);
    QVERIFY(state_old->getCanvasDataFlags()[1] == true);
    QVERIFY(state_old->getCanvasDataFlags()[2] == true);
    QVERIFY(state_old->getCanvasToolFlags()[0] == true);
    QVERIFY(state_old->getCanvasToolFlags()[1] == true);
    QVERIFY(state_old->getCanvasToolFlags()[2] == true);

    /* take a bookmark */
    this->onBookmark();

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
    osg::ref_ptr<entity::SceneState> state_new = m_rootScene->getSceneState();
    QVERIFY(m_rootScene->getAxesVisibility() == true);
    QVERIFY(m_rootScene->getBookmarkToolVisibility() == true);
    QVERIFY(canvas->getVisibilityAll() == false);
    QVERIFY(item->data(0, cher::DelegateVisibilityRole).toBool() == true);

    QVERIFY(state_new->isEmpty() == false);
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

QTEST_MAIN(SceneStateTest)
#include "SceneStateTest.moc"
