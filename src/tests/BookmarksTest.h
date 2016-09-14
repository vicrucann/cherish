#ifndef BOOKMARKSTEST_H
#define BOOKMARKSTEST_H

#include "BaseGuiTest.h"

class BookmarksTest : public BaseGuiTest
{
Q_OBJECT
private slots:
    void testAddBookmark();

private:
    bool isWhite(const QPixmap& pmap);
};

#endif // BOOKMARKSTEST_H
