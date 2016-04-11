#include "tests.h"
#include "bspline.h"
#include <stdio.h>
#include <iostream>
#include "revsurface.h"
#include <QDebug>
#include "model.h"
#include "bspline_blended.h"

using namespace std;

void RevSurface_1()
{
    // spin it about the y axis  origin at 0,0,0

    vector<QVector2D*> pts;
    pts.push_back(new QVector2D(1, 0));
    pts.push_back(new QVector2D(1, 1));
    pts.push_back(new QVector2D(1, 2));

    BSpline c = BSpline(2, &pts);  // m, pts, order, //..  standard knots

    RevSurface rs = RevSurface(&c);

    QVector3D expected = QVector3D(-1, 1, 0);
    QVector3D *actual = rs.eval(0.5, 0.5);

    if (expected != *actual)
        qDebug() << "RevSurface_1 fail";
    else
        qDebug() << "RevSurface_1 pass";
}

// tesselation test
void RevSurface_Quads1()
{
    vector<QVector2D*> pts;
    pts.push_back(new QVector2D(1, 0));
    pts.push_back(new QVector2D(1, 1));
    pts.push_back(new QVector2D(1, 2));

    BSpline c = BSpline(2, &pts);  // m, pts, order, //..  standard knots

    RevSurface rs = RevSurface(&c);
    float ustep = 0.5;
    float vstep = 0.25;

    vector<QVector3D*> expected;
    expected.push_back(new QVector3D(1, 0, 0));
    expected.push_back(new QVector3D(0, 0, 1));
    expected.push_back(new QVector3D(0, 1, 1));
    expected.push_back(new QVector3D(1, 1, 0));

    expected.push_back(new QVector3D(0, 0, 1));
    expected.push_back(new QVector3D(-1, 0, 0));
    expected.push_back(new QVector3D(-1, 1, 0));
    expected.push_back(new QVector3D(0, 1, 1));

    expected.push_back(new QVector3D(-1, 0, 0));
    expected.push_back(new QVector3D(0, 0, -1));
    expected.push_back(new QVector3D(0, 1, -1));
    expected.push_back(new QVector3D(-1, 1, 0));

    expected.push_back(new QVector3D(0, 0, -1));
    expected.push_back(new QVector3D(1, 0, 0));
    expected.push_back(new QVector3D(1, 1, 0));
    expected.push_back(new QVector3D(0, 1, -1));

    expected.push_back(new QVector3D(1, 1, 0));
    expected.push_back(new QVector3D(0, 1, 1));
    expected.push_back(new QVector3D(0, 2, 1));
    expected.push_back(new QVector3D(1, 2, 0));

    expected.push_back(new QVector3D(0, 1, 1));
    expected.push_back(new QVector3D(-1, 1, 0));
    expected.push_back(new QVector3D(-1, 2, 0));
    expected.push_back(new QVector3D(0, 2, 1));

    expected.push_back(new QVector3D(-1, 1, 0));
    expected.push_back(new QVector3D(0, 1, -1));
    expected.push_back(new QVector3D(0, 2, -1));
    expected.push_back(new QVector3D(-1, 2, 0));

    expected.push_back(new QVector3D(0, 1, -1));
    expected.push_back(new QVector3D(1, 1, 0));
    expected.push_back(new QVector3D(1, 2, 0));
    expected.push_back(new QVector3D(0, 2, -1));

    vector<QVector3D*> *actual = rs.evalQuads(ustep, vstep);

    bool pass = true;
    for (uint i = 0 ; i < expected.size(); i++)
    {
        QVector3D &a = *(expected)[i];
        QVector3D &b = *(*actual)[i];
        for (int j = 0 ; j < 3; j++)
        {
            if ((int)a[j] != (int)b[j])
               pass = false;
        }
    }

    if (!pass)
        qDebug() << "RevSurface_Quad1 fail";
    else
        qDebug() << "RevSurface_Quad1 pass";
}

