#include "tests.h"
#include "bspline.h"
#include <stdio.h>
#include <iostream>
#include "revsurface.h"

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

    QVector3D expected = QVector3D(-1, 1.5, 0);
    QVector3D *actual = rs.eval(0.5, 0.5);

    if (expected != *actual)
        cout << "RevSurface_1 fail\n";
    else
        cout << "RevSurface_1 pass\n";
}

// tesselation test
void RevSurface_Quad1()
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
    expected.push_back(new QVector3D(1, 1, 0));
    expected.push_back(new QVector3D(0, 1, 1));
    expected.push_back(new QVector3D(0, 0, 1));

    vector<QVector3D*> *actual = rs.evalQuads(ustep, vstep);

    bool pass = true;
    for (int i = 0 ; i < actual->size(); i++)
    {
        for (int j = 0 ; j < 3; j++)
        {
            QVector3D &a = *(*actual)[i];
            QVector3D &b = *(*actual)[i];
            if (a[j] != b[j])
               pass = false;
        }
    }

    if (!pass)
        cout << "RevSurface_Quad1 fail\n";
    else
        cout << "RevSurface_Quad1 pass\n";
}

/*
void RevSurface_1()
{
    // spin it about the y axis  origin at 0,0,0

    vector<QVector2D*> pts1;
    pts1.push_back(new QVector2D(0,0));

    vector<QVector2D*> pts2;

    BSpline cu = BSpline(3, &pts1);  // m, pts, order, //..  standard knots
    BSpline cv = BSpline(3, &pts2);

    RevSurface rs = RevSurface(&cu, &cv);

    QVector3D expected = QVector3D(0, 0, 0);
    QVector3D actual = rs.eval(0,0);

    if (expected != actual)
        cout << "RevSurface_1 fail\n";
    cout << "RevSurface_1 pass\n";
}*/

Tests::Tests()
{
    RevSurface_1();
    RevSurface_Quad1();
}
