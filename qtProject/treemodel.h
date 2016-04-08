#ifndef TREEGEN_H
#define TREEGEN_H

#include <QMatrix4x4>
#include "objModel.h"
#include "revsurface.h"

class Node
{
public:
    QMatrix4x4 transform;
};

class Trunk : public Node
{
public:
    Trunk(int level, float radius, QMatrix4x4 trans);
    Node *next;
    vector<Node*> branches;
};

class Branch : public Node
{
public:
    Branch(float length, QMatrix4x4 trans);
    Node *middle;
    Node *left;
    Node *right;
};

class Leaf : public Node
{
public:
    Leaf();
};

class TreeModel
{
public:
    TreeModel(QVector3D pos);
    ObjModel *getObjModel(float u_step, float v_step);
    Trunk *trunk;
};

static Node *makeBranch(float length, QMatrix4x4 trans);

#endif // TREEGEN_H
