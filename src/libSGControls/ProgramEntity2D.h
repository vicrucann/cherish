#ifndef PROGRAMENTITY2D_H
#define PROGRAMENTITY2D_H

#include <osg/Program>
#include <osg/StateSet>
#include <osg/observer_ptr>
#include <osg/Camera>
#include <osg/MatrixTransform>

#include <QtGlobal>
#include <QDebug>

/*! \class ProgramEntity2D
 * \brief A virtual class to be inhereted by program for stroke, polygon and other entities.
*/

class ProgramEntity2D : public osg::Program
{
public:
    ProgramEntity2D();

    virtual void initialize(osg::StateSet* state, osg::Camera* camera, osg::MatrixTransform* t);

    void updateTransform(osg::MatrixTransform* t);

    osg::MatrixTransform* getTransform() const;

    osg::Camera* getCamera() const;

protected:
    virtual bool addPresetShaders() = 0;
    virtual bool addPresetUniforms() = 0;

    /* templated methods - write implementations in headers to avoid template declarations of inhereted classes */
    template <typename T>
    bool addUniform(const std::string& name, osg::Uniform::Type type, T* updateCallback = 0){
        if (!m_state.get() || !updateCallback){
            qCritical("State or callback is NULL");
            return false;
        }
        osg::Uniform* uniform = m_state->getOrCreateUniform(name, type);
        uniform->setUpdateCallback(updateCallback);

        return true;
    }

    bool addUniformCanvasMatrix();

    template <typename T>
    bool addUniform(const std::string& name, osg::Uniform::Type type, T value){
        if (!m_state.get()){
            qCritical("State is nULL");
            return false;
        }
        osg::Uniform* uniform = m_state->getOrCreateUniform(name, type);
        uniform->set(value);

        return true;
    }

protected:
    osg::observer_ptr<osg::StateSet> m_state; // of entity::Canvas::m_geodeStrokes
    osg::observer_ptr<osg::Camera> m_camera; // of GLWidget::getCamera()
    osg::observer_ptr<osg::MatrixTransform> m_transform; // of Canvas::m_transform
};

#endif // PROGRAMENTITY2D_H
