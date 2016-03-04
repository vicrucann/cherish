#ifndef BOOKMARKWIDGET_H
#define BOOKMARKWIDGET_H

#include <QListWidget>
#include <QListView>
#include <QEvent>
#include "Bookmarks.h"
#include "BookmarkDelegate.h"

class BookmarkWidget : public QListWidget
{
public:
    BookmarkWidget(QWidget* parent = 0);

    BookmarkDelegate* getBookmarkDelegate() const;
};

#endif // BOOKMARKWIDGET_H
