#ifndef TERRAIN_H
#define TERRAIN_H
#pragma once
#include "renderable.h"
#include "QImage"
#include "QColor"
#include "QVector3D"
#include <vector>
#include <iostream>
class Terrain : public Renderable
{
public:
    Terrain(QImage * heightMap, float heightToAreaScale = 1) : Renderable()
    {
        createControlMesh(heightMap, heightToAreaScale);
        populateVAO();

    }
    virtual ~Terrain(){}
    virtual void draw(){}

private://members

    QImage * m_uvImage;
    int m_meshWidth;
    int m_meshHeight;


   std::vector<QVector3D> m_controlMesh;
   //vbo

private:
   void createControlMesh(QImage * heightMap, float heightToAreaScale);
   void populateVAO();
};

#endif // TERRAIN_H
