#ifndef TERRAIN_H
#define TERRAIN_H
#pragma once

#include <QDebug>
#include "renderable.h"
#include "QImage"
#include "QColor"
#include "QVector3D"
#include <vector>
#include <iostream>
#include "objModel.h"
#include "ray.h"

class Terrain : public Renderable
{
public:
    Terrain(QImage * heightMap, float heightToAreaScale = 10) : Renderable()
    {
        createControlMesh(heightMap, heightToAreaScale);
        populateVAO();

    }
    virtual ~Terrain(){}
    virtual void draw(){}


    int getControlMeshSize();
    std::vector<QVector3D> m_selectableControlMesh;
    std::vector<int> m_selectabledFlag;
    int low_res_modifier = 30;



    ObjModel *getObjModel();

private://members

    QImage * m_uvImage;
    int m_meshWidth;
    int m_meshHeight;

    QVector3D faceNormal;


   std::vector<QVector3D> m_controlMesh;

   //vbo

private:
   void createControlMesh(QImage * heightMap, float heightToAreaScale);
   void populateVAO();
};

#endif // TERRAIN_H
