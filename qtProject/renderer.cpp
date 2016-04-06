#include "renderer.h"
#include <QTextStream>
#include <QOpenGLBuffer>
#include <cmath>
#include "trackball.h"
#include <QFileDialog>
#include "tests.h"

#define CHECKERBOARD_SIZE   50
#define FPS                 60.0
#define TIME_PER_FRAME      1.0/FPS

// shader override colour defaults
float def_override[3] = {0, 0, 0};
float red_override[3] = {1, 0, 0};
float grn_override[3] = {0, 1, 0};

// ambient lighting defaults
float def_light[3] = {0.1, 0.1, 0.1};


float select_light[3] = {0.3, 0.3, 0.3};    // selected model has extra ambient light applied

float selectDistance = 10;
int mode = 0;

// constructor
Renderer::Renderer(QWidget *parent)
    : QOpenGLWidget(parent)
{
    // update timer
    renderTimer = new QTimer(this);
    connect(renderTimer, SIGNAL(timeout()), this, SLOT(update()));
    renderTimer->start(TIME_PER_FRAME);

    drawMode = FACES;    
    normalMode = VERTEX;
    displayNormals = false;

    m_submodel = NULL;
    selectedModel = NULL;
    sel_modelIdx = 0;
    ctrlDown = false;
    shiftDown = false;

    resetView();    // initialize camera
}

// constructor
Renderer::~Renderer()
{

}

// called once by Qt GUI system, to allow initialization for OpenGL requirements
void Renderer::initializeGL()
{
    // Qt support for inline GL function calls
	initializeOpenGLFunctions();

    // sets the background clour
    glClearColor(0.7f, 0.7f, 1.0f, 1.0f);

    // links to and compiles the shaders, used for drawing simple objects
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, "textured-phong.vs.glsl");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, "textured-phong.fs.glsl");
    m_program->link();
    m_posAttr = m_program->attributeLocation("position_attr");
    m_colAttr = m_program->attributeLocation("colour_attr");
    m_norAttr = m_program->attributeLocation("normal_attr");
    m_uvAttr = m_program->attributeLocation("texcoords_attr");
    m_PMatrixUniform = m_program->uniformLocation("proj_matrix");
    m_VMatrixUniform = m_program->uniformLocation("view_matrix");
    m_MMatrixUniform = m_program->uniformLocation("model_matrix");
    m_TextureUniform = m_program->uniformLocation("texObject");
    m_OverrideColourUniform = m_program->uniformLocation("overrideColour");
    m_AmbientUniform = m_program->uniformLocation("ambient");
    m_SelectCodeUniform = m_program->uniformLocation("selectCode");
    m_programID = m_program->programId();

    m_terrain = NULL;
    createWhiteTexture();
    setupGround();
}

// called by the Qt GUI system, to allow OpenGL drawing commands
void Renderer::paintGL()
{
    // Clear the screen buffers
    QTextStream cout(stdout);


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Set the current shader program

    glUseProgram(m_programID);

    glUniformMatrix4fv(m_VMatrixUniform, 1, false, m_view.data());
    glUniform1i(m_SelectCodeUniform, -1);

    // Not implemented: set up lighting (if necessary)

    drawCheckerboard();


    //findIntersection



    for(std::vector<Model*>::iterator it = m_models.begin(); it != m_models.end(); ++it)
    {
        Model *m_model = (*it);

        drawModel(m_model);
        if (displayNormals)
        {
            drawNormals(m_model);
        }

        if (mode == 1)
        {
            glPointSize(10);

    //        glColor3f (0.0f, 0.0f, 1.0f);
    //        cout << "num control points: " << m_terrain->getControlMeshSize();
            if(m_model == selectedModel && m_terrain)
            {
                    //Render the entire vector each time (Should be optimized.......maybee..... nah.... )

                    for(int i = 0; i < m_terrain->getControlMeshSize(); i += 1)
                    {
                        if(m_terrain->m_selectabledFlag.at(i) == 1)
                        {
    //                      cout << "selected: " << i <<endl;
                            glUniform3fv(m_OverrideColourUniform, 1, &red_override[0]);
                        }
                        else
                        {
                            glUniform3fv(m_OverrideColourUniform, 1, &grn_override[0]);

                        }

                        glBegin(GL_POINTS);
                        glVertex3f(m_terrain->m_selectableControlMesh.at(i)[0], m_terrain->m_selectableControlMesh.at(i)[1],m_terrain->m_selectableControlMesh.at(i)[2]);
                        glEnd();
                    }
            }
        }
    }

/*
    if (m_terrain != NULL)
    {
        long nBufferSize = m_terrain->getSizeNorms() * sizeof(float),
                vBufferSize = m_terrain->getSizeVerts() * sizeof(float),
                cBufferSize = m_terrain->getSizeColors() * sizeof(float);

        QMatrix4x4 model_matrix = m_terrain->getModelMatrix();
        glUniformMatrix4fv(m_MMatrixUniform, 1, false, model_matrix.data());
        glUniform3fv(m_AmbientUniform, 1, &def_light[0]);
        glUniform3fv(m_OverrideColourUniform, 1, &def_override[0]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_whiteTexture);
        glUniform1i(m_TextureUniform, 0);


        glBindBuffer(GL_ARRAY_BUFFER, m_terrain->getVBO());

        glEnableVertexAttribArray(this->m_posAttr);
        glEnableVertexAttribArray(this->m_norAttr);
        glEnableVertexAttribArray(this->m_colAttr);

        glVertexAttribPointer(this->m_posAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)0);
        glVertexAttribPointer(this->m_norAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)vBufferSize);
        glVertexAttribPointer(this->m_colAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)(vBufferSize + nBufferSize));
        glDrawArrays(GL_QUADS, 0, m_terrain->getSizeVerts());
    }
*/
    // deactivate the program


    m_program->release();
}

