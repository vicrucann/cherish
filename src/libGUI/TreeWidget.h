#ifndef TREEWIDGET_H
#define TREEWIDGET_H

#include <QTreeWidget>

#include "ListDelegate.h"

/*! \class CanvasPhotoWidget
 * \brief Re-defined QTreeWidget that contains info on canvases and their photos. Contains method to obtain corresponding delegate,
 * as well as related signals and slots for content modification.
*/
class CanvasPhotoWidget : public QTreeWidget
{
    Q_OBJECT
public:
    /*! Constructor. */
    CanvasPhotoWidget(QWidget* parent = 0);

    /*! \return a non-const pointer to CanvasDelegate. */
    CanvasDelegate* getCanvasDelegate() const;

signals:
    /*! Signal to inform whenver user performs a right click action. */
    void rightClicked(const QModelIndex& index);

    /*! Signal to inform whenver user performs drag-and-drop action. */
    void photoDraggedAndDropped(int parent, int start, int end, int destination, int row);

public slots:
    /*! Slot called after a canvas was added to scene graph. \param name is the string name of the canvas that will be added as a new item to the widget. */
    void onCanvasAdded(const std::string& name);

    /*! Slot called after a photo was added to scene graph. \param name is the string name of newly added photo, \param rowParent is the index of canvas-parent. */
    void onPhotoAdded(const std::string& name, int rowParent);

    /*! Slot called after a canvas is removed from scene graph. \param row is the integer row of an item to be removed from the canvas widget. */
    void onCanvasRemoved(int row);

    /*! Slot called after a photo was removed from scene graph. \param rowP is row index of canvas-parent, \param row is the index of photo-child. */
    void onPhotoRemoved(int rowP, int row);

    /*! Slot called whenever the status of canvas is changed: to "current", or "previous" or "rest".
     * \param row is the integer row number of the corresponding item in the widget,
     * \param color is a color indicator */
    void onCanvasSelectedColor(int row, int color);

    /*! Slot called whenever canvas visibility is set (not by user), e.g., on file read or by going to specific bookmark. */
    void onCanvasVisibilitySet(int row, bool visibility);

protected:
    virtual void mousePressEvent(QMouseEvent* event);
//    virtual void dropEvent(QDropEvent* event);

};

#endif // TREEWIDGET_H
