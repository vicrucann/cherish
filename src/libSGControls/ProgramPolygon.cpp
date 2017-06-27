#include "ProgramPolygon.h"

#include <QtGlobal>
#include <QDebug>

#include <osg/Shader>

#include "Settings.h"
#include "CameraCallbacks.h"
#include "Data.h"

ProgramPolygon::ProgramPolygon()
    : ProgramEntity2D()
    , m_isFogged(false)
{
    this->setName("ProgramPolygon");
}

void ProgramPolygon::initialize(osg::StateSet *state, osg::Camera *camera, osg::MatrixTransform *t, bool isFogged)
{
    ProgramEntity2D::initialize(state, camera, t);
    m_isFogged = isFogged;
}

void ProgramPolygon::updateIsFogged(bool f)
{
    m_isFogged = f;
    this->addUniform<bool>("IsFogged", osg::Uniform::BOOL, m_isFogged);
}

bool ProgramPolygon::getIsFogged() const
{
    return m_isFogged;
}

bool ProgramPolygon::addPresetShaders()
{
    if (this->getNumShaders() == 2){
        qWarning("Shaders already seems to be added");
        return true;
    }

    /* load and add shaders to the program */
    osg::ref_ptr<osg::Shader> vertShader = new osg::Shader(osg::Shader::VERTEX);
    vertShader->setShaderSource(Data::polygonVertexShader());
    if (!this->addShader(vertShader.get())){
        qWarning("Could not add vertext shader");
        return false;
    }

    osg::ref_ptr<osg::Shader> fragShader = new osg::Shader(osg::Shader::FRAGMENT);
    fragShader->setShaderSource(Data::polygonFragmentShader());
    if (!this->addShader(fragShader.get())){
        qWarning("Could not add fragment shader");
        return false;
    }

    return true;
}

bool ProgramPolygon::addPresetUniforms()
{
    /* model view proj matrix */
    if (!m_camera.get() || !m_transform.get()){
        qWarning("Camera or canvas transform is null");
        return false;
    }
    if (!this->addUniform<ModelViewProjectionMatrixCallback>("ModelViewProjectionMatrix",
                                                             osg::Uniform::FLOAT_MAT4,
                                                             new ModelViewProjectionMatrixCallback(m_camera.get()) ))
    {
        qWarning("Could not add MVP uniform");
        return false;
    }

    /* camera eye */
    if (!this->addUniform<CameraEyeCallback>("CameraEye",
                                             osg::Uniform::FLOAT_VEC4,
                                             new CameraEyeCallback(m_camera.get()) ))
    {
        qWarning("Could not add CameraEye uniform");
        return false;
    }

    /* canvas matrix transform */
    if (!this->addUniformCanvasMatrix())
    {
        qWarning("Could not add CameraEye uniform");
        return false;
    }

    /* fog factor related */
    if (!this->addUniform<float>("FogMin", osg::Uniform::FLOAT, cher::STROKE_FOG_MIN)){
        qCritical("Could not initialize fog factor uniform");
        return false;
    }

    if (!this->addUniform<float>("FogMax", osg::Uniform::FLOAT, cher::STROKE_FOG_MAX)){
        qCritical("Could not initialize fog max factor");
        return false;
    }

    if (!this->addUniform<bool>("IsFogged", osg::Uniform::BOOL, m_isFogged)){
        qCritical("Could not initialize fog min factor");
        return false;
    }
    if (!this->addUniform<osg::Vec4f>("FogColor", osg::Uniform::FLOAT_VEC4, cher::BACKGROUND_CLR)){
        qCritical("Could not initialize fog color uniform");
        return false;
    }

    return true;
}
