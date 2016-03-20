#include<GLFW/glfw3.h>
#include "glm/vec2.hpp"
#include<iostream>
#include <cmath>
#include <vector>


#define PI 3.14159265
#define CONTROL_POINT_SIZE 8
#define WINDOW_START_WIDTH 900
#define WINDOW_START_HEIGHT 700
#define STARTING_ORDER 2
#define STARTING_UINC 0.001

class ControlPoint
{
private:
	float m_xpos;
	float m_ypos;
	float m_weight;
public:
	ControlPoint()
	{
		m_xpos = 0;
		m_ypos = 0;
		m_weight = 1;
	}
	ControlPoint(float xpos, float ypos) : m_xpos(xpos) , m_ypos(ypos)
	{
		m_weight = 1;
	}
	~ControlPoint(){}
	void getPos(float& xpos, float& ypos)
	{
		xpos = m_xpos;
		ypos = m_ypos;
	}
	void setPos(float xpos, float ypos)
	{
		m_xpos = xpos;
		m_ypos = ypos;
	}
	void scalePos(float xscale, float yscale)
	{
		m_xpos *= xscale;
		m_ypos *= yscale;
	}
	void setWeight(float weight)
	{
		m_weight = weight;
	} 
	void getWeight(float& weight)
	{
		weight = m_weight;
	}

	ControlPoint operator + (ControlPoint b)
	{	
		float xpos, ypos;
		b.getPos(xpos, ypos);
		return ControlPoint(m_xpos + xpos, m_ypos + ypos);
	} 
};




//ui variables
double m_mouseXPos;
double m_mouseYPos;
int m_windowWidth;
int m_windowHeight;
bool m_leftMouseHeld;
bool m_isRevolution;
bool m_isGeometricTrace;
ControlPoint * m_selectedPoint;
float m_traceU;

//BSpline variables
int m_order;
float u_inc;
std::vector<float> m_knotSequence;
std::vector<ControlPoint *> m_controlPoints;
std::vector<glm::vec2> m_bsplineCurve;
std::vector<glm::vec2> m_knotPoints;
std::vector<glm::vec2> m_tracePoints;

bool getDelta(float u, int &delta)
{
	for (int i = 0; i < m_controlPoints.size() + m_order -1; i++)
	{
		
		if (u >= m_knotSequence[i] && u < m_knotSequence[i+1])
		{
			if (i > delta ){
				delta = i;
				return true;
			} else {
				return false;
			}
		}
			
	}
	delta = -1;
	return false;
}

/////Generate Functions
void updateKnotSequence()
{
	m_knotSequence.clear();
	for (int i = 0; i < m_controlPoints.size() + m_order; i++)
	{
		if (i < m_order)
		{
			m_knotSequence.push_back(0);

		} else if (i >= m_controlPoints.size()) {
			m_knotSequence.push_back(1); 
		} else {
			float knot = i - (m_order - 1);
			knot /= m_controlPoints.size() - m_order + 1; 
			m_knotSequence.push_back(knot);
		}
		//std::cout << m_knotSequence[m_knotSequence.size()-1] << std::endl;
	}
	
	
}

void generateKnots()
{
	m_knotPoints.clear();
	float delta = -1;
	for (int i = 0; i < m_knotSequence.size(); i++)
	{
		if (m_knotSequence[i] > delta)
		{
			delta = m_knotSequence[i];
			int index = delta * (m_bsplineCurve.size() - 1);
			m_knotPoints.push_back(m_bsplineCurve[index]);
		} 	
	}
	
	
}

void generateTracePoints()
{
	m_tracePoints.clear();
	int delta = 0;
	getDelta(m_traceU, delta);
	if (delta == -1)
		return;
	glm::vec2 points[m_order];
	float x,y;
	std::cout << delta << std::endl;
	for (int k = 0; k < m_order; k++)
	{
		m_controlPoints[delta - k]->getPos(x,y);
		points[m_order - k - 1] = glm::vec2(x,y);
		m_tracePoints.push_back(points[m_order - k - 1]);
	}
	for (int j = 0; j < m_order - 1; j++)
	{
		for (int i = 0; i < m_order - 1 - j; i++)
		{	
			points[i] = points[i] * (1 - m_traceU) + points[i+1] * m_traceU;
			m_tracePoints.push_back(points[i]);
		}
	}
}

