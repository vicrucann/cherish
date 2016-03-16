#include "ListDelegate.h"

#include <QStyleOptionButton>
#include <QApplication>
#include <QMouseEvent>
#include <QDialog>
#include <QStandardItemModel>
#include <QListWidget>
#include <QPushButton>
#include <QStylePainter>

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

CanvasDelegate::CanvasDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void CanvasDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    QRect r = option.rect;
    QStyleOptionButton buttonDelete, buttonVis;

    buttonDelete.rect = getButtonDeleteRect(r);
    buttonDelete.iconSize = QSize(dureu::APP_WIDGET_BUTTON, dureu::APP_WIDGET_BUTTON);
    buttonDelete.icon = Data::editDeleteIcon();
    buttonDelete.state = QStyle::State_Enabled;
    buttonDelete.features = QStyleOptionButton::None;

    buttonVis.rect = getButtonVisibilityRect(r);
    buttonVis.iconSize = QSize(dureu::APP_WIDGET_BUTTON, dureu::APP_WIDGET_BUTTON);
    buttonVis.icon = Data::controlCanvasVisibilityIcon();

    if (option.state.testFlag(QStyle::State_Sunken)){
        outLogMsg("option is sunken");
        buttonVis.state = QStyle::State_Enabled | QStyle::State_Off;
    }
    else
        buttonVis.state = QStyle::State_Enabled | QStyle::State_On;

    //buttonVis.state = QStyle::State_Enabled | QStyle::State_On;

    buttonVis.features = QStyleOptionButton::None;
    buttonVis.icon.On;

    QApplication::style()->drawControl(QStyle::CE_PushButton, &buttonDelete, painter);
    QApplication::style()->drawControl(QStyle::CE_PushButton, &buttonVis, painter);
}

bool CanvasDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonPress ||
            event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent * e = (QMouseEvent *)event;
        int clickX = e->x();
        int clickY = e->y();

        QRect r = option.rect;
        QRect br = this->getButtonDeleteRect(r);
        QRect bv = this->getButtonVisibilityRect(r);

        if( clickX > br.x() && clickX < br.x() + br.width() )
            if( clickY > br.y() && clickY < br.y() + br.height() )
            {
                if (event->type() == QEvent::MouseButtonPress)
                    return true;
                else{
                    outLogMsg("canvas delegate: clicked delete");
                    emit this->clickedDelete(index);
                }
                return true;
            }

        if( clickX > bv.x() && clickX < bv.x() + bv.width() )
            if( clickY > bv.y() && clickY < bv.y() + bv.height() )
            {
                if (event->type() == QEvent::MouseButtonPress){
                    outLogMsg("canvas delegate: clicked visibility");
                    emit this->clickedVisibility(index);
//                    if (option.checkState == Qt::Checked)
//                        option.checkState = QStyle::State_Enabled | QStyle::State_Off;
//                    else option.checkState = QStyle::State_Enabled | QStyle::State_On;

                }
                return true;
            }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QRect CanvasDelegate::getButtonDeleteRect(const QRect &rect) const
{
    int sz = dureu::APP_WIDGET_BUTTON;
    int x,y,w,h;
    x = rect.left() + rect.width() - sz;
    y = rect.top() + (rect.height() - sz)/2;
    w = h = sz;
    return QRect(x,y,w,h);
}

QRect CanvasDelegate::getButtonVisibilityRect(const QRect &rect) const
{
    int sz = dureu::APP_WIDGET_BUTTON;
    int x,y,w,h;
    x = rect.left() + rect.width() - 2.5*sz;
    y = rect.top() + (rect.height() - sz)/2;
    w = h = sz;
    return QRect(x,y,w,h);
}
