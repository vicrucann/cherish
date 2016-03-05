#ifndef BOOKMARKWIDGET_H
#define BOOKMARKWIDGET_H

#include <QListWidget>
#include <QListView>
#include <QEvent>
#include <QModelIndex>
#include "Bookmarks.h"
#include "BookmarkDelegate.h"

class BookmarkWidget : public QListWidget
{
 //   Q_OBJECT
public:
    BookmarkWidget(QWidget* parent = 0);

    BookmarkDelegate* getBookmarkDelegate() const;
};

#endif // BOOKMARKWIDGET_H
