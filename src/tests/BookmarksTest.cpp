#include "BookmarksTest.h"

#include <QSignalSpy>
#include <QList>
#include <QListWidgetItem>
#include <QIcon>

#include "Stroke.h"
#include "Bookmarks.h"


void BookmarksTest::testAddBookmark()
{
    qInfo("Prepare basic scene");
    qInfo("Add stroke");
    osg::ref_ptr<entity::Stroke> stroke = new entity::Stroke;
    stroke->initializeProgram(m_canvas2->getProgramStroke());
    QVERIFY(stroke.get());
    stroke->appendPoint(0,0);
    stroke->appendPoint(0.5,0.5);
    stroke->appendPoint(1,1);
    stroke->appendPoint(2,2);
    QVERIFY(m_canvas2->addEntity(stroke));

    qInfo("Re-define the stroke look");
    QVERIFY(stroke->redefineToCurve());

    qInfo("Check bookmark widget is empty");
    QCOMPARE( m_bookmarkWidget->count(), 0);

    qInfo("Save view as bookmark");
    this->onBookmark();

    const entity::Bookmarks* bs = m_scene->getBookmarks();
    QVERIFY(bs);
    qInfo("Check internal vector sizes");
    QVERIFY(bs->getNumBookmarks() == 1);
    QVERIFY(bs->getEyes().size() == 1);
    QVERIFY(bs->getCenters().size() == 1);
    QVERIFY(bs->getUps().size() == 1);
    QVERIFY(bs->getNames().size() == 1);
    QVERIFY(bs->getFovs().size() == 1);

    qInfo("Check the widget");
    QCOMPARE(m_bookmarkWidget->count(), 1);
    QListWidgetItem* item = m_bookmarkWidget->item(0);
    QVERIFY(item);
    QCOMPARE( item->text(), QString("Bookmark0"));
    QIcon icon = item->icon();
    QVERIFY(!icon.isNull());

    qInfo("Check screenshot grab functionality");
    QPixmap pmap = this->getScreenshot(bs->getEyes().at(0), bs->getCenters().at(0), bs->getUps().at(0));
    QVERIFY(!pmap.isNull());
    QVERIFY(!isWhite(pmap));



}

bool BookmarksTest::isWhite(const QPixmap &pmap)
{
    QImage image = pmap.toImage();
    for (int i=0; i<image.width(); ++i){
        for (int j=0; j<image.height(); ++j){
            if (image.pixel(i,j) != image.pixel(0,0))
                return false;
        }
    }
    return true;
}

QTEST_MAIN(BookmarksTest)
#include "BookmarksTest.moc"
