#ifndef BSPLINE_BLENDED_H
#define BSPLINE_BLENDED_H

#include <vector>
#include "bspline.h"

using namespace std;

class BSpline_Blended : public BSpline
{
public:
    BSpline_Blended(int k, vector<BSpline*> *b);
    QVector2D *evalPoint(float blend_factor, float u);
    void getLinePoints(vector<QVector2D*> *list, vector<float> *u_list, float step_u) override;

private:
    vector<BSpline*> *splines;
    QVector2D *effSum(int d, float blend_factor, float u);

/*
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

    int getOrder();                         // getter
    float *getKnots();                      // getter
    vector<QVector2D*> *getControlPts();    // getter

private:
    int k;						// curve order
    float *knots;				// knot array
    vector<QVector2D*> *ctrlPts;	// a pointer to the control point list in program
*/


};

#endif // BSPLINE_BLENDED_H
