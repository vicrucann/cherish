#include <iostream>
#include <QtWidgets>
#include "tabletapplication.h"

bool TabletApplication::event(QEvent* event) {
    if (event->type() == QEvent::TabletEnterProximity || event->type() == QEvent::TabletLeaveProximity) {
        bool active = event->type() == QEvent::TabletEnterProximity? 1 : 0;
        emit sendTabletDevice(active);
        return true;
    }
    return QApplication::event(event);
}
