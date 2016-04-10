/*
 * CPSC 453 - Introduction to Computer Graphics
 * Assignment 1
 *
 * Renderer - OpenGL widget for drawing scene
 */

#ifndef RENDERER_H
#define RENDERER_H


#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_2_Core>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QMouseEvent>
#include <QTimer>
#include "model.h"
#include "camera.h"
#include "terrain.h"
#include "treenode.h"
#include "ray.h"

using namespace std;

class Renderer : public QOpenGLWidget, protected QOpenGLFunctions_4_2_Core
{

    // informs the qmake that a Qt moc_* file will need to be generated
    Q_OBJECT

public:
    // constructor
    Renderer(QWidget *parent = 0);

    enum Direction {DIR_NONE, DIR_X, DIR_Y, DIR_Z};
    enum DrawMode {WIRE, FACES, TEXTURED};
    enum NormalMode {VERTEX, SURFACE};
    enum EditMode {VIEW_R, VIEW_T, VIEW_P, MODEL_R, MODEL_S, MODEL_T, VIEWPORT};

    // destructor
    virtual ~Renderer();

    void makeVbo(Model *model);
    void setModel(ObjModel *model);
    void setSubmodel(ObjModel *model);
    Model *setSubmodel_hack(ObjModel *obj_m);
    void setNormalMode(NormalMode mode);

    QString getDrawMode();
    QString getNormalMode();
    QString getSelectedModel();

public slots:
    void setKeyPressed(int val);
    void setKeyReleased(int val);
    void setModelTexture(QImage *image);
    void setDrawMode(DrawMode mode);
    void clearModels();
    void resetView();
    void resetAll();
    void resetModels();
    void toggleNormals();
    void deselectModel();
    void cycleModel();
    void selectModel(int idx);
    Terrain *createTerrain(QImage* image);

    void selectMesh();

protected:

    // override fundamental drawing functions

    // Called when OpenGL is first initialized
    void initializeGL();

    // Called when the window is resized
    void resizeGL(int w, int h);

    // Called when the window needs to be redrawn
    void paintGL();

    // override mouse event functions

    // Called when a mouse button is pressed
    virtual void mousePressEvent(QMouseEvent * event);

    // Called when a mouse button is released
    virtual void mouseReleaseEvent(QMouseEvent * event);

    // Called when the mouse moves
    virtual void mouseMoveEvent(QMouseEvent * event);

    void vPerformTransfo(float fOldX, float fNewX, float fOldY, float fNewY);

    void drawCylinder(float r1,float r2, QVector3D p1, QVector3D p2);

private slots:
    void update();

private:
    Model *m_cylinder;
    // member variables for shader manipulation
    GLuint m_programID;
    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_norAttr;
    GLuint m_uvAttr;
    GLuint m_MMatrixUniform; // model matrix
    GLuint m_VMatrixUniform; // view matrix
    GLuint m_PMatrixUniform; // projection matrix
    GLuint m_TextureUniform; // projection matrix
    GLuint m_AmbientUniform;
    GLuint m_OverrideColourUniform;
    GLuint m_SelectCodeUniform;

    QOpenGLShaderProgram *m_program;

    GLuint m_whiteTexture;
    GLuint m_groundVbo;

    vector<Model*> m_models;
    Model *m_submodel;
    Model *selectedModel;
    Model *selectedModelRay;

    int sel_modelIdx;

    float curr_x, curr_y;   // mouse positions
    float prev_x, prev_y;

    QTimer *renderTimer;
    QMatrix4x4 m_view;
    Terrain * m_terrain;
    Camera camera;

    QVector3D old_cam_position; //use for select mode

   //QVector3D cam_up;
   //QVector3D cam_pos;
   //QVector3D cam_target;

    // keep track of which renderering mode to draw
    // 0 = wireframe, 1 = face, 2 = textured
    DrawMode drawMode;
    NormalMode normalMode;
    bool displayNormals;
    EditMode editMode;

    // mouse buttons that are currently pressed
    int mouseButtons;
    bool altDown;
    bool ctrlDown;
    bool shiftDown;    

    void normalizeMouseToSelect(float &, float &);
    void connectPoints(int oldPoint, int newPoint);

//    QVector3D old_select_point;
//    QVector3D current_selected_point;
    int old_select_point;
    int current_selected_point;
    int numSelectedPoints = 0;
    std::vector<QVector3D> m_currentlySelected;


    float elapsedTime;

    // helper function for loading shaders
    GLuint loadShader(GLenum type, const char *source);

    void initCylinder();
    void handleInteraction();
    void createWhiteTexture();
    void updateCamera();
    void setupGround();
    void drawCheckerboard();
    void applyTransform(QMatrix4x4 *t);
    void drawModel(Model * m_model);
    void drawNormals(Model * m_model);
    void doTrackball();
    void populateTerrainVAO();
    void drawTree_cylinders(Tree *t);
    void drawTree_wireframe(Tree *t);
};

#endif // RENDERER_H
