#include "ProgramLineSegment.h"

#include "Data.h"

ProgramLineSegment::ProgramLineSegment()
    : ProgramPolygon()
{
    this->setName("ProgramLineSegment");
}

void ProgramLineSegment::updateIsFogged(bool f)
{
    m_isFogged = f;
    this->addUniform<bool>("IsFogged", osg::Uniform::BOOL, m_isFogged);
}

bool ProgramLineSegment::addPresetShaders()
{
    if (this->getNumShaders() == 2){
        qWarning("Shaders already seems to be added");
        return true;
    }

    /* load and add shaders to the program */
    osg::ref_ptr<osg::Shader> vertShader = new osg::Shader(osg::Shader::VERTEX);
    vertShader->setShaderSource(Data::linesegmentVertexShader());
    if (!this->addShader(vertShader.get())){
        qWarning("Could not add vertext shader");
        return false;
    }

    osg::ref_ptr<osg::Shader> fragShader = new osg::Shader(osg::Shader::FRAGMENT);
    fragShader->setShaderSource(Data::linesegementFragmentShader());
    if (!this->addShader(fragShader.get())){
        qWarning("Could not add fragment shader");
        return false;
    }

    return true;
}
