#ifndef TREEWIDGET_H
#define TREEWIDGET_H

#include <QTreeWidget>

#include "ListDelegate.h"

class CanvasPhotoWidget : public QTreeWidget
{
    Q_OBJECT
public:
    CanvasPhotoWidget(QWidget* parent = 0);

    CanvasDelegate* getCanvasDelegate() const;

signals:
    void rightClicked(const QModelIndex& index);
    void photoDraggedAndDropped(int parent, int start, int end, int destination, int row);

public slots:
    void onCanvasAdded(const std::string& name);
    void onPhotoAdded(const std::string& name, int rowParent);
    void onCanvasRemoved(int row);
    void onPhotoRemoved(int rowP, int row);
    void onCanvasSelectedColor(int row, int color);

protected:
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void dropEvent(QDropEvent* event);

};

#endif // TREEWIDGET_H
