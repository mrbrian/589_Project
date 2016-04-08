#include "treemodel.h"

#define MAX_LEVEL 5
#define DEF_RADIUS 1

TreeModel::TreeModel(QVector3D pos)
{
    QMatrix4x4 t;
    t.translate(pos);
    qDebug() << "make tree";
    trunk = new Trunk(MAX_LEVEL, DEF_RADIUS, t);
}

Node *makeTrunk(int level, float radius, QMatrix4x4 trans)
{
    if (level <= 0)
        return new Leaf();
    return new Trunk(level, radius, trans);
}

Trunk::Trunk(int level, float radius, QMatrix4x4 trans)
{
    QVector3D p;
    p = trans * p;
    qDebug() << "new trunk " << level << p;
    transform = trans;

    trans.translate(QVector3D(0,1,0));
    QMatrix4x4 b_trans;

    for (int i = 0; i < 4; i++)
    {
        b_trans.rotate(90 * i, 0, 1, 0);
        b_trans = b_trans * trans;
        branches.push_back(makeBranch(level - 1, b_trans));
    }

    next = makeTrunk(level - 1, radius - 1, trans);
}

Node *makeBranch(float length, QMatrix4x4 trans)
{
    if (length < 1)
        return new Leaf();
    return new Branch(length, trans);
}

Branch::Branch(float length, QMatrix4x4 trans)
{
    QVector3D p;
    p = trans * p;
    qDebug() << "new branch " << length << p;

    transform = trans;

    QVector3D fwd = QVector3D(length,0,0);

    QMatrix4x4 b_trans_m;
    b_trans_m.translate(fwd);
    b_trans_m = b_trans_m * trans;

    middle = makeBranch(length / 2, b_trans_m);

    // go forward a bit .
    // split into three branches
    QMatrix4x4 b_trans_l;

    b_trans_l.rotate(45,0,1,0);
    b_trans_l.translate(fwd);
    b_trans_l = trans * b_trans_l;

    left = makeBranch(length / 2, b_trans_l);

    QMatrix4x4 b_trans_r;
    b_trans_r.rotate(-45,0,1,0);
    b_trans_r.translate(fwd);
    b_trans_r = trans * b_trans_r;

    right = makeBranch(length / 2, b_trans_r);
}

Leaf::Leaf()
{
   // qDebug() << "new leaf";
}
