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

    QVector3D expected = QVector3D(-1, 1, 0);
    QVector3D *actual = rs.eval(0.5, 0.5);

    if (expected != *actual)
        cout << "RevSurface_1 fail" << endl;
    else
        cout << "RevSurface_1 pass" << endl;
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
    for (int i = 0 ; i < actual->size(); i++)
    {
        QVector3D &a = *(expected[i]);
        QVector3D &b = *(*actual)[i];
        for (int j = 0 ; j < 3; j++)
        {
            if ((int)a[j] != (int)b[j])
               pass = false;
        }
    }

    if (!pass)
        cout << "RevSurface_Quad1 fail" << endl;
    else
        cout << "RevSurface_Quad1 pass" << endl;
}

void RevSurface_Quad2()
{
    vector<QVector2D*> pts;
    pts.push_back(new QVector2D(1, 0));
    pts.push_back(new QVector2D(1, 1));
    pts.push_back(new QVector2D(1, 2));

    BSpline c = BSpline(2, &pts);  // m, pts, order, //..  standard knots

    RevSurface rs = RevSurface(&c);
    float ustep = 1;
    float vstep = 0.125;

    int expected = 8 * 4; // 8 faces , 4 verts

    vector<QVector3D*> *actual = rs.evalQuads(ustep, vstep);

    bool pass = true;
    if (actual->size() != expected)
        pass = false;
    if (!pass)
        cout << "RevSurface_Quad2 fail" << endl;
    else
        cout << "RevSurface_Quad2 pass" << endl;
}

Tests::Tests()
{
    RevSurface_1();
    //RevSurface_Quad1();
    RevSurface_Quad2();
}
