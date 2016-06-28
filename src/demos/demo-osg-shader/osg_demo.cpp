#include <osg/Notify>
#include <osg/ref_ptr>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Point>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Program>
#include <osg/Shader>
#include <osg/Uniform>
#include <osgViewer/Viewer>

#include <Windows.h>

#define GLSL120(src) "#version 120\n #extension GL_EXT_geometry_shader4 : enable\n" #src
#define GLSL150(src) "#version 150\n" #src

/* VERTEX */

const char* vertSource = GLSL120 (
    varying vec4 v_color;
void main(void)
{
    v_color = gl_Vertex;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
);


/* GEOMETRY */

const char* geomSource = GLSL120(
    uniform float thickness; // the thickness of the line in pixels
    uniform float miter_limit; // 1.0: always miter, -1.0: never miter, 0.75: default
    varying in vec4 v_color[];
    varying out vec4 v_color_out;
void main(void)
{
    vec4 v = gl_PositionIn[0];
    v_color_out = v_color[0];

    gl_Position = v + vec4(0.5f ,0. ,0. ,0.);
    EmitVertex();

    gl_Position = v - vec4(0.5f, 0., 0., 0.);
    EmitVertex();

    EndPrimitive();

    gl_Position = v + vec4(0., 1.0-0.5f, 0., 0.);
    EmitVertex();

    gl_Position = v - vec4(0., 1.0-0.5f, 0., 0.);
    EmitVertex();

    EndPrimitive();
}
);


/* FRAGMENT / COLOR */

const char* fragSource = GLSL150(
    varying vec4 v_color_out;
void main(void)
{
    gl_FragColor = v_color_out;
}
);


osg::Program* createShader()
{
    osg::Program* pgm = new osg::Program;

    pgm->addShader( new osg::Shader( osg::Shader::VERTEX,   vertSource ) );
    pgm->addShader( new osg::Shader( osg::Shader::FRAGMENT, fragSource ) );
    pgm->addShader( new osg::Shader( osg::Shader::GEOMETRY, geomSource ) );

    pgm->setParameter( GL_GEOMETRY_VERTICES_OUT_EXT, 4 );
    pgm->setParameter( GL_GEOMETRY_INPUT_TYPE_EXT, GL_POINTS );
    pgm->setParameter( GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_LINE_STRIP );

    return pgm;
}

class SomePoints : public osg::Geometry
{
public:
    SomePoints()
    {
        osg::Vec4Array* cAry = new osg::Vec4Array;
        setColorArray( cAry, osg::Array::BIND_OVERALL );
        cAry->push_back( osg::Vec4(1,1,1,1) );

        osg::Vec3Array* vAry = new osg::Vec3Array;
        setVertexArray( vAry );
        vAry->push_back( osg::Vec3(0,0,0) );
        vAry->push_back( osg::Vec3(0,1,0) );
        vAry->push_back( osg::Vec3(1,0,0) );
        vAry->push_back( osg::Vec3(1,1,0) );
        vAry->push_back( osg::Vec3(0,0,1) );
        vAry->push_back( osg::Vec3(0,1,1) );
        vAry->push_back( osg::Vec3(1,0,1) );
        vAry->push_back( osg::Vec3(1,1,1) );

        addPrimitiveSet( new osg::DrawArrays( GL_POINTS, 0, vAry->size() ) );

        osg::StateSet* sset = getOrCreateStateSet();
        sset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

        osg::Point* p = new osg::Point;
        p->setSize(6);
        sset->setAttribute( p );

        sset->setAttribute( createShader() );

        osg::Uniform* u_thickness = new osg::Uniform("thickness", 5.f);
        osg::Uniform* u_miter = new osg::Uniform("miter_limit", 1.f);
        sset->addUniform(u_thickness);
        sset->addUniform(u_miter);
    }
};

int main( int , char** )
{
    ::SetProcessDPIAware();
    osg::Geode* root = new osg::Geode;
    root->addDrawable(new SomePoints);

    osgViewer::Viewer viewer;
    viewer.setSceneData( root );
    viewer.setUpViewOnSingleScreen(0);
    viewer.getCamera()->setClearColor(osg::Vec4( 0.9f, 0.9f, 1.f, 1.f ));
    return viewer.run();
}
