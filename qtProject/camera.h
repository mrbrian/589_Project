#ifndef CAMERA_H
#define CAMERA_H

#include "QVector3D"
#include "QMatrix4x4"

class Camera
{
public:
    Camera();
    Camera(QVector3D pos, QVector3D rot, QVector3D targ, QVector3D u);

    QVector3D getPosition();
    QVector3D getRotation();
    QVector3D getTarget();

    QVector3D getForward();
    QVector3D getRight();
    QVector3D getUp();

    float getFov();
    float getNearClip();
    float getFarClip();

    void setPosition(QVector3D p);
    void setRotation(QVector3D p);
    void setTarget(QVector3D p);
    void setForward(QVector3D p);
    void setRight(QVector3D p);
    void setUp(QVector3D p);
    void setFov(float v);
    void setNearClip(float v);
    void setFarClip(float v);

    QMatrix4x4 *getTransform();

private:
    QVector3D _forward;
    QVector3D _right;
    QVector3D _up;

    QVector3D _position;
    QVector3D _rotation;
    QVector3D _target;   //lookat
    float _fov;
    float _near;
    float _far;
    QMatrix4x4 _transform;

    void updateDirections();
};

#endif // CAMERA_H
