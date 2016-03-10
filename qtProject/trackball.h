#ifndef TRACKBALL_H
#define TRACKBALL_H

#include <QMatrix4x4>

class Trackball
{
public:
    // Function prototypes
    static void vCalcRotVec(float fNewX, float fNewY,
                     float fOldX, float fOldY,
                     float fDiameter,
                     float *fVecX, float *fVecY, float *fVecZ);
    static void vAxisRotMatrix(float fVecX, float fVecY, float fVecZ, QMatrix4x4 *mNewMat);
};

#endif
