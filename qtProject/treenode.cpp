#include "treenode.h"
#include "renderer.h"

#define PI 3.14159265
#define WINDOW_START_WIDTH 600
#define WINDOW_START_HEIGHT 600
#define POINT_ADDITION_WAIT .25

#define NUM_TRUNK_NODES 12
#define NUM_PRIMARY_BRANCH_NODES 10
#define NUM_SECONDARY_BRANCH_NODES 2

//(QMatrix4x4 *t)

// i need the local transform as an argument.
TreeNode::TreeNode(int type, QVector4D position, TreeNode *mPrevious, QMatrix4x4 *t)
{
    //std::cout << "made new node at : " << position.x() << "," << position.y() << "," << position.z() << std::endl;
    mType = type;
    mPosition = position;
    mPreviousNode = mPrevious;
    mDrawn = false;
    mTransform = t;
    //mTransform->translate(position);
}

int TreeNode::getType() {return mType;}
QVector4D TreeNode::getGrowthDirection()
{
    if (mPreviousNode != NULL)
    {
        //return (glm::normalize(mPosition - mPreviousNode->getPosition()));
        return ((mPosition - mPreviousNode->getPosition()).normalized());
    }

    return QVector4D(0,1,0,0);
}
QVector4D TreeNode::getPosition() {return mPosition;}
TreeNode * TreeNode::getPreviousNode() {return mPreviousNode;}
void TreeNode::setDrawn(bool drawn) {mDrawn = drawn;}
bool TreeNode::hasBeenDrawn(){return mDrawn;}

#define NUM_TRUNK_NODES 12
#define NUM_PRIMARY_BRANCH_NODES 10
#define NUM_SECONDARY_BRANCH_NODES 2

Tree::Tree(float height, float crownRadius, float trunkRadius) : mHeight(height), mCrownRadius(crownRadius), mTrunkRadius(trunkRadius)
{
    //start the tree growing at the trunk
    QMatrix4x4 *t = new QMatrix4x4();
    growTree(NUM_TRUNK_NODES - 1, new TreeNode(0, QVector4D(0,0,0,1), NULL, t));
}