float vertList [] = {
    0.0, 0.0, 1.0,
    1.0, 0.0, 1.0,
    1.0, 0.0, 0.0,
    0.0, 0.0, 0.0,

    -1.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    0.0, 0.0, 0.0,
    -1.0, 0.0, 0.0,

    0.0, 0.0, 0.0,
    1.0, 0.0, 0.0,
    1.0, 0.0, -1.0,
    0.0, 0.0, -1.0,

    -1.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, -1.0,
    -1.0, 0.0, -1.0,
};

float colourList [] = {
    1, 1, 1,
    1, 1, 1,
    1, 1, 1,
    1, 1, 1,

    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,

    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,

    1, 1, 1,
    1, 1, 1,
    1, 1, 1,
    1, 1, 1,
};

float normalList [] = {
    0, 1.0, 0,
    0, 1.0, 0,
    0, 1.0, 0,
    0, 1.0, 0,
    0, 1.0, 0,
    0, 1.0, 0,
    0, 1.0, 0,
    0, 1.0, 0,
    0, 1.0, 0,
    0, 1.0, 0,
    0, 1.0, 0,
    0, 1.0, 0,
    0, 1.0, 0,
    0, 1.0, 0,
    0, 1.0, 0,
    0, 1.0, 0,
};

// Saves all the cube info to the VBO
void Renderer::setupGround()
{
    long cBufferSize = sizeof(colourList) * sizeof(float);
    long vBufferSize = sizeof(vertList) * sizeof(float);
    long nBufferSize = sizeof(normalList) * sizeof(float);

    glGenBuffers(1, &this->m_groundVbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->m_groundVbo);

    // Allocate buffer
    glBufferData(GL_ARRAY_BUFFER, vBufferSize + cBufferSize + nBufferSize, NULL, GL_STATIC_DRAW);

    // Upload the data to the GPU
    glBufferSubData(GL_ARRAY_BUFFER, 0, vBufferSize, &vertList[0]);
    glBufferSubData(GL_ARRAY_BUFFER, vBufferSize, cBufferSize, &colourList[0]);
    glBufferSubData(GL_ARRAY_BUFFER, vBufferSize + cBufferSize, nBufferSize, &normalList[0]);
}

