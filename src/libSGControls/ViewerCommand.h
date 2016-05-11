#ifndef VIEWERCOMMAND_H
#define VIEWERCOMMAND_H

#include <QUndoCommand>

#include <osg/observer_ptr>

#include "Manipulator.h"
#include "../libGUI/GLWidget.h"

class GLWidget;

/*! \class ViewerCommand
 * \brief The ViewerCommand is used from GLWidget to save all the camera views within
 * undo/redo framework.
 *
 * Whenever the user changes the camera view, for example, by using mouse manipulator,
 * the corresponding camera parameters (eye, center, up) for the current and previous views
 * are passed to create the ViewerCommand. The result command is put into
 * GLWidget::m_viewStack.
 *
 * The undo and redo functions are served to change the camera views from previous to current,
 * and the opposite.
*/

class ViewerCommand : public QUndoCommand
{
public:
    /*! Main constructor
     * \param widget is of GLWidget class;
     * \param man is the manipulator that is attached to GLWidget;
     * \param eye is the camera eye vector
     * \param center is the camera center vector
     * \param up is the camera up vector
     * \param d1 is the difference between the next camera view eye and passed eye
     * \param d2 is the difference between the next camera view center and passed center
     * \param d3 is the difference between the next camera view up and passed up
     * \param parent is optional, and normally is passed uninitialized
    */
    ViewerCommand(GLWidget* widget, Manipulator* man,
                  const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up,
                  const osg::Vec3d& d1, const osg::Vec3d& d2, const osg::Vec3d& d3,
                  QUndoCommand* parent = 0);

    /*! \fn undo
     * Performs the redo command by setting the camera parameters to the initial ones */
    void undo() Q_DECL_OVERRIDE;

    /*! \fn redo
     * Performs the redo command by adding the difference vectors to the initial camera parameters */
    void redo() Q_DECL_OVERRIDE;

private:
    GLWidget* m_widget; /*!< GLWidget variable */
    osg::observer_ptr<Manipulator> m_man; /*!< Manipulator variable */
    osg::Vec3d m_eye; /*!< Initial camera's eye vector */
    osg::Vec3d m_center; /*!< Initial camera's center vector */
    osg::Vec3d m_up; /*!< Initial camera's up vector */
    osg::Vec3d de; /*!< Difference vector for camera's eye */
    osg::Vec3d dc; /*!< Difference vector for camera's center */
    osg::Vec3d du; /*!< Difference vector for camera's up */
};

#endif // VIEWERCOMMAND_H
