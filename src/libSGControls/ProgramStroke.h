#ifndef PROGRAMSTROKE_H
#define PROGRAMSTROKE_H

#include <osg/Program>
#include <osg/StateSet>
#include <osg/observer_ptr>
#include <osg/Camera>
#include <osg/MatrixTransform>

/*! \class ProgramStroke
 * \brief An interface class that deals with entity::Stroke shader's state.
 *
 * It is a part of entity::Canvas private members and is applyied to the stroke group, entity::Canvas::m_geodeStrokes.
 * The main goal of the class is to provide an easy to use interface to program's modification, e.g., when isFogged parameter
 * is changed from MainWindow::onStrokeFogFactor().
*/
class ProgramStroke : public osg::Program
{
public:
    /*! Constructor that also loads the shaders from files and set preset uniforms. */
    ProgramStroke();
    void initialize(osg::StateSet* state, osg::Camera* camera, osg::MatrixTransform* t, bool isFogged);

    /*! A method to update one of the uniforms of the state set - fog factor. */
    void updateIsFogged(bool f);

private:
    bool addPresetShaders();
    bool addPresetUniforms();

    template <typename T>
    bool addUniform(const std::string& name, osg::Uniform::Type type, T* updateCallback = 0);

    template <typename T>
    bool addUniform(const std::string& name, osg::Uniform::Type type, T value);

    osg::observer_ptr<osg::StateSet> m_state; // of entity::Canvas::m_geodeStrokes
    osg::observer_ptr<osg::Camera> m_camera; // of GLWidget::getCamera()
    osg::observer_ptr<osg::MatrixTransform> m_transform; // of Canvas::m_transform
    bool m_isFogged; // of MainWindow::m_actionStrokeFogFactor
};

#endif // PROGRAMSTROKE_H
