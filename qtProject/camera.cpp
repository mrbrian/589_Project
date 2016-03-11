#include "camera.h"

Camera::Camera(){

}

Camera::Camera(QVector3D pos, QVector3D rot, QVector3D targ, QVector3D u){
    position = pos;
    rotation = rot;
    target = targ;
    up = u;
}

//getters
QVector3D Camera::getPosition(){
    return(position);
    // update transform
}

QVector3D Camera::getRotation(){
    return(rotation);
}

QVector3D Camera::getTarget(){
    return(target);
}

QVector3D Camera::getUp(){
    return(up);
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

// setters
void Camera::setPosition(QVector3D p){
    position = p;
    // update transform
}

void Camera::setRotation(QVector3D r){
    rotation = r;
}

void Camera::setTarget(QVector3D t){
    target = t;
}

void Camera::setUp(QVector3D u){
    up = u;
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
    return &transform;
}
