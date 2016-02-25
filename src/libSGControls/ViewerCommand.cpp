#include "ViewerCommand.h"

ViewerCommand::ViewerCommand(GLWidget *widget, Manipulator *man, const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up, const osg::Vec3d &d1, const osg::Vec3d &d2, const osg::Vec3d &d3, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_widget(widget)
    , m_man(man)
    , m_eye(eye)
    , m_center(center)
    , m_up(up)
    , de(d1)
    , dc(d2)
    , du(d3)
{
    this->setText(QString("Change of view point"));
}

void ViewerCommand::undo()
{
    m_man->setTransformation(m_eye, m_center, m_up);
    m_widget->update();
}

void ViewerCommand::redo()
{
    m_man->setTransformation(m_eye+de, m_center+dc, m_up+du);
    m_widget->update();
}
