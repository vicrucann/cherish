#include "BookmarkWidget.h"

#include "Settings.h"

BookmarkWidget::BookmarkWidget(QWidget *parent)
    : QListView(parent)
{
    this->setViewMode(QListView::IconMode);
    this->setMovement(QListView::Snap);
    this->setIconSize(QSize(100,100));
    this->setResizeMode(QListView::Adjust);
    this->setSelectionBehavior(QAbstractItemView::SelectItems);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setDragDropMode(QAbstractItemView::InternalMove);
}

void BookmarkWidget::setModel(entity::Bookmarks *bookmarks)
{
    QListView::setModel(bookmarks);
    outLogMsg("Bookmarks are set as a model for bookmarks widget");
}

