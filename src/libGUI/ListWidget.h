#ifndef LISTWIDGET_H
#define LISTWIDGET_H

#include <string>

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
    Q_OBJECT
public:
    CanvasWidget(QWidget* parent = 0);

public slots:
    void onCanvasAdded(const std::string& name);
    void onCanvasRemoved(int row);
    void onCanvasSelectedColor(int row, int color);
};

#endif // LISTWIDGET_H
