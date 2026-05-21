#include "vector.h"


#ifndef matrix_h
#define matrix_h


typedef struct {
    double cells[3][3];
} Matrix3;


Matrix3 newMatrix(const double *values);
Matrix3 multiplyMatrix(const Matrix3* m1, const Matrix3* m2);
Vector3 multiplyMatrixVector(const Matrix3* m, const Vector3* v);

Matrix3 getRotationMatrix(const Vector3* r);
Matrix3 getInverseRotationMatrix(const Vector3* r);


#endif
