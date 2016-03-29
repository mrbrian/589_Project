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



#define PRESET_MAX_RADIUS 0.05
#define PRESET_MAX_AGE 100

class RevSurface;

class TreeSimulation
{
public:
    TreeSimulation(QVector2D origin, float radius, float growthRate)
    {
        m_origin = origin;
        m_radius = radius;
        m_growthRate = growthRate;
        m_age = 1;
        m_maxRadius = PRESET_MAX_RADIUS;
        m_toDie = false;

    }
    ~TreeSimulation()
    {

    }

    void grow()
    {
        m_radius *= m_growthRate;
        if (m_radius > PRESET_MAX_RADIUS)
            m_toDie = true;
        m_age++;
        if (m_age > PRESET_MAX_AGE)
            m_toDie = true;
    }

    float getRadius();
    float getAge();
    float getHeight();
private:

    QVector2D m_origin;
    float m_radius;
    float m_age;
    float m_height;
    //growth variables
    float m_growthRate;
    bool m_toDie;
    float m_maxRadius;


};


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

    void addTreesToTerain(std::vector<QVector3D> controlPoints);
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
    std::vector<RevSurface*> m_trees;
   //vbo

private:
   void createControlMesh(QImage * heightMap, float heightToAreaScale);
   void populateVAO();
};

#endif // TERRAIN_H
