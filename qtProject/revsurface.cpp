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
    int num_u = ceil(1.0 / u_step);
    int num_v = ceil(1.0 / v_step);

    for (int i_u = 0; i_u < num_u; i_u++)
    {
        for (int i_v = 0; i_v < num_v; i_v++)
        {
            float u = u_step * i_u;
            float v = v_step * i_v * M_PI * 2;

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
            {
                result->push_back(quad[i]);
                //qDebug() << i_v + (i_u * num_u) << ": " << *(quad[i]);
            }
        }
    }

    return (result);
}
vector<QVector3D*> *RevSurface::getControlMesh(float u_step, float v_step)
{
    int num_u_vert_rows = ceil(1.0 / u_step) + 1;
    int num_v_faces = ceil(1.0 / v_step);

    vector<QVector3D*> *controlMesh = new vector<QVector3D*>;

    for (int i_u = 0; i_u < num_u_vert_rows; i_u++)
    {
        for (int i_v = 0; i_v < num_v_faces; i_v++)
        {
            float u = u_step * i_u;
            float v = v_step * i_v * 360;// * M_PI * 2;

            QMatrix4x4 r;
            r.rotate(-v, 0, 1, 0);

            QVector3D *u_pt1 = new QVector3D(*(curve->evalPoint(u)));
            QVector3D *outpt = new QVector3D(r * (*u_pt1));

            controlMesh->push_back(outpt);
        }
    }
    return controlMesh;
}

ObjModel *RevSurface::getObjModel(float u_step, float v_step)
{
    ObjModel *result = new ObjModel();

    int num_u_faces = ceil(1.0 / u_step);
    int num_u_verts = num_u_faces + 1;
    int num_v_faces = ceil(1.0 / v_step);
    int num_v_verts = num_v_faces;

    result->num_tris = num_u_faces * num_v_faces * 2;
    result->num_xyz = (num_u_faces + 1) * num_v_faces;

    vector<QVector3D*> &m_controlMesh = *getControlMesh(u_step, v_step);

    for (uint i = 0; i < m_controlMesh.size(); i++ )
    {
        QVector3D &p = *m_controlMesh[i];

        //qDebug() << i << ": "<< p;
        result->m_vertices.push_back(vec3(p[0], p[1], p[2]));
    }

    for (int u = 0; u < num_u_verts; u++ )
    {
        for (int v = 0; v < num_v_verts; v++)
        {
            result->texs.push_back(
                        vec2((float)u / num_u_faces,
                             (float)v / num_v_faces));
        }
    }

    for (int i_u = 0; i_u < num_u_faces; i_u++)
    {
        for (int i_v = 0; i_v < num_v_faces; i_v++)
        {
            GLuint a,b,c,d;

            int next_uIdx = i_u + 1;// % num_u;
            int next_vIdx = (i_v + 1) % num_v_faces;

            a = i_u * num_v_faces + i_v;
            b = i_u * num_v_faces + next_vIdx;
            c = (next_uIdx) * num_v_faces + next_vIdx;
            d = (next_uIdx) * num_v_faces + i_v;

            tri t1;
            tri t2;

            t1.index_xyz[0] = a;
            t1.index_xyz[1] = d;
            t1.index_xyz[2] = b;

            t1.index_uv[0] = a;
            t1.index_uv[1] = d;
            t1.index_uv[2] = b;

            t2.index_xyz[0] = d;
            t2.index_xyz[1] = c;
            t2.index_xyz[2] = b;

            t2.index_uv[0] = d;
            t2.index_uv[1] = c;
            t2.index_uv[2] = b;

            result->tris.push_back(t1);
            result->tris.push_back(t2);

            /*
            qDebug() << "Num verts: " << m_controlMesh.size();
            for (int z = 0; z < 3; z++)
            {
                QVector3D &v = *m_controlMesh[t1.index_xyz[z]];
                qDebug() << t1.index_xyz[z];
                qDebug() << i_v + (i_u * num_v) << ": " << v[0] << ", " << v[1] << ", " << v[2] ;
            }
            for (int z = 0; z < 3; z++)
            {
                QVector3D &v = *m_controlMesh[t2.index_xyz[z]];
                qDebug() << i_v + (i_u * num_v) << ": " << v[0] << ", " << v[1] << ", " << v[2] ;
            }
            */
        }
    }
    return result;
}
