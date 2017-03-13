#ifndef BOOKMARKSTEST_H
#define BOOKMARKSTEST_H

#include "BaseGuiTest.h"

/*! \class BookmarksTest
 * \brief Class that allows testing of bookmark functionality.
*/

class BookmarksTest : public BaseGuiTest
{
Q_OBJECT
private slots:
    /* Test bookmarking the current view. */
    void testAddBookmark();

    /* Test bookmark add through the SVM data. The steps follow closely the same steps
     * from MainWindow::onBookmarkNew() slot.
     * \sa testNewBookmarkNoise();
*/
    void testNewBookmarkPure();

    /* Test bookmark add through the SVM data (noise is added to data). The steps follow closely the same steps
     * from MainWindow::onBookmarkNew() slot.
     * \sa testNewBookmarkPure();
*/
    void testNewBookmarkNoise();

    /*! The two canvases are located perpendicular to each other, not parallel like in
     * testNewBookmarkPure(). */
    void testNewBookmarkPerspective();

    /*! Given known H, and the points matching, test the Homography::solve() function. */
    void testHomographyCalculation();

    /*! Given a known transformation of one plane into another, find the rotation and translation based
     * on Homography matrix and compare the obtained values with the initial ones. */
    void testRotationTranslation();

private:
    bool isWhite(const QPixmap& pmap);

    void printCameraPose(const std::string& name, const osg::Vec3f& eye, const osg::Vec3f& center, const osg::Vec3f& up);

    osg::Vec3f projectToPlane(const osg::Vec3f& D, const osg::Vec3f& normal, const osg::Vec3f& origin);
};

#endif // BOOKMARKSTEST_H
