#include "treenode.h"
#include "renderer.h"

#define PI 3.14159265
#define WINDOW_START_WIDTH 600
#define WINDOW_START_HEIGHT 600
#define POINT_ADDITION_WAIT .25

#define NUM_TRUNK_NODES 12
#define NUM_PRIMARY_BRANCH_NODES 10
#define NUM_SECONDARY_BRANCH_NODES 2


TreeNode::TreeNode(int type, QVector4D position, TreeNode *mPrevious)
{
    //std::cout << "made new node at : " << position.x() << "," << position.y() << "," << position.z() << std::endl;
    mType = type;
    mPosition = position;
    mPreviousNode = mPrevious;
    mDrawn = false;
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
    growTree(NUM_TRUNK_NODES - 1, new TreeNode(0, QVector4D(0,0,0,1), NULL));
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
                QMatrix4x4 t;

                //trunk growth will always be up right now
                QVector4D growthDirection(0,1,0,0);
                float rotationAngle;
                //rotate to the horizontal plane
                //rotationAngle = (80.0f / 180.0f) * 3.14; //potential to be randomized. (85-75 degrees)
                //growthDirection = glm::rotateX(growthDirection, rotationAngle);
                rotationAngle = 80.0f; //potential to be randomized. (85-75 degrees)
                t.rotate(rotationAngle, 1, 0, 0);
                growthDirection = t * growthDirection;
                //rotate around the y axis, staggered at each new level
                //rotationAngle = (2.0f / NUM_PRIMARY_TO_TRUNK) * 3.14f * (float)i + nodeDepth * 5;
                //growthDirection = glm::rotateY(growthDirection, rotationAngle);
                QMatrix4x4 t2;
                t2.rotate(rotationAngle,0,1,0);
                rotationAngle = (360.0f / NUM_PRIMARY_TO_TRUNK) * (float)i + nodeDepth * 5;
                t2.rotate(rotationAngle, 0, 1, 0);
                growthDirection = t2 * growthDirection;

                //scale the new branch length
                growthDirection *= (mCrownRadius / NUM_PRIMARY_BRANCH_NODES);
                //create new primary branch
                nextNode = new TreeNode(1, growthDirection + previousNode->getPosition(), previousNode);
                mTreeNodes.push_back(nextNode);
                //number of nodes along primary branch should decrease as it gets higher up in the tree
                int depth = floor((float)(nodeDepth) / NUM_TRUNK_NODES * NUM_PRIMARY_BRANCH_NODES);
                growTree(depth, nextNode);

            }

            //continue trunk growth upwards
            nextNode = new TreeNode(0, previousNode->getGrowthDirection() * (mHeight / NUM_TRUNK_NODES) + previousNode->getPosition(), previousNode);
            mTreeNodes.push_back(nextNode);
            growTree(nodeDepth-1, nextNode);
            break;
        }
        case(1):
        {
            QMatrix4x4 t;

            //make 2 '2' nodes rotated (35 / -35) degrees about y axis. can be randomized slightly
            TreeNode * nextNode;
            //35 degree branch
            //glm::vec4 growthDirection = previousNode->getGrowthDirection();
            QVector4D growthDirection = previousNode->getGrowthDirection();
            //float rotationAngle = 35.0f / 180.0f * 3.14;
            float rotationAngle = 35.0f;
            t.rotate(rotationAngle, 0, 1, 0);
            //glm::vec4 newDirection = glm::rotateY(growthDirection, rotationAngle);
            QVector4D newDirection = t * growthDirection;

            //scale the branch length based off its distance from trunk 'age of branch'
            newDirection *= (mHeight * 2 / NUM_PRIMARY_BRANCH_NODES / (NUM_PRIMARY_BRANCH_NODES / nodeDepth));
            //tilt the branch upwards slightly
            newDirection[1] += 0.004;
            nextNode = new TreeNode(2, newDirection + previousNode->getPosition(), previousNode);
            mTreeNodes.push_back(nextNode);
            //start point for drawing
            mLeafNodes.push_back(nextNode);
            growTree(NUM_SECONDARY_BRANCH_NODES, nextNode);
            //-35 degree branch
            QMatrix4x4 t2;
            t2.rotate(-rotationAngle,0,1,0);
            newDirection = t2 * growthDirection;    //glm::rotateY(growthDirection, -rotationAngle);
            newDirection *= (mHeight * 2 / NUM_PRIMARY_BRANCH_NODES / (NUM_PRIMARY_BRANCH_NODES / nodeDepth));
            newDirection[1] += 0.004;
            nextNode = new TreeNode(2, newDirection + previousNode->getPosition(), previousNode);
            mTreeNodes.push_back(nextNode);
            mLeafNodes.push_back(nextNode);
            growTree(NUM_SECONDARY_BRANCH_NODES, nextNode);

            //if its the end of the branch add a '2' node along growth direction
            if (nodeDepth == 1)
            {

                nextNode = new TreeNode(2, growthDirection * (mHeight * 2 / NUM_PRIMARY_BRANCH_NODES / (NUM_PRIMARY_BRANCH_NODES / nodeDepth)) + previousNode->getPosition(), previousNode);
                mTreeNodes.push_back(nextNode);
                mLeafNodes.push_back(nextNode);
                growTree(NUM_SECONDARY_BRANCH_NODES, nextNode);
            }
            //make a new 1 node along growth direction
            growthDirection *= (mCrownRadius / NUM_PRIMARY_BRANCH_NODES);
            growthDirection[1] += 0.0005 * (NUM_PRIMARY_BRANCH_NODES - nodeDepth);
            nextNode = new TreeNode(1, growthDirection + previousNode->getPosition(), previousNode);
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
