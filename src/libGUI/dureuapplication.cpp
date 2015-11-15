#include <iostream>

#include <QtWidgets>

#include "dureuapplication.h"

DureuApplication::DureuApplication(int &argv, char **argc):
    QApplication(argv, argc)
{}

DureuApplication::~DureuApplication(){}

bool DureuApplication::event(QEvent* event){
    if (event->type() == QEvent::TabletEnterProximity || event->type() == QEvent::TabletLeaveProximity) {
            bool active = event->type() == QEvent::TabletEnterProximity? 1 : 0;
            std::cerr << "tablet active: " << active << std::endl;
            emit sendTabletActivity(active);
            return true;
        }
    return QApplication::event(event);
}

