#include <math.h>
#include "msCommon.h"
#include "msMatrix.h"

#pragma once

class msMatrixTransform
{
    msMatrix *matrix;

public:
    msMatrixTransform();
    msMatrixTransform(msMatrix &m);
    ~msMatrixTransform(void);

    msMatrixTransform* scale(float sx, float sy, float sz);
    msMatrixTransform* scale(msPoint3f &s)
    {
        return scale(s.x, s.y, s.z);
    }

    msMatrixTransform* translate(float tx, float ty, float tz);
    msMatrixTransform* translate(msPoint3f &t)
    {
        return translate(t.x, t.y, t.z);
    }
    
    msMatrixTransform* rotate(float angle, float x, float y, float z);
    msMatrixTransform* rotate(float angle, msPoint3f &rotateVector)
    {
        return rotate(angle, rotateVector.x, rotateVector.y, rotateVector.z);
    }

    msMatrixTransform* frustum(float left, float right, float bottom, float top, float nearZ, float farZ);
    msMatrixTransform* ortho(float left, float right, float bottom, float top, float nearZ, float farZ);

    msMatrixTransform* perspective(float fieldOfViewY, float aspectRatio, float zNearPlane, double zFarPlane);
    msMatrixTransform* viewport(float width, float height);

    void multiplyMatrix( msMatrix &matrix );

    msMatrix* getMatrix() const
    {
        return this->matrix;
    }

};

