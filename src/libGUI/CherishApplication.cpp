#include <iostream>

#include <QtWidgets>

#include "CherishApplication.h"
#include "Settings.h"

CherishApplication::CherishApplication(int &argv, char **argc):
    QApplication(argv, argc)
{
    Q_INIT_RESOURCE(Actions);
}

bool CherishApplication::event(QEvent* event){
    if (event->type() == QEvent::TabletEnterProximity || event->type() == QEvent::TabletLeaveProximity) {
            bool active = event->type() == QEvent::TabletEnterProximity? 1 : 0;
            outLogVal("tablet activity: ", active);
            emit sendTabletActivity(active);
            return true;
        }
    return QApplication::event(event);
}

