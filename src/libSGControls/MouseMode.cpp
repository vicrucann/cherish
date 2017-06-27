#include "MouseMode.h"

#include "Utilities.h"

MouseMode::MouseMode()
    : m_current(cher::SELECT_ENTITY)
    , m_previous(cher::SELECT_ENTITY)
{

}

void MouseMode::set(const cher::MOUSE_MODE &mode)
{
    m_previous = m_current;
    m_current = mode;
}

const cher::MOUSE_MODE MouseMode::getCurrent() const
{
    return m_current;
}

const cher::MOUSE_MODE MouseMode::getPrevious() const
{
    return m_previous;
}

QCursor MouseMode::getCursor() const
{
    return Utilities::getCursorFromMode(m_current);
}
