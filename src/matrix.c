#include <math.h>

#include "matrix.h"


Matrix3 newMatrix(const double *values) {
    Matrix3 m;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            m.cells[i][j] = values[i * 3 + j];
        }
    }
    return m;
}


Matrix3 multiplyMatrix(const Matrix3* m1, const Matrix3* m2) {
    Matrix3 result = {0};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                result.cells[i][j] += m1->cells[i][k] * m2->cells[k][j];
            }
        }
    }
    return result;
}


Vector3 multiplyMatrixVector(const Matrix3* m, const Vector3* v) {
    Vector3 result = {0};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result.values[i] += m->cells[i][j] * v->values[j];
        }
    }
    return result;
}


Matrix3 getRotationMatrix(const Vector3* r) {
    const double rX[9] = {
        1, 0, 0,
        0, cos(r->values[0]), -sin(r->values[0]),
        0, sin(r->values[0]), cos(r->values[0])
    };
    const double rY[9] = {
        cos(r->values[1]), 0, sin(r->values[1]),
        0, 1, 0,
        -sin(r->values[1]), 0, cos(r->values[1])
    };
    const double rZ[9] = {
        cos(r->values[2]), -sin(r->values[2]), 0,
        sin(r->values[2]), cos(r->values[2]), 0,
        0, 0, 1
    };
    const Matrix3 rXM = newMatrix(rX);
    const Matrix3 rYM = newMatrix(rY);
    const Matrix3 rZM = newMatrix(rZ);
    const Matrix3 r1 = multiplyMatrix(&rXM, &rYM);
    const Matrix3 r2 = multiplyMatrix(&r1, &rZM);

    return r2;
}


Matrix3 getInverseRotationMatrix(const Vector3* r) {
    const double rX[9] = {
        1, 0, 0,
        0, cos(r->values[0]), sin(r->values[0]),
        0, -sin(r->values[0]), cos(r->values[0])
    };
    const double rY[9] = {
        cos(r->values[1]), 0, -sin(r->values[1]),
        0, 1, 0,
        sin(r->values[1]), 0, cos(r->values[1])
    };
    const double rZ[9] = {
        cos(r->values[2]), sin(r->values[2]), 0,
        -sin(r->values[2]), cos(r->values[2]), 0,
        0, 0, 1
    };
    const Matrix3 rXM = newMatrix(rX);
    const Matrix3 rYM = newMatrix(rY);
    const Matrix3 rZM = newMatrix(rZ);
    const Matrix3 r1 = multiplyMatrix(&rZM, &rYM);
    const Matrix3 r2 = multiplyMatrix(&r1, &rXM);

    return r2;
}
