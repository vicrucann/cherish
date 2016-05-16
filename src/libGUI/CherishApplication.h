#ifndef DUREUAPPLICATION_H
#define DUREUAPPLICATION_H

#include <QApplication>
#include <QTabletEvent>

/*! \class CherishApplication
 * \brief Class that is derivative of QApplication and runs the application process.
*/
class CherishApplication : public QApplication {
    Q_OBJECT

public:
    CherishApplication(int& argv, char** argc);

signals:
    /*! A signal which connects Wacom pen's activity (on and off) with the corresponding
     * slot at GLWidget. This signal is obsolete for Qt version >= 5.6. But it is necessary
     * for earlier versions, for more details, see:
     * http://stackoverflow.com/questions/31172532/mousepressevent-of-qwidget-gets-called-even-though-qtabletevent-was-accepted/33525420#33525420
     * \param active is a boolean variable that denotes whether Wacom pen enters (true) or
     * leaves (false) the tablet proximity. */
    void sendTabletActivity(bool active);

protected:
    /*! A redefined event method which captures Wacom pen's activities (enters or leaves tablet
     * proximity). This method emits sendTabletActivity().
     * \param event is the event that enters the application. */
    bool event(QEvent* event) Q_DECL_OVERRIDE;
};

#endif // DUREUAPPLICATION_H
