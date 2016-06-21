#ifndef EDITENTITYCOMMAND_H
#define EDITENTITYCOMMAND_H

#include <vector>

#include <QUndoCommand>

#include <osg/observer_ptr>

#include "UserScene.h"
#include "Canvas.h"
#include "Photo.h"
#include "Stroke.h"
#include "Bookmarks.h"
#include "SceneState.h"

namespace entity {
class UserScene;
class Bookmarks;
}

namespace fur{

/*! \class EditCanvasOffsetCommand
 * \brief QUndoCommand that performs offset of a canvas.
*/
class EditCanvasOffsetCommand : public QUndoCommand
{
public:
    /*! \param scene is the scene graph to edit, \param translate is 3D translation vector, \param parent is normally 0. */
    EditCanvasOffsetCommand(entity::UserScene* scene, const osg::Vec3f& translate, QUndoCommand* parent = 0);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::Vec3f m_translate;
};

/*! \class EditCanvasRotateCommand
  * \brief QUndoCommand that performs rotation of a canvas.
*/
class EditCanvasRotateCommand : public QUndoCommand
{
public:
    /*! \param scene is the scene graph to edit, \param rotate is 3D rotation quad, \param parent is normally 0. */
    EditCanvasRotateCommand(entity::UserScene* scene, const osg::Quat& rotate, QUndoCommand* parent = 0);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::Quat m_rotate;
    osg::Vec3f m_center;
};

/*! \class EditCanvasDeleteCommand
  * \brief QUndoCommand that performs deletion of a canvas.
*/
class EditCanvasDeleteCommand : public QUndoCommand
{
public:
    /*! \param scene is the scene graph to edit, \param canvas is a canvas to delete, \param parent is normally 0. */
    EditCanvasDeleteCommand(entity::UserScene* scene, entity::Canvas* canvas, QUndoCommand* parent = 0);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::ref_ptr<entity::Canvas> m_canvas;
    osg::observer_ptr<entity::Bookmarks> m_bookmarks;
};

/*! \class EditPhotoDeleteCommand
 * \brief QUndoCommand that performs deletion of a photo.
*/
class EditPhotoDeleteCommand : public QUndoCommand
{
public:
    /*! \param scene is the scene graph to edit, \param canvas is a canvas to delete from, \param photo is a photo to delete, \param parent is normally 0. */
    EditPhotoDeleteCommand(entity::UserScene* scene, entity::Canvas* canvas, entity::Photo* photo, QUndoCommand* parent = 0);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::ref_ptr<entity::Photo> m_photo;
    osg::observer_ptr<entity::Bookmarks> m_bookmarks;
};

/*! \class EditStrokesPushCommand
 * \brief QUndoCommand that performs push operation of a set of strokes.
*/
class EditStrokesPushCommand : public QUndoCommand
{
public:
    /*! \param scene is the scene graph to edit, \param entities is the vector of pointers on strokes to edit,
     * \param current is the source canvas which contains the strokes, \param target is the destination canvas where strokes will be pushed to,
     * \param eye is the camera eye position when the push strokes is performed, \param parent is normally 0. */
    EditStrokesPushCommand(entity::UserScene* scene, const std::vector<entity::Entity2D*>& entities, entity::Canvas* current, entity::Canvas* target,
                           const osg::Vec3f& eye, QUndoCommand* parent = 0);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

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

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

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

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

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

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

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

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */
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

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

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

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

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

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_source;
    osg::observer_ptr<entity::Canvas> m_destination;
    osg::observer_ptr<entity::Photo> m_photo;
};

} // namespace fur

#endif // EDITENTITYCOMMAND_H
