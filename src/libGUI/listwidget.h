#ifndef LISTWIDGET_H
#define LISTWIDGET_H

#include <QListWidget>
#include <QListView>
#include <QEvent>
#include <QModelIndex>
#include "Bookmarks.h"
#include "ListDelegate.h"

class BookmarkWidget : public QListWidget
{
 //   Q_OBJECT
public:
    BookmarkWidget(QWidget* parent = 0);

    BookmarkDelegate* getBookmarkDelegate() const;
};

class CanvasWidget : public QListWidget
{
public:
    CanvasWidget(QWidget* parent = 0){}
};

#endif // LISTWIDGET_H
