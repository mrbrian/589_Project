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
};

#endif // REVSURFACE_H
