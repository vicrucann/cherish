#include "ListWidget.h"

#include <QListWidgetItem>

#include "Settings.h"
#include "Utilities.h"

BookmarkWidget::BookmarkWidget(QWidget *parent)
    : QListWidget(parent)
{
    //this->setMovement(QListWidget::Snap);
    this->setIconSize(QSize(dureu::APP_SCREENSHOT_HEIGHT, dureu::APP_SCREENSHOT_HEIGHT));
    //this->setResizeMode(QListWidget::Adjust);
    this->setSelectionBehavior(QAbstractItemView::SelectItems);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setDragDropMode(QAbstractItemView::InternalMove);
    this->setSpacing(2);
    this->setEditTriggers(QListWidget::DoubleClicked);
    this->setMinimumWidth(dureu::APP_SCREENSHOT_HEIGHT*2+50);
    this->setTabKeyNavigation(false);
}

BookmarkDelegate *BookmarkWidget::getBookmarkDelegate() const
{
    return dynamic_cast<BookmarkDelegate*>(this->itemDelegate());
}

CanvasWidget::CanvasWidget(QWidget *parent)
    : QListWidget(parent)
{
    this->setSelectionBehavior(QAbstractItemView::SelectItems);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setDragDropMode(QAbstractItemView::NoDragDrop);
    this->setSpacing(2);
    this->setEditTriggers(QListWidget::DoubleClicked);
    this->setMinimumWidth(dureu::APP_SCREENSHOT_HEIGHT*2+50);
    this->setTabKeyNavigation(false);
}

CanvasDelegate *CanvasWidget::getCanvasDelegate() const
{
    return dynamic_cast<CanvasDelegate*>(this->itemDelegate());
}

void CanvasWidget::onCanvasAdded(const std::string &name)
{
    this->addItem(QString(name.c_str()));
    QListWidgetItem* item = this->item(this->count()-1);
    if (!item){
        outLogMsg("onCanvasAdded: could not extract just added item, "
                  "properties are not set correctly");
        return;
    }
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    outLogVal("Canvas added to widget", name);
}

void CanvasWidget::onCanvasRemoved(int row)
{
    if (row >= this->count()){
        outErrMsg("onCanvasRemoved: canvas index is out of range, "
                  "removed will not be performed");
        outLogVal("row", row);
        outLogVal("count", this->count());
        return;
    }
    QListWidgetItem* item = this->takeItem(row);
    if (item)
        delete item;
}

void CanvasWidget::onCanvasSelectedColor(int row, int color)
{
    if (row >= this->count()){
        outLogMsg("onCanvasSelectedColor: canvas index is out of range, "
                  "selection on widget will not be performed");
        outLogVal("row", row);
        outLogVal("count", this->count());
        return;
    }
    QListWidgetItem* item = this->item(row);
    if (!item){
        outLogMsg("onCanvasSelectedColor: could not extract item, "
                  "properties are not set");
        return;
    }
    QColor qcolor;
    switch (color){
    case 0:
        qcolor = Utilities::getQColor(dureu::CANVAS_CLR_REST);
        break;
    case 1:
        qcolor = Utilities::getQColor(dureu::CANVAS_CLR_CURRENT);
        break;
    case 2:
        qcolor = Utilities::getQColor(dureu::CANVAS_CLR_PREVIOUS);
        break;
    default:
        qcolor = Utilities::getQColor(dureu::CANVAS_CLR_REST);
        break;
    }
    item->setBackgroundColor(qcolor);
}

void CanvasWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton){

        QPoint p = event->pos();
        const QModelIndex& index = this->indexAt(p);
        if (index.isValid())
            emit this->rightClicked(index);
        else
            outErrMsg("mouse right: no item was chosen for selection");
    }
    QListWidget::mousePressEvent(event);
}

PhotoWidget::PhotoWidget(QWidget *parent)
    : QListWidget(parent)
{
    this->setSelectionBehavior(QAbstractItemView::SelectItems);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setDragDropMode(QAbstractItemView::NoDragDrop);
    this->setSpacing(2);
    this->setEditTriggers(QListWidget::DoubleClicked);
    this->setMinimumWidth(dureu::APP_SCREENSHOT_HEIGHT*2+50);
    this->setTabKeyNavigation(false);
}

PhotoDelegate *PhotoWidget::getPhotoDelegate() const
{
    return dynamic_cast<PhotoDelegate*>(this->itemDelegate());
}

void PhotoWidget::onPhotoAdded(const std::string &name)
{
    this->addItem(QString(name.c_str()));
    QListWidgetItem* item = this->item(this->count()-1);
    if (!item){
        outLogMsg("onCanvasAdded: could not extract just added item, "
                  "properties are not set correctly");
        return;
    }
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    outLogVal("Photo added to widget", name);
}

void PhotoWidget::onPhotoRemoved(int row)
{
    if (row >= this->count()){
        outErrMsg("onPhotoRemoved: photo index is out of range, "
                  "removed will not be performed");
        outLogVal("row", row);
        outLogVal("count", this->count());
        return;
    }
    QListWidgetItem* item = this->takeItem(row);
    if (item)
        delete item;
}
