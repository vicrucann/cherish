#include "TestSupportOperation.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

TestSupportOperation::TestSupportOperation()
    : osg::Referenced(true)
    , osg::GraphicsOperation("TestSupportOperation", false)
    , m_supported(true)
    , m_errorMsg()
    , m_version(1.3f)
{
}

void TestSupportOperation::operator()(osg::GraphicsContext *gc)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(m_mutex);
    osg::GLExtensions* gl2ext = gc->getState()->get<osg::GLExtensions>();

    if( gl2ext ){

        if( !gl2ext->isGlslSupported )
        {
        m_supported = false;
        m_errorMsg = "ERROR: GLSL not supported by OpenGL driver.";
        }
        else
            m_version = gl2ext->glVersion;
    }
    else{
        m_supported = false;
        m_errorMsg = "ERROR: GLSL not supported.";
    }
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
