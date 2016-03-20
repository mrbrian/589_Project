#ifndef MODEL
#define MODEL

#include <QVector3D>
#include <QMatrix4x4>
#include "objModel.h"
#include <iostream>

class Model
{
public:
    Model(ObjModel *m, Model *parent);
    ~Model();

    QVector3D getCenter();                  // determine center point from min/max bounds
    QMatrix4x4 getWorldTransform();
    QMatrix4x4 getLocalTransform();
    void resetLocalTransform();
    void setLocalTransform(QMatrix4x4 m);
    float getNormalizeScale();
    float getWorldScale();

    const float *verts;                 // vertex float array
    const float *colours;               // colours float array
    const float *uvs;
    const float *vert_normals;
    const float *face_normals;

    const float *lines_vert_normals;
    const float *lines_face_normals;

    long sizeVerts;
    long sizeColours;
    long sizeUvs;
    long sizeVertNormals;
    long sizeFaceNormals;
    long sizeVertNormalLines;
    long sizeFaceNormalLines;   // size of arrays

    int numVerts;       // number of vertices
    int numTris;        // number of triangles

    GLuint m_modelVbo;          // vbo reference
    GLuint texture;             // texture reference
    Model *parent;              // parent of the model (if any)

private:
    float scale;
    float bounds[6];
    QMatrix4x4 transform_;

    void updateBounds();        // update the min/max vertex bounds
    float *getObjectVertexNormals(ObjModel *source);

    float *readUVs(ObjModel *source, long *size);
    float *calcVertexNormals(ObjModel *source, long *size);     // calculate smooth vertex normals
    float *calcFaceNormals(long *size);                         // calculate flat surface normals
    float *readVerts(ObjModel *source, long *size);             //
    float *generateFaceNormalLines(long *size);
    float *generateVertexNormalLines(long *size);
};

#endif // MISC

