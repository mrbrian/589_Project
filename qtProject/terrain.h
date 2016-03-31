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






class Terrain : public Renderable
{
public:
    Terrain(QImage * heightMap, float heightToAreaScale = 10) : Renderable()
    {
        m_uvImage = heightMap;
        createControlMesh(heightMap, heightToAreaScale);
        populateVAO();

    }
    virtual ~Terrain(){}
    virtual void draw(){}

    void addTreesToTerrain(std::vector<QVector3D> controlPoints);
    std::vector<TreeSimulation *> simulateTreeGrowth(std::vector<QVector3D> controlPoints);





    int getControlMeshSize();
    std::vector<QVector3D> m_selectableControlMesh;
    std::vector<int> m_selectabledFlag;



    ObjModel *getObjModel();

private://members

    QImage * m_uvImage;
    int m_meshWidth;
    int m_meshHeight;

    QVector3D faceNormal;
    std::vector<QVector3D> m_controlMesh;
    std::vector<RevSurface *> m_trees;
   //vbo

private:
   void createControlMesh(QImage * heightMap, float heightToAreaScale);
   void populateVAO();
};

#endif // TERRAIN_H
