#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "cube.h"


CubeList* newCubeList(size_t length) {
    CubeList *cl = malloc(sizeof(*cl));
    cl->length = length;
    cl->cubes = malloc(length * sizeof(*cl->cubes));
    for (size_t i = 0; i < length; i++) { cl->cubes[i] = NULL; }
    return cl;
}


void freeCubeList(CubeList* cl, bool free_cubes) {
    if (free_cubes) {
        for (size_t i = 0; i < cl->length; i++) { free(cl->cubes[i]); }
    }
    free(cl->cubes);
    free(cl);
}



Vector3 rotateVectorByCubeR(const Vector3* v, const Cube* c) {
    if (
        (c->r.values[0] == 0) &&
        (c->r.values[1] == 0) &&
        (c->r.values[2] == 0)
    ) {
        return newVector(v->values);  // easy win
    }

    Vector3 result = multiplyMatrixVector(&c->rotation_matrix, v);
    return result;
}


Vector3 inverseRotateVectorByCubeR(const Vector3* v, const Cube* c) {
    if (
        (c->r.values[0] == 0) &&
        (c->r.values[1] == 0) &&
        (c->r.values[2] == 0)
    ) {
        return newVector(v->values);  // easy win
    }

    Vector3 result = multiplyMatrixVector(&c->inverse_rotation_matrix, v);
    return result;
}


Vector3List* getVertexPoints(const Cube* c, int face_index) {
    const Vector3 v1 = c->side_vectors[(face_index + 0) % 3];
    const Vector3 v2 = c->side_vectors[(face_index + 1) % 3];
    const Vector3 v3 = c->side_vectors[(face_index + 2) % 3];
    const int sign = (face_index % 2) ? -1 : 1;

    const Vector3 v1_scaled = scaleVector(&v1, -0.5);
    const Vector3 v2_scaled = scaleVector(&v2, -0.5);
    const Vector3 v3_scaled = scaleVector(&v3, 0.5 * sign);
    const Vector3 v1_plus_v2 = addVector(&v1, &v2);
    Vector3 offset = addVector(&v1_scaled, &v2_scaled);
    offset = addVector(&offset, &v3_scaled);
    offset = addVector(&offset, &c->p);

    Vector3 v_array[4];
    v_array[0] = offset;
    v_array[1] = addVector(&offset, &v1);
    v_array[2] = addVector(&offset, &v2);
    v_array[3] = addVector(&offset, &v1_plus_v2);

    Vector3List *points = newVectorList(4);
    vectorListCopy(points, v_array);
    return points;
}


Vector3List* getFacePoints(const Cube* c, int face_index) {
    const Vector3 v1 = c->side_vectors[(face_index + 0) % 3];
    const Vector3 v2 = c->side_vectors[(face_index + 1) % 3];
    const Vector3 v3 = c->side_vectors[(face_index + 2) % 3];
    const int sign = (face_index % 2) ? -1 : 1;

    const Vector3 v1_scaled = scaleVector(&v1, -0.5);
    const Vector3 v2_scaled = scaleVector(&v2, -0.5);
    const Vector3 v3_scaled = scaleVector(&v3, 0.5 * sign);
    Vector3 offset = addVector(&v1_scaled, &v2_scaled);
    offset = addVector(&offset, &v3_scaled);
    offset = addVector(&offset, &c->p);

    Vector3 v1_div = scaleVector(&v1, 1.0 / DIVS);
    Vector3 v2_div = scaleVector(&v2, 1.0 / DIVS);

    Vector3 v_array[(DIVS + 1) * (DIVS + 1)];
    for (size_t i = 0; i <= DIVS; i++) {
        const Vector3 v1_div_scaled = scaleVector(&v1_div, i);
        for (size_t j = 0; j <= DIVS; j++) {
            const Vector3 v2_div_scaled = scaleVector(&v2_div, j);
            const Vector3 face_point_unoffset = addVector(&v1_div_scaled, &v2_div_scaled);
            v_array[i * (DIVS + 1) + j] = addVector(&face_point_unoffset, &offset);
        }
    }

    Vector3List *face_points = newVectorList((DIVS + 1) * (DIVS + 1));
    vectorListCopy(face_points, v_array);
    return face_points;
}


