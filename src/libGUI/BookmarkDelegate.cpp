#include "BookmarkDelegate.h"

#include <QStyleOptionButton>
#include <QApplication>
#include <QMouseEvent>
#include <QDialog>
#include <QStandardItemModel>

#include "Data.h"
#include "Settings.h"

BookmarkDelegate::BookmarkDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void BookmarkDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    QStyledItemDelegate::paint(painter, option, index);

    int sz = dureu::APP_WIDGET_BUTTON;
    QRect r = option.rect;
    QStyleOptionButton button;

    button.rect = getButtonRect(r);
    button.iconSize = QSize(dureu::APP_WIDGET_BUTTON, dureu::APP_WIDGET_BUTTON);
    button.icon = Data::editDeleteIcon();
    button.state = QStyle::State_Enabled;
    button.features = QStyleOptionButton::None;

    QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
}

bool BookmarkDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent * e = (QMouseEvent *)event;
        int clickX = e->x();
        int clickY = e->y();

        QRect r = option.rect;
        QRect br = this->getButtonRect(r);

        if( clickX > br.x() && clickX < br.x() + br.width() )
            if( clickY > br.y() && clickY < br.y() + br.height() )
            {
                if (event->type() == QEvent::MouseButtonRelease)
                    return true;
                else
                    emit this->clickedDelete(model, index);
            }
        return true;
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QRect BookmarkDelegate::getButtonRect(const QRect &rect) const
{
    int sz = dureu::APP_WIDGET_BUTTON;
    int x,y,w,h;
    x = rect.left() + rect.width() - sz;
    y = rect.top() + (rect.height() - sz)/2;
    w = h = sz;
    return QRect(x,y,w,h);
}
