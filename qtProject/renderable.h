#ifndef RENDERABLE_H
#define RENDERABLE_H

#pragma once
#include <glm/glm.hpp>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <vector>
#include <QOpenGLFunctions_4_2_Core>



// triangle
struct triangle
{
    QVector3D coords[3];    // indexes to triangle's vertices
    float index_uv[2];     // indexes to triangles's  texture coordinates
    QVector3D normal;

} ;

class Renderable
{
public:
    virtual void draw() = 0;
    virtual ~Renderable()
    {
        delete m_verts;
        delete m_uvs;
        delete m_colors;
        delete m_faceNorms;
        m_parent = NULL;
    }

protected://members
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_texture;

    QMatrix4x4 m_modelMatrix;

    float * m_verts;
    float * m_uvs;
    float * m_colors;
    float * m_faceNorms;

    long m_sizeVerts;
    long m_sizeUVs;
    long m_sizeColors;
    long m_sizeNorms;

    int m_numVerts;
    int m_numQuads;

    Renderable* m_parent;

    //std::vector<triangle> m_triangles;
public:
    GLuint getVAO() { return m_vao;}
    GLuint getVBO() {return m_vbo;}

    void setVBO(GLuint vbo) { m_vbo = vbo;}
    void setVAO(GLuint vao) { m_vbo = vao;}

    QMatrix4x4 getModelMatrix() {return m_modelMatrix;}

    float * getVerts(){return m_verts;}
    float * getUVs() {return m_uvs;}
    float * getColors() {return m_colors;}
    float * getNormals() {return m_faceNorms;}

    long getSizeVerts() {return m_sizeVerts;}
    long getSizeUVs() {return m_sizeUVs;}
    long getSizeColors() {return m_sizeColors;}
    long getSizeNorms() {return m_sizeNorms;}

    int getNumVerts(){return m_numVerts;}
    int getNumQuads() {return m_numQuads;}

    Renderable * getModelParent();


    //std::vector<triangle> getTriangles() {return m_triangles;}

};

#endif // RENDERABLE_H