// tesselation test
void RevSurface_Quads2()
{
    vector<QVector2D*> pts;
    pts.push_back(new QVector2D(1, 0));
    pts.push_back(new QVector2D(1, 1));
    pts.push_back(new QVector2D(1, 2));

    BSpline c = BSpline(2, &pts);  // m, pts, order, //..  standard knots

    RevSurface rs = RevSurface(&c);
    float ustep = 1;
    float vstep = 0.25;

    vector<QVector3D*> expected;
    QVector3D p1 = QVector3D(1, 0, 0);
    QVector3D p2 = QVector3D(0, 0, 1);
    QVector3D p3 = QVector3D(0, 2, 1);
    QVector3D p4 = QVector3D(1, 2, 0);

    QMatrix4x4 r;
    r.rotate(-vstep * 360, 0, 1, 0);

    for (int i = 0 ; i < 4; i++)
    {
        expected.push_back(new QVector3D(p1));
        expected.push_back(new QVector3D(p2));
        expected.push_back(new QVector3D(p3));
        expected.push_back(new QVector3D(p4));

        p1 = r * p1;
        p2 = r * p2;
        p3 = r * p3;
        p4 = r * p4;
    }
    vector<QVector3D*> *actual = rs.evalQuads(ustep, vstep);

    bool pass = true;
    for (uint i = 0 ; i < expected.size(); i++)
    {
        QVector3D &a = *(expected[i]);
        QVector3D &b = *(*actual)[i];
        for (int j = 0 ; j < 3; j++)
        {
            if (abs(a[j] - b[j]) > 0.01)
               pass = false;
        }
    }

    if (!pass)
        qDebug() << "RevSurface_Quad2 fail";
    else
        qDebug() << "RevSurface_Quad2 pass";
}


void RevSurface_ObjTest1()
{
    bool pass = true;

    vector<QVector2D*> *pts = new vector<QVector2D*>();

    pts->push_back(new QVector2D(1, 0));
    pts->push_back(new QVector2D(1, 1));

    vector<QVector3D*> *expected = new vector<QVector3D*>();

    //qDebug() << "expected";
    for (int i =0 ; i < 4; i++)
    {
        QMatrix4x4 r;
        r.rotate(-360 / 4 * i, 0, 1, 0);

        QVector3D a = r * QVector3D(1, 0, 0);
        QVector3D b = r * QVector3D(0, 0, 1);
        QVector3D c = r * QVector3D(0, 1, 1);
        QVector3D d = r * QVector3D(1, 1, 0);

        expected->push_back(new QVector3D(a));
        expected->push_back(new QVector3D(d));
        expected->push_back(new QVector3D(b));

        expected->push_back(new QVector3D(d));
        expected->push_back(new QVector3D(c));
        expected->push_back(new QVector3D(b));
/*
        qDebug() << a;
        qDebug() << d;
        qDebug() << b;

        qDebug() << d;
        qDebug() << c;
        qDebug() << b;*/
    }
    //qDebug() << "revsurface";

    BSpline *bs = new BSpline(2, pts);
    RevSurface *rs = new RevSurface(bs);

    //qDebug() << "OBJmodel";
    ObjModel *obj = rs->getObjModel(1, 0.25);

   // qDebug() << "model";
    Model *m_model = new Model(obj, NULL);

    for (int i = 0 ; i < 2 * 4; i++)
    {
        QVector3D e = *((*expected)[i]);
        QVector3D a = QVector3D(m_model->verts[i*3], m_model->verts[(i*3)+1], m_model->verts[(i*3)+2]);

        for (int j = 0 ; j < 3; j++)
        {
            if (e[j] != a[j])
                pass = false;
        }
    }

    if (!pass)
    {
        qDebug() << "RevSurface_ObjTest1 fail";
        throw std::invalid_argument( "RevSurface_ObjTest1 fail" );
    }
    else
        qDebug() << "RevSurface_ObjTest1 pass";
}

