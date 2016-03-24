#include "ray.h"

Ray::Ray ()
{
    origin = QVector3D(0,0,0);
    direction = QVector3D(1,0,0);
}

Ray::Ray (Camera Cam)
{
    origin = Cam.getPosition();
    direction = Cam.getRotation();
}

Ray::Ray (QVector3D o, QVector3D d)
{
    origin = o;
    direction = d;
}
