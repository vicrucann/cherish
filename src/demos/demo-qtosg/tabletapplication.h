#ifndef TABLETAPPLICATION
#define TABLETAPPLICATION

/* TabletApplication
 * Inherits from QApplication so that to handle tablet events
 * which are not sent to tabletEvent(), and also so that to
 * reimplement the event() function.
 * The device type of the events which are handled in the event()
 * function are sent to the OSGWidget.
 * The TabletEnterProximity and TabletLeaveProximity events are
 * not sent to the QApplication objects, while other tablet events
 * are sent to the QWidget event(), which sends them on to tabletEvent().
 *
 * Victoria Rudakova, Yale Graphics, 2015
 * <victoria.rudakova@yale.edu>
 */

#include <QApplication>
#include <QTabletEvent>
#include "osgwidget.h"

class TabletApplication : public QApplication {
    Q_OBJECT
public:
    TabletApplication(int& argv, char** argc): QApplication(argv, argc) {}
signals:
    void tabletProximity(bool near, QTabletEvent::PointerType pointer);
    void sendTabletDevice(bool active);
protected:
    bool event(QEvent* event) Q_DECL_OVERRIDE;
private:
};

#endif // TABLETAPPLICATION

