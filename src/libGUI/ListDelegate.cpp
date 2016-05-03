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
    /* delegate for parent */
    if (index.data(dureu::DelegateChildRole).toInt() == 1){
        if( (option.state & QStyle::State_Selected) || (option.state & QStyle::State_MouseOver) ){
            QVariant var = index.model()->data(index, Qt::BackgroundRole);
            painter->fillRect(option.rect, var.value<QColor>());
            painter->drawText(option.rect, index.model()->data(index, Qt::DisplayRole).toString());
        }
        else
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

        bool isNotChecked = index.data(dureu::DelegateVisibilityRole).toBool();
        buttonVis.state |= QStyle::State_Enabled;
        buttonVis.state |= isNotChecked? QStyle::State_Off : QStyle::State_On;

        QApplication::style()->drawControl(QStyle::CE_PushButton, &buttonDelete, painter);
        QApplication::style()->drawControl(QStyle::CE_PushButton, &buttonVis, painter);
    }
    /* delegate for child */
    else{
        QStyledItemDelegate::paint(painter, option, index);
    }
}

bool CanvasDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    /* delegate for parent */
    if (index.data(dureu::DelegateChildRole).toInt() == 1){
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

                        bool value = index.data(dureu::DelegateVisibilityRole).toBool();
                        model->setData(index, !value, dureu::DelegateVisibilityRole);
                    }
                    return true;
                }
        }
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }
    /* delegate for child */
    else{
        //return QStyledItemDelegate::editorEvent(event, model, option, index);
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

PhotoDelegate::PhotoDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void PhotoDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    QRect r = option.rect;
    QStyleOptionButton buttonDelete;
    QStyleOptionButton buttonPlus, buttonMinus; // photo transparency

    buttonDelete.rect = getButtonDeleteRect(r);
    buttonDelete.iconSize = QSize(dureu::APP_WIDGET_BUTTON, dureu::APP_WIDGET_BUTTON);
    buttonDelete.icon = Data::editDeleteIcon();
    buttonDelete.state = QStyle::State_Enabled;
    buttonDelete.features = QStyleOptionButton::None;

    buttonPlus.rect = getButtonPlus(r);
    buttonPlus.iconSize = QSize(dureu::APP_WIDGET_BUTTON, dureu::APP_WIDGET_BUTTON);
    buttonPlus.icon = Data::sceneImageTransparencyOnIcon();
    buttonPlus.state = QStyle::State_Enabled;
    buttonPlus.features = QStyleOptionButton::None;

    buttonMinus.rect = getButtonMinus(r);
    buttonMinus.iconSize = QSize(dureu::APP_WIDGET_BUTTON, dureu::APP_WIDGET_BUTTON);
    buttonMinus.icon = Data::sceneImageTransparencyOffIcon();
    buttonMinus.state = QStyle::State_Enabled;
    buttonMinus.features = QStyleOptionButton::None;

//    QStyleOptionSlider slider;
//    slider.rect = getButtonTransparencyRect(r);
//    slider.state = QStyle::State_Enabled;
//    slider.minimum = 0;
//    slider.maximum = 100;
//    slider.singleStep = 25;
//    slider.sliderPosition = 100;
//    slider.tickPosition = QSlider::TicksBelow;
//    QApplication::style()->drawComplexControl(QStyle::CC_Slider, &slider, painter);

    QApplication::style()->drawControl(QStyle::CE_PushButton, &buttonDelete, painter);
    QApplication::style()->drawControl(QStyle::CE_PushButton, &buttonMinus, painter);
    QApplication::style()->drawControl(QStyle::CE_PushButton, &buttonPlus, painter);
}

bool PhotoDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent * e = (QMouseEvent *)event;
        int clickX = e->x();
        int clickY = e->y();

        QRect r = option.rect;
        QRect bplus = this->getButtonPlus(r);
        QRect bminus = this->getButtonMinus(r);
        QRect br = this->getButtonDeleteRect(r);

        if( clickX > br.x() && clickX < br.x() + br.width() )
            if( clickY > br.y() && clickY < br.y() + br.height() )
            {
                emit this->clickedDelete(index);
                return true;
            }

        if( clickX > bplus.x() && clickX < bplus.x() + bplus.width() )
            if( clickY > bplus.y() && clickY < bplus.y() + bplus.height() )
            {
                emit this->clickedTransparencyPlus(index);
                return true;
            }

        if( clickX > bminus.x() && clickX < bminus.x() + bminus.width() )
            if( clickY > bminus.y() && clickY < bminus.y() + bminus.height() )
            {
                emit this->clickedTransparencyMinus(index);
                return true;
            }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QRect PhotoDelegate::getButtonDeleteRect(const QRect &rect) const
{
    int sz = dureu::APP_WIDGET_BUTTON;
    int x,y,w,h;
    x = rect.left() + rect.width() - sz;
    y = rect.top() + (rect.height() - sz)/2;
    w = h = sz;
    return QRect(x,y,w,h);
}

QRect PhotoDelegate::getButtonPlus(const QRect &rect) const
{
    int sz = dureu::APP_WIDGET_BUTTON;
    int x,y,w,h;
    x = rect.left() + rect.width() - sz*2 - sz/2;
    y = rect.top() + (rect.height() - sz)/2;
    w = h = sz;
    return QRect(x,y,w,h);
}

QRect PhotoDelegate::getButtonMinus(const QRect &rect) const
{
    int sz = dureu::APP_WIDGET_BUTTON;
    int x,y,w,h;
    x = rect.left() + rect.width() - sz*3 - sz;
    y = rect.top() + (rect.height() - sz)/2;
    w = h = sz;
    return QRect(x,y,w,h);
}
