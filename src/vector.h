#include <stddef.h>
#include <stdbool.h>


#ifndef vector_h
#define vector_h


typedef struct {
    double values[3];
} Vector3;


Vector3 newVector(const double *values);
double pythagoreanDistanceSquared(const Vector3* v1, const Vector3* v2);
// double taxicabDistance(const Vector3* v1, const Vector3* v2);
bool checkVectorsIdentical(const Vector3* v1, const Vector3* v2);
Vector3 scaleVector(const Vector3* v, double s);
Vector3 addVector(const Vector3* v1, const Vector3* v2);


typedef struct {
    size_t length;
    Vector3* vectors;
} Vector3List;


Vector3List* newVectorList(size_t length);
void vectorListCopy(Vector3List* vl, Vector3 v_array[]);
void freeVectorList(Vector3List* vl);


#endif
