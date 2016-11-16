#ifndef PROGRAMSTROKE_H
#define PROGRAMSTROKE_H

#include <osg/Program>
#include <osg/StateSet>
#include <osg/observer_ptr>
#include <osg/Camera>
#include <osg/MatrixTransform>

#include "ProgramEntity2D.h"

/*! \class ProgramStroke
 * \brief An interface class that deals with entity::Stroke shader's state.
 *
 * It is a part of entity::Canvas private members and is applyied to the stroke group, entity::Canvas::m_geodeStrokes.
 * The main goal of the class is to provide an easy to use interface to program's modification, e.g., when isFogged parameter
 * is changed from MainWindow::onStrokeFogFactor().
*/
class ProgramStroke : public ProgramEntity2D
{
public:
    /*! Constructor that also loads the shaders from files and set preset uniforms. */
    ProgramStroke();

    /*! A method to initialize the internal variables for the future use, must be run right after the constructor. */
    void initialize(osg::StateSet* state, osg::Camera* camera, osg::MatrixTransform* t, bool isFogged);

    /*! A method to update one of the uniforms of the state set - fog factor. */
    void updateIsFogged(bool f);

    bool getIsFogged() const;

protected:
    virtual bool addPresetShaders();
    virtual bool addPresetUniforms();

private:
    bool m_isFogged; // of MainWindow::m_actionStrokeFogFactor
};

#endif // PROGRAMSTROKE_H
