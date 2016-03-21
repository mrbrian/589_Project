#ifndef REVSURFACE_H
#define REVSURFACE_H

#include <QMatrix4x4>
#include <QVector3D>
#include "bspline.h"
#include <cmath>
#include "objModel.h"

class RevSurface
{
public:
    RevSurface(BSpline *u);
    BSpline *curve;
    QVector3D *eval(float u, float v);
    vector<QVector3D*> *evalQuads(float u_step, float v_step);    
    vector<QVector3D*> *evalTriangles(float u_step, float v_step);
    ObjModel *getObjModel(float u_step, float v_step);

private:
    vector<QVector3D*> *getControlMesh(float u_step, float v_step);
};

#endif // REVSURFACE_H
