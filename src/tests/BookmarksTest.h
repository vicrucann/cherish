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

private:
    bool isWhite(const QPixmap& pmap);

    void printCameraPose(const std::string& name, const osg::Vec3f& eye, const osg::Vec3f& center, const osg::Vec3f& up);
};

#endif // BOOKMARKSTEST_H
