#include "ListView.h"

#include <QMimeData>
#include <QByteArray>
#include <QDataStream>
#include <QPixmap>
#include <QtGlobal>
#include <QStandardItem>
#include <QModelIndexList>
#include <QDrag>

#include "Settings.h"

PhotoWidget::PhotoWidget(QWidget *parent)
    : QListView(parent)
{
//    this->setViewMode(QListView::IconMode);
    this->setIconSize(QSize(cher::APP_WIDGET_ICONSIZE_W * cher::DPI_SCALING,
                            cher::APP_WIDGET_ICONSIZE_H * cher::DPI_SCALING));
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setSpacing(cher::APP_WIDGET_GAP * cher::DPI_SCALING);
    this->setAcceptDrops(false);
    this->setDragEnabled(true);
    this->setDropIndicatorShown(true);
}

void PhotoWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(cher::MIME_PHOTO))
        event->accept();
    else
        event->ignore();
}

void PhotoWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat(cher::MIME_PHOTO)){
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
    else
        event->ignore();
}

void PhotoWidget::dropEvent(QDropEvent *event)
{
    // don't do anything when dropped back
    event->ignore();
}
