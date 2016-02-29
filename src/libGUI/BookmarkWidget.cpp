#include "BookmarkWidget.h"

BookmarkWidget::BookmarkWidget(QWidget *parent)
    : QListWidget(parent)
{
    this->setViewMode(QListWidget::IconMode);
    this->setMovement(QListView::Free);
    this->setIconSize(QSize(100,100));
    this->setResizeMode(QListWidget::Adjust);
    this->setSelectionBehavior(QAbstractItemView::SelectItems);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
}
