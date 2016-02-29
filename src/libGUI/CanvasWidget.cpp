#include "CanvasWidget.h"

CanvasWidget::CanvasWidget(QWidget *parent)
    : QListWidget(parent)
{
    this->setViewMode(QListWidget::ListMode);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
}
