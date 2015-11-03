#include <iostream>
#include <QtWidgets>
#include "tabletapplication.h"

bool TabletApplication::event(QEvent* event) {
    if (event->type() == QEvent::TabletEnterProximity || event->type() == QEvent::TabletLeaveProximity) {
        //QTabletEvent* tabEvent = static_cast<QTabletEvent*>(event);
        //emit tabletProximity(event->type()==QEvent::TabletEnterProximity, tabEvent->pointerType());
        //std::cout << "proximity, device: " <<(int)static_cast<QTabletEvent*>(event)->device() << std::endl;
        bool active = event->type() == QEvent::TabletEnterProximity? 1 : 0;
        std::cout << "TabletApplication active: " << active << std::endl;
        emit sendTabletDevice(active);
        return true;
    }
    return QApplication::event(event);
}
