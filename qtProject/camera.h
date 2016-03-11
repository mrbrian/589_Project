#ifndef CAMERA_H
#define CAMERA_H

#include "QVector3D"
#include "QMatrix4x4"

class Camera
{
public:
    Camera();   
    QVector3D position;
    QVector3D rotation;
    QVector3D target;
    QVector3D up;
    float _fov;
    float _near;
    float _far;
    QMatrix4x4 transform;
};

#endif // CAMERA_H