void generateBSpline()
{
	m_isGeometricTrace = false;
	updateKnotSequence();
	m_bsplineCurve.clear();

	if (m_controlPoints.size() < m_order) return;

	ControlPoint points[m_order];
	int delta = 0;
	for (float u = 0; u <= 1; u += u_inc)
	{
		
		getDelta(u, delta);
		
		for (int i = 0; i < m_order; i++)
		{
			points[i] = *m_controlPoints[delta - i];
		}
		
		for (int r = m_order; r >= 2; r--)
		{
			float i = delta;
			for (int s = 0; s < r - 1; s++)
			{
				
				float temp = m_knotSequence[i + r-1] - m_knotSequence[i];
				if (temp != 0)
				{
					temp = (u - m_knotSequence[i]) / temp;
				}
				float weight;
				ControlPoint a = points[s];
				a.getWeight(weight);
				
				weight *= temp;
				a.scalePos(weight, weight);
				ControlPoint b = points[s+1];
				b.getWeight(weight);
				weight *= (1-temp);
				b.scalePos(weight, weight);
				points[s] = a + b;
				points[s].getWeight(weight);
				i--;
			}
		}
		float xpos, ypos;
		points[0].getPos(xpos, ypos);
		m_bsplineCurve.push_back(glm::vec2(xpos,ypos));
	}
	generateKnots();
}
/////Draw Functions
#define V_INCREMENT 0.1
void drawRotateSurface()
{
	glBegin(GL_QUADS);
	for (int u = 0; u < m_bsplineCurve.size() - 3; u++)
	{
		
		for (float v = 0; v < 2 * 3.14; v+= V_INCREMENT)
		{
			glColor3f(.2f,0.2,0.2);
		
			glVertex3f(2* ((m_bsplineCurve[u].x  / (float)m_windowWidth - 0.5) * cos(v + V_INCREMENT)),  
					2* (m_bsplineCurve[u].y / (float)m_windowHeight - 0.5),
					(m_bsplineCurve[u].x  / (float)m_windowWidth - 0.5) * sin(v+V_INCREMENT));

			glColor3f(1,1,1);
		
			glVertex3f(2* ((m_bsplineCurve[u +3].x  / (float)m_windowWidth - 0.5) * cos(v+V_INCREMENT)),  
					2* (m_bsplineCurve[u +3].y / (float)m_windowHeight - 0.5),
					(m_bsplineCurve[u +3].x  / (float)m_windowWidth - 0.5) * sin(v+V_INCREMENT));
			glColor3f(1,1,1);
		
			glVertex3f(2* ((m_bsplineCurve[u+3].x  / (float)m_windowWidth - 0.5) * cos(v)),  
					2* (m_bsplineCurve[u+3].y / (float)m_windowHeight - 0.5),
					(m_bsplineCurve[u+3].x  / (float)m_windowWidth - 0.5) * sin(v));

			glColor3f(.2f,0.2,0.2);
		
			glVertex3f(2* ((m_bsplineCurve[u].x  / (float)m_windowWidth - 0.5) * cos(v)),  
					2* (m_bsplineCurve[u].y / (float)m_windowHeight - 0.5),
					(m_bsplineCurve[u].x  / (float)m_windowWidth - 0.5) * sin(v));
		}
		
	}
	glEnd();
}

