#include "revsurface.h"

#define ONE_REV_DEG 360

RevSurface::RevSurface(BSpline *u)
{
    curve = u;
}


// v range = [0,1]
QVector3D *RevSurface::eval(float u, float v)
{
    float deg = v * ONE_REV_DEG;

    QVector2D *p2 = curve->evalPoint(u);
    QVector3D *p3 = new QVector3D(*p2);
    QMatrix4x4 r;
    r.rotate(deg, QVector3D(0,1,0));
    QVector3D *result = new QVector3D(r * (*p3));

    return (result);
}

vector<QVector3D*> *RevSurface::evalQuads(float u_step, float v_step)
{
/*
    for (int y = 0; v < ONE_REV_DEG; v += v_step)
    {
        for (int u = 0; u < ONE_REV_DEG; u += u_step)
        {
            float u_rad = u * ONE_REV_DEG;
            float v_rad = v * ONE_REV_DEG;

            QVector2D *p0 = eval(u, v);
        }
    }
*/
    vector<QVector3D*> *result = new vector<QVector3D*>;

    float V_INCREMENT = v_step * M_PI * 2;

    for (float u = 0; u < 1 - u_step; u += u_step)
    {
        for (float v = 0; v < M_PI * 2; v+= V_INCREMENT)
        {
            QVector3D **quad = new QVector3D*[4];

            QVector2D *u_pt1 = curve->evalPoint(u);
            QVector2D *u_pt2 = curve->evalPoint(u + u_step);

            quad[0] = new QVector3D
                    ((*u_pt1)[0]  * cos(v + V_INCREMENT),
                    (*u_pt1)[1],
                    (*u_pt1)[0] * sin(v + V_INCREMENT));

            quad[1] = new QVector3D
                    ((*u_pt2)[0] * cos(v + V_INCREMENT),
                    (*u_pt2)[1],
                    (*u_pt2)[0] * sin(v+V_INCREMENT));

            quad[2] = new QVector3D((*u_pt2)[0]  * cos(v), (*u_pt2)[1], (*u_pt2)[0] * sin(v));
            quad[3] = new QVector3D((*u_pt1)[0]  * cos(v), (*u_pt1)[1], (*u_pt1)[0] * sin(v));

            for (int i = 0; i < 4; i++)
                result->push_back(quad[i]);
        }
    }
    /*QMatrix4x4 r;
    r.rotate(rad, QVector3D(0,1,0));*/

    return (result);
}
