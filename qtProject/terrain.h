#ifndef TERRAIN_H
#define TERRAIN_H
#pragma once

#include <QDebug>
#include "renderable.h"
#include "QImage"
#include "QColor"
#include "QVector3D"
#include "QVector2D"
#include <vector>
#include <iostream>
#include "objModel.h"
#include "ray.h"
#include "revsurface.h"
#include "simulation.h"

class RevSurface;

class Terrain : public Renderable
{
public:
    Terrain(QImage * heightMap, float heightToAreaScale = 1) : Renderable()
    {
        m_uvImage = heightMap;
        createControlMesh(heightMap, heightToAreaScale);
        populateVAO();

    }
    virtual ~Terrain(){}
    virtual void draw(){}

    std::vector<RevSurface*> *addTreesToTerrain(std::vector<QVector3D> controlPoints);
    std::vector<TreeSimulation *> simulateTreeGrowth(std::vector<QVector3D> controlPoints);





    int getControlMeshSize();
    std::vector<QVector3D> m_selectableControlMesh;
//    std::vector<int> m_selectabledFlag;


//    std::vector<QVector3D> m_currentlySelectedPoints;


    ObjModel *getObjModel();
    QImage *getImage();
    int get_meshWidth();
    int get_meshHeight();
    float get_y_height(float x, float z);
    static float get_y_height(float x, float z, int width, int height, vector<QVector3D> pts);

private://members
    std::vector<QVector3D> m_controlMesh;
    ObjModel *m_cachedObjModel;
    QImage * m_uvImage;
    int m_meshWidth;
    int m_meshHeight;

    QVector3D faceNormal;
    std::vector<RevSurface*> m_trees;
   //vbo

private:
   void createControlMesh(QImage * heightMap, float heightToAreaScale);
   void populateVAO();
};

#endif // TERRAIN_H
