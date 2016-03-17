#include "bspline.h"

BSpline :: BSpline(int k, vector<QVector2D*> *pts){
    ctrlPts = pts;
    knots = BSpline::standardKnotSeq(pts->size(), k);
    this->k = k;
}

/* Determine the index of focus for a given u */
int BSpline::getIndexOfFocus(float u)
{
	int i = 0;
	int m = ctrlPts->size() - 1;

	if (u >= 1)		// for end of the curve, index of focus = m
		return m;

	for (i = 0; i < m + k; i++)		// go through knot list 
	{
		if (u >= knots[i] && u < knots[i + 1])	// if this interval contains u
			return i;
	}
	return -1;
}

void BSpline::setControlPoints(vector<QVector2D*> *p)
{
	ctrlPts = p;
}

void BSpline::setOrder(int in_k)
{
	k = in_k;
	if (k < 1)	// minimum order = 1
		k = 1;
}

void BSpline::setKnots(float *U)
{
	if (knots)
		free(knots);	// free memory
	knots = U;
}

/* 
effsum: Efficient algorithm for BSplines
Given a index of focus, evaluate the point at u, and add the geometric interpretation points, 
and influencing control points into the given geoPts, convexPts lists	
*/
QVector2D *BSpline::effSum(int d, float u, vector<QVector2D*> *geoPts, vector<QVector2D*> *convexPts)
{
	// clear the lists
	if (convexPts)
		convexPts->clear();
	if (geoPts)
		geoPts->clear();

    QVector2D *c = new QVector2D[k];

	for (int i = 0; i <= k - 1; i++)
	{
        if (d - i < 0)
            throw std::invalid_argument( "tried to access invalid array index" );
		c[i] = *(*ctrlPts)[d - i];		//nonzero coefficients
		if (convexPts)
			convexPts->push_back((*ctrlPts)[d - i]);	// add as a contributing control point
	}
	int step = 0;
	for (int r = k; r >= 2; r--)		// the columns of the table
	{
		int i = d;
		for (int s = 0; s <= r - 2; s++)	// the elements of the column
		{
			float omega = (u - knots[i]) / (knots[i + r - 1] - knots[i]);
			c[s] = c[s] * omega + c[s + 1] * (1 - omega);
			i = i - 1;
			if (geoPts)		// if rendering the geometric display
			{
                geoPts->push_back(new QVector2D(c[s]));	// add the non-final point to the geometric list
				if (s > 0 && s < r - 2)
                    geoPts->push_back(new QVector2D(c[s]));	// add the same point twice if its shared between two lines
			}
		}
	}
    QVector2D *result = new QVector2D(c[0]);
	return result;
}

QVector2D *BSpline::evalPoint(float u)
{
    int m = ctrlPts->size() - 1;
    if (m + 1 < k)		// need # of control points >= k for a line
        throw std::invalid_argument( "not enough control points" );

    int d = 0;
    while (u < 1 && u >= knots[d + 1] && d < m + k)
        d++;

    return (effSum(d, u, 0, 0));	// evaluate the final curve point and store it
}

void BSpline::getLinePoints(vector<QVector2D*> *list, vector<float> *u_list, float step_u)
{
	list->clear();
	u_list->clear();	// clear the target lists

	int m = ctrlPts->size() - 1;
	float u = 0;
	float prev_u = 0;

	if (m + 1 < k)		// need # of control points >= k for a line
		return;

	int d = 0;
	bool end = false;	// flag if we clamped u to the end of the curve (only clamp it once so the loop can end after)
	while (u <= 1)
	{
		while (u < 1 && u >= knots[d + 1] && d < m + k)
			d++;

		list->push_back(effSum(d, u, 0, 0));	// evaluate the final curve point and store it
		u_list->push_back(u);					// store the u of the point
		
		u += step_u;
		
		if (prev_u < 1 && u > 1 && !end)	// if we overshot the end of the curve, clamp it 
		{
			end = true;		// only clamp it once
			u = 1;			// clamp u to the end of the curve
		}
		prev_u = u;
	}
}

/* return the standard knot sequence for m & k */
float *BSpline::standardKnotSeq(int m, int k)
{
	float *knots = new float[m + k + 1];
	for (int i = 0; i < k; i++)			// first k knots = 0
		knots[i] = 0;

	float step = (float)1 / ((m + 1) - (k - 1));

	float v = step;
	for (int i = k; i < m + 1; i++)		// the middle knots interpolate between 0 to 1
	{
		knots[i] = v;
		v += step;
	}
	for (int i = m + 1; i <= m + k; i++)	// last k knots are = 1
	{
		knots[i] = 1;
	}
	return knots;
}

double BSpline::bSplineBasis(int i, int m, int k, double u, float *knots)
{
	if (k <= 0)										// weight of negative orders = 0
		return 0;
	if (k == 1)										// base case: order 1
	{
		if (knots[i] <= u && u < knots[i + 1])		// if u is inside the ith interval, return 1
			return 1;
		else
			return 0;
	}
	double denomA = knots[i + k - 1] - knots[i];	// calculate denomator of intervalA
	double denomB = knots[i + k] - knots[i + 1];	// calc denominator of intervalB
	double a = denomA == 0 ? 0 : (u - knots[i]) / denomA * bSplineBasis(i, m, k - 1, u, knots);		// if size of intervalA is 0, make the weight of intervalA = 0
	double b = denomB == 0 ? 0 : (knots[i + k] - u) / denomB * bSplineBasis(i + 1, m, k - 1, u, knots);		// same for B
	return (a + b);		// return weight sum
}