void drawTracePoints()
{
	//std::cout << m_tracePoints.size()<<std::endl;
	glPointSize(CONTROL_POINT_SIZE);
	glBegin(GL_POINTS);
	float xpos, ypos;
	for (int i = 0; i < m_tracePoints.size(); i++)
	{	
		glColor3f(.2f,0.2,0.2);
		
		glVertex3f(2* (m_tracePoints[i].x / (float)m_windowWidth - 0.5),  
					2* (m_tracePoints[i].y / (float)m_windowHeight - 0.5),
					0);
	}
	glEnd();

	glBegin(GL_LINES);
	int orderCount = m_order - 1;
	int counter = 0;
		for (int i = 0; i < m_tracePoints.size() - 1; i++)
		{	
			glColor3f(.2f,0.2,0.2);
			glVertex3f(2* (m_tracePoints[i].x / (float)m_windowWidth - 0.5),  
						2* (m_tracePoints[i].y / (float)m_windowHeight - 0.5),
						0);
			glColor3f(.2f,0.2,0.2);
			glVertex3f(2* (m_tracePoints[i + 1].x / (float)m_windowWidth - 0.5),  
						2* (m_tracePoints[i + 1].y / (float)m_windowHeight - 0.5),
						0);
			counter++;
			if (counter == orderCount)
			{
				i++;
				orderCount--;
				counter = 0;
			}
			
		}
	
	glEnd();

}

void drawBSpline()
{
	glLineWidth(2);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < m_bsplineCurve.size(); i++)
	{
		glColor3f(1,0,0);
		glVertex3f(2* (m_bsplineCurve[i].x / (float)m_windowWidth - 0.5),  
					2* (m_bsplineCurve[i].y / (float)m_windowHeight - 0.5),
					0);
	}
	glEnd();
}

void drawControlPoints()
{
	glPointSize(CONTROL_POINT_SIZE);
	glBegin(GL_POINTS);
	float xpos, ypos;
	for (int i = 0; i < m_controlPoints.size(); i++)
	{
		glColor3f(1.0f,0,0);
		m_controlPoints[i]->getPos(xpos, ypos);
		glVertex3f(2* (xpos / (float)m_windowWidth - 0.5),  
					2* (ypos / (float)m_windowHeight - 0.5),
					0);
	}
	glEnd();
}

void drawKnotPoints()
{
	glPointSize(CONTROL_POINT_SIZE - 2);
	glBegin(GL_POINTS);
	for (int i = 0; i < m_knotPoints.size(); i++)
	{
		
		glColor3f(0.0f,0,1.0f);
		glVertex3f(2* (m_knotPoints[i].x / (float)m_windowWidth - 0.5),  
					2* (m_knotPoints[i].y / (float)m_windowHeight - 0.5),
					0.01);
	}
	glEnd();
	
}
////Input Functions

////GLFW Window 












ControlPoint * checkForPointAtCursor()
{
	float xpos, ypos;
	for (int i = 0; i < m_controlPoints.size(); i++)
	{
		m_controlPoints[i]->getPos(xpos, ypos);
		if (std::abs( xpos - m_mouseXPos) < CONTROL_POINT_SIZE &&
			 std::abs( ypos - m_mouseYPos) < CONTROL_POINT_SIZE)
			return m_controlPoints[i];
	}
	return NULL;
}




