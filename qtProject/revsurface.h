#ifndef REVSURFACE_H
#define REVSURFACE_H

#include <QMatrix4x4>
#include <QVector3D>
#include "bspline.h"
#include <cmath>
class RevSurface
{
public:
    RevSurface(BSpline *u);
    BSpline *curve;
    QVector3D *eval(float u, float v);
    vector<QVector3D*> *evalQuads(float u_step, float v_step);
};

#endif // REVSURFACE_H
