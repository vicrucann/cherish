#ifndef BOOKMARKWIDGET_H
#define BOOKMARKWIDGET_H

#include <QListWidget>
#include <QListView>
#include "Bookmarks.h"

class BookmarkWidget : public QListView
{
public:
    BookmarkWidget(QWidget* parent = 0);
};

#endif // BOOKMARKWIDGET_H