void RevSurface_ObjTest2()
{
    bool pass = true;

    vector<QVector2D*> *pts = new vector<QVector2D*>();

    pts->push_back(new QVector2D(1, 0));
    pts->push_back(new QVector2D(1, 1));

    vector<QVector3D*> *expected = new vector<QVector3D*>();

    //qDebug() << "expected";
    for (int i =0 ; i < 4; i++)
    {
        QMatrix4x4 r;
        r.rotate(-360 / 4 * i, 0, 1, 0);

        QVector3D a = r * QVector3D(1, 0, 0);
        QVector3D b = r * QVector3D(0, 0, 1);
        QVector3D c = r * QVector3D(0, 1, 1);
        QVector3D d = r * QVector3D(1, 1, 0);

        expected->push_back(new QVector3D(a));
        expected->push_back(new QVector3D(d));
        expected->push_back(new QVector3D(b));

        expected->push_back(new QVector3D(d));
        expected->push_back(new QVector3D(c));
        expected->push_back(new QVector3D(b));
/*
        qDebug() << a;
        qDebug() << d;
        qDebug() << b;

        qDebug() << d;
        qDebug() << c;
        qDebug() << b;*/
    }
    //qDebug() << "revsurface";

    BSpline *bs = new BSpline(2, pts);
    RevSurface *rs = new RevSurface(bs);

    //qDebug() << "OBJmodel";
    ObjModel *obj = rs->getObjModel(0.05, 0.05);

   // qDebug() << "model";
    Model *m_model = new Model(obj, NULL);

    for (int i = 0 ; i < 2 * 4; i++)
    {
        QVector3D e = *((*expected)[i]);
        QVector3D a = QVector3D(m_model->verts[i*3], m_model->verts[(i*3)+1], m_model->verts[(i*3)+2]);

        for (int j = 0 ; j < 3; j++)
        {
            if (e[j] != a[j])
                pass = false;
        }
    }

    if (!pass)
    {
        qDebug() << "RevSurface_ObjTest1 fail";        
        throw std::invalid_argument( "RevSurface_ObjTest1 fail" );
    }
    else
        qDebug() << "RevSurface_ObjTest1 pass";
}

void BSplineBlend_Test1()
{
    vector<BSpline*> *splines = new vector<BSpline*>();

    vector<QVector2D*> *pts_1 = new vector<QVector2D*>();
    pts_1->push_back(new QVector2D(0,0));
    pts_1->push_back(new QVector2D(0,1));

    vector<QVector2D*> *pts_2 = new vector<QVector2D*>();
    pts_2->push_back(new QVector2D(0,0));
    pts_2->push_back(new QVector2D(1,0));

    BSpline *b_1 = new BSpline(2, pts_1);
    BSpline *b_2 = new BSpline(2, pts_2);

    splines->push_back(b_1);
    splines->push_back(b_2);

    BSpline_Blended *bsb = new BSpline_Blended(0.5, 2, splines);

    QVector2D *expected = new QVector2D(0.5, 0.5);
    QVector2D *actual = bsb->evalPoint(0.5, 1);

    if ((*expected)[0] != (*actual)[1] ||
        (*expected)[1] != (*actual)[1])
        qDebug() << "BSplineBlend_Test1 fail";
    else
        qDebug() << "BSplineBlend_Test1 pass";

}

void BSplineBlend_Test2()
{
    vector<BSpline*> *splines = new vector<BSpline*>();

    vector<QVector2D*> *pts_1 = new vector<QVector2D*>();
    pts_1->push_back(new QVector2D(0,1));
    pts_1->push_back(new QVector2D(0,0));

    vector<QVector2D*> *pts_2 = new vector<QVector2D*>();
    pts_2->push_back(new QVector2D(0,0));
    pts_2->push_back(new QVector2D(1,0));

    BSpline *b_1 = new BSpline(2, pts_1);
    BSpline *b_2 = new BSpline(2, pts_2);

    splines->push_back(b_1);
    splines->push_back(b_2);

    BSpline_Blended *bsb = new BSpline_Blended(0.5, 2, splines);

    QVector2D *expected = new QVector2D(0, 0.5);
    QVector2D *actual = bsb->evalPoint(0.5, 0);

    if (*expected != *actual)
        qDebug() << "BSplineBlend_Test2 fail";
    else
        qDebug() << "BSplineBlend_Test2 pass";
}

