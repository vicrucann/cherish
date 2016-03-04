#ifndef VIEWERCOMMAND_H
#define VIEWERCOMMAND_H

#include <QUndoCommand>

#include <osg/observer_ptr>

#include "Manipulator.h"
#include "../libGUI/GLWidget.h"

class GLWidget;

class ViewerCommand : public QUndoCommand
{
public:
    ViewerCommand(GLWidget* widget, Manipulator* man,
                  const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up,
                  const osg::Vec3d& d1, const osg::Vec3d& d2, const osg::Vec3d& d3,
                  QUndoCommand* parent = 0);

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    GLWidget* m_widget;
    osg::observer_ptr<Manipulator> m_man;
    osg::Vec3d m_eye, m_center, m_up;
    osg::Vec3d de, dc, du;
    osg::Matrixd m_M;
};

#endif // VIEWERCOMMAND_H
