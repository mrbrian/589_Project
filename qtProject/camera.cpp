#include "camera.h"

Camera::Camera(){
    _position = QVector3D(0,0,0);
    _rotation = QVector3D(0,0,0);
    _target = QVector3D(0,0,0);
    _up = QVector3D(0,1,0);

    updateDirections();
}

Camera::Camera(QVector3D pos, QVector3D rot, QVector3D targ, QVector3D u){
    _position = pos;
    _rotation = rot;
    _target = targ;
    _up = u;

    updateDirections();
}

//getters
QVector3D Camera::getPosition(){
    return(_position);
    // update transform
}

QVector3D Camera::getRotation(){
    return(_rotation);
}

QVector3D Camera::getTarget(){
    return(_target);
}

QVector3D Camera::getForward(){
    return _forward;
}

QVector3D Camera::getRight(){
    return _right;
}

QVector3D Camera::getUp(){
    return(_up);
}

float Camera::getFov(){
    return(_fov);
}

float Camera::getNearClip(){
    return (_near);
}

float Camera::getFarClip(){
    return (_far);
}

void Camera::updateDirections()
{
    _forward = (_target - _position);
    _forward.normalize();

    QMatrix4x4 rot_y;
    rot_y.rotate(90, 0.0, 1.0, 0.0);    
    _right = rot_y * _forward;
    _right[1] = 0;
    _right.normalize();

    QMatrix4x4 rot_x;
    rot_x.rotate(-90, _right);
    _up = rot_x * _forward;
    _up.normalize();
}

// setters
void Camera::setPosition(QVector3D p){
    _position = p;
    updateDirections();
}

void Camera::setRotation(QVector3D r){
    _rotation = r;
}

void Camera::setTarget(QVector3D t){
    _target = t;
    updateDirections();
}

void Camera::setForward(QVector3D u){
    _forward = u;
}

void Camera::setRight(QVector3D u){
    _right = u;
}

void Camera::setUp(QVector3D u){
    _up = u;
}

void Camera::setFov(float v){
    _fov = v;
}

void Camera::setNearClip(float v){
    _near = v;
}

void Camera::setFarClip(float v){
    _far = v;
}

QMatrix4x4 *Camera::getTransform(){
    return &_transform;
}