bool Tree::growTree(int nodeDepth, TreeNode * previousNode)
{
    if (nodeDepth == 0)
        return false;

    switch(previousNode->getType())
    {
        case(0):
        {
            TreeNode * nextNode;

            //make 5-6 '1' nodes rotated around y axis
            #define NUM_PRIMARY_TO_TRUNK 6 // potential to be randomized/varied
            for (int i = 0; i < NUM_PRIMARY_TO_TRUNK; i++)
            {
                QMatrix4x4 *t;
                QMatrix4x4 &t1 = *t;

                //trunk growth will always be up right now
                QVector4D growthDirection(0,1,0,0);
                float rotationAngle;
                //rotate to the horizontal plane
                rotationAngle = 80.0f; //potential to be randomized. (85-75 degrees)
                t1.rotate(rotationAngle, 1, 0, 0);
                growthDirection = t1 * growthDirection;
                //rotate around the y axis, staggered at each new level
                QMatrix4x4 t2;
                rotationAngle = (360.0f / NUM_PRIMARY_TO_TRUNK) * (float)i + nodeDepth * 5;
                t2.rotate(rotationAngle, 0, 1, 0);

                growthDirection = t2 * growthDirection;

                //scale the new branch length
                growthDirection *= (mCrownRadius / NUM_PRIMARY_BRANCH_NODES);

                QMatrix4x4 t3;
                QVector4D pos = growthDirection + previousNode->getPosition();

                t3.translate(pos[0], pos[1], pos[2]);

                t1 = t3 * t2 * t1;

                //create new primary branch
                nextNode = new TreeNode(1, growthDirection + previousNode->getPosition(), previousNode, t);
                mTreeNodes.push_back(nextNode);
                //number of nodes along primary branch should decrease as it gets higher up in the tree
                int depth = floor((float)(nodeDepth) / NUM_TRUNK_NODES * NUM_PRIMARY_BRANCH_NODES);
                growTree(depth, nextNode);

            }

            QMatrix4x4 *trans = new QMatrix4x4();
            QMatrix4x4 &trans1 = *trans;

            QVector4D pos = previousNode->getGrowthDirection() * (mHeight / NUM_TRUNK_NODES) + previousNode->getPosition();
            trans1.translate(pos[0], pos[1], pos[2]);

            //continue trunk growth upwards
            nextNode = new TreeNode(0, previousNode->getGrowthDirection() * (mHeight / NUM_TRUNK_NODES) + previousNode->getPosition(), previousNode, trans);
            mTreeNodes.push_back(nextNode);
            growTree(nodeDepth-1, nextNode);
            break;
        }
        case(1):
        {
            QMatrix4x4 *_t1 = new QMatrix4x4();
            QMatrix4x4 &t1 = *_t1;

            //make 2 '2' nodes rotated (35 / -35) degrees about y axis. can be randomized slightly
            TreeNode * nextNode;
            //35 degree branch
            //glm::vec4 growthDirection = previousNode->getGrowthDirection();
            QVector4D growthDirection = previousNode->getGrowthDirection();
            //float rotationAngle = 35.0f / 180.0f * 3.14;
            float rotationAngle = 35.0f;
            t1.rotate(rotationAngle, 0, 1, 0);
            //glm::vec4 newDirection = glm::rotateY(growthDirection, rotationAngle);
            QVector4D newDirection = t1 * growthDirection;

            QMatrix4x4 t2;
            t2.translate();
            t1 = t2 * t1
            //scale the branch length based off its distance from trunk 'age of branch'
            newDirection *= (mHeight * 2 / NUM_PRIMARY_BRANCH_NODES / (NUM_PRIMARY_BRANCH_NODES / nodeDepth));
            //tilt the branch upwards slightly
            newDirection[1] += 0.004;
            nextNode = new TreeNode(2, newDirection + previousNode->getPosition(), previousNode, _t1);
            mTreeNodes.push_back(nextNode);
            //start point for drawing
            mLeafNodes.push_back(nextNode);
            growTree(NUM_SECONDARY_BRANCH_NODES, nextNode);
            //-35 degree branch
            QMatrix4x4 *_t2 = new QMatrix4x4();
            QMatrix4x4 &t2 = *_t2;
            t2.rotate(-rotationAngle,0,1,0);
            newDirection = t2 * growthDirection;    //glm::rotateY(growthDirection, -rotationAngle);
            newDirection *= (mHeight * 2 / NUM_PRIMARY_BRANCH_NODES / (NUM_PRIMARY_BRANCH_NODES / nodeDepth));
            newDirection[1] += 0.004;
            nextNode = new TreeNode(2, newDirection + previousNode->getPosition(), previousNode, _t2);
            mTreeNodes.push_back(nextNode);
            mLeafNodes.push_back(nextNode);
            growTree(NUM_SECONDARY_BRANCH_NODES, nextNode);

            //if its the end of the branch add a '2' node along growth direction
            if (nodeDepth == 1)
            {

                nextNode = new TreeNode(2, growthDirection * (mHeight * 2 / NUM_PRIMARY_BRANCH_NODES / (NUM_PRIMARY_BRANCH_NODES / nodeDepth)) + previousNode->getPosition(), previousNode, _t1);
                mTreeNodes.push_back(nextNode);
                mLeafNodes.push_back(nextNode);
                growTree(NUM_SECONDARY_BRANCH_NODES, nextNode);
            }
            //make a new 1 node along growth direction
            growthDirection *= (mCrownRadius / NUM_PRIMARY_BRANCH_NODES);
            growthDirection[1] += 0.0005 * (NUM_PRIMARY_BRANCH_NODES - nodeDepth);
            QMatrix4x4 *ugh = new QMatrix4x4();
            nextNode = new TreeNode(1, growthDirection + previousNode->getPosition(), previousNode, ugh);
            mTreeNodes.push_back(nextNode);
            //mLeafNodes.push_back(nextNode);
            growTree(nodeDepth-1, nextNode);
            break;
        }
        case(2):
        {
            //if node depth == 0 make 1 '3' node along growth direction
            //make 2 '3' nodes rotated along y axis
            break;
        }
    }
    return true;
}