// draw a big checkerboard using the small predefined 2x2 checker
void Renderer::drawCheckerboard()
{
    long cBufferSize = sizeof(colourList) * sizeof(float);
    long vBufferSize = sizeof(vertList) * sizeof(float);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_whiteTexture);
    glUniform1i(m_TextureUniform, 0); // Give it the 0'th texture unit
    glUniform3fv(m_OverrideColourUniform, 1, &def_override[0]);
    glUniform3fv(m_AmbientUniform, 1, &def_light[0]);

    glBindBuffer(GL_ARRAY_BUFFER, this->m_groundVbo);

    // Enable the attribute arrays
    glEnableVertexAttribArray(this->m_posAttr);
    glEnableVertexAttribArray(this->m_colAttr);
    glEnableVertexAttribArray(this->m_norAttr);

    // Specifiy where these are in the VBO
    glVertexAttribPointer(this->m_posAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)0);
    glVertexAttribPointer(this->m_colAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)(vBufferSize));
    glVertexAttribPointer(this->m_norAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)(vBufferSize + cBufferSize));

    for (int i = -CHECKERBOARD_SIZE / 2; i < CHECKERBOARD_SIZE; i++)
    {
        for (int j = -CHECKERBOARD_SIZE / 2; j < CHECKERBOARD_SIZE; j++)
        {
            QMatrix4x4 model_matrix;
            model_matrix.translate(i * 2, -1.0f, j * 2);
            glUniformMatrix4fv(m_MMatrixUniform, 1, false, model_matrix.data());

            // Draw the faces
            glDrawArrays(GL_QUADS, 0, 16); // 16 vertices
        }
    }
    glDisableVertexAttribArray(m_norAttr);
    glDisableVertexAttribArray(m_colAttr);
    glDisableVertexAttribArray(m_posAttr);
}

// draw face/vertex normals according to current normal mode
void Renderer::drawNormals(Model *m_model)
{    
    // apply the model's world transform
    QMatrix4x4 model_matrix = m_model->getWorldTransform();
    glUniformMatrix4fv(m_MMatrixUniform, 1, false, model_matrix.data());

    long cBufferSize = m_model->sizeColours;
    long vBufferSize = m_model->sizeVerts;
    long nBufferSize = m_model->sizeFaceNormals;
    long uvBufferSize = m_model->sizeUvs;
    long fnBufferSize = m_model->sizeFaceNormalLines;

    // Bind to the correct context
    glBindBuffer(GL_ARRAY_BUFFER, m_model->m_modelVbo);

    // Enable the attribute arrays
    glEnableVertexAttribArray(this->m_posAttr);
    glEnableVertexAttribArray(this->m_colAttr);
    glEnableVertexAttribArray(this->m_norAttr);
    glEnableVertexAttribArray(this->m_uvAttr);

    // Specifiy where these are in the VBO
    glVertexAttribPointer(this->m_colAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)(vBufferSize));
    glVertexAttribPointer(this->m_norAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)(vBufferSize));
    glVertexAttribPointer(this->m_uvAttr, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)(vBufferSize + cBufferSize + 2 * nBufferSize));

    int numPoints = 0;
    switch (normalMode)
    {
        case SURFACE:
            // point to position of surface normal lines
            glVertexAttribPointer(this->m_posAttr, 3, GL_FLOAT, 0, GL_FALSE,
                            (const GLvoid*)(vBufferSize + cBufferSize + 2 * nBufferSize + uvBufferSize));
            // set override colour to red (make lines always red)
            glUniform3fv(m_OverrideColourUniform, 1, &red_override[0]);
            numPoints = m_model->numTris * 2;
            break;
        case VERTEX:
            // point to position of vertex normal lines
            glVertexAttribPointer(this->m_posAttr, 3, GL_FLOAT, 0, GL_FALSE,
                            (const GLvoid*)(vBufferSize + cBufferSize + 2 * nBufferSize + uvBufferSize + fnBufferSize));
            // set override colour to green (make lines always green)
            glUniform3fv(m_OverrideColourUniform, 1, &grn_override[0]);
            numPoints = m_model->numVerts * 2;
        break;
    }

    glDrawArrays(GL_LINES, 0, numPoints);    // 2 verts per tri

    glDisableVertexAttribArray(m_norAttr);
    glDisableVertexAttribArray(m_colAttr);
    glDisableVertexAttribArray(m_posAttr);
    glDisableVertexAttribArray(m_uvAttr);
}

