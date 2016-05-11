#ifndef EDITENTITYCOMMAND_H
#define EDITENTITYCOMMAND_H

#include <vector>

#include <QUndoCommand>

#include <osg/observer_ptr>

#include "UserScene.h"
#include "Canvas.h"
#include "Photo.h"
#include "Stroke.h"

/*! \class EditCanvasOffsetCommand
 * Class description
*/
class EditCanvasOffsetCommand : public QUndoCommand
{
public:
    EditCanvasOffsetCommand(entity::UserScene* scene, const osg::Vec3f& translate, QUndoCommand* parent = 0);
    ~EditCanvasOffsetCommand();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::Vec3f m_translate;
};

/*! \class EditCanvasRotateCommand
 * Class description
*/
class EditCanvasRotateCommand : public QUndoCommand
{
public:
    EditCanvasRotateCommand(entity::UserScene* scene, const osg::Quat& rotate, QUndoCommand* parent = 0);
    ~EditCanvasRotateCommand();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::Quat m_rotate;
    osg::Vec3f m_center;
};

/*! \class EditCanvasDeleteCommand
 * Class description
*/
class EditCanvasDeleteCommand : public QUndoCommand
{
public:
    EditCanvasDeleteCommand(entity::UserScene* scene, entity::Canvas* canvas, QUndoCommand* parent = 0);
    ~EditCanvasDeleteCommand() {}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::ref_ptr<entity::Canvas> m_canvas;
};

/*! \class EditPhotoDeleteCommand
 * Class description
*/
class EditPhotoDeleteCommand : public QUndoCommand
{
public:
    EditPhotoDeleteCommand(entity::UserScene* scene, entity::Canvas* canvas, entity::Photo* photo, QUndoCommand* parent = 0);
    ~EditPhotoDeleteCommand() {}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::ref_ptr<entity::Photo> m_photo;
};

/*! \class EditStrokesPushCommand
 * Class description
*/
class EditStrokesPushCommand : public QUndoCommand
{
public:
    EditStrokesPushCommand(entity::UserScene* scene, const std::vector<entity::Entity2D*>& entities, entity::Canvas* current, entity::Canvas* target,
                           const osg::Vec3f& eye, QUndoCommand* parent = 0);
    ~EditStrokesPushCommand() {}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:

    void doPushStrokes(entity::Canvas& source, entity::Canvas& target);

    osg::observer_ptr<entity::UserScene> m_scene;
    const std::vector<entity::Entity2D*> m_entities;
    osg::observer_ptr<entity::Canvas> m_canvasCurrent;
    osg::observer_ptr<entity::Canvas> m_canvasTarget;
    osg::Vec3f m_eye;
};

/*! \class EditEntitiesMoveCommand
 * Class description
*/
class EditEntitiesMoveCommand : public QUndoCommand
{
public:
    EditEntitiesMoveCommand(entity::UserScene* scene, const std::vector<entity::Entity2D*>& entities, entity::Canvas* canvas,
                           double du, double dv, QUndoCommand* parent = 0);
    ~EditEntitiesMoveCommand() {}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    std::vector<entity::Entity2D*> m_entities;
    osg::observer_ptr<entity::Canvas> m_canvas;
    double m_du;
    double m_dv;
};

/*! \class EditEntitiesScaleCommand
 * Class description
*/
class EditEntitiesScaleCommand : public QUndoCommand
{
public:
    EditEntitiesScaleCommand(entity::UserScene* scene, const std::vector<entity::Entity2D*>& entities,
                             entity::Canvas* canvas,
                             double scaleX, double scaleY,
                             osg::Vec3f center,
                             QUndoCommand* parent = 0);
    ~EditEntitiesScaleCommand() {}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    std::vector<entity::Entity2D*> m_entities;
    osg::observer_ptr<entity::Canvas> m_canvas;
    double m_scaleX, m_scaleY;
    osg::Vec3f m_center;
};

/*! \class EditEntitiesRotateCommand
 * Class description
*/
class EditEntitiesRotateCommand : public QUndoCommand
{
public:
    EditEntitiesRotateCommand(entity::UserScene* scene, const std::vector<entity::Entity2D*>& entities, entity::Canvas* canvas,
                             double theta, osg::Vec3f center, QUndoCommand* parent = 0);
    ~EditEntitiesRotateCommand() {}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    std::vector<entity::Entity2D*> m_entities;
    osg::observer_ptr<entity::Canvas> m_canvas;
    double m_theta;
    osg::Vec3f m_center;
};

/*! \class EditStrokeDeleteCommand
 * Serves for stroke deletion.
*/
class EditStrokeDeleteCommand : public QUndoCommand
{
public:
    EditStrokeDeleteCommand(entity::UserScene* scene, entity::Canvas* canvas, entity::Stroke* stroke, QUndoCommand* parent = 0);
    ~EditStrokeDeleteCommand() {}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::ref_ptr<entity::Stroke> m_stroke;
};

/*! \class EditPasteCommand
 * Class description
*/
class EditPasteCommand : public QUndoCommand
{
public:
    EditPasteCommand(entity::UserScene* scene, entity::Canvas* target,
                     const std::vector< osg::ref_ptr<entity::Entity2D> >& buffer, QUndoCommand* parent=0);
    ~EditPasteCommand() {}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    std::vector<entity::Entity2D*> m_entities;
};

/*! \class EditCutCommand
 * Class description
*/
class EditCutCommand : public QUndoCommand
{
public:
    EditCutCommand(entity::UserScene* scene, entity::Canvas*  canvas,
                   const std::vector<entity::Entity2D*>& selected,
                   std::vector< osg::ref_ptr<entity::Entity2D> >& buffer, QUndoCommand* parent=0);
    ~EditCutCommand() {}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    std::vector< osg::ref_ptr<entity::Entity2D> >& m_buffer;
    const std::vector<entity::Entity2D*>& m_selected;
};

/*! \class EditPhotoPushCommand
 * Class description
*/
class EditPhotoPushCommand : public QUndoCommand
{
public:
    EditPhotoPushCommand(entity::UserScene* scene, entity::Canvas* source, entity::Canvas* destination,
                         entity::Photo* photo, QUndoCommand* parent=0);
    ~EditPhotoPushCommand() {}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_source;
    osg::observer_ptr<entity::Canvas> m_destination;
    osg::observer_ptr<entity::Photo> m_photo;
};

#endif // EDITENTITYCOMMAND_H
