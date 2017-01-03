#include "ProgramEntity2D.h"

#include <QtGlobal>
#include <QDebug>

#include "CameraCallbacks.h"

ProgramEntity2D::ProgramEntity2D()
    : osg::Program()
    , m_state(0)
    , m_camera(0)
    , m_transform(0)
{
}

void ProgramEntity2D::initialize(osg::StateSet *state, osg::Camera *camera, osg::MatrixTransform *t)
{
    m_state = state;
    m_camera = camera;
    m_transform = t;

    if (!this->addPresetShaders())
        qCritical("Could not add necessary shaders");
    if (!this->addPresetUniforms())
        qCritical("Could not add necessary uniforms");

    qInfo("Entity shader was successfully initialized");
}

void ProgramEntity2D::updateTransform(osg::MatrixTransform *t)
{
    m_transform = t;
    /* canvas matrix transform */
    if (!this->addUniformCanvasMatrix())
    {
        qWarning("Could not update CanvasMatrix uniform");
    }
}

osg::MatrixTransform *ProgramEntity2D::getTransform() const
{
    return m_transform.get();
}

osg::Camera *ProgramEntity2D::getCamera() const
{
    return m_camera.get();
}

bool ProgramEntity2D::addUniformCanvasMatrix()
{
    /* canvas matrix transform */
    if (!this->addUniform<CanvasTransformCallback>("CanvasMatrix",
                                                   osg::Uniform::FLOAT_MAT4,
                                                   new CanvasTransformCallback(m_transform.get()) ))
    {
        qWarning("Could not add CameraEye uniform");
        return false;
    }
    return true;
}