// draw the given model
void Renderer::drawModel(Model *m_model)
{
    QTextStream cout(stdout);

    int glDrawMode = GL_TRIANGLES;
    QMatrix4x4 model_matrix = m_model->getWorldTransform();

    glUniformMatrix4fv(m_MMatrixUniform, 1, false, model_matrix.data());

    Ray cam_ray (camera.getPosition(), camera.getRotation());

    if(m_model == selectedModel)
    {
        if(mode == 1)
        {           
//            cout << "num control points: " << m_terrain->getControlMeshSize();
            double a =  m_model->findIntersection(cam_ray);

//            if(a != 0)
//                glUniform3fv(m_AmbientUniform, 1, &def_light[0]);
//            else
//                glUniform3fv(m_AmbientUniform, 1, &def_light[0]);
        }
    }

    glUniform3fv(m_OverrideColourUniform, 1, &def_override[0]);

    long cBufferSize = m_model->sizeColours;
    long vBufferSize = m_model->sizeVerts;
    long nBufferSize = m_model->sizeFaceNormals;

    // Bind to the correct context
    glBindBuffer(GL_ARRAY_BUFFER, m_model->m_modelVbo);

    // Enable the attribute arrays
    glEnableVertexAttribArray(this->m_posAttr);
    glEnableVertexAttribArray(this->m_colAttr);
    glEnableVertexAttribArray(this->m_norAttr);
    glEnableVertexAttribArray(this->m_uvAttr);

    // Specifiy where these are in the VBO
    glVertexAttribPointer(this->m_posAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)0);
    glVertexAttribPointer(this->m_colAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)(vBufferSize));

    switch (normalMode)     // smooth or flat shading
    {
        case VERTEX:
        glVertexAttribPointer(this->m_norAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)(vBufferSize + cBufferSize));
            break;
        case SURFACE:
        glVertexAttribPointer(this->m_norAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)(vBufferSize + cBufferSize + nBufferSize));
            break;
    }

    // process drawing mode
    glActiveTexture(GL_TEXTURE0);
    switch (drawMode)
    {
        case WIRE:     // wireframe
            glDrawMode = GL_LINES;
            break;
        case FACES:     // regular faces
            glBindTexture(GL_TEXTURE_2D, m_whiteTexture);
            break;
        case TEXTURED:     // multicolor
            glBindTexture(GL_TEXTURE_2D, m_model->texture);
            break;
    }
    glUniform1i(m_TextureUniform, 0); // Give it the 0'th texture unit

    glVertexAttribPointer(this->m_uvAttr, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)(vBufferSize + cBufferSize + 2 * nBufferSize));

    // Draw the faces
    glDrawArrays(glDrawMode, 0, m_model->numVerts);

    glDisableVertexAttribArray(m_norAttr);
    glDisableVertexAttribArray(m_colAttr);
    glDisableVertexAttribArray(m_posAttr);
    glDisableVertexAttribArray(m_uvAttr);
}

// clear models list and free memory
void Renderer::clearModels()
{
    selectedModel = NULL;

    while (!m_models.empty())
    {
        std::vector<Model*>::iterator it = m_models.end();
        it--;
        delete((*it));
        m_models.erase(it);
    }
}

// Change the draw mode (Wire, Face, Multicolor)
void Renderer::setDrawMode(DrawMode mode)
{
    drawMode = mode;
}

// Toggles display of the normals
void Renderer::toggleNormals()
{
    displayNormals = !displayNormals;
}

// Change the draw mode (Wire, Face, Multicolor)
void Renderer::setNormalMode(NormalMode mode)
{
    normalMode = mode;
}

// called by the Qt GUI system, to allow OpenGL to respond to widget resizing
void Renderer::resizeGL(int w, int h)
{
    // width and height are better variables to use
    Q_UNUSED(w); Q_UNUSED(h);

    // update viewing projections
    glUseProgram(m_programID);

    // Set up perspective projection, using current size and aspect
    // ratio of display
    QMatrix4x4 projection_matrix;
    projection_matrix.perspective(40.0f, (GLfloat)width() / (GLfloat)height(),
                                  0.1f, 10000.0f);
    glUniformMatrix4fv(m_PMatrixUniform, 1, false, projection_matrix.data());

    glViewport(0, 0, width(), height());
}

// override mouse press event
void Renderer::mousePressEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);

    mouseButtons = event->buttons();
    curr_x = event->x();
    curr_y = event->y();

    prev_x = curr_x;
    prev_y = curr_y;

    float nor_mouse_x = event->x();
    float nor_mouse_y = event->y();

