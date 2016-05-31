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
    QStyleOptionButton buttonDelete, buttonMove, buttonState;

    buttonDelete.rect = getButtonDeleteRect(r);
    buttonDelete.iconSize = QSize(cher::APP_WIDGET_BUTTON*cher::DPI_SCALING, cher::APP_WIDGET_BUTTON*cher::DPI_SCALING);
    buttonDelete.icon = Data::editDeleteIcon();
    buttonDelete.state = QStyle::State_Enabled;
    buttonDelete.features = QStyleOptionButton::None;

    buttonMove.rect = getButtonMoveRect(r);
    buttonMove.iconSize = QSize(cher::APP_WIDGET_BUTTON*cher::DPI_SCALING, cher::APP_WIDGET_BUTTON*cher::DPI_SCALING);
    buttonMove.icon = Data::controlMoveIcon();
    buttonMove.state = QStyle::State_Enabled;
    buttonMove.features = QStyleOptionButton::None;

    buttonState.rect = getButtonStateRect(r);
    buttonState.iconSize = QSize(cher::APP_WIDGET_BUTTON*cher::DPI_SCALING, cher::APP_WIDGET_BUTTON*cher::DPI_SCALING);
    buttonState.icon = Data::controlBookmarkStateIcon();
    buttonState.state = QStyle::State_Enabled;
    buttonState.features = QStyleOptionButton::None;

    QApplication::style()->drawControl(QStyle::CE_PushButtonLabel, &buttonDelete, painter);
    QApplication::style()->drawControl(QStyle::CE_PushButtonLabel, &buttonMove, painter);
    QApplication::style()->drawControl(QStyle::CE_PushButtonLabel, &buttonState, painter);
}

bool BookmarkDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonPress ||
            event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent * e = (QMouseEvent *)event;
        int clickX = e->x();
        int clickY = e->y();

        QRect r = option.rect;

        if( isInside(clickX, clickY, this->getButtonDeleteRect(r)) )
            {
                if (event->type() == QEvent::MouseButtonPress)
                    return true;
                else{
                    outLogMsg("delegate: clicked delete");
                    emit this->clickedDelete(index);
                }
                return true;
            }

        if( isInside(clickX, clickY, this->getButtonStateRect(r)) )
            {
                if (event->type() == QEvent::MouseButtonPress)
                    return true;
                else{
                    outLogMsg("delegate: clicked apply scene state");
                    emit this->clickedApplyState(index);
                }
                return true;
            }

        if( isInside(clickX, clickY, this->getButtonMoveRect(r)) )
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
    int sz = cher::APP_WIDGET_BUTTON*cher::DPI_SCALING;
    int x,y,w,h;
    x = rect.left() + rect.width() - sz;
    y = rect.top() + (rect.height() - sz)/2;
    w = h = sz;
    return QRect(x,y,w,h);
}

QRect BookmarkDelegate::getButtonMoveRect(const QRect &rect) const
{
    int sz = cher::APP_WIDGET_BUTTON*cher::DPI_SCALING;
    int x,y,w,h;
    x = rect.left() + rect.width() - 2.5*sz;
    y = rect.top() + (rect.height() - sz)/2;
    w = h = sz;
    return QRect(x,y,w,h);
}

QRect BookmarkDelegate::getButtonStateRect(const QRect &rect) const
{
    int sz = cher::APP_WIDGET_BUTTON*cher::DPI_SCALING;
    int x,y,w,h;
    x = rect.left() + rect.width() - 4*sz;
    y = rect.top() + (rect.height() - sz)/2;
    w = h = sz;
    return QRect(x,y,w,h);
}

bool BookmarkDelegate::isInside(int clickX, int clickY, const QRect &rect) const
{
    return (clickX > rect.x() && clickX < rect.x()+rect.width() &&
            clickY > rect.y() && clickY < rect.y()+rect.height());
}

