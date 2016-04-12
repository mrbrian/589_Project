#include<GLFW/glfw3.h>
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/gtx/norm.hpp"
#include <glm/gtx/rotate_vector.hpp>

#include<iostream>
#include <cmath>
#include <vector>


#define PI 3.14159265
#define WINDOW_START_WIDTH 600
#define WINDOW_START_HEIGHT 600
#define POINT_ADDITION_WAIT .25 
//ui variables
double m_mouseXPos;
double m_mouseYPos;
int m_windowWidth;
int m_windowHeight;
bool m_leftMouseHeld;
class Tree;
Tree * mTree;
 

class TreeNode
{
public:
	TreeNode(int type, glm::vec4 position, TreeNode *mPrevious)
	{
		//std::cout << "made new node at : " << position.x << "," << position.y << "," << position.z << std::endl;
		mType = type;
		mPosition = position;
		mPreviousNode = mPrevious;	
		mDrawn = false;
	}
	
	int getType() {return mType;}
	glm::vec4 getGrowthDirection()
	{
		if (mPreviousNode != NULL)
		{
			return (glm::normalize(mPosition - mPreviousNode->getPosition()));
		}

		return glm::vec4(0,1,0,0);
	}
	glm::vec4 getPosition() {return mPosition;}
	TreeNode * getPreviousNode() {return mPreviousNode;}
	void setDrawn(bool drawn) {mDrawn = drawn;}
	bool hasBeenDrawn(){return mDrawn;}

private: //members
	int mType;
	glm::vec4 mPosition;
	TreeNode * mPreviousNode;
	bool mDrawn;
};

#define NUM_TRUNK_NODES 12
#define NUM_PRIMARY_BRANCH_NODES 10
#define NUM_SECONDARY_BRANCH_NODES 2
class Tree
{
public:
	Tree(float height, float crownRadius, float trunkRadius) : mHeight(height), mCrownRadius(crownRadius), mTrunkRadius(trunkRadius)
	{
		//start the tree growing at the trunk
		growTree(NUM_TRUNK_NODES - 1, new TreeNode(0, glm::vec4(0,0,0,1), NULL));		
	}

	bool growTree(int nodeDepth, TreeNode * previousNode)
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
					//trunk growth will always be up right now
					glm::vec4 growthDirection(0,1,0,0);
					float rotationAngle;
					//rotate to the horizontal plane
					rotationAngle = (80.0f / 180.0f) * 3.14; //potential to be randomized. (85-75 degrees)
					growthDirection = glm::rotateX(growthDirection, rotationAngle);
					//rotate around the y axis, staggered at each new level
					rotationAngle = (2.0f / NUM_PRIMARY_TO_TRUNK) * 3.14f * (float)i + nodeDepth * 5;
					growthDirection = glm::rotateY(growthDirection, rotationAngle);
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
				//make 2 '2' nodes rotated (35 / -35) degrees about y axis. can be randomized slightly
				TreeNode * nextNode;
				//35 degree branch
				glm::vec4 growthDirection = previousNode->getGrowthDirection();
				float rotationAngle = 35.0f / 180.0f * 3.14;
				glm::vec4 newDirection = glm::rotateY(growthDirection, rotationAngle);
				//scale the branch length based off its distance from trunk 'age of branch'
				newDirection *= (mHeight * 2 / NUM_PRIMARY_BRANCH_NODES / (NUM_PRIMARY_BRANCH_NODES / nodeDepth));
				//tilt the branch upwards slightly
				newDirection.y += 0.004;
				nextNode = new TreeNode(2, newDirection + previousNode->getPosition(), previousNode);
				mTreeNodes.push_back(nextNode);
				//start point for drawing
				mLeafNodes.push_back(nextNode);	
				growTree(NUM_SECONDARY_BRANCH_NODES, nextNode);
				//-35 degree branch
				newDirection = glm::rotateY(growthDirection, -rotationAngle);
				newDirection *= (mHeight * 2 / NUM_PRIMARY_BRANCH_NODES / (NUM_PRIMARY_BRANCH_NODES / nodeDepth));
				newDirection.y += 0.004;
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
				growthDirection.y += 0.0005 * (NUM_PRIMARY_BRANCH_NODES - nodeDepth);
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

