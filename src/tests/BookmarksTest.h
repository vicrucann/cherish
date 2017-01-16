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
     * from MainWindow::onBookmarkNew() slot. */
    void testNewBookmark();

private:
    bool isWhite(const QPixmap& pmap);
};

#endif // BOOKMARKSTEST_H
