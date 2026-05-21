#include <stdlib.h>
#include <stdbool.h>

#include "vector.h"


Vector3 newVector(const double *values) {
    Vector3 v;
    for (int i = 0; i < 3; i++) {
        v.values[i] = values[i];
    }
    return v;
}


double pythagoreanDistanceSquared(const Vector3* v1, const Vector3* v2) {
    double distance = 0;
    for (int i = 0; i < 3; i++) {
        double delta = (v1->values[i] - v2->values[i]);
        distance += delta * delta;
    }
    return distance;
}


// double taxicabDistance(const Vector3* v1, const Vector3* v2) {
//     double distance = 0;
//     for (int i = 0; i < 3; i++) {
//         distance += fabs(v1->values[i] - v2->values[i]);
//     }
//     return distance;
// }


bool checkVectorsIdentical(const Vector3* v1, const Vector3* v2) {
    for (int i = 0; i < 3; i++) {
        if (v1->values[i] != v2->values[i]) { return false; }
    }
    return true;
}


Vector3 scaleVector(const Vector3* v, double s) {
    Vector3 result;
    for (int i = 0; i < 3; i++) {
        result.values[i] = v->values[i] * s;
    }
    return result;
}


Vector3 addVector(const Vector3* v1, const Vector3* v2) {
    Vector3 result;
    for (int i = 0; i < 3; i++) {
        result.values[i] = v1->values[i] + v2->values[i];
    }
    return result;
}


Vector3List* newVectorList(size_t length) {
    Vector3List *vl = malloc(sizeof(*vl));
    vl->length = length;
    vl->vectors = malloc(length * sizeof(*vl->vectors));
    return vl;
}


void vectorListCopy(Vector3List* vl, Vector3 v_array[]) {
    for (size_t i = 0; i < vl->length; i++) {
        for (int j = 0; j < 3; j++) {
            vl->vectors[i].values[j] = v_array[i].values[j];
        }
    }
}


void freeVectorList(Vector3List* vl) {
    free(vl->vectors);
    free(vl);
}