//    cout << "*********************************\n";

    if(mode == 1)
    {
//        mode = 1;
//        cout << "mode 1 pressed" << endl;
        normalizeMouseToSelect(nor_mouse_x, nor_mouse_y);



        for (int i = 0; i < m_terrain->m_selectableControlMesh.size(); i++)
        {
//            cout << "loop point: " << i << endl;
            if(i == 0)
            {
//                cout << "abs x: " << abs(m_terrain->m_selectableControlMesh.at(i)[0] - nor_mouse_x) << endl;
//                cout << "abs y: " << abs(m_terrain->m_selectableControlMesh.at(i)[2] - nor_mouse_y) << endl;
            }


            if (abs(m_terrain->m_selectableControlMesh.at(i)[0] - nor_mouse_x) < selectDistance && abs(m_terrain->m_selectableControlMesh.at(i)[2] - nor_mouse_y) < selectDistance)
            {
                if(m_terrain->m_selectabledFlag.at(i) == 0 )
                {
                    m_terrain->m_selectabledFlag.at(i) = 1;
                    old_select_point = current_selected_point;
                    current_selected_point = i;
                    numSelectedPoints++;
//                    cout << "selected point: " << i << endl;

                    if(numSelectedPoints > 1)
                    {
//                        connectPoints(old_select_point, current_selected_point); Not Sure if nessecary?
                    }

                }

//                cout << "old control : " << old_select_point << endl;
//                cout << "curr control : " << current_selected_point << endl;



            }
        }

    }
    else if (mode == 0)
    {
//        mode = 0;
        cout << "mode 0 pressed" << endl;

    }

//    cout << "Stub: Motion at " << nor_mouse_x << ", " << nor_mouse_y << ".\n";
//    cout << "first point: " << m_terrain->m_selectableControlMesh.at(0)[0] << ", " <<  m_terrain->m_selectableControlMesh.at(0)[2]
//         << endl << "mid point: " << m_terrain->m_selectableControlMesh.at(m_terrain->m_selectableControlMesh.size()/2)[0] << ", " <<  m_terrain->m_selectableControlMesh.at(m_terrain->m_selectableControlMesh.size()/2)[2]
//         << endl << "last point: " << m_terrain->m_selectableControlMesh.at(m_terrain->m_selectableControlMesh.size()-1)[0] << ", " <<  m_terrain->m_selectableControlMesh.at(m_terrain->m_selectableControlMesh.size()-1)[2]
//         << endl;
//    cout << "size control mesh : " << m_terrain->m_selectableControlMesh.size() << endl;
//    cout << "size control flag : " << m_terrain->m_selectabledFlag.size() << endl;



}

// override mouse release event
void Renderer::mouseReleaseEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);
    mouseButtons = event->buttons();
}

// look for modifier keys
void Renderer::setKeyPressed(int val)
{
    switch (val)
    {
        case Qt::Key_Alt:
            altDown = true;
        break;
        case Qt::Key_Shift:
            shiftDown = true;
        break;
        case Qt::Key_Control:
            ctrlDown = true;
        break;        
    }
}

// undo modifier keys
void Renderer::setKeyReleased(int val)
{
    switch (val)
    {
        case Qt::Key_Alt:
            altDown = false;
        break;
        case Qt::Key_Shift:
            shiftDown = false;
        break;
        case Qt::Key_Control:
            ctrlDown = false;
        break;
    }
}

// override mouse move event
void Renderer::mouseMoveEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);
    prev_x = curr_x;
    prev_y = curr_y;

    curr_x = event->x();
    curr_y = event->y();

    handleInteraction();       // adjust the models or camera
}

void Renderer::normalizeMouseToSelect(float & x , float & y)
{
    x = ((x - 10) / (593 - 10)) * 510;
    y = ((y - 10) / (593 - 10)) * 510;
}

// load a model and make the selected model it's parent
void Renderer::setSubmodel(ObjModel *obj_m)
{
    m_submodel = new Model(obj_m, selectedModel);
    //selectedModel = m_submodel;
    m_models.push_back(m_submodel);
    selectModel(m_models.size() - 1);   // select this model
    makeVbo(m_submodel);

    m_submodel->texture = m_whiteTexture;
}

// load a model and make the selected model it's parent
Model *Renderer::setSubmodel_hack(ObjModel *obj_m)
{
    m_submodel = new Model(obj_m, selectedModel);
    //selectedModel = m_submodel;
    m_models.push_back(m_submodel);
    makeVbo(m_submodel);

    m_submodel->texture = m_whiteTexture;
    return m_submodel;
}

// create a Model using a given ObjModel
void Renderer::setModel(ObjModel *obj_m)
{
    // create new model
    Model *m_model = new Model(obj_m, NULL);   // NULL = no parent

    m_models.push_back(m_model);        // add to model list

    selectModel(m_models.size() - 1);   // select this model
    makeVbo(m_model);

    m_model->texture = m_whiteTexture;
}

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
}

