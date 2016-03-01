#include "BookmarkWidget.h"

BookmarkWidget::BookmarkWidget(QWidget *parent)
    : QListView(parent)
{
    this->setViewMode(QListView::IconMode);
    this->setMovement(QListView::Snap);
    this->setIconSize(QSize(100,100));
    this->setResizeMode(QListView::Adjust);
    this->setSelectionBehavior(QAbstractItemView::SelectItems);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
}
