#include "renderer.h"
#include <QTextStream>
#include <QOpenGLBuffer>
#include <cmath>
#include "trackball.h"

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
/*
// generate a VBO for a given model
void Renderer::makeVbo(Model *model)
{
    // get buffersizes
    long cBufferSize = model->sizeColours;
    long vBufferSize = model->sizeVerts;
    long nBufferSize = model->sizeFaceNormals;
    long uvBufferSize = model->sizeUvs;
    long fnBufferSize = model->sizeFaceNormalLines;
    long vnBufferSize = model->sizeVertNormalLines;

    glGenBuffers(1, &model->m_modelVbo);
    glBindBuffer(GL_ARRAY_BUFFER, model->m_modelVbo);

    // Allocate buffer
    glBufferData(GL_ARRAY_BUFFER, vBufferSize + cBufferSize + 2 * nBufferSize + uvBufferSize + fnBufferSize + vnBufferSize, NULL, GL_STATIC_DRAW);

    // Upload the data to the GPU
    glBufferSubData(GL_ARRAY_BUFFER, 0, vBufferSize, &model->verts[0]);
    glBufferSubData(GL_ARRAY_BUFFER, vBufferSize, cBufferSize, &model->colours[0]);

    glBufferSubData(GL_ARRAY_BUFFER, vBufferSize + cBufferSize,
                    nBufferSize,
                    &model->vert_normals[0]);

    glBufferSubData(GL_ARRAY_BUFFER,
                    vBufferSize + cBufferSize + nBufferSize,
                    nBufferSize,
                    &model->face_normals[0]);

    glBufferSubData(GL_ARRAY_BUFFER,
                    vBufferSize + cBufferSize + 2 * nBufferSize,
                    uvBufferSize,
                    &model->uvs[0]);

    glBufferSubData(GL_ARRAY_BUFFER,
                    vBufferSize + cBufferSize + 2 * nBufferSize + uvBufferSize,
                    fnBufferSize,
                    &model->lines_face_normals[0]);

    glBufferSubData(GL_ARRAY_BUFFER,
                    vBufferSize + cBufferSize + 2 * nBufferSize + uvBufferSize + fnBufferSize,
                    vnBufferSize,
                    &model->lines_vert_normals[0]);

}                   */

// override mouse move event
void Renderer::mouseMoveEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);
    cout << "Stub: Motion at " << event->x() << ", " << event->y() << ".\n";

    prev_x = curr_x;
    prev_y = curr_y;

    curr_x = event->x();
    curr_y = event->y();

    handleInteraction();       // adjust the models or camera
}

// trackball rotation logic
void Renderer::doTrackball()
{
    float diameter = width() * 0.75;    // trackball take up 3/4 of screen width
    float vx, vy, vz;

    float cx = curr_x - width() / 2;    // center mouse coords on the trackball
    float cy = curr_y - height() / 2;

    float px = prev_x - width() / 2;
    float py = prev_y - height() / 2;

    QMatrix4x4 q;
    Trackball::vCalcRotVec(cx, cy, px, py, diameter, &vx, &vy, &vz);    // do trackball stuff
    Trackball::vAxisRotMatrix(vx, -vy, vz, &q);

    //applyTransform(&q);     // transform selected model or all models
}

// user interacting with the scene via mouse
void Renderer::handleInteraction()
{
    QVector3D delta;
    float dx = (float)(curr_x - prev_x) / 100;
    float dy = (float)(curr_y - prev_y) / 100;

    QMatrix4x4 modelTrans;

    if (shiftDown)           // modify camera position
    {
        if (mouseButtons & Qt::LeftButton)
        {
            delta[0] = dx;
            delta[1] = -dy;
            camera.position += delta;
        }
        if (mouseButtons & Qt::MidButton)
        {
            delta[2] = dx;
            camera.position  += delta;
        }
        if (mouseButtons & Qt::RightButton)
        {
            QMatrix4x4 m;
            m.rotate(dx * 10, 0, 0, 1);
            camera.up = m * camera.up;
        }
    }

    if (ctrlDown)    // modify camera lookat target
    {
        if (mouseButtons & Qt::LeftButton)      // LB modifies along x-axis & y-axis
        {
            delta[0] = dx;
            delta[1] = -dy;
            camera.target += delta;
        }
        if (mouseButtons & Qt::MidButton)      // LB modifies along z-axis
        {
            delta[2] = dx;
            camera.target += delta;
        }
    }

    if (!shiftDown && !ctrlDown)                // translate / rotate the model
    {
        if (mouseButtons & Qt::LeftButton)      // LB modifies along x-axis & y-axis
        {
            delta[0] = dx;
            delta[1] = -dy;
            modelTrans.translate(delta);
            //applyTransform(&modelTrans);
        }
        if (mouseButtons & Qt::MiddleButton)    // MB modifies  along z-axis
        {
            delta[2] = dx;
            modelTrans.translate(delta);
            //applyTransform(&modelTrans);
        }
        if (mouseButtons & Qt::RightButton)     // RB rotates model via trackball
        {
            doTrackball();
        }
    }
}

// override mouse press event
void Renderer::mousePressEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);
    cout << "Stub: Button " << event->button() << " pressed.\n";
    mouseButtons = event->buttons();

    curr_x = event->x();
    curr_y = event->y();

    prev_x = curr_x;
    prev_y = curr_y;
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
