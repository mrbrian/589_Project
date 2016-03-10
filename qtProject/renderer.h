/*
 * CPSC 453 - Introduction to Computer Graphics
 * Assignment 0
 *
 * Renderer - OpenGL widget for drawing simple shapes
 */

#ifndef RENDERER_H
#define RENDERER_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QMouseEvent>

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
    virtual void mousePressEvent(QMouseEvent * event);
    virtual void mouseReleaseEvent(QMouseEvent * event);

private:

    // constant for drawing ovals
    static const int NUM_OVAL_STEPS;

    // member variables for mouse tracking
    QVector2D mStartPos;

    // member variables for shader manipulation
    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_matrixUniform;

    QOpenGLShaderProgram *m_program;

    // helper function for loading shaders
    GLuint loadShader(GLenum type, const char *source);

    // converts mouse position to world coordinates
    QVector2D windowToWorld(QPoint point);

};

#endif // RENDERER_H
