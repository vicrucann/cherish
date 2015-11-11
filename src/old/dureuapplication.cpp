#include <iostream>
#include "dureuapplication.h"
#include <QtWidgets>

DureuApplication::DureuApplication(int &argv, char **argc):
    QApplication(argv, argc)
{}

DureuApplication::~DureuApplication(){}

bool DureuApplication::event(QEvent* event){
    if (event->type() == QEvent::TabletEnterProximity || event->type() == QEvent::TabletLeaveProximity) {
            bool active = event->type() == QEvent::TabletEnterProximity? 1 : 0;
            emit sendTabletActivity(active);
            return true;
        }
        return QApplication::event(event);
}

