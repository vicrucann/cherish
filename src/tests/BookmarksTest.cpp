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

#include "HomographyMatrix.h"

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
}

void BookmarksTest::testNewBookmarkNoise()
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

    float noise[8] = {0.001f, 0.0035f, -0.0043f, -0.0097f, 0.0029f, 0.007f, -0.0003f, -0.004f};
    float noise2[8] = {-0.002f, -0.07f, 0.003f, 0.04f, 0.0063f, -0.003f, -0.003f, 0.003f};

    wall->pick(0); wall->editPick(w+noise[0],w+noise[1]);
    wall->pick(1); wall->editPick(-w+noise[2],w+noise[3]);
    wall->pick(2); wall->editPick(-w+noise[4],-w+noise[5]);
    wall->pick(3); wall->editPick(w+noise[6],-w+noise[7]);
    wall->unpick();

    floor->pick(0); floor->editPick(w2+noise2[0],w2+noise2[1]);
    floor->pick(1); floor->editPick(-w2+noise2[2],w2+noise2[3]);
    floor->pick(2); floor->editPick(-w2+noise2[4],-w2+noise2[5]);
    floor->pick(3); floor->editPick(w2+noise2[6],-w2+noise2[7]);
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
    qInfo("Compare the result with ground truth within threshold tolerance");
    osg::Vec3f diffE = rEye - gEye;
    qDebug() << "diff=" << std::fabs(diffE.x()) << std::fabs(diffE.y()) << std::fabs(diffE.z());
    float tolerance = 0.1f;
    QVERIFY( std::fabs(diffE.x()) < tolerance &&
             std::fabs(diffE.y()) < tolerance &&
             std::fabs(diffE.z()) < tolerance);
    QCOMPARE(rCenter, gCenter);
    QCOMPARE(rUp, gUp);
}

void BookmarksTest::testNewBookmarkPerspective()
{
    qInfo("Test bookmark calculation through SVM data manipulation. ");
    qInfo("Take two canvases perpendicular to each other.");
    entity::Canvas* curr = m_scene->getCanvasCurrent();
    entity::Canvas* prev = m_scene->getCanvasPrevious();
    QVERIFY(curr && prev);

    qInfo("Test canvas locations");
    QCOMPARE(curr->getCenter(), cher::CENTER);
    QCOMPARE(prev->getCenter().y(), cher::CANVAS_MINW*0.5);
    QCOMPARE(curr->getNormal().y(), -1.f);
    QCOMPARE(prev->getNormal().x(), -1.f);

    qInfo("Camera ground params");
    osg::Vec3f gEye = osg::Vec3f(5, -5, 0), gCenter = cher::CENTER, gUp = cher::UP;

    qInfo("Cast rays from camera eye onto canvases");
    float w = 0.3f;
    auto vx = gUp ^ (gCenter - gEye);
    auto C = (gCenter-gEye)*10;
    auto p1 = C + vx*w + gUp*w;
    auto p2 = C - vx*w + gUp*w;
    auto p3 = C - vx*w - gUp*w;
    auto p4 = C + vx*w - gUp*w;

    osg::Vec3f P1c, P2c, P3c, P4c;
    osg::Vec3f P1p, P2p, P3p, P4p;
    QVERIFY(Utilities::getRayPlaneIntersection(curr->getPlane(), curr->getCenter(), gEye, p1, P1c));
    QVERIFY(Utilities::getRayPlaneIntersection(curr->getPlane(), curr->getCenter(), gEye, p2, P2c));
    QVERIFY(Utilities::getRayPlaneIntersection(curr->getPlane(), curr->getCenter(), gEye, p3, P3c));
    QVERIFY(Utilities::getRayPlaneIntersection(curr->getPlane(), curr->getCenter(), gEye, p4, P4c));

    QVERIFY(Utilities::getRayPlaneIntersection(prev->getPlane(), prev->getCenter(), gEye, p1, P1p));
    QVERIFY(Utilities::getRayPlaneIntersection(prev->getPlane(), prev->getCenter(), gEye, p2, P2p));
    QVERIFY(Utilities::getRayPlaneIntersection(prev->getPlane(), prev->getCenter(), gEye, p3, P3p));
    QVERIFY(Utilities::getRayPlaneIntersection(prev->getPlane(), prev->getCenter(), gEye, p4, P4p));

    qInfo("Convert global intersections into local coordinate system");
    osg::Vec3f p1c, p2c, p3c, p4c;
    osg::Vec3f p1p, p2p, p3p, p4p;
    QVERIFY(Utilities::getLocalFromGlobal(P1c, curr->getMatrixInverse(), p1c));
    QVERIFY(Utilities::getLocalFromGlobal(P2c, curr->getMatrixInverse(), p2c));
    QVERIFY(Utilities::getLocalFromGlobal(P3c, curr->getMatrixInverse(), p3c));
    QVERIFY(Utilities::getLocalFromGlobal(P4c, curr->getMatrixInverse(), p4c));

    QVERIFY(Utilities::getLocalFromGlobal(P1p, prev->getMatrixInverse(), p1p));
    QVERIFY(Utilities::getLocalFromGlobal(P2p, prev->getMatrixInverse(), p2p));
    QVERIFY(Utilities::getLocalFromGlobal(P3p, prev->getMatrixInverse(), p3p));
    QVERIFY(Utilities::getLocalFromGlobal(P4p, prev->getMatrixInverse(), p4p));

    qInfo("Add simulated SVMData");
    qInfo("Create new bookmark and add it to the widgets.");
    osg::Vec3d eye, center, up;
    double fov;
    this->m_glWidget->getCameraView(eye, center, up, fov);
    m_rootScene->addBookmark(m_bookmarkWidget, eye, center, up, fov);
    this->printCameraPose("original", eye, center, up);
    this->printCameraPose("ground", gEye, gCenter, gUp);

    QVERIFY(m_rootScene->addSVMData());
    entity::SVMData* svm = m_rootScene->getSVMDataCurrent();
    QVERIFY(svm);
    entity::DraggableWire* wall = svm->getWallWire();
    entity::DraggableWire* floor = svm->getFlootWire();
    QVERIFY(wall && floor);

    qInfo("Feed the simulated intersections to an SVM structure.");
    wall->pick(0); wall->editPick(p1c.x(),p1c.y());
    wall->pick(1); wall->editPick(p2c.x(),p2c.y());
    wall->pick(2); wall->editPick(p3c.x(),p3c.y());
    wall->pick(3); wall->editPick(p4c.x(),p4c.y());
    wall->unpick();

    floor->pick(0); floor->editPick(p1p.x(),p1p.y());
    floor->pick(1); floor->editPick(p2p.x(),p2p.y());
    floor->pick(2); floor->editPick(p3p.x(),p3p.y());
    floor->pick(3); floor->editPick(p4p.x(),p4p.y());
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
    auto diffE = gEye-rEye;
    float th = 0.01f;
    QVERIFY(std::fabs(diffE.x())<th && std::fabs(diffE.y())<th && std::fabs(diffE.z())<th);
    QCOMPARE(gCenter, rCenter);
    QCOMPARE(gUp, rUp);
}

