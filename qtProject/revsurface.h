#ifndef REVSURFACE_H
#define REVSURFACE_H

#include <QMatrix4x4>
#include <QVector3D>
#include "bspline.h"
//#include <cmath>
#include "math.h"
#include "objModel.h"
#include "bspline_blended.h"
#include "terrain.h"

class TreeSimulation;

class RevSurface
{
public:
    RevSurface(BSpline *u, QVector3D c = QVector3D(1,1,1));
    static RevSurface *makeCylinder(float r, float h, QVector3D clr);
    static RevSurface *makeRevSurface(float trunkRad, float radius, float height, QVector3D treeClr, QVector3D pos);
    static RevSurface *makeRevSurface(TreeSimulation *tree, float y, QImage *img);
    BSpline *curve;
    QVector3D *eval(float u, float v);
    vector<QVector3D*> *evalQuads(float u_step, float v_step);    
    ObjModel *getObjModel(float u_step, float v_step);
    QVector3D color;
    QVector3D position;

private:
    vector<QVector3D*> *getControlMesh(float u_step, float v_step);
};

#endif // REVSURFACE_H
