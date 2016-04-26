#ifndef LISTWIDGET_H
#define LISTWIDGET_H

#include <string>

#include <QListWidget>
#include <QListView>
#include <QEvent>
#include <QModelIndex>
#include <QMouseEvent>

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

    CanvasDelegate* getCanvasDelegate() const;

signals:
    void rightClicked(const QModelIndex& index);

public slots:
    void onCanvasAdded(const std::string& name);
    void onCanvasRemoved(int row);
    void onCanvasSelectedColor(int row, int color);

protected:
    virtual void mousePressEvent(QMouseEvent* event);
};

class PhotoWidget : public QListWidget
{
    Q_OBJECT
public:
    PhotoWidget(QWidget* parent = 0);

    PhotoDelegate* getPhotoDelegate() const;

signals:

public slots:
    void onPhotoAdded(const std::string& name);
    void onPhotoRemoved(int row);

protected:
};

#endif // LISTWIDGET_H
