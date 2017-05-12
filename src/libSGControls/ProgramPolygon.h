#ifndef PROGRAMPOLYGON_H
#define PROGRAMPOLYGON_H

#include <osg/Program>
#include <osg/StateSet>
#include <osg/observer_ptr>
#include <osg/Camera>
#include <osg/MatrixTransform>

#include "ProgramEntity2D.h"

class ProgramPolygon : public ProgramEntity2D
{
public:
    ProgramPolygon();

    virtual void initialize(osg::StateSet* state, osg::Camera* camera, osg::MatrixTransform* t, bool isFogged);

    /*! A method to update one of the uniforms of the state set - fog factor. */
    virtual void updateIsFogged(bool f);

    bool getIsFogged() const;

protected:
    virtual bool addPresetShaders();
    virtual bool addPresetUniforms();

protected:
    bool    m_isFogged;

};

#endif // PROGRAMPOLYGON_H
