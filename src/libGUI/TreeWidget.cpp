#include "TreeWidget.h"

#include <QMouseEvent>
#include <QHeaderView>
#include <QPalette>
#include <QFile>
#include <QDebug>
#include <QtGlobal>

#include "Settings.h"
#include "Utilities.h"

CanvasPhotoWidget::CanvasPhotoWidget(QWidget *parent)
    : QTreeWidget(parent)
{
    this->setMinimumWidth(cher::APP_WIDGET_WIDTH*cher::DPI_SCALING);
    this->setDropIndicatorShown(true);
    this->header()->close();
}

CanvasDelegate *CanvasPhotoWidget::getCanvasDelegate() const
{
    return dynamic_cast<CanvasDelegate*>(this->itemDelegate());
}

void CanvasPhotoWidget::doAddCanvas(const std::string &name)
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    if (!item) throw std::runtime_error("Could not allocate QTreeWidgetItem");
    item->setText(0, QString(name.c_str()));
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    item->setData(0,cher::DelegateChildRole,1);
    item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    this->addTopLevelItem(item);
}

void CanvasPhotoWidget::doAddPhoto(const std::string &name, int rowParent)
{
    if (rowParent < 0 || rowParent >= this->topLevelItemCount())
    {
        qWarning("doAddPhoto: canvas index is out of range, "
                  "addition will not be performed");
        qDebug() << "row " <<  rowParent;
        qDebug() << "count " << this->topLevelItemCount();
        return;
    }
    QTreeWidgetItem* parent = this->topLevelItem(rowParent);
    if (!parent) throw std::runtime_error("doAddPhoto(): parent is NULL");
    QTreeWidgetItem* child = new QTreeWidgetItem();
    if (!child) throw std::runtime_error("doAddPhoto(): could not allocate item");
    child->setText(0, QString(name.c_str()));
    child->setFlags(child->flags() | Qt::ItemIsEditable);
    child->setData(0,cher::DelegateChildRole,2);
    parent->addChild(child);
}

void CanvasPhotoWidget::doSelectCanvas(int row, int color)
{
    if (row >= this->topLevelItemCount() || row < 0){
        qDebug("doSelectCanvas: canvas index is out of range, "
                  "selection on widget will not be performed");
        qDebug() << "row " << row;
        qDebug() << "count " << this->topLevelItemCount();
        return;
    }

    QTreeWidgetItem* item = this->topLevelItem(row);
    if (!item) throw std::runtime_error("doSelectCanvas(): item is NULL");
    QColor qcolor;
    switch (color){
    case 0:
        qcolor = Qt::white;
        break;
    case 1:
        qcolor = Utilities::getQColor(cher::CANVAS_CLR_CURRENT);
        break;
    case 2:
        qcolor = Utilities::getQColor(cher::CANVAS_CLR_PREVIOUS);
        break;
    default:
        qcolor = Qt::white;
        break;
    }
    item->setData(0, cher::DelegateBGColor, qcolor);
}

void CanvasPhotoWidget::onCanvasRemoved(int row)
{
    if (row >= this->topLevelItemCount() || row < 0){
        qWarning("onCanvasRemoved: canvas index is out of range, "
                  "removed will not be performed");
        qDebug() << "row " << row;
        qDebug() << "count " << this->topLevelItemCount();
        return;
    }
    QTreeWidgetItem* item = this->takeTopLevelItem(row);
    if (!item) return;

    delete item;
}

void CanvasPhotoWidget::onPhotoRemoved(int rowP, int row)
{
    if (rowP >= this->topLevelItemCount() || rowP < 0){
        qWarning("onPhotoRemoved: canvas index is out of range, "
                  "removed will not be performed");
        qDebug() << "rowP " << rowP;
        qDebug() << "count " << this->topLevelItemCount();
        return;
    }
    QTreeWidgetItem* parent = this->topLevelItem(rowP);
    if (!parent) return;

    QTreeWidgetItem* item = parent->takeChild(row);
    if (!item) return;

    delete item;
}

void CanvasPhotoWidget::onCanvasVisibilitySet(int row, bool visibility)
{
    if (row >= this->topLevelItemCount() || row < 0){
        qDebug("onCanvasVisibilitySet: canvas index is out of range, "
                  "selection on widget will not be performed");
        qDebug() << "row " << row;
        qDebug() << "count " << this->topLevelItemCount();
        return;
    }
    QTreeWidgetItem* item = this->topLevelItem(row);
    if (!item) {
        qWarning("Could not exatrac item pointer");
        return;
    }

//    bool value = item->data(0, cher::DelegateVisibilityRole).toBool();
    qDebug() << "Requested visibility " << visibility;
    item->setData(0, cher::DelegateVisibilityRole, !visibility );
}

void CanvasPhotoWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton){
        QPoint p = event->pos();
        const QModelIndex& index = this->indexAt(p);
        if (index.isValid())
            emit this->rightClicked(index);
        else
            qWarning("mouse right: no item was chosen for selection");
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
//    qDebug() << "inserting to end", destination->childCount());
//    destination->insertChild(destination->childCount(), child);
//    qDebug() << "inserted", destination->childCount());

//    /* emit signal about the move */
//    emit this->photoDraggedAndDropped(this->indexOfTopLevelItem(parent), start, end, this->indexOfTopLevelItem(destination), row);
//}
