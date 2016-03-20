#include "tests.h"
#include "bspline.h"
#include <stdio.h>
#include <iostream>
#include "revsurface.h"
#include <QDebug>

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

Tests::Tests()
{
    RevSurface_1();
    RevSurface_Quads1();
    RevSurface_Quads2();
}
