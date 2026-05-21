#include "vector.h"
#include "matrix.h"


#ifndef cube_h
#define cube_h


#define DIVS 20
#define ERROR 0.01


typedef struct {
    Vector3 p, s, r;
    char f[6];

    // what the editor uses
    Vector3 p_fake;

    // compute once to improve performance
    double diagonal_length;
    Vector3 p_inverse_rotated;
    Vector3 p_inverse_rotated_negative;
    Vector3 side_vectors[3];
    Matrix3 rotation_matrix;
    Matrix3 inverse_rotation_matrix;
} Cube;


Vector3 rotateVectorByCubeR(const Vector3* v, const Cube* c);
Vector3 inverseRotateVectorByCubeR(const Vector3* v, const Cube* c);
Vector3List* getVertexPoints(const Cube* c, int face_index);
Vector3List* getFacePoints(const Cube* c, int face_index);
bool checkPointInsideCube(const Vector3 *v, const Cube *c);


typedef struct {
    size_t length;
    Cube** cubes;
} CubeList;


CubeList* newCubeList(size_t length);
void freeCubeList(CubeList* cl, bool free_cubes);
CubeList* getTouchingCubes(const Cube* c, const CubeList* cl);


void computeCubeSideVectors(Cube* c);
void computeCubeRotationMatrices(Cube* c);
void computeCubeDiagonals(Cube* c);
void computeCubeFields(Cube *c);


#endif
