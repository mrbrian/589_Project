/*
 * CPSC 453 - Introduction to Computer Graphics
 * Assignment 0
 *
 * Renderer - OpenGL widget for drawing simple shapes
 */

#ifndef RENDERER_H
#define RENDERER_H

//#define _USE_MATH_DEFINES
#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QMouseEvent>
#include "camera.h"

using namespace std;

class Renderer : public QOpenGLWidget, protected QOpenGLFunctions
{

    // informs the qmake that a Qt moc_* file will need to be generated
    Q_OBJECT

public:

    // constructor
    Renderer(QWidget *parent = 0);

    // destructor
    virtual ~Renderer();

public slots:
    // Clears the screen
    void clear();

protected:

    // override fundamental drawing functions
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    // override mouse event functions
    virtual void mouseMoveEvent(QMouseEvent * event);
    virtual void mousePressEvent(QMouseEvent * event);
    virtual void mouseReleaseEvent(QMouseEvent * event);

private:
    Camera camera;

    float curr_x, curr_y;   // mouse positions
    float prev_x, prev_y;

    // mouse buttons that are currently pressed
    int mouseButtons;
    bool ctrlDown;
    bool shiftDown;
    float elapsedTime;

    GLuint m_triVbo;

    void handleInteraction();

    // member variables for mouse tracking
    QVector2D mStartPos;

    // member variables for shader manipulation
    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_norAttr;

    QOpenGLShaderProgram *m_program;
    GLuint m_MMatrixUniform; // model matrix
    GLuint m_VMatrixUniform; // view matrix
    GLuint m_PMatrixUniform; // projection matrix

    // helper function for loading shaders
    GLuint loadShader(GLenum type, const char *source);

    // converts mouse position to world coordinates
    QVector2D windowToWorld(QPoint point);
    //void makeVbo(Model *model);
    void doTrackball();
    void generateBorderTriangles();
    void drawTriangles(QMatrix4x4 * transform);
};


#endif // RENDERER_H