bool checkPointInsideCube(const Vector3 *v, const Cube *c) {
    Vector3 v2 = inverseRotateVectorByCubeR(v, c);
    v2 = addVector(&v2, &c->p_inverse_rotated_negative);
    return (
        (-c->s.values[0]/2 <= (v2.values[0] + ERROR)) && ((v2.values[0] - ERROR) <= c->s.values[0]/2) &&
        (-c->s.values[1]/2 <= (v2.values[1] + ERROR)) && ((v2.values[1] - ERROR) <= c->s.values[1]/2) &&
        (-c->s.values[2]/2 <= (v2.values[2] + ERROR)) && ((v2.values[2] - ERROR) <= c->s.values[2]/2)
    );
}


CubeList* getTouchingCubes(const Cube* c, const CubeList* cl) {
    CubeList *cl_1 = newCubeList(cl->length);
    size_t cl_1_counter = 0;
    for (size_t i = 0; i < cl->length; i++) {
        Cube *c2 = cl->cubes[i];

        if (
            checkVectorsIdentical(&c->p, &c2->p) &&
            checkVectorsIdentical(&c->s, &c2->s) &&
            checkVectorsIdentical(&c->r, &c2->r)
        ) {
            continue;  // possibly self; skip all identical cubes regardless
        }

        const double diagonals_sum = c->diagonal_length + c2->diagonal_length;
        const double p_distance = sqrt(pythagoreanDistanceSquared(&c->p, &c2->p));
        // double p_distance = taxicabDistance(c.p, c2.p);  // not worth cheaping out here

        if (p_distance <= diagonals_sum / 2  + ERROR) {
            cl_1->cubes[cl_1_counter] = c2;
            cl_1_counter += 1;
        }
    }

    cl_1->length = cl_1_counter;
    return cl_1;
}


void computeCubeSideVectors(Cube* c) {
    const double s1[] = {c->s.values[0], 0, 0};
    const double s2[] = {0, c->s.values[1], 0};
    const double s3[] = {0, 0, c->s.values[2]};
    Vector3 v1 = newVector(s1);
    Vector3 v2 = newVector(s2);
    Vector3 v3 = newVector(s3);
    v1 = rotateVectorByCubeR(&v1, c);
    v2 = rotateVectorByCubeR(&v2, c);
    v3 = rotateVectorByCubeR(&v3, c);

    c->side_vectors[0] = v1;
    c->side_vectors[1] = v2;
    c->side_vectors[2] = v3;
}


void computeCubeRotationMatrices(Cube* c) {
    c->rotation_matrix = getRotationMatrix(&c->r);
    c->inverse_rotation_matrix = getInverseRotationMatrix(&c->r);
}


void computeCubeDiagonals(Cube* c) {
    const double sX = c->s.values[0];
    const double sY = c->s.values[1];
    const double sZ = c->s.values[2];

    c->diagonal_length = sqrt(sX*sX + sY*sY + sZ*sZ);
}


void computeCubeFields(Cube *c) {
    computeCubeRotationMatrices(c);

    double s_fake[3] = {0, c->s.values[1] / 2,0};
    Vector3 s_fake_vector = newVector(s_fake);
    Vector3 s_fake_vector_rotated = rotateVectorByCubeR(&s_fake_vector, c);
    c->p = addVector(&c->p_fake, &s_fake_vector_rotated);

    c->p_inverse_rotated = inverseRotateVectorByCubeR(&c->p, c);
    c->p_inverse_rotated_negative = scaleVector(&c->p_inverse_rotated, -1);

    computeCubeSideVectors(c);
    computeCubeDiagonals(c);
}