// creates a small white texture and stores on gpu
void Renderer::createWhiteTexture()
{
    QImage *image = new QImage(16, 16, QImage::Format_RGBA8888);    // Convert it to a usable format
    image->fill(0xFFFFFF);

    glGenTextures(1, &m_whiteTexture); // Generate a texture handle
    glActiveTexture(GL_TEXTURE0); // Make sure we're using texture unit 0
    glBindTexture(GL_TEXTURE_2D, m_whiteTexture); // bind the texture handle

    // Write it to the GPU
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 image->width(),
                 image->height(),
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_INT_8_8_8_8_REV,
                 (const GLvoid *)image->bits());
    // Set the filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

// saves the texture to the gpu and saves a reference in the model
void Renderer::setModelTexture(QImage *image)
{
    if (selectedModel == NULL)                  // if no selected model, just exit
        return;

    glGenTextures(1, &selectedModel->texture);  // Generate a texture handle
    glActiveTexture(GL_TEXTURE0);               // Make sure we're using texture unit 0
    glBindTexture(GL_TEXTURE_2D, selectedModel->texture); // bind the texture handle

    *image = image->convertToFormat(QImage::Format_RGBA8888); // Convert it to a usable format

    // Write it to the GPU
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 image->width(),
                 image->height(),
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_INT_8_8_8_8_REV,
                 (const GLvoid *)image->bits());
    // Set the filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glVertexAttribPointer(this->m_uvAttr, 2, GL_FLOAT, GL_FALSE, 0, &selectedModel->uvs[0]);
}

// update view transform from camera settings
void Renderer::updateCamera()
{
    m_view = QMatrix4x4();
    m_view.lookAt(camera.getPosition(), camera.getTarget(), camera.getUp());

    if(mode == 1)
    {
        m_view.rotate(90, camera.getUp());
    }
}

// triggers paint update
void Renderer::update()
{
    elapsedTime += TIME_PER_FRAME;
    QOpenGLWidget::update();
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

    applyTransform(&q);     // transform selected model or all models
}

 // transform the selected model or all models if there is no selected model
void Renderer::applyTransform(QMatrix4x4 *t)
{
    // if a model is selected, only transform that model
    if (selectedModel)
    {
        selectedModel->setLocalTransform((*t) * selectedModel->getLocalTransform());
        return;
    }

    for(std::vector<Model*>::iterator it = m_models.begin(); it != m_models.end(); ++it)
    {
        Model *m_model = (*it);

        // don't transform children since they will inherit from the parent
        if (!m_model->parent)
            m_model->setLocalTransform((*t) * m_model->getLocalTransform());
    }
}

