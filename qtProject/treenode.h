#ifndef TREENODE_H
#define TREENODE_H

#include <iostream>
#include <cmath>
#include <vector>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>

class Renderer;

class TreeNode
{
public:
    TreeNode(int type, QVector4D position, TreeNode *mPrevious, QMatrix4x4 *t);
    int getType();
    QVector4D getGrowthDirection();
    QVector4D getPosition();
    TreeNode * getPreviousNode();
    void setDrawn(bool drawn);
    bool hasBeenDrawn();

private: //members
    int mType;
    QVector4D mPosition;
    QMatrix4x4 *mTransform;
    TreeNode * mPreviousNode;
    bool mDrawn;
};

class Tree
{
public:
    Tree(float height, float crownRadius, float trunkRadius);

    bool growTree(int nodeDepth, TreeNode * previousNode);
    void draw(Renderer *r);

    std::vector<TreeNode *> getTreeNodes(){return mTreeNodes;}
    std::vector<TreeNode *> getLeafNodes(){return mLeafNodes;}
    float getHeight(){return mHeight;}
    float getCrownRadius(){return mCrownRadius;}
    float getTrunkRadius(){return mTrunkRadius;}

private: // members
    std::vector<TreeNode *> mTreeNodes;
    std::vector<TreeNode *> mLeafNodes;
    float mHeight;
    float mCrownRadius;
    float mTrunkRadius;
};


#endif // TREENODE_H
