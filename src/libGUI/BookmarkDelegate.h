#ifndef BOOKMARKDELEGATE_H
#define BOOKMARKDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QModelIndex>
#include <QStyle>

/* BookmarkDelegate is class responsible for look and functionality of
 * BookmarkWidget. More precisely, it provides info on how and where to locate
 * a "delete" button within each of the bookmark.
 * Whenever the button is pressed, a dialogue is popped up asking the user to confirm
 * if they really want to delete the corresponding bookmark.
 *
 * For more info on Qt delegates, refer to offician Qt doc pages: model/view framework,
 * and delegates.
*/

class BookmarkDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    BookmarkDelegate(QObject* parent=0);
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

signals:
    void clickedDelete(const QModelIndex& index);

private:
    QRect getButtonRect(const QRect& rect) const;
};

#endif // BOOKMARKDELEGATE_H