void BSplineBlend_Test3()
{
    vector<BSpline*> *splines = new vector<BSpline*>();

    vector<QVector2D*> *pts_1 = new vector<QVector2D*>();
    pts_1->push_back(new QVector2D(0,0));
    pts_1->push_back(new QVector2D(0,1));

    vector<QVector2D*> *pts_2 = new vector<QVector2D*>();
    pts_2->push_back(new QVector2D(0,0));
    pts_2->push_back(new QVector2D(1,1));

    vector<QVector2D*> *pts_3 = new vector<QVector2D*>();
    pts_3->push_back(new QVector2D(0,0));
    pts_3->push_back(new QVector2D(1,0));

    BSpline *b_1 = new BSpline(2, pts_1);
    BSpline *b_2 = new BSpline(2, pts_2);
    BSpline *b_3 = new BSpline(2, pts_3);

    splines->push_back(b_1);
    splines->push_back(b_2);
    splines->push_back(b_3);

    // 0    0.5     0.5     0       0.125   .25
    // 0.5  0       0.5     0.125   0       .25
    BSpline_Blended *bsb = new BSpline_Blended(0.5, 3, splines);

    QVector2D *expected = new QVector2D(0.375, 0.375);
    QVector2D *actual = bsb->evalPoint(0.5, 0.5);

    if (*expected != *actual)
        qDebug() << "BSplineBlend_Test3 fail";
    else
        qDebug() << "BSplineBlend_Test3 pass";
}


void RevSurf_1()
{
    RevSurface *r = RevSurface::makeRevSurface(0, 1, 1, QVector3D(0,1,0), QVector3D(0,0,0));

    QVector3D expected = QVector3D(0.6, 0, 0);
    QVector3D *actual = r->eval(0,0);

    if (expected[0] != (*actual)[0] ||
        expected[1] != (*actual)[1] ||
        expected[2] != (*actual)[2])
        qDebug() << "RevSurf_1 fail";
    else
        qDebug() << "RevSurf_1 pass";
}

void RevSurf_2()
{
    RevSurface *r = RevSurface::makeRevSurface(0, 1, 1, QVector3D(0,1,0), QVector3D(1,1,1));

    QVector3D expected = QVector3D(0.6, 0, 0);
    QVector3D *actual = r->eval(0,0);

    if (expected[0] != (*actual)[0] ||
        expected[1] != (*actual)[1] ||
        expected[2] != (*actual)[2])
        qDebug() << "RevSurf_1 fail";
    else
        qDebug() << "RevSurf_1 pass";
}

void getY()
{
    vector<QVector3D> pts;

    pts.push_back(QVector3D(0,1,0)); // a
    pts.push_back(QVector3D(1,2,0)); // b
    pts.push_back(QVector3D(0,3,1)); // c
    pts.push_back(QVector3D(1,4,1)); // d

    float expected = 2.5;
    float actual = Terrain::get_y_height(0.5, 0.5, 1, 1, pts);

    if (expected != actual)
        qDebug() << "getY fail";
    else
        qDebug() << "getY pass";
}

void getY2()
{
    vector<QVector3D> pts;

    pts.push_back(QVector3D(0,1,0)); // a
    pts.push_back(QVector3D(1,2,0)); // b
    pts.push_back(QVector3D(0,3,1)); // c
    pts.push_back(QVector3D(1,4,1)); // d

    float expected = 3.25;
    float actual = Terrain::get_y_height(0.75, 0.75, 1, 1, pts);

    if (expected != actual)
        qDebug() << "getY fail";
    else
        qDebug() << "getY pass";
}

void getY3()
{
    vector<QVector3D> pts;

    pts.push_back(QVector3D(0,1,0)); // a
    pts.push_back(QVector3D(1,2,0)); // b
    pts.push_back(QVector3D(2,3,0)); // c
    pts.push_back(QVector3D(3,4,0)); // d

    pts.push_back(QVector3D(0,1,1)); // a
    pts.push_back(QVector3D(1,2,1)); // b
    pts.push_back(QVector3D(2,3,1)); // c
    pts.push_back(QVector3D(1,4,1)); // d

    pts.push_back(QVector3D(0,1,2)); // a
    pts.push_back(QVector3D(1,2,2)); // b
    pts.push_back(QVector3D(2,3,2)); // c
    pts.push_back(QVector3D(3,4,2)); // d

    pts.push_back(QVector3D(0,1,3)); // a
    pts.push_back(QVector3D(1,2,3)); // b
    pts.push_back(QVector3D(2,3,3)); // c
    pts.push_back(QVector3D(3,4,3)); // d

    float expected = 2.75;
    float actual = Terrain::get_y_height(1.75/3, 1.75/3, 3, 3, pts);

    if (expected != actual)
        qDebug() << "getY3 fail";
    else
        qDebug() << "getY3 pass";
}

