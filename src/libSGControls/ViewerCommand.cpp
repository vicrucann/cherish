#include "ViewerCommand.h"

ViewerCommand::ViewerCommand(Manipulator *man, const osg::Matrixd &invM0, const osg::Matrixd &invM1, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_man(man)
    , m_invM0(invM0)
    , m_invM1(invM1)
{
    this->setText(QString("Change of view point"));
}

void ViewerCommand::undo()
{
    m_man->setByInverseMatrix(m_invM0);
}

void ViewerCommand::redo()
{
    m_man->setByInverseMatrix(m_invM1);
}
