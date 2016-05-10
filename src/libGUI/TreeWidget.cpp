#include "TreeWidget.h"

#include <QMouseEvent>
#include <QHeaderView>
#include <QPalette>
#include <QFile>

#include "Settings.h"
#include "Utilities.h"

CanvasPhotoWidget::CanvasPhotoWidget(QWidget *parent)
    : QTreeWidget(parent)
{
    this->setMinimumWidth(dureu::APP_WIDGET_WIDTH);
//    this->setDragEnabled(false);
//    this->setAcceptDrops(false);
    this->setDropIndicatorShown(true);
//    this->setDragDropMode(QAbstractItemView::InternalMove);
//    this->invisibleRootItem()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    this->header()->close();
}

CanvasDelegate *CanvasPhotoWidget::getCanvasDelegate() const
{
    return dynamic_cast<CanvasDelegate*>(this->itemDelegate());
}

void CanvasPhotoWidget::onCanvasAdded(const std::string &name)
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    if (!item) return;
    item->setText(0, QString(name.c_str()));
    item->setFlags(item->flags() | Qt::ItemIsEditable);
//    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled |
//                   Qt::ItemIsEditable | Qt::ItemIsDropEnabled);
    item->setData(0,dureu::DelegateChildRole,1);
    item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    this->addTopLevelItem(item);
}

void CanvasPhotoWidget::onPhotoAdded(const std::string &name, int rowParent)
{
    if (rowParent < 0 || rowParent >= this->topLevelItemCount())
    {
        outErrMsg("onPhotoAdded: canvas index is out of range, "
                  "addition will not be performed");
        outLogVal("row", rowParent);
        outLogVal("count", this->topLevelItemCount());
        return;
    }
    QTreeWidgetItem* parent = this->topLevelItem(rowParent);
    if (!parent) return;
    QTreeWidgetItem* child = new QTreeWidgetItem();
    if (!child) return;
    child->setText(0, QString(name.c_str()));
    child->setFlags(child->flags() | Qt::ItemIsEditable);
//    child->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled |
//                    Qt::ItemIsEditable | Qt::ItemIsDragEnabled );
    child->setData(0,dureu::DelegateChildRole,2);
    parent->addChild(child);
}

void CanvasPhotoWidget::onCanvasRemoved(int row)
{
    if (row >= this->topLevelItemCount() || row < 0){
        outErrMsg("onCanvasRemoved: canvas index is out of range, "
                  "removed will not be performed");
        outLogVal("row", row);
        outLogVal("count", this->topLevelItemCount());
        return;
    }
    QTreeWidgetItem* item = this->takeTopLevelItem(row);
    if (!item) return;

    delete item;
}

void CanvasPhotoWidget::onPhotoRemoved(int rowP, int row)
{
    if (rowP >= this->topLevelItemCount() || rowP < 0){
        outErrMsg("onPhotoRemoved: canvas index is out of range, "
                  "removed will not be performed");
        outLogVal("rowP", rowP);
        outLogVal("count", this->topLevelItemCount());
        return;
    }
    QTreeWidgetItem* parent = this->topLevelItem(rowP);
    if (!parent) return;

    QTreeWidgetItem* item = parent->takeChild(row);
    if (!item) return;

    delete item;
}

void CanvasPhotoWidget::onCanvasSelectedColor(int row, int color)
{
    if (row >= this->topLevelItemCount() || row < 0){
        outLogMsg("onCanvasSelectedColor: canvas index is out of range, "
                  "selection on widget will not be performed");
        outLogVal("row", row);
        outLogVal("count", this->topLevelItemCount());
        return;
    }

    QTreeWidgetItem* item = this->topLevelItem(row);
    if (!item) return;
    QColor qcolor;
    switch (color){
    case 0:
        qcolor = Qt::white;
        break;
    case 1:
        qcolor = Utilities::getQColor(dureu::CANVAS_CLR_CURRENT);
        break;
    case 2:
        qcolor = Utilities::getQColor(dureu::CANVAS_CLR_PREVIOUS);
        break;
    default:
        qcolor = Qt::white;
        break;
    }
//    item->setBackgroundColor(0, qcolor);
    item->setData(0, dureu::DelegateBGColor, qcolor);
}

void CanvasPhotoWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton){
        QPoint p = event->pos();
        const QModelIndex& index = this->indexAt(p);
        if (index.isValid())
            emit this->rightClicked(index);
        else
            outErrMsg("mouse right: no item was chosen for selection");
    }
    QTreeWidget::mousePressEvent(event);
}

//void CanvasPhotoWidget::dropEvent(QDropEvent *event)
//{
//    /* list of items that are about to be dragged */
//    QList< QTreeWidgetItem* > kids = this->selectedItems();

//    /* row number before the drag - initial position */
//    if (kids.size() == 0) return;
//    int start = this->indexFromItem(kids.at(0)).row();
//    int end = start;
//    QTreeWidgetItem* parent = kids.at(0)->parent();

//    /* perform the default implementation */
//    QTreeWidget::dropEvent(event);

//    /* get new index */
//    int row = this->indexFromItem(kids.at(0)).row();
//    QTreeWidgetItem* destination = kids.at(0)->parent();

//    if (!parent || !destination){
//        event->setDropAction(Qt::IgnoreAction);
//        return;
//    }

//    QTreeWidgetItem* child = destination->child(row);
//    destination->removeChild(child);
//    outLogVal("inserting to end", destination->childCount());
//    destination->insertChild(destination->childCount(), child);
//    outLogVal("inserted", destination->childCount());

//    /* emit signal about the move */
//    emit this->photoDraggedAndDropped(this->indexOfTopLevelItem(parent), start, end, this->indexOfTopLevelItem(destination), row);
//}