void BookmarksTest::testHomographyCalculation()
{
    qInfo("Given a set of match points, find H and compare it to the ground truth.");
    qInfo("Add simulated SVMData");
    qInfo("Create new bookmark and add it to the widgets.");
    osg::Vec3d eye, center, up;
    double fov;
    this->m_glWidget->getCameraView(eye, center, up, fov);
    m_rootScene->addBookmark(m_bookmarkWidget, eye, center, up, fov);
    this->printCameraPose("original", eye, center, up);

    qInfo("Add SVM structure and edit point positions to known local coords.");
    QVERIFY(m_rootScene->addSVMData());
    entity::SVMData* svm = m_rootScene->getSVMDataCurrent();
    QVERIFY(svm);
    entity::DraggableWire* wall = svm->getWallWire();
    entity::DraggableWire* floor = svm->getFlootWire();
    QVERIFY(wall && floor);

    qInfo("Feed the simulated intersections to an SVM structure.");
    osg::Vec3d x1 = osg::Vec3d(1.41, 1.31, 0);
    osg::Vec3d x2 = osg::Vec3d(4.80, 1.59, 0);
    osg::Vec3d x3 = osg::Vec3d(4.93, 6.30, 0);
    osg::Vec3d x4 = osg::Vec3d(0.64, 6.01, 0);
    osg::Vec3d y1 = osg::Vec3d(3.18, 2.56, 0);
    osg::Vec3d y2 = osg::Vec3d(5.34, 3.72, 0);
    osg::Vec3d y3 = osg::Vec3d(3.16, 6.70, 0);
    osg::Vec3d y4 = osg::Vec3d(0.73, 4.73, 0);


    wall->pick(0); wall->editPick(x1.x(), x1.y());
    wall->pick(1); wall->editPick(x2.x(), x2.y());
    wall->pick(2); wall->editPick(x3.x(), x3.y());
    wall->pick(3); wall->editPick(x4.x(), x4.y());
    wall->unpick();

    floor->pick(0); floor->editPick(y1.x(), y1.y());
    floor->pick(1); floor->editPick(y2.x(), y2.y());
    floor->pick(2); floor->editPick(y3.x(), y3.y());
    floor->pick(3); floor->editPick(y4.x(), y4.y());
    floor->unpick();

    qInfo("Compare two H estimations");
    osg::Matrix H0 = HomographyMatrix::solve(svm);
    qDebug() << "H0=";
    qDebug() << H0(0,0) << H0(0,1) << H0(0,2);
    qDebug() << H0(1,0) << H0(1,1) << H0(1,2);
    qDebug() << H0(2,0) << H0(2,1) << H0(2,2);

    osg::Matrix H1 = HomographyMatrix::solveEigen(svm);
    qDebug() << "H1=";
    qDebug() << H1(0,0) << H1(0,1) << H1(0,2);
    qDebug() << H1(1,0) << H1(1,1) << H1(1,2);
    qDebug() << H1(2,0) << H1(2,1) << H1(2,2);

    qInfo("Check Homography wraping works for the points");
    double error = HomographyMatrix::evaluate(svm, H0);
    QVERIFY(std::fabs(error) < 0.001);

    qInfo("Extract R|t matrix");
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
