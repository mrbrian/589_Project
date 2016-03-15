#ifndef BSPLINE_H
#define BSPLINE_H

#include <vector>
#include <iostream>
#include <QVector2D>
#include <stdexcept>

using namespace std;

class BSpline
{
public:
    BSpline(int k, vector<QVector2D*> *pts);
	void setOrder(int in_k);						// setter
	void setKnots(float *U);						// setter
    void setControlPoints(vector<QVector2D*> *p);		// setter    
    QVector2D *evalPoint(float step_u);
    virtual void getLinePoints(vector<QVector2D*> *list, vector<float> *u_list, float step_u);	// evaluate a list of points on the curve

	int getIndexOfFocus(float u);					// find the index of focus
    QVector2D *effSum(int d, float u, vector<QVector2D*> *geoPts, vector<QVector2D*> *convexPts);		// efficient algorithm to evaluate a point on the curve

	static float *standardKnotSeq(int m, int k);								// generates the standard knot sequence for given m and k
	static double bSplineBasis(int i, int m, int k, double u, float *knots);	// evaluate a specified bspline basis func 

protected:
	int k;						// curve order
	float *knots;				// knot array
    vector<QVector2D*> *ctrlPts;	// a pointer to the control point list in program
};

#endif