	void draw()
	{
		glLineWidth(2);
		glBegin(GL_LINES);
		for (int i = 0; i < mLeafNodes.size(); i++)
		{
			
			TreeNode * node = mLeafNodes[i];
			while (!node->hasBeenDrawn() && node->getPreviousNode() != NULL)
			{
				
				node->setDrawn(true);
				glm::vec3 color;
				switch (node->getType())
				{
				case(0):
					glLineWidth(5);
					color = glm::vec3(1,0,0);
					break;
				case(1):
					glLineWidth(3);
					color = glm::vec3(1,0,0);
					break;
				case(2):
					glLineWidth(1);
					color = glm::vec3(0,1,0);
					break;
				}
				
				glm::vec4 currentPosition = node->getPosition();
				glm::vec4 previousPosition = node->getPreviousNode()->getPosition();
								
				glColor3f(color.x, color.y, color.z);
				glVertex3f(currentPosition.x, currentPosition.y, currentPosition.z);
				std::cout <<currentPosition.x << "," << currentPosition.y << "," << currentPosition.z << std::endl;
				glColor3f(color.x, color.y, color.z);
				glVertex3f(previousPosition.x, previousPosition.y, previousPosition.z);
				node = node->getPreviousNode();
			}
			
		}
		glEnd();
		for (int i = 0; i < mTreeNodes.size(); i++)
		{
			mTreeNodes[i]->setDrawn(false);
		}
		
	}
private: // members
	std::vector<TreeNode *> mTreeNodes;
	std::vector<TreeNode *> mLeafNodes;
	float mHeight;
	float mCrownRadius;
	float mTrunkRadius;
};



////GLFW Window 

void render () {
	//m_dt = glfwGetTime() - m_currentTime;
	//m_currentTime = glfwGetTime();
	
	glEnable (GL_DEPTH_TEST);
	//glEnable (GL_LIGHTING);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.9,0.9,0.9,0);


	//Functions for changing transformation and projection matrices
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,.4f , -1.f);
	glRotatef(15,1,0,0);
	glRotatef(15,0,1,0);
	//glScalef(0.5f, 0.5f, 1.0f);

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glOrtho (-2.0f, 2.0f, 0.0f, 2.0f, -2.0f, 2.0f);
	mTree->draw();
	
	

}

//mouse click event handler
void mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		
		
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		
	
		
			
	}
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	glfwGetCursorPos(window, &m_mouseXPos, &m_mouseYPos);
	//m_mouseYPos = m_windowHeight - m_mouseYPos;

	
}

//record the new size of the screen for scaling purposes
void window_size_callback(GLFWwindow * window, int width, int height)
{
	
}




void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
		
	
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
	{
     	
	}	
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
	{
		
	}	

	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		
		

	}	
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		
		
	}	
	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
    {
		
	}   

	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		
	}

	if (key == GLFW_KEY_KP_2 && action == GLFW_PRESS)
	{

	}
	
	if (key == GLFW_KEY_KP_4 && action == GLFW_PRESS)
	{

	}

	if (key == GLFW_KEY_KP_6 && action == GLFW_PRESS)
	{
	
	}

	if (key == GLFW_KEY_KP_7 && action == GLFW_PRESS)
	{
		
	}

	if (key == GLFW_KEY_KP_9 && action == GLFW_PRESS)
	{

	}

	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
	
	}
	
}


int main () {
	//initialize glfw
	if (!glfwInit()) {
		std::cout << "Could not initialize.\n";
		return 1;
	}

	//create window
	GLFWwindow *window = glfwCreateWindow (m_windowWidth = WINDOW_START_WIDTH,
											m_windowHeight = WINDOW_START_HEIGHT, 
											"Assignment 2", NULL, NULL);
	if (!window) {
		std::cout << "Could not create window.\n";
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent (window);

	//set callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);

	//initialize starting variables 
	mTree = new Tree(1.5, 1, .04);
	//render loop
	while (!glfwWindowShouldClose (window)) {
		render ();
		glfwSwapBuffers (window);
		glfwPollEvents();
	}

	//clean up
	//todo: clean up pointers
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

