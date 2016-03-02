#ifndef BOOKMARKWIDGET_H
#define BOOKMARKWIDGET_H

#include <QListWidget>
#include <QListView>
#include <QEvent>
#include "Bookmarks.h"

class BookmarkWidget : public QListWidget
{
public:
    BookmarkWidget(QWidget* parent = 0);
};

#endif // BOOKMARKWIDGET_H
