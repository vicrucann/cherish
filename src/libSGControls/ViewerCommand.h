#ifndef VIEWERCOMMAND_H
#define VIEWERCOMMAND_H

#include <QUndoCommand>

#include <osg/observer_ptr>

#include "Manipulator.h"

class ViewerCommand : public QUndoCommand
{
public:
    ViewerCommand(Manipulator* man,
                  const osg::Matrixd& invM0,
                  const osg::Matrixd& invM1,
                  QUndoCommand* parent = 0);

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    osg::observer_ptr<Manipulator> m_man;
    osg::Matrixd m_invM0;
    osg::Matrixd m_invM1;
};

#endif // VIEWERCOMMAND_H
