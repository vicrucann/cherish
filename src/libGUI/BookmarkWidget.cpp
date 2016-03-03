#include "BookmarkWidget.h"

#include "Settings.h"

BookmarkWidget::BookmarkWidget(QWidget *parent)
    : QListWidget(parent)
{
    //this->setViewMode(QListWidget::IconMode);
    //this->setMovement(QListWidget::Snap);
    this->setIconSize(QSize(100,100));
    //this->setResizeMode(QListWidget::Adjust);
    this->setSelectionBehavior(QAbstractItemView::SelectItems);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setDragDropMode(QAbstractItemView::InternalMove);
    this->setSpacing(2);
    this->setEditTriggers(QListWidget::DoubleClicked);
}