CanvasDelegate::CanvasDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void CanvasDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect r = option.rect;

    /* delegate for parent (canvas) */
    if (index.data(cher::DelegateChildRole).toInt() == 1){
        QStyledItemDelegate::paint(painter, option, index);

        QVariant var = index.model()->data(index, cher::DelegateBGColor);
        QColor color = var.value<QColor>();
        if (color != Qt::white){
            QString text = index.model()->data(index, Qt::DisplayRole).toString();
            QFontMetrics fm = painter->fontMetrics();
            int width = fm.width(text);
            int lineWidth = cher::APP_WIDGET_LINEWIDTH*cher::DPI_SCALING;

            /* make sure line is drawn strictly within a row, for this
             * we substract in y coordinate according to linewidth, and
             * we add same value in x coordinate */
            QPoint P1 = r.bottomLeft() - QPoint(-lineWidth/2, lineWidth/2);
            QPoint P2 = P1 + QPoint(width, 0);
            QPen pen(color);
            pen.setWidth(lineWidth);

            painter->setPen(pen);
            painter->drawLine(P1, P2);
        }


        QStyleOptionButton buttonDelete, buttonVis;

        buttonDelete.rect = getButtonDeleteRect(r);
        buttonDelete.iconSize = QSize(cher::APP_WIDGET_BUTTON*cher::DPI_SCALING,
                                      cher::APP_WIDGET_BUTTON*cher::DPI_SCALING);
        buttonDelete.icon = Data::editDeleteIcon();
        buttonDelete.state = QStyle::State_Enabled;
        buttonDelete.features = QStyleOptionButton::None;

        buttonVis.rect = getButtonVisibilityRect(r);
        buttonVis.iconSize = QSize(cher::APP_WIDGET_BUTTON*cher::DPI_SCALING,
                                   cher::APP_WIDGET_BUTTON*cher::DPI_SCALING);
        buttonVis.icon = Data::controlCanvasVisibilityIcon();

        bool isNotChecked = index.data(cher::DelegateVisibilityRole).toBool();
        buttonVis.state |= QStyle::State_Enabled;
        buttonVis.state |= isNotChecked? QStyle::State_Off : QStyle::State_On;

        QApplication::style()->drawControl(QStyle::CE_PushButtonLabel, &buttonDelete, painter);
        QApplication::style()->drawControl(QStyle::CE_PushButtonLabel, &buttonVis, painter);
    }
    /* delegate for child photo */
    else{
        QStyledItemDelegate::paint(painter, option, index);

        QStyleOptionButton buttonDelete;
        buttonDelete.rect = getButtonDeleteRect(r);
        buttonDelete.iconSize = QSize(cher::APP_WIDGET_BUTTON*cher::DPI_SCALING,
                                      cher::APP_WIDGET_BUTTON*cher::DPI_SCALING);
        buttonDelete.icon = Data::editDeleteIcon();
        buttonDelete.state = QStyle::State_Enabled;
        buttonDelete.features = QStyleOptionButton::None;
        QApplication::style()->drawControl(QStyle::CE_PushButtonLabel, &buttonDelete, painter);

        QStyleOptionButton buttonPlus;
        buttonPlus.rect = getButtonPlusRect(r);
        buttonPlus.iconSize = QSize(cher::APP_WIDGET_BUTTON*cher::DPI_SCALING,
                                    cher::APP_WIDGET_BUTTON*cher::DPI_SCALING);
        buttonPlus.icon = Data::sceneImageTransparencyOnIcon();
        buttonPlus.state = QStyle::State_Enabled;
        buttonPlus.features = QStyleOptionButton::None;
        QApplication::style()->drawControl(QStyle::CE_PushButtonLabel, &buttonPlus, painter);

        QStyleOptionButton buttonMinus;
        buttonMinus.rect = getButtonMinusRect(r);
        buttonMinus.iconSize = QSize(cher::APP_WIDGET_BUTTON*cher::DPI_SCALING,
                                     cher::APP_WIDGET_BUTTON*cher::DPI_SCALING);
        buttonMinus.icon = Data::sceneImageTransparencyOffIcon();
        buttonMinus.state = QStyle::State_Enabled;
        buttonMinus.features = QStyleOptionButton::None;
        QApplication::style()->drawControl(QStyle::CE_PushButtonLabel, &buttonMinus, painter);
    }
}

bool CanvasDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    /* delegate for parent */
    if (index.data(cher::DelegateChildRole).toInt() == 1){
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

                        bool value = index.data(cher::DelegateVisibilityRole).toBool();
                        model->setData(index, !value, cher::DelegateVisibilityRole);
                    }
                    return true;
                }
        }
    }
    /* delegate for child */
    else{
        if (event->type() == QEvent::MouseButtonPress ||
                event->type() == QEvent::MouseButtonRelease){
            QMouseEvent * e = (QMouseEvent *)event;
            int clickX = e->x();
            int clickY = e->y();
            QRect r = option.rect;
            QRect rDelete = this->getButtonDeleteRect(r);
            if( clickX > rDelete.x() && clickX < rDelete.x() + rDelete.width() )
                if( clickY > rDelete.y() && clickY < rDelete.y() + rDelete.height() )
                {
                    if (event->type() == QEvent::MouseButtonPress)
                        return true;
                    else{
                        outLogMsg("canvas delegate: clicked delete photo");
                        emit this->clickedDeletePhoto(index);
                    }
                    return true;
                }

            // TODO: should the transparency be included in undo/redo FW?
            QRect rPlus = this->getButtonPlusRect(r);
            if( clickX > rPlus.x() && clickX < rPlus.x() + rPlus.width() )
                if( clickY > rPlus.y() && clickY < rPlus.y() + rPlus.height() )
                {
                    if (event->type() == QEvent::MouseButtonPress)
                        return true;
                    outLogMsg("canvas delegate: photo transparency plus");
                    emit this->clickedTransparencyPlus(index);
                    return true;
                }

            QRect rMinus = this->getButtonMinusRect(r);
            if( clickX > rMinus.x() && clickX < rMinus.x() + rMinus.width() )
                if( clickY > rMinus.y() && clickY < rMinus.y() + rMinus.height() )
                {
                    if (event->type() == QEvent::MouseButtonPress)
                        return true;
                    outLogMsg("canvas delegate: photo transparency minus");
                    emit this->clickedTransparencyMinus(index);
                    return true;
                }
        }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QSize CanvasDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QStyledItemDelegate::sizeHint(option, index) +
            QSize(0,cher::APP_WIDGET_GAP*cher::DPI_SCALING);
}

