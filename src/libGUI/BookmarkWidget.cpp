#include "BookmarkWidget.h"

#include "Settings.h"

BookmarkWidget::BookmarkWidget(QWidget *parent)
    : QListWidget(parent)
{
    //this->setViewMode(QListWidget::IconMode);
    //this->setMovement(QListWidget::Snap);
    this->setIconSize(QSize(dureu::APP_SCREENSHOT_HEIGHT, dureu::APP_SCREENSHOT_HEIGHT));
    //this->setResizeMode(QListWidget::Adjust);
    this->setSelectionBehavior(QAbstractItemView::SelectItems);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setDragDropMode(QAbstractItemView::InternalMove);
    this->setSpacing(2);
    this->setEditTriggers(QListWidget::DoubleClicked);
    this->setMinimumWidth(dureu::APP_SCREENSHOT_HEIGHT*2+50);
}

BookmarkDelegate *BookmarkWidget::getBookmarkDelegate() const
{
    return dynamic_cast<BookmarkDelegate*>(this->itemDelegate());
}
