#pragma once

#include "../msCommon.h"


class msSpline
{
    void BSpline_ComputeCoeffs_Private(float c0, float c1, float c2, float c3, float *a, float *b, float *c, float *d);

public:
    msSpline(void);
    ~msSpline(void);

    int resolution;
    int count;

    //msPoints m_controlPoints;

    msPoint3f CtrlPt[80];

    void BSpline_ComputeCoeffs(int N, msPoint3f *Ap, msPoint3f *Bp, msPoint3f *Cp, msPoint3f *Dp);

    msPoint3f Spline_Calc(msPoint3f Ap, msPoint3f Bp, msPoint3f Cp, msPoint3f Dp, float T, float D);

    void draw();

    void addControlPoint(msPoint3f p);

    void addControlPoint(float x, float y);

    void getSplinePoints(int resolution, msPoint3f *points, int *count, int startIndex = 0, bool loop = false);
};