QRect CanvasDelegate::getButtonDeleteRect(const QRect &rect) const
{
    int sz = cher::APP_WIDGET_BUTTON*cher::DPI_SCALING;
    int x,y,w,h;
    x = rect.left() + rect.width() - sz;
    y = rect.top() + (rect.height() - sz)/2;
    w = h = sz;
    return QRect(x,y,w,h);
}

QRect CanvasDelegate::getButtonVisibilityRect(const QRect &rect) const
{
    int sz = cher::APP_WIDGET_BUTTON*cher::DPI_SCALING;
    int x,y,w,h;
    x = rect.left() + rect.width() - 2.5*sz;
    y = rect.top() + (rect.height() - sz)/2;
    w = h = sz;
    return QRect(x,y,w,h);
}

QRect CanvasDelegate::getButtonPlusRect(const QRect &rect) const
{
    int sz = cher::APP_WIDGET_BUTTON*cher::DPI_SCALING;
    int x,y,w,h;
    x = rect.left() + rect.width() - 2.5*sz;
    y = rect.top() + (rect.height() - sz)/2;
    w = h = sz;
    return QRect(x,y,w,h);
}

QRect CanvasDelegate::getButtonMinusRect(const QRect &rect) const
{
    int sz = cher::APP_WIDGET_BUTTON*cher::DPI_SCALING;
    int x,y,w,h;
    x = rect.left() + rect.width() - 4*sz;
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
    buttonDelete.iconSize = QSize(cher::APP_WIDGET_BUTTON*cher::DPI_SCALING,
                                  cher::APP_WIDGET_BUTTON*cher::DPI_SCALING);
    buttonDelete.icon = Data::editDeleteIcon();
    buttonDelete.state = QStyle::State_Enabled;
    buttonDelete.features = QStyleOptionButton::None;

    buttonPlus.rect = getButtonPlus(r);
    buttonPlus.iconSize = QSize(cher::APP_WIDGET_BUTTON*cher::DPI_SCALING,
                                cher::APP_WIDGET_BUTTON*cher::DPI_SCALING);
    buttonPlus.icon = Data::sceneImageTransparencyOnIcon();
    buttonPlus.state = QStyle::State_Enabled;
    buttonPlus.features = QStyleOptionButton::None;

    buttonMinus.rect = getButtonMinus(r);
    buttonMinus.iconSize = QSize(cher::APP_WIDGET_BUTTON*cher::DPI_SCALING,
                                 cher::APP_WIDGET_BUTTON*cher::DPI_SCALING);
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
    int sz = cher::APP_WIDGET_BUTTON*cher::DPI_SCALING;
    int x,y,w,h;
    x = rect.left() + rect.width() - sz;
    y = rect.top() + (rect.height() - sz)/2;
    w = h = sz;
    return QRect(x,y,w,h);
}

QRect PhotoDelegate::getButtonPlus(const QRect &rect) const
{
    int sz = cher::APP_WIDGET_BUTTON*cher::DPI_SCALING;
    int x,y,w,h;
    x = rect.left() + rect.width() - sz*2 - sz/2;
    y = rect.top() + (rect.height() - sz)/2;
    w = h = sz;
    return QRect(x,y,w,h);
}

QRect PhotoDelegate::getButtonMinus(const QRect &rect) const
{
    int sz = cher::APP_WIDGET_BUTTON*cher::DPI_SCALING;
    int x,y,w,h;
    x = rect.left() + rect.width() - sz*3 - sz;
    y = rect.top() + (rect.height() - sz)/2;
    w = h = sz;
    return QRect(x,y,w,h);
}