void render () {
	glEnable (GL_DEPTH_TEST);
	//glEnable (GL_LIGHTING);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.9,0.9,0.9,0);


	//Functions for changing transformation and projection matrices
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(0.5f, 0.5f, 0.0f);
	//glScalef(0.5f, 0.5f, 1.0f);

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glOrtho (-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	
	
	
	
	if (!m_isRevolution)
	{
		drawControlPoints();
		drawBSpline();
		drawKnotPoints();
	} else {
		
		drawRotateSurface();
	}
	
	if (m_isGeometricTrace)
		drawTracePoints();

	



	
}

//mouse click event handler
void mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (m_selectedPoint != NULL) //user added a new point but hasn't released the mouse button
		{
			//cancel adding the point
			delete m_controlPoints[m_controlPoints.size() - 1];
			m_controlPoints.erase(m_controlPoints.begin() + m_controlPoints.size() - 1);
			m_selectedPoint = NULL;
			generateBSpline();
		} else {
			//check for a control point at the right click point
			float xpos, ypos;
			for (int i = 0; i < m_controlPoints.size(); i++)
			{
				m_controlPoints[i]->getPos(xpos, ypos);
				if (std::abs( xpos - m_mouseXPos) < CONTROL_POINT_SIZE &&
					 std::abs( ypos - m_mouseYPos) < CONTROL_POINT_SIZE)
				{
					if (m_controlPoints.size() == m_order) m_order--;
					//remove the control point
					delete m_controlPoints[i];	
					m_controlPoints.erase(m_controlPoints.begin() + i);
					generateBSpline();
				}	
			}
		}
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS) 
		{		
			if ((m_selectedPoint = checkForPointAtCursor()) == NULL) //is there already a point at the click position?
			{
				//if not add a new one and set it to selectedPoint. 
				m_controlPoints.push_back(m_selectedPoint = new ControlPoint(m_mouseXPos, m_mouseYPos));
				generateBSpline();
			} 
		} 		
	
		if (action == GLFW_RELEASE)
		{
			m_selectedPoint = NULL;
		}	
	}

	
	
}

//record the new size of the screen for scaling purposes
void window_size_callback(GLFWwindow * window, int width, int height)
{
	//float xscaling = (float)width / (float)m_windowWidth;
	//float yscaling = (float)height /(float)m_windowHeight ;
	glfwGetWindowSize(window, &m_windowWidth, &m_windowHeight);
	//std::cout << m_windowWidth << "<" << m_windowHeight << std::endl;
	
	for (int i = 0; i < m_controlPoints.size(); i++)
	{
		//m_controlPoints[i]->scalePos(xscaling, yscaling);
	}
	if (m_windowWidth < 400)
	{
		//glfwSetWindowSize(window, m_windowWidth = 400, m_windowHeight);
	}	
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	glfwGetCursorPos(window, &m_mouseXPos, &m_mouseYPos);
	m_mouseYPos = m_windowHeight - m_mouseYPos;

	if (m_selectedPoint != NULL)
	{
		m_selectedPoint->setPos(m_mouseXPos, m_mouseYPos);
		generateBSpline();
	}
	
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
		
	
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
	{
       if ((u_inc /= 3) < 0.0001) u_inc = 0.0001;
	}	
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
	{
		if ((u_inc *= 3) > 0.1) u_inc = 0.1;
	}	

	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		//std::cout << "inter" << std::endl;
        if (m_order < m_controlPoints.size()) m_order++;
		generateBSpline();
		

	}	
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		if (m_order > 2) m_order--;
		generateBSpline();
	}	
	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
    {
		if (m_controlPoints.size() >= m_order)
		{
			if (m_isGeometricTrace) m_isGeometricTrace = false;
			else {
				m_isGeometricTrace = true;
				m_traceU = 0;
				generateTracePoints();		
			}
		}
	}   

	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		if (m_controlPoints.size() >= m_order)
		{
			if (m_isRevolution) m_isRevolution = false;
			else {
				m_isRevolution = true;
				
				generateBSpline();		
			}
		}
	}

	if (key == GLFW_KEY_KP_2 && action == GLFW_PRESS)
	{

	}

	
	if (key == GLFW_KEY_KP_4 && action == GLFW_PRESS)
	{
		if (m_isGeometricTrace){
			if ((m_traceU-= 0.05) < 0)
			m_traceU = 0;
			std::cout << m_traceU << std::endl;
			generateTracePoints();
		}

	}

	if (key == GLFW_KEY_KP_6 && action == GLFW_PRESS)
	{
		if (m_isGeometricTrace){
			if ((m_traceU+= 0.05) > 1)
				m_traceU = 1;
			std::cout << m_traceU << std::endl;
			generateTracePoints();
			
		}
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
	m_order = STARTING_ORDER;
	u_inc = STARTING_UINC;

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

