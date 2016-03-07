#include "BookmarkDelegate.h"

#include <QStyleOptionButton>
#include <QApplication>
#include <QMouseEvent>
#include <QDialog>
#include <QStandardItemModel>
#include <QListWidget>

#include "Data.h"
#include "Settings.h"

BookmarkDelegate::BookmarkDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void BookmarkDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    QStyledItemDelegate::paint(painter, option, index);

    QRect r = option.rect;
    QStyleOptionButton buttonDelete, buttonMove;

    buttonDelete.rect = getButtonDeleteRect(r);
    buttonDelete.iconSize = QSize(dureu::APP_WIDGET_BUTTON, dureu::APP_WIDGET_BUTTON);
    buttonDelete.icon = Data::editDeleteIcon();
    buttonDelete.state = QStyle::State_Enabled;
    buttonDelete.features = QStyleOptionButton::None;

    buttonMove.rect = getButtonMoveRect(r);
    buttonMove.iconSize = QSize(dureu::APP_WIDGET_BUTTON, dureu::APP_WIDGET_BUTTON);
    buttonMove.icon = Data::controlMoveIcon();
    buttonMove.state = QStyle::State_Enabled;
    buttonMove.features = QStyleOptionButton::None;

    QApplication::style()->drawControl(QStyle::CE_PushButton, &buttonDelete, painter);
    QApplication::style()->drawControl(QStyle::CE_PushButton, &buttonMove, painter);
}

bool BookmarkDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonPress ||
            event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent * e = (QMouseEvent *)event;
        int clickX = e->x();
        int clickY = e->y();

        QRect r = option.rect;
        QRect br = this->getButtonDeleteRect(r);
        QRect bm = this->getButtonMoveRect(r);

        if( clickX > br.x() && clickX < br.x() + br.width() )
            if( clickY > br.y() && clickY < br.y() + br.height() )
            {
                if (event->type() == QEvent::MouseButtonPress)
                    return true;
                else{
                    outLogMsg("delegate: clicked delete");
                    emit this->clickedDelete(index);
                }
                return true;
            }
        if( clickX > bm.x() && clickX < bm.x() + bm.width() )
            if( clickY > bm.y() && clickY < bm.y() + bm.height() )
            {
                if (event->type() == QEvent::MouseButtonPress){
                    // emit to change current before performing the move
                    emit this->clickedMove(index);
                }
            }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QRect BookmarkDelegate::getButtonDeleteRect(const QRect &rect) const
{
    int sz = dureu::APP_WIDGET_BUTTON;
    int x,y,w,h;
    x = rect.left() + rect.width() - sz;
    y = rect.top() + (rect.height() - sz)/2;
    w = h = sz;
    return QRect(x,y,w,h);
}

QRect BookmarkDelegate::getButtonMoveRect(const QRect &rect) const
{
    int sz = dureu::APP_WIDGET_BUTTON;
    int x,y,w,h;
    x = rect.left() + rect.width() - 2.5*sz;
    y = rect.top() + (rect.height() - sz)/2;
    w = h = sz;
    return QRect(x,y,w,h);
}
