#ifndef MOUSEMODE_H
#define MOUSEMODE_H

#include <QObject>
#include "Settings.h"

/*! \class MouseMode
 * \brief Class that holds information and functionality on the current mouse state.
*/
class MouseMode : public QObject
{
    Q_OBJECT
public:
    /*! Constructor: initializes default mouse mode. */
    MouseMode();

    void set(const cher::MOUSE_MODE& mode);

    const cher::MOUSE_MODE getCurrent() const;

    const cher::MOUSE_MODE getPrevious() const;

    QCursor getCursor() const;

private:
    cher::MOUSE_MODE m_current; /*!< current mouse mode. */
    cher::MOUSE_MODE m_previous; /*!< previous mouse mode. */
};

#endif // MOUSEMODE_H
