#ifndef BOOKMARKWIDGET_H
#define BOOKMARKWIDGET_H

#include <QListWidget>
#include <QListView>
#include <QEvent>
#include "Bookmarks.h"

class BookmarkWidget : public QListView
{
public:
    BookmarkWidget(QWidget* parent = 0);

    void setModel(entity::Bookmarks* bookmarks);
};

#endif // BOOKMARKWIDGET_H
