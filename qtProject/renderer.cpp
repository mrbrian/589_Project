#include "renderer.h"
#include <QTextStream>
#include <QOpenGLBuffer>
#include <cmath>

// intialization of a constant variable
const int Renderer::NUM_OVAL_STEPS = 20;

// constructor
Renderer::Renderer(QWidget *parent)
    : QOpenGLWidget(parent)

{

}

// constructor
Renderer::~Renderer()
{

}

// clears the screen of all objects
void Renderer::clear()
{
    // request Qt perform a re-draw
    update();
}

// simple vertex shader
// Note: Does not need modification for this assignment
static const char *vertexShaderSource =
    "attribute highp vec4 posAttr;\n"
    "attribute lowp vec4 colAttr;\n"
    "varying lowp vec4 col;\n"
    "uniform highp mat4 matrix;\n"
    "void main() {\n"
    "   col = colAttr;\n"
    "   gl_Position = matrix * posAttr;\n"
    "}\n";

// simple fragment shader
// Note: Does not need modification for this assignment
static const char *fragmentShaderSource =
    "varying lowp vec4 col;\n"
    "void main() {\n"
    "   gl_FragColor = col;\n"
    "}\n";


// called once by Qt GUI system, to allow initialization for OpenGL requirements
void Renderer::initializeGL()
{
    // Qt support for inline GL function calls
	initializeOpenGLFunctions();

    // sets the background clour
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // links to and compiles the shaders, used for drawing simple objects
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
    m_posAttr = m_program->attributeLocation("posAttr");
    m_colAttr = m_program->attributeLocation("colAttr");
    m_matrixUniform = m_program->uniformLocation("matrix");

    // ensure all arrays are cleared to start with
    clear();

}

// called by the Qt GUI system, to allow OpenGL drawing commands
// Note: Does not need modification for this assignment (unless adding shapes)
// Note: Most of the magic number "2" refer to the 2 coordinates (x, y) for 2D;
//          Colours use 3 (red, green, blue) coordinates in this assignment.
void Renderer::paintGL()
{
    // clears the background buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // sets the current shader program
    m_program->bind();



    // deactivate the program
    m_program->release();

}

// called by the Qt GUI system, to allow OpenGL to respond to widget resizing
void Renderer::resizeGL(int w, int h)
{
    // width and height are better variables to use
    Q_UNUSED(w); Q_UNUSED(h);

    // update viewing projections
    m_program->bind();

    // hardcoded values and aspect ratios for the time being
    QMatrix4x4 matrix;
    matrix.ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f);
    m_program->setUniformValue(m_matrixUniform, matrix);

    glViewport(0, 0, width(), height());

}

// converts world coordinates to screen coordinates
QVector2D Renderer::windowToWorld(QPoint point)
{
    return QVector2D((float)point.x() / (float)width() * 2.0 - 1.0,
                     -(float)point.y() / (float)height() * 2.0 + 1.0);
}

// override mouse press event
void Renderer::mousePressEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton)
    {
        // track mouse position
        mStartPos = windowToWorld(event->pos());
    }
}

// override mouse release event
void Renderer::mouseReleaseEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton)
    {
        // pass in mouse position to appropriate drawing object
        QVector2D pos = windowToWorld(event->pos());

        // request Qt perform a re-draw
        update();
    }
}
