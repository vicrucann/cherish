#ifndef LISTDELEGATE_H
#define LISTDELEGATE_H

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

/*! \class BookmarkDelegate
 * Class description
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
    void clickedMove(const QModelIndex& index);
    void clickedApplyState(const QModelIndex& index);

private:
    QRect getButtonDeleteRect(const QRect& rect) const;
    QRect getButtonMoveRect(const QRect& rect) const;
    QRect getButtonStateRect(const QRect& rect) const;

    bool isInside(int clickX, int clickY, const QRect& rect) const;
};

/*! \class CanvasDelegate
 * Class description
*/
class CanvasDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    CanvasDelegate(QObject* parent=0);

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:
    void clickedDelete(const QModelIndex& index);
    void clickedDeletePhoto(const QModelIndex& index);
    void clickedVisibilitySet(int index);
    void clickedTransparencyPlus(const QModelIndex& index);
    void clickedTransparencyMinus(const QModelIndex& index);

public:
    QRect getButtonDeleteRect(const QRect& rect) const;
    QRect getButtonVisibilityRect(const QRect& rect) const;
    QRect getButtonPlusRect(const QRect& rect) const;
    QRect getButtonMinusRect(const QRect& rect) const;

};


/*! \class PhotoDelegate
 * Class description
*/
class PhotoDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    PhotoDelegate(QObject* parent = 0);

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

signals:
    void clickedDelete(const QModelIndex& index);
    void clickedTransparencyPlus(const QModelIndex& index);
    void clickedTransparencyMinus(const QModelIndex& index);

private:
    QRect getButtonDeleteRect(const QRect& rect) const;
    QRect getButtonPlus(const QRect& rect) const;
    QRect getButtonMinus(const QRect& rect) const;
};

#endif // LISTDELEGATE_H
