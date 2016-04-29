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
    this->setMinimumWidth(dureu::APP_SCREENSHOT_HEIGHT*2+50);
    this->header()->close();
    this->setIndentation(0); // will have to remove when adding photos to tree

//    QFile file(":/CanvasWidget.qss");
//    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
//        this->setStyleSheet(file.readAll());
//        file.close();
//    }
}

CanvasDelegate *CanvasPhotoWidget::getCanvasDelegate() const
{
    return dynamic_cast<CanvasDelegate*>(this->itemDelegate());
}

void CanvasPhotoWidget::onCanvasAdded(const std::string &name)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(this);
    if (!item) return;
    item->setText(0, QString(name.c_str()));
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    this->addTopLevelItem(item);
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
//    this->style()->unpolish(this);
//    this->style()->polish(this);
//    this->update();
    item->setBackgroundColor(0, qcolor);
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
