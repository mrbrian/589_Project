#ifndef RAY_H
#define RAY_H


#ifndef _RAY_H
#define _RAY_H

//#include "vect.h"
#include "camera.h"

class Ray
{
    QVector3D origin, direction;

    public:

    Ray ();
    Ray (Camera);
    Ray (QVector3D, QVector3D);

    // method functions
    QVector3D getRayOrigin () { return origin; }
    QVector3D getRayDirection () { return direction; }


};


#endif


#endif // RAY_H
