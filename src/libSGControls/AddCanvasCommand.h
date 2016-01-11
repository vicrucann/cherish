#ifndef ADDCANVASCOMMAND_H
#define ADDCANVASCOMMAND_H

#include <QUndoCommand>

#include <osg/ref_ptr>
#include <osg/observer_ptr>
#include <osg/MatrixTransform>

#include "rootscene.h"
#include "Canvas.h"

class RootScene;

class AddCanvasCommand : public QUndoCommand
{
public:
    AddCanvasCommand(RootScene* scene, osg::MatrixTransform* transform, const std::string& name,
                     QUndoCommand* parent = 0);
    AddCanvasCommand(RootScene* scene, const osg::Matrix& R, const osg::Matrix& T,
                     const std::string& name, QUndoCommand* parent = 0);
    ~AddCanvasCommand();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:

    osg::observer_ptr<RootScene> m_scene;
    osg::ref_ptr<entity::Canvas> m_canvas;
};

#endif // ADDCANVASCOMMAND_H