// user interacting with the scene via mouse
void Renderer::handleInteraction()
{
    if(mode == 1)
    {
        return;
    }

    QVector3D delta;
    float dx = (float)(curr_x - prev_x) / 100;
    float dy = (float)(curr_y - prev_y) / 100;

    QMatrix4x4 modelTrans;

    if (altDown)
    {
        if (mouseButtons & Qt::RightButton)
        {
            delta[0] = dx;

            QVector3D camPos = camera.getPosition();
            QVector3D forward = camera.getForward();

            camPos += forward * delta[0];
            QVector3D dir = forward - camPos;

            if (dir[0] != 0 || dir[2] != 0) // not facing straight down
                camera.setPosition(camPos);
        }
        else if (mouseButtons & Qt::MiddleButton)
        {
            delta[0] = dx;
            delta[1] = dy;

            QVector3D targ = camera.getTarget();
            QVector3D camPos = camera.getPosition();

            QVector3D right = camera.getRight();
            QVector3D up = camera.getUp();

            right *= delta[0];
            up *= delta[1];

            targ += right + up;
            camPos += right + up;

            camera.setTarget(targ);
            camera.setPosition(camPos);            
        }
        else if (mouseButtons & Qt::LeftButton)
        {
            delta[0] = -dx * 25;
            delta[1] = dy * 25;

            QVector3D targ = camera.getTarget();
            QVector3D camPos = camera.getPosition();

            QMatrix4x4 t;
            t.translate(targ);

            QMatrix4x4 invT;
            invT.translate(-targ);

            QMatrix4x4 rot;

            QVector3D up = QVector3D(0,1,0);
            QVector3D right = camera.getRight();

            rot.rotate(delta[0], up);
            rot.rotate(delta[1], right);

            QMatrix4x4 transform = t * rot * invT;

            camPos = transform * camPos;
            camera.setPosition(camPos);
        }
        updateCamera();
        return;
    }

    if (shiftDown)                // modify camera position
    {
        if (mouseButtons & Qt::LeftButton)
        {
            delta[0] = dx;
            delta[1] = -dy;
            camera.setPosition(camera.getPosition() + delta);
        }
        if (mouseButtons & Qt::MidButton)
        {
            delta[2] = dx;
            camera.setPosition(camera.getPosition() + delta);
        }
        if (mouseButtons & Qt::RightButton)
        {
            QMatrix4x4 m;
            m.rotate(dx * 10, 0, 0, 1);
            camera.setUp(m * camera.getUp());
        }
        updateCamera();
    }
    if (ctrlDown)    // modify camera lookat target
    {
//        if (mouseButtons & Qt::LeftButton)      // LB modifies along x-axis & y-axis
//        {
//            delta[0] = dx;
//            delta[1] = -dy;
//            camera.setTarget(camera.getTarget() + delta);
//        }
//        if (mouseButtons & Qt::MidButton)      // LB modifies along z-axis
//        {
//            delta[2] = dx;
//            camera.setTarget(camera.getTarget() + delta);
//        }
//        updateCamera();
    }


    if (!shiftDown && !ctrlDown && !altDown)    // translate / rotate the model
    {
        if (mouseButtons & Qt::LeftButton)      // LB modifies along x-axis & y-axis
        {
            delta[0] = dx;
            delta[1] = -dy;
            modelTrans.translate(delta);
            applyTransform(&modelTrans);
        }
        if (mouseButtons & Qt::MiddleButton)    // MB modifies  along z-axis
        {
            delta[2] = dx;
            modelTrans.translate(delta);
            applyTransform(&modelTrans);
        }
        if (mouseButtons & Qt::RightButton)     // RB rotates model via trackball
        {
            doTrackball();
        }
    }

}

// reset model(s) transforms
void Renderer::resetModels()
{
    for(std::vector<Model*>::iterator it = m_models.begin(); it != m_models.end(); ++it)
    {
        Model *m_model = (*it);
        m_model->resetLocalTransform();
    }
}

// reset camera
void Renderer::resetView()
{
    camera.setTarget(QVector3D(0, 0, 0));    // camera points at the origin
    camera.setPosition(QVector3D(0, 0, 4));       // starting position
    camera.setUp(QVector3D(0, 1, 0));        // up vector
    updateCamera();                     // update view matrix
}

// reset cam and models
void Renderer::resetAll()
{
    resetModels();
    resetView();
}

// deselects model
void Renderer::deselectModel()
{
    selectedModel = NULL;
    sel_modelIdx = 0;
}

// cycle through the models
void Renderer::cycleModel()
{
    selectModel(sel_modelIdx + 1);
}

void Renderer::selectMesh()
{

    QTextStream cout(stdout);

    if(mode == 1)
    {
        cout << "select mesh mode off" << endl;
        mode = 0;

        camera.setPosition(old_cam_position);
        updateCamera();
        std::cout << "gathering control points\n" << endl;

        if (numSelectedPoints > 2)
        {
            m_currentlySelected.clear();
            for (int i = 0; i < m_terrain->m_selectableControlMesh.size(); i++)
            {
                if(m_terrain->m_selectabledFlag.at(i) == 1 )
                {                    
                    QVector3D point = QVector3D(m_terrain->m_selectableControlMesh.at(i).x() / (float)width(), 0 , m_terrain->m_selectableControlMesh.at(i).z() / (float) height());
                    //std::cout << point.x() << "," << m_terrain->m_selectableControlMesh.at(i).y() << "," << point.z() << std::endl;
                    m_currentlySelected.push_back(point);


                }
            }
        }

        //std::cout << "Added trees to terrain\n";
        vector<RevSurface*> *treeRevs = m_terrain->addTreesToTerrain(m_currentlySelected);

        for (int i = 0; i < treeRevs->size(); i++)
        {
            RevSurface *tree = (*treeRevs)[i];
            ObjModel *obj = tree->getObjModel(0.01, 0.01);

            QMatrix4x4 trans;
            //trans.scale(0.002);
            QVector3D treePos = tree->position;
            treePos[0] *= m_terrain->get_meshWidth();
            treePos[2] *= m_terrain->get_meshHeight();
            trans.setColumn(3, QVector4D(treePos, 1));

            std::cout << treePos.x() << "," << treePos.y() << "," << treePos.z() << std::endl;

            //this->setModel(obj);
            //this->selectedModel->setLocalTransform(trans);

            Model *treeModel = this->setSubmodel_hack(obj);
            treeModel->setLocalTransform(trans);

            //selectedModel->setLocalTransform(trans * selectedModel->getLocalTransform());
        }

        //cout << "num selected: " << m_currentlySelected.size();

    }
    else if (mode == 0)
    {
        cout << "select mesh mode on" << endl;
        mode = 1;

        old_cam_position = camera.getPosition();

        QVector3D newPosition = QVector3D(0.001, 2.8, 0);

        camera.setPosition(newPosition);
        camera.setTarget(QVector3D(0,0,0));

        camera.setForward(QVector3D(0,0,1));
        camera.setRight(QVector3D(1,0,0));
        camera.setUp(QVector3D(0,1,0));

        updateCamera();
        resetModels();

    }


}

