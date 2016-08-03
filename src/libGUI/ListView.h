#ifndef PHOTOWIDGET_H
#define PHOTOWIDGET_H

#include <QListView>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>

/*! \class PhotoWidget
 * \brief Class that serves as a local image file browser and allows to view all the image file of a given
 * directory.
 * It is designed so that to also perform image import to the scene by doing drag and drop movement.
*/
class PhotoWidget : public QListView
{
public:
    /*! Constructor that sets some parameters to obtain the desired look, e.g., icon representation of the image. */
    PhotoWidget(QWidget* parent = 0);

protected:
    // drag and drop functionality between the PhotoWidget and GLWidget
//    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void dropEvent(QDropEvent *event);
    void startDrag(Qt::DropAction);
};

#endif // PHOTOWIDGET_H
