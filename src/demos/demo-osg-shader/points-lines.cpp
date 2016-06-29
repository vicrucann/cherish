#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Material>
#include <osg/Vec3>
#include <osg/MatrixTransform>
#include <osg/Texture2D>
#include <osg/PolygonStipple>
#include <osg/TriangleFunctor>
#include <osg/io_utils>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgGA/TrackballManipulator>
#include <osgViewer/Viewer>
#include <osg/Math>
#include <osg/Point>
#include <osgUtil/SceneView>
#include <osg/Uniform>
#include <osg/GraphicsContext>
#include <osg/LineWidth>

#include <iostream>
#include <stdio.h>
#include <Windows.h>

#define GLSL120(src) "#version 120\n #extension GL_EXT_geometry_shader4 : enable\n" #src
#define GLSL150(src) "#version 150\n" #src

const char* vertexShaderSrc = GLSL120(
    varying vec4 v_color;
void main()
{
    v_color = gl_Color;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
);

const char* geometryShaderSrc = GLSL120(
varying in vec4 v_color[];
varying out vec4 v_color_out;
void main()
{
    v_color_out = v_color[0];
    vec4 v = gl_PositionIn[0];

    gl_Position = v + vec4(-0.1, 0.0, 0.0, 0.0);
    EmitVertex();

    gl_Position = v + vec4(0.1, 0.0, 0.0, 0.0);
    EmitVertex();

    EndPrimitive();
}
);

const char* geometryShaderBezierSrc = GLSL120(
varying in vec4 v_color[];
varying out vec4 v_color_out;
uniform int segments;
void main()
{
    v_color_out = v_color[0];
    float delta = 1.0 / float(segments);

    gl_Position = gl_PositionIn[0];
    EmitVertex();

    gl_Position = gl_PositionIn[3];
    EmitVertex();

//    vec4 v;
//    for ( int i=0; i<=segments; ++i )
//    {
//        float t = delta * float(i);
//        float t2 = t * t;
//        float one_minus_t = 1.0 - t;
//        float one_minus_t2 = one_minus_t * one_minus_t;
//        v = gl_PositionIn[0] * one_minus_t2 * one_minus_t + gl_PositionIn[1] * 3.0 * t * one_minus_t2 +
//                gl_PositionIn[2] * 3.0 * t2 * one_minus_t +  gl_PositionIn[3] * t2 * t;
//        gl_Position = v;
//        EmitVertex();
//    }

    EndPrimitive();
}
);

const char* fragmentShaderSrc = GLSL120(
    varying vec4 v_color_out;
void main(void)
{
    gl_FragColor = v_color_out;
}
);

osg::Program* createShaderLine()
{
    osg::Program* program = new osg::Program;

    program->addShader(new osg::Shader(osg::Shader::VERTEX, vertexShaderSrc));
    program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fragmentShaderSrc));
    program->addShader(new osg::Shader(osg::Shader::GEOMETRY, geometryShaderSrc));

    program->setParameter(GL_GEOMETRY_VERTICES_OUT_EXT, 2);
    program->setParameter(GL_GEOMETRY_INPUT_TYPE_EXT, GL_POINTS);
    program->setParameter(GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_LINE_STRIP);

    return program;
}

const int segments = 10;

osg::Program* createShaderBezier()
{
    osg::Program* program = new osg::Program;

    program->addShader(new osg::Shader(osg::Shader::VERTEX, vertexShaderSrc));
    program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fragmentShaderSrc));
    program->addShader(new osg::Shader(osg::Shader::GEOMETRY, geometryShaderBezierSrc));

    program->setParameter(GL_GEOMETRY_VERTICES_OUT_EXT, 2);
    program->setParameter(GL_GEOMETRY_INPUT_TYPE_EXT, GL_LINES_ADJACENCY);
    program->setParameter(GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_LINE_STRIP);

    return program;
}

osg::Node* createScene()
{
    osg::Geode* geode = new osg::Geode();
    // create POINTS
    {
        osg::Geometry* pointsGeom = new osg::Geometry();
        pointsGeom->getOrCreateStateSet()->setAttribute( new osg::Point( 10.0f ), osg::StateAttribute::ON );

        osg::Vec3Array* vertices = new osg::Vec3Array;
        vertices->push_back(osg::Vec3(-0.45f,  0.45f, -0.45f));
        vertices->push_back(osg::Vec3(0.45f,  0.45f, -0.45f));
        vertices->push_back(osg::Vec3(0.45f, -0.45f, -0.45f));
        vertices->push_back(osg::Vec3(-0.45f, -0.45f, -0.45f));
        vertices->push_back(osg::Vec3(-0.45f,  0.45f,  0.45f));
        vertices->push_back(osg::Vec3(0.45f,  0.45f,  0.45f));
        vertices->push_back(osg::Vec3(0.45f, -0.45f,  0.45f));
        vertices->push_back(osg::Vec3(-0.45f, -0.45f,  0.45f));

        pointsGeom->setVertexArray(vertices);

        osg::Vec4Array* colors = new osg::Vec4Array;
        colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1));
        colors->push_back(osg::Vec4(0.0f, 1.0f, 1.0f, 1));
        colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1 ));
        colors->push_back(osg::Vec4(0.0f, 0.7f, 0.5f, 1));
        colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1));
        colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1));
        colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1));
        colors->push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1));
        pointsGeom->setColorArray(colors, osg::Array::BIND_PER_VERTEX);

        pointsGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,vertices->size()));

        geode->addDrawable(pointsGeom);

        pointsGeom->getOrCreateStateSet()->setAttribute(createShaderLine());
        pointsGeom->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

    }

    // create BEZIER
    {
        osg::ref_ptr<osg::Vec3Array> verticesB = new osg::Vec3Array;
        verticesB->push_back( osg::Vec3(0.0f, 0.0f, 0.0f) );
        verticesB->push_back( osg::Vec3(1.0f, 1.0f, 1.0f) );
        verticesB->push_back( osg::Vec3(2.0f, 1.0f,-1.0f) );
        verticesB->push_back( osg::Vec3(3.0f, 0.0f, 0.0f) );
        osg::ref_ptr<osg::Geometry> controlPoints = new osg::Geometry;
        controlPoints->setVertexArray( verticesB.get() );

        controlPoints->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::LINES_ADJACENCY, 0, verticesB->size()) );

        geode->addDrawable( controlPoints.get() );

        controlPoints->getOrCreateStateSet()->setAttribute(createShaderBezier());
        controlPoints->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
        controlPoints->getOrCreateStateSet()->addUniform(new osg::Uniform("segments", segments));
        osg::ref_ptr<osg::LineWidth> lineWidth = new osg::LineWidth;
        lineWidth->setWidth( 10.0f );
        controlPoints->getOrCreateStateSet()->setAttribute( lineWidth.get() );
    }

    return geode;
}

int main(int, char **)
{
    ::SetProcessDPIAware();
    osg::Group* root = new osg::Group;
    root->addChild( createScene() );

    osgViewer::Viewer viewer;

    viewer.setSceneData( root );
    viewer.getCamera()->setClearColor(osg::Vec4( 0.9f, 0.9f, 1.f, 1.f ));

    viewer.setUpViewOnSingleScreen(0);
    return viewer.run();
}
