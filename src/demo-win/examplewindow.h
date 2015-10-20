#ifndef EXAMPLEWINDOW
#define EXAMPLEWINDOW

#include "graphicswindow.h"

#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QScreen>

static const char *vertexShaderSource =
    "attribute highp vec4 posAttr;\n"
    "attribute lowp vec4 colAttr;\n"
    "varying lowp vec4 col;\n"
    "uniform highp mat4 matrix;\n"
    "void main() {\n"
    "   col = colAttr;\n"
    "   gl_Position = matrix * posAttr;\n"
    "}\n";

static const char *fragmentShaderSource =
    "varying lowp vec4 col;\n"
    "void main() {\n"
    "   gl_FragColor = col;\n"
    "}\n";

class ExampleWindow : public GraphicsWindow{
public:
    ExampleWindow(): m_pProg(0), m_frame(0){}
    ~ExampleWindow(){
        if (m_pProg)
            delete m_pProg;
    }
    void initialize() Q_DECL_OVERRIDE {
        m_pProg = new QOpenGLShaderProgram(this);
        m_pProg->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
        m_pProg->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
        m_pProg->link();
        m_posAttr = m_pProg->attributeLocation("posAttr");
        m_colAttr = m_pProg->attributeLocation("colAttr");
        m_matUni = m_pProg->uniformLocation("matrix");
    }
    void render() Q_DECL_OVERRIDE {
        const qreal retinaScale = devicePixelRatio();
        glViewport(0, 0, width() * retinaScale, height() * retinaScale);
        glClear(GL_COLOR_BUFFER_BIT);
        m_pProg->bind();

        QMatrix4x4 matrix;
        matrix.perspective(60.0f, 4.0f/3.0f, 0.1f, 100.0f);
        matrix.translate(0, 0, -2);
        matrix.rotate(100.0f * m_frame / screen()->refreshRate(), 0, 1, 0);
        m_pProg->setUniformValue(m_matUni, matrix);

        GLfloat vertices[] = {
            0.0f, 0.707f,
            -0.5f, -0.5f,
            0.5f, -0.5f };
        GLfloat colors[] = {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f };
        glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, 0, vertices);
        glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
        m_pProg->release();
        ++m_frame;
    }
private:
    GLuint loadShader(GLenum type, const char* source){
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, 0);
        glCompileShader(shader);
        return shader;
    }

    QOpenGLShaderProgram* m_pProg;
    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_matUni;
    int m_frame;
};

#endif // EXAMPLEWINDOW

