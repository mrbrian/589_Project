#include "bspline_blended.h"

BSpline_Blended :: BSpline_Blended(int k, vector<BSpline*> *b)
    : BSpline()
{
/*
    int k;						// curve order
    float *knots;				// knot array
    vector<QVector2D*> *ctrlPts;	should not really have this->
*/
    this->k = k;
    knots = standardKnotSeq(b->size() - 1, k);
    //ctrlPts
    splines = b;
}

QVector2D *BSpline_Blended::evalPoint(float blend_factor, float u)
{
    int m = splines->size() - 1;
    if (m + 1 < k)		// need # of control points >= k for a line
        throw std::invalid_argument( "not enough control points" );

    int d = 0;

    if (blend_factor < 1)
    {
        while (blend_factor >= knots[d + 1] && d < m + k)
            d++;
    }
    else if (blend_factor >= 1)
    {
        blend_factor = 1;      // clamp to end of curve
        while (blend_factor > knots[d+1] && d < m + k)
            d++;
    }

    return (effSum(d, blend_factor, u));	// evaluate the final curve point and store it
}

void BSpline_Blended :: getLinePoints(vector<QVector2D*> *list, vector<float> *u_list, float step_u)	// evaluate a list of points on the curve
{
//    list->push_back();
}

// determine the contribution of each bspline and add into a final point
QVector2D *BSpline_Blended::effSum(int d, float blend_factor, float u)
{
    QVector2D **c = new QVector2D*[k];

    for (int i = 0; i <= k - 1; i++)
    {
        if (d - i < 0)
            throw std::invalid_argument( "tried to access invalid array index" );
        BSpline *bs = (*splines)[i];
        QVector2D *pt = bs->evalPoint(u);
        c[i] = pt;		//nonzero coefficients
    }

    for (int r = k; r >= 2; r--)		// the columns of the table
    {
        int i = d;
        for (int s = 0; s <= r - 2; s++)	// the elements of the column
        {
            float omega = (blend_factor - knots[i]) / (knots[i + r - 1] - knots[i]);
            QVector2D &cs = *c[s];
            QVector2D &cs_next = *c[s + 1];
            cs = cs * omega + cs_next * (1 - omega);
            i = i - 1;
        }
    }
    QVector2D *result = new QVector2D(*c[0]);
    return result;
}