void getY4()
{
    vector<QVector3D> pts;

    pts.push_back(QVector3D(0,1,0)); // a
    pts.push_back(QVector3D(1,2,0)); // b
    pts.push_back(QVector3D(2,3,0)); // c
    pts.push_back(QVector3D(3,4,0)); // d

    pts.push_back(QVector3D(0,1,1)); // a
    pts.push_back(QVector3D(1,2,1)); // b
    pts.push_back(QVector3D(2,3,1)); // c
    pts.push_back(QVector3D(1,4,1)); // d

    pts.push_back(QVector3D(0,1,2)); // a
    pts.push_back(QVector3D(1,2,2)); // b
    pts.push_back(QVector3D(2,3,2)); // c
    pts.push_back(QVector3D(3,4,2)); // d

    pts.push_back(QVector3D(0,1,3)); // a
    pts.push_back(QVector3D(1,2,3)); // b
    pts.push_back(QVector3D(2,3,3)); // c
    pts.push_back(QVector3D(3,4,3)); // d

    float expected = 2.2;
    float actual = Terrain::get_y_height(0.4, 0.4, 3, 3, pts);

    if (expected != actual)
        qDebug() << "getY4 fail";
    else
        qDebug() << "getY4 pass";
}
/*
void getY5()
{
    QString filename = "../datafiles/example2.jpg";

    QImage *i = new QImage();
    i->load(filename);

    Terrain *terrain = new Terrain(i, 25);

    //ObjModel *obj = terrain->getObjModel();

    float w = terrain->get_meshWidth();
    float h = terrain->get_meshHeight();
    float expected = 0;
    float actual = Terrain::get_y_height(92,204, terrain->get_meshWidth(), terrain->get_meshHeight(), terrain->m_controlMesh);
//    float actual = Terrain::get_y_height(0.18,0.4, terrain->get_meshWidth(), terrain->get_meshHeight(), terrain->m_controlMesh);
    actual = Terrain::get_y_height(0.4,0.18, terrain->get_meshWidth(), terrain->get_meshHeight(), terrain->m_controlMesh);
    actual = Terrain::get_y_height(171,2, terrain->get_meshWidth(), terrain->get_meshHeight(), terrain->m_controlMesh);
    actual = Terrain::get_y_height(286, 138, terrain->get_meshWidth(), terrain->get_meshHeight(), terrain->m_controlMesh);

    if (expected != actual)
        qDebug() << "getY5 fail";
    else
        qDebug() << "getY5 pass";
}
*/

void rotTest()
{
    QVector3D fwd = QVector3D(1,0,0);
    QMatrix4x4 b_trans;
    for (int i = 0 ; i < 4; i++)
    {
        qDebug() << (b_trans * fwd);
        b_trans.rotate(90, 0, 1, 0);
        //b_trans = b_trans * trans;
    }
    //branches.push_back(makeBranch(level - 1, b_trans));

}

void matrix_rotTest()
{
    QVector3D from = QVector3D(0,0,0);
    QVector3D to = QVector3D(0,1,0);

    float r = (to - from).length();
    QVector3D dir = to - from;
    dir.normalize();

    QVector3D up = QVector3D(0,0,1);
    QMatrix4x4 trans;
    trans.lookAt(from, to, up);
    trans = trans.inverted();

    QVector3D fwd = QVector3D(0,1,0);
    QVector3D expected = dir;
    QVector3D actual = trans * fwd;

    if (expected[0] != actual[0] ||
        expected[1] != actual[1] ||
        expected[2] != actual[2])
        qDebug() << "matrix_rotTest fail";
    else
        qDebug() << "matrix_rotTest pass";
}

Tests:: Tests()
{
   // matrix_rotTest();
   // rotTest();
   // treeTest();

    /*getY5();
    getY4();
    getY();
    getY2();
    getY3();
    //RevSurf_2();

    BSplineBlend_Test1();
    BSplineBlend_Test2();
    BSplineBlend_Test3();
    /*
    RevSurface_ObjTest1();
    RevSurface_1();
    RevSurface_Quads1();
    RevSurface_Quads2();*/
}
