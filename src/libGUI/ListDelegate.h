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
 * \brief Inherited from QStyledItemDelegate, it defines the look and functionality of BookmarkWidget.
*/
class BookmarkDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    /*! Constructor. */
    BookmarkDelegate(QObject* parent=0);

    /*! Re-defined method that describes how to place elements within the widget. */
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /*! Re-defined method that provides functionality within the widget. */
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

signals:
    /*! Signal which is emitted whenver the user presses delete button for the corresponding bookmark. */
    void clickedDelete(const QModelIndex& index);

    /*! Signal which is emitted whenver the user presses the move button of the corresponding bookmark. */
    void clickedMove(const QModelIndex& index);

    /*! Signal which is emitted whenver the user presses the cog button of the corresponding bookmark. */
    void clickedApplyState(const QModelIndex& index);

private:
    QRect getButtonDeleteRect(const QRect& rect) const;
    QRect getButtonMoveRect(const QRect& rect) const;
    QRect getButtonStateRect(const QRect& rect) const;

    bool isInside(int clickX, int clickY, const QRect& rect) const;
};

/*! \class CanvasDelegate
 * \brief Inherited from QStyledItemDelegate, it defines the look and functionality of CanvasPhotoWidget.
*/
class CanvasDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    /*! Constructor. */
    CanvasDelegate(QObject* parent=0);

    /*! Re-defined method that describes how to place elements within the widget. */
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /*! Re-defined method that provides functionality within the widget. */
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

    /*! Re-defined method that makes sure the consistency of widget item height across different DPIs. */
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:
    /*! Signal which is emitted whenver the user presses delete button of the corresponding canvas. */
    void clickedDelete(const QModelIndex& index);

    /*! Signal which is emitted whenver the user presses delete button of the corresponding canvas' photo. */
    void clickedDeletePhoto(const QModelIndex& index);

    /*! Signal which is emitted whenver the user changes the visibility of the corresponding canvas. */
    void clickedVisibilitySet(int index);

    /*! Signal which is emitted whenver the user increases transparency of corresponding photo. */
    void clickedTransparencyPlus(const QModelIndex& index);

    /*! Signal which is emitted whenver the user decreases transparency of corresponding photo. */
    void clickedTransparencyMinus(const QModelIndex& index);

public: // made public to make sure tests have access to these methods
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    QRect getButtonDeleteRect(const QRect& rect) const;
    QRect getButtonVisibilityRect(const QRect& rect) const;
    QRect getButtonPlusRect(const QRect& rect) const;
    QRect getButtonMinusRect(const QRect& rect) const;
#endif // DOXYGEN_SHOULD_SKIP_THIS
};

#endif // LISTDELEGATE_H
