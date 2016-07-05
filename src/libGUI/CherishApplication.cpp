#include <iostream>
#include <algorithm>

#include <QtWidgets>
#include <QtGlobal>

#include "CherishApplication.h"
#include "Settings.h"
#include "Data.h"

namespace cher{
double DPI_SCALING = 1;
}

CherishApplication::CherishApplication(int &argv, char **argc):
    QApplication(argv, argc)
{
    this->setDPIScalingGUI();
}

bool CherishApplication::event(QEvent* event){
    if (event->type() == QEvent::TabletEnterProximity || event->type() == QEvent::TabletLeaveProximity) {
            bool active = event->type() == QEvent::TabletEnterProximity? 1 : 0;
            qDebug() << "tablet activity: " << active;
            emit setTabletActivity(active);
            return true;
        }
    return QApplication::event(event);
}

void CherishApplication::setDPIScalingGUI()
{
    int dpiX = QApplication::desktop()->physicalDpiX();
    int dpiY = QApplication::desktop()->physicalDpiY();
    int dpi = std::min(dpiX, dpiY);

    double scale = 1.;
    if (dpi <= 96)                  scale = 1.;
    else if (dpi>96 && dpi<=120)    scale = 1.25;
    else if (dpi>120 && dpi<=144)   scale = 1.5;
    else if (dpi>144 && dpi<=192)   scale = 2.0;
    else if (dpi>192 && dpi<=240)   scale = 2.5;
    else if (dpi>240 && dpi<=288)   scale = 3.0;
    else if (dpi>288 && dpi<=384)   scale = 4.0;
    else if (dpi>383 && dpi<=480)   scale = 5.0;
    else if (dpi>480)               scale = 6.0;

    cher::DPI_SCALING = scale;
    qDebug() << "DPI Scale is set to " << cher::DPI_SCALING;
}

