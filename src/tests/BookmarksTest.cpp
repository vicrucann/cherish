#include "BookmarksTest.h"

#include <QSignalSpy>
#include <QList>
#include <QListWidgetItem>
#include <QIcon>

#include "Stroke.h"
#include "Bookmarks.h"
#include "SVMData.h"
#include "DraggableWire.h"
#include "Utilities.h"

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

void BookmarksTest::testNewBookmarkPure()
{
    qInfo("Test bookmark calculation through SVM data manipulation. ");
    qInfo("Create two canvases parallel to each other and perpendicular to Z axis.");
    qInfo("Create canvases within XZ plane");
    this->onNewCanvasXZ();
    this->onNewCanvasXZ();
    QCOMPARE(m_scene->getNumCanvases(), 5);
    entity::Canvas* curr = m_scene->getCanvasCurrent();
    entity::Canvas* prev = m_scene->getCanvasPrevious();
    QVERIFY(curr && prev);

    qInfo("Translate created canvases in known distance along Z axis");
    const float d = 1.f;
    curr->translate(osg::Matrix::translate(0,d,0));
    prev->translate(osg::Matrix::translate(0,2*d,0));

    QCOMPARE(curr->getCenter(), osg::Vec3f(0,d,0));
    QCOMPARE(prev->getCenter(), osg::Vec3f(0,2*d,0));
    QCOMPARE(curr->getGlobalAxisU(), osg::Vec3f(1,0,0));
    QCOMPARE(prev->getGlobalAxisU(), osg::Vec3f(1,0,0));
    QCOMPARE(curr->getGlobalAxisV().z(), 1.f);
    QCOMPARE(prev->getGlobalAxisV().z(), 1.f);

    qInfo("Create new bookmark and add it to the widgets.");
    osg::Vec3d eye, center, up;
    double fov;
    this->m_glWidget->getCameraView(eye, center, up, fov);
    m_rootScene->addBookmark(m_bookmarkWidget, eye, center, up, fov);
    this->printCameraPose("original", eye, center, up);

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
    osg::Vec3f gEye = cher::CENTER;
    osg::Vec3f gCenter = osg::Vec3f(0,1,0);
    osg::Vec3f gUp = osg::Vec3f(0,0,1);
    printCameraPose("ground", gEye, gCenter, gUp);

    qInfo("Edit SVMData to some fixed values.");
    entity::SVMData* svm = m_rootScene->getSVMDataCurrent();
    QVERIFY(svm);
    entity::DraggableWire* wall = svm->getWallWire();
    entity::DraggableWire* floor = svm->getFlootWire();
    QVERIFY(wall && floor);

    const float sqrt3_1 = 1.f/std::sqrt(3); // tangent of 30 degrees
    const float w = d*sqrt3_1; // frame half width
    const float w2 = 2.f*d*sqrt3_1;
    qDebug() << "width=" << w;
    qDebug() << "width2=" << w2;

    wall->pick(0); wall->editPick(w,w);
    wall->pick(1); wall->editPick(-w,w);
    wall->pick(2); wall->editPick(-w,-w);
    wall->pick(3); wall->editPick(w,-w);
    wall->unpick();

    floor->pick(0); floor->editPick(w2,w2);
    floor->pick(1); floor->editPick(-w2,w2);
    floor->pick(2); floor->editPick(-w2,-w2);
    floor->pick(3); floor->editPick(w2,-w2);
    floor->unpick();

    qInfo("Test the provided SVMData for camera position calculation");
    qInfo("Simulate RootScene::hideAndUpdateSVMData()");
    QVERIFY(m_scene->getBookmarks());
    int num = m_scene->getBookmarks()->getNumBookmarks();
    QCOMPARE(num, 1);
    entity::SceneState* ss = m_scene->getBookmarksModel()->getSceneState(0);
    QVERIFY(ss);
    QCOMPARE(ss->getSVMData(), svm);
    QVERIFY(svm->getVisibility());
    osg::Vec3f rEye, rCenter, rUp; // result camera positions
    QVERIFY(Utilities::getCameraPosition(svm, rEye, rCenter, rUp));
    printCameraPose("result", rEye, rCenter, rUp);
    qInfo("Compare the result with ground truth");
    QCOMPARE(gEye, rEye);
    QCOMPARE(gCenter, rCenter);
    QCOMPARE(gEye, rEye);

    qInfo("Update the bookmark's data");
    entity::Bookmarks* bms = m_scene->getBookmarksModel();
    QVERIFY(bms);
    QVERIFY(bms->editBookmarkPose(0, rEye, rCenter, rUp));
    entity::BookmarkTool* bt = m_rootScene->getBookmarkTool(0);
    QVERIFY(bt);
    bt->setPose(rEye, rCenter, rUp);
    svm->setVisibility(false);
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

void BookmarksTest::printCameraPose(const std::string &name, const osg::Vec3f &eye, const osg::Vec3f &center, const osg::Vec3f &up)
{
    qDebug() << QString(name.c_str()) << ": eye=[" << eye.x() << eye.y() << eye.z()<<"]";
    qDebug() << QString(name.c_str()) << "center=[" << center.x() << center.y() << center.z()<<"]";
    qDebug() << QString(name.c_str()) << "up=[" << up.x() << up.y() << up.z()<<"]";
}

QTEST_MAIN(BookmarksTest)
#include "BookmarksTest.moc"
