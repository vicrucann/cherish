/* AddStrokeCommand
 *
 * One of the class' purpose is to provide undo/redo
 * functionality for new stroke addition to a current canvas.
 * Another main purpose is the control over Stroke class. It means
 * this class is the higher level API to manage the Stroke data variable.
 *
 * This class needs to be considered in combination with other QUndoCommand-
 * derived classes such as deleteStrokeCommand, pushStrokeCommand,
 * editStrokeCommand.
 *
 * For more info  on QUndoCommand, see example:
 * http://doc.qt.io/qt-5/qtwidgets-tools-undoframework-example.html
 */

#ifndef ADDSTROKECOMMAND_H
#define ADDSTROKECOMMAND_H

#include <QUndoCommand>

#include <osg/ref_ptr>
#include <osg/observer_ptr>

#include "UserScene.h"
#include "Canvas.h"
#include "Stroke.h"

class AddStrokeCommand : public QUndoCommand
{
public:
    AddStrokeCommand(entity::UserScene* scene, entity::Stroke* stroke, QUndoCommand *parent = 0);
    ~AddStrokeCommand();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    bool checkPointers() const;

    osg::observer_ptr<entity::UserScene> m_scene;
    osg::ref_ptr<entity::Stroke> m_stroke;
};

#endif // ADDSTROKECOMMAND_H
