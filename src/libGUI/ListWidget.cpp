#include "ListWidget.h"

#include <QListWidgetItem>

#include "Settings.h"
#include "Utilities.h"

BookmarkWidget::BookmarkWidget(QWidget *parent)
    : QListWidget(parent)
{
    //this->setMovement(QListWidget::Snap);
    this->setIconSize(QSize(cher::APP_SCREENSHOT_HEIGHT * cher::DPI_SCALING,
                            cher::APP_SCREENSHOT_HEIGHT * cher::DPI_SCALING ));
    //this->setResizeMode(QListWidget::Adjust);
    this->setSelectionBehavior(QAbstractItemView::SelectItems);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setDragDropMode(QAbstractItemView::InternalMove);
    this->setSpacing(2);
    this->setEditTriggers(QListWidget::DoubleClicked);
    this->setMinimumWidth(cher::APP_WIDGET_WIDTH*cher::DPI_SCALING);
    this->setTabKeyNavigation(false);
}

BookmarkDelegate *BookmarkWidget::getBookmarkDelegate() const
{
    return dynamic_cast<BookmarkDelegate*>(this->itemDelegate());
}
