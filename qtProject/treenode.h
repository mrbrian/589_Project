#ifndef TREENODE_H
#define TREENODE_H

#include <iostream>
#include <cmath>
#include <vector>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>
#include "objModel.h"

class Renderer;

class TreeNode
{
public:
    TreeNode(int type, QVector4D position, TreeNode *mPrevious, float radius = 0);
    int getType();
    QVector4D getGrowthDirection();
    QVector4D getPosition();
    TreeNode * getPreviousNode();
    void setDrawn(bool drawn);
    bool hasBeenDrawn();
    void setRadius(float radius);
    float getRadius() { return mRadius;}

private: //members
    float mRadius;
    int mType;
    QVector4D mPosition;
    TreeNode * mPreviousNode;
    bool mDrawn;
};

class Tree
{
public:
    Tree(float height, float crownRadius, float trunkRadius);

    ObjModel *getObjModel(float u_step, float v_step, float radius);
    bool growTree(int nodeDepth, TreeNode * previousNode);
    void draw(Renderer *r);

    std::vector<TreeNode *> getTrunkNodes(){return mTrunkNodes;}
    std::vector<TreeNode *> getPBranchNodes(){return mPBranchNodes;}
    std::vector<TreeNode *> getLeafNodes(){return mLeafNodes;}
    float getHeight(){return mHeight;}
    float getCrownRadius(){return mCrownRadius;}
    float getTrunkRadius(){return mTrunkRadius;}

private: // members
    std::vector<TreeNode *> mTrunkNodes;
    std::vector<TreeNode *> mPBranchNodes;
    std::vector<TreeNode *> mLeafNodes;
    float mHeight;
    float mCrownRadius;
    float mTrunkRadius;
};


#endif // TREENODE_H
