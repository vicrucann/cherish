#ifndef TESTSUPPORTOPERATION_H
#define TESTSUPPORTOPERATION_H

#include <stdio.h>
#include <iostream>
#include <string>

#include <osg/GraphicsThread>
#include <osg/GraphicsContext>
#include <OpenThreads/Mutex>
#include <osg/GLExtensions>

/*! \class TestSupportOperation
 * \brief An OSG-derived class that helps to identify an OpenGL version and whether GLSL is supported or not.
 * It is mainly used when deciding if GLSL shaders are used for entity::Stroke geometry or the default GL_LINE_STRIP_ADJACENCY.
 * The usage must be performed within an OpenGL conetext, for example:
 * \code
 * { // ...
 *      osg::Viewer viewer;
 *
 *      osg::ref_ptr<TestSupportOperation> tso = new TestSupportOperation;
 *      viewer.setRealizeOperation(tso.get());
 *      viewer.realize();
 *
 *      if (!tso->m_supported)
 *          std::cout << tso->m_errorMsg << std::endl;
 *      else
 *          std::cout << "OpenGLVersion=" << tso->m_version << std::endl;
 * // ...
 * }
 *
 * \endcode
 *
*/
class TestSupportOperation : public osg::GraphicsOperation
{
public:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    TestSupportOperation();
    virtual void operator() (osg::GraphicsContext* gc);

    OpenThreads::Mutex  m_mutex;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

    bool                m_supported; /*!< Flag that defined whether GLSL is supported (true) or not (false). */
    std::string         m_errorMsg; /*!< When GLSL is not supported the variable contains a short message on why it is not supported. */
    float               m_version; /*!< Float variable that will contains the OpenGL (GLSL) version. */
};

#endif // TESTSUPPORTOPERATION_H