// selects a model
void Renderer::selectModel(int idx)
{
    selectedModel = NULL;
    sel_modelIdx = idx;

    if (m_models.size() > 0)
    {
        sel_modelIdx =  sel_modelIdx % m_models.size();
        selectedModel = m_models[sel_modelIdx];
    }
}

// public getter
QString Renderer::getDrawMode()
{
    switch (drawMode)
    {
        case WIRE:
            return "DrawMode: Wireframe    ";
        case FACES:
            return "DrawMode: Faces    ";
        case TEXTURED:
            return "DrawMode: Textured    ";
    }
    return "DrawMode: Unknown";
}

QString Renderer::getNormalMode()
{
    switch (normalMode)
    {
    case VERTEX:
        return "Normals: Vertex    ";
    case SURFACE:
        return "Normals: Face    ";
    }
    return "Normals: Unknown    ";
}

QString Renderer::getSelectedModel()
{
    if (!selectedModel)
        return "Model: None";

    return ("Model: " + QString::number(sel_modelIdx + 1));
}

Terrain *Renderer::createTerrain(QImage * image)
{
        std::cout << "got here" << std::endl;
        GLuint terrainVAO;
        QOpenGLFunctions_4_2_Core::glGenVertexArrays(1, &terrainVAO);
        m_terrain = new Terrain(image, 25);
        populateTerrainVAO();
        return m_terrain;
}

void Renderer::populateTerrainVAO()
{

    long nBufferSize = m_terrain->getSizeNorms() * sizeof(float),
            vBufferSize = m_terrain->getSizeVerts() * sizeof(float),
            cBufferSize = m_terrain->getSizeColors() * sizeof(float);

    GLuint terrainVBO;
    glGenBuffers(1, &terrainVBO);
    m_terrain->setVBO(terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);

    glBufferData(GL_ARRAY_BUFFER,
                 nBufferSize + vBufferSize + cBufferSize,
                 NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, vBufferSize, m_terrain->getVerts());
    glBufferSubData(GL_ARRAY_BUFFER, vBufferSize, nBufferSize, m_terrain->getNormals());
    glBufferSubData(GL_ARRAY_BUFFER, vBufferSize + nBufferSize, cBufferSize, m_terrain->getColors());



}


void Renderer::connectPoints(int oldPoint, int newPoint)
{

    QTextStream cout(stdout);
    cout <<"##################\n";

//    cout <<"starting connections\n";
//    cout << "old point at: " <<  oldPoint[0] << endl;


    int controlMeshRowsSeperator =  sqrt(m_terrain->m_selectableControlMesh.size());
    cout <<"seperator: " << controlMeshRowsSeperator << endl;


    int oldCol = oldPoint % controlMeshRowsSeperator;
//    cout <<"oldpoint col: " << oldCol << endl;

    int oldRow = oldPoint / controlMeshRowsSeperator;
//    cout <<"oldpoint col: " << oldRow << endl;

    int newCol = newPoint % controlMeshRowsSeperator;
    cout <<"newPoint col: " << newCol << endl;

    int newRow = newPoint / controlMeshRowsSeperator;
    cout <<"newPoint col: " << newRow << endl;



//                    if(m_terrain->m_selectabledFlag.at(i) == 0 )
//                    {
//                        m_terrain->m_selectabledFlag.at(i) = 1;

//                    }

    cout <<"############################\n";

}
