#include "revsurface.h"

RevSurface::RevSurface(BSpline *u)
{
    curve = u;
}


QVector3D *RevSurface::eval(float u, float v)
{
    float rad = v *360;// (M_PI * 2);

    QVector2D *p2 = curve->evalPoint(u);
    QVector3D *p3 = new QVector3D(*p2);
    QMatrix4x4 r;
    r.rotate(rad, QVector3D(0,1,0));
    QVector3D *result = new QVector3D(r * (*p3));

    return (result);
    //return QVector3D(-1,-1,-1);
}
