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
    this->setViewMode(QListView::IconMode);
//    this->setMovement(QListView::Snap);
    this->setIconSize(QSize(cher::APP_WIDGET_ICONSIZE * cher::DPI_SCALING,
                            cher::APP_WIDGET_ICONSIZE * cher::DPI_SCALING));
//    this->setDragDropMode(QAbstractItemView::DragDrop);
//    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
//    this->setSpacing(cher::APP_WIDGET_GAP * cher::DPI_SCALING);
//    this->setDragEnabled(true);
//    this->setDropIndicatorShown(true);
}

//void PhotoWidget::dragEnterEvent(QDragEnterEvent *event)
//{
//    if (event->mimeData()->hasImage())
//        event->accept();
//    else
//        event->ignore();
//}

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
    const QMimeData* mime = event->mimeData();
    if (mime->hasFormat(cher::MIME_PHOTO)){
        QByteArray dataImage = mime->data(cher::MIME_PHOTO);
        QDataStream dataStream(&dataImage, QIODevice::ReadOnly);
        QString fileName;
        dataStream >> fileName;

        qInfo() << "name exatracted " << fileName;

        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
    else
        event->ignore();
}

void PhotoWidget::startDrag(Qt::DropAction /* supportedActions */ )
{
    QModelIndex index = this->currentIndex();
    QString fileName = index.data(Qt::DisplayRole).toString();

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << fileName;

    QMimeData* mimeData = new QMimeData;
    mimeData->setData(cher::MIME_PHOTO, itemData);

    QDrag* drag = new QDrag(this);
    drag->setMimeData(mimeData);

    if (drag->exec(Qt::CopyAction) == Qt::CopyAction)
        qInfo("Drag data copied successfully");
}
