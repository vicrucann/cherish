#include "BookmarksTest.h"

#include <QSignalSpy>
#include <QList>
#include <QListWidgetItem>
#include <QIcon>

#include "Stroke.h"
#include "Bookmarks.h"
#include "SVMData.h"
#include "DraggableWire.h"

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
    QVERIFY(stroke->redefineToShape());

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

void BookmarksTest::testNewBookmark()
{
    qInfo("Test bookmark calculation through SVM data manipulation. ");
    qInfo("Create new bookmark and add it to the widgets.");
    osg::Vec3d eye, center, up;
    double fov;
    this->m_glWidget->getCameraView(eye, center, up, fov);
    m_rootScene->addBookmark(m_bookmarkWidget, eye, center, up, fov);

    const entity::Bookmarks* bs = m_scene->getBookmarks();
    QVERIFY(bs);
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

    qInfo("Add simulated SVMData");
    QVERIFY(m_rootScene->addSVMData());

    qInfo("Create ground truth camera");
    osg::Vec3f camEyeGround = cher::CENTER;
    osg::Matrix tGround = osg::Matrix::translate(4, 4, 1);
    osg::Matrix rGround = osg::Matrix::rotate(cher::PI*0.25, 0,1,0); // 45 degrees
    rGround.rotate(cher::PI*0.125, osg::Vec3f(1,0,0)); // 22.5 degrees
    camEyeGround = camEyeGround * rGround * tGround;
    osg::Vec3f camCenterGround = cher::CENTER;
    osg::Vec3f camUpGround = osg::Vec3f(0,1,0);
    qDebug() << "camera ground=(" << camEyeGround.x() << ", " << camEyeGround.y() << ", " << camEyeGround.z() << ")";

    qInfo("Edit SVMData to some fixed values.");
    entity::SVMData* svm = m_rootScene->getSVMDataCurrent();
    QVERIFY(svm);
    entity::DraggableWire* wall = svm->getWallWire();
    entity::DraggableWire* floor = svm->getFlootWire();
    QVERIFY(wall && floor);

    wall->pick(0); wall->editPick(0,0);
    wall->pick(1); wall->editPick(0,0);
    wall->pick(2); wall->editPick(0,0);
    wall->pick(3); wall->editPick(0,0);
    wall->unpick();

    floor->pick(0); floor->editPick(0,0);
    floor->pick(1); floor->editPick(0,0);
    floor->pick(2); floor->editPick(0,0);
    floor->pick(3); floor->editPick(0,0);
    floor->unpick();

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
