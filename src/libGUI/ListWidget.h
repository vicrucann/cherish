#ifndef LISTWIDGET_H
#define LISTWIDGET_H

#include <string>

#include <QListWidget>
#include <QListView>
#include <QEvent>
#include <QModelIndex>
#include <QMouseEvent>

#include "Bookmarks.h"
#include "ListDelegate.h"

/*! \class BookmarkWidget
 * \brief Re-defined QListWidget that contains a method to obtain the corresponding delegate.
*/
class BookmarkWidget : public QListWidget
{
public:
    /*! Constructor. */
    BookmarkWidget(QWidget* parent = 0);

    /*! \return a non-const pointer to the BookmarkDelegate. */
    BookmarkDelegate* getBookmarkDelegate() const;
};

#endif // LISTWIDGET_H
