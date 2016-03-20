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
    vector<QVector3D*> *result = new vector<QVector3D*>;

    float V_INCREMENT = v_step * M_PI * 2;

    for (float u = 0; u <= 1 - u_step; u += u_step)
    {
        for (float vp = 0; vp < 1; vp += v_step)
        {
            float v = v * M_PI * 2;
            QVector3D **quad = new QVector3D*[4];

            QVector2D *u_pt1 = curve->evalPoint(u);
            QVector2D *u_pt2 = curve->evalPoint(u + u_step);

            quad[0] = new QVector3D((*u_pt1)[0]  * cos(v), (*u_pt1)[1], (*u_pt1)[0] * sin(v));

            quad[1] = new QVector3D
                    ((*u_pt1)[0]  * cos(v + V_INCREMENT),
                    (*u_pt1)[1],
                    (*u_pt1)[0] * sin(v + V_INCREMENT));

            quad[2] = new QVector3D
                    ((*u_pt2)[0] * cos(v + V_INCREMENT),
                    (*u_pt2)[1],
                    (*u_pt2)[0] * sin(v + V_INCREMENT));

            quad[3] = new QVector3D((*u_pt2)[0]  * cos(v), (*u_pt2)[1], (*u_pt2)[0] * sin(v));

            for (int i = 0; i < 4; i++)
                result->push_back(quad[i]);
        }
    }

    return (result);
}

vector<QVector3D*> *RevSurface::evalTriangles(float u_step, float v_step)
{
    vector<QVector3D*> *result = new vector<QVector3D*>;

    float V_INCREMENT = v_step * M_PI * 2;
    int num_u = ceil(1.0 / u_step);
    int num_v = ceil(1.0 / v_step);

    for (int i_u = 0; i_u < num_u; i_u++)
    {
        for (int i_v = 0; i_v < num_v; i_v++)
        {
            float u = u_step * i_u;
            float v = v_step * i_v * M_PI * 2;

            QVector3D **tri_1 = new QVector3D*[3];
            QVector3D **tri_2 = new QVector3D*[3];

            QVector2D *u_pt1 = curve->evalPoint(u);
            QVector2D *u_pt2 = curve->evalPoint(u + u_step);

            tri_1[0] = new QVector3D((*u_pt1)[0]  * cos(v), (*u_pt1)[1], (*u_pt1)[0] * sin(v));

            tri_1[1] = new QVector3D
                    ((*u_pt1)[0]  * cos(v + V_INCREMENT),
                    (*u_pt1)[1],
                    (*u_pt1)[0] * sin(v + V_INCREMENT));

            tri_1[2] = new QVector3D
                    ((*u_pt2)[0] * cos(v + V_INCREMENT),
                    (*u_pt2)[1],
                    (*u_pt2)[0] * sin(v+V_INCREMENT));

            tri_2[0] = new QVector3D
                    ((*u_pt2)[0] * cos(v + V_INCREMENT),
                    (*u_pt2)[1],
                            (*u_pt2)[0] * sin(v+V_INCREMENT));

            tri_2[1] = new QVector3D((*u_pt2)[0]  * cos(v), (*u_pt2)[1], (*u_pt2)[0] * sin(v));

            tri_2[2] = new QVector3D((*u_pt1)[0]  * cos(v), (*u_pt1)[1], (*u_pt1)[0] * sin(v));

            for (int i = 0; i < 3; i++)
            {
                result->push_back(tri_1[i]);
            }
            for (int i = 0; i < 3; i++)
            {
                result->push_back(tri_2[i]);
            }
        }
    }
    /*QMatrix4x4 r;
    r.rotate(rad, QVector3D(0,1,0));*/

    return (result);
}
