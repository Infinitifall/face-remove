#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
// #include <assert.h>

#define DIVS 20
#define CL_SIZE_MAX 15000
#define ERROR 0.01


typedef struct {
    double cells[3][3];
} Matrix3;


typedef struct {
    double values[3];
} Vector3;


typedef struct {
    Vector3 p, s, r;
    char    f[6];

    // what the editor expects
    Vector3 p_fake;

    // compute to improve performance
    double diagonal_length;
    Vector3
        p_inverse_rotated,
        p_inverse_rotated_negative,
        side_vectors[3];
    Matrix3
        rotation_matrix,
        inverse_rotation_matrix;
} Cube;


// Matrix functions


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


// Vector functions


Vector3 newVector(const double *values) {
    Vector3 v;
    for (int i = 0; i < 3; i++) {
        v.values[i] = values[i];
    }
    return v;
}


// double taxicabDistance(const Vector3* v1, const Vector3* v2) {
//     double distance = 0;
//     for (int i = 0; i < 3; i++) {
//         distance += fabs(v1->values[i] - v2->values[i]);
//     }
//     return distance;
// }


double pythagoreanDistanceSquared(const Vector3* v1, const Vector3* v2) {
    double distance = 0;
    for (int i = 0; i < 3; i++) {
        double delta = (v1->values[i] - v2->values[i]);
        distance += delta * delta;
    }
    return distance;
}


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


Vector3 multiplyMatrixVector(const Matrix3* m, const Vector3* v) {
    Vector3 result = {0};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result.values[i] += m->cells[i][j] * v->values[j];
        }
    }
    return result;
}


// List functions


typedef struct {
    size_t length;
    Vector3* vectors;
} Vector3List;


typedef struct {
    size_t length;
    Cube** cubes;
} CubeList;


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


// Rotation functions


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


// Housekeeping functions


int getFakeFaceIndex(int face_index) {
    // assert((face_index >= 0) && (face_index < 6));
    const int lookup[6] = {4, 1, 2, 5, 0, 3};
    return lookup[face_index];
}


// Cube functions


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


// TODO: optimize this to give spiral points
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


// Read/write functions


CubeList* readInCubes(FILE *f) {
    CubeList *cl_1 = newCubeList(CL_SIZE_MAX);
    size_t cl_1_counter = 0;

    char *ptr;
    char t[100];
    while(true) {
        Cube *c = malloc(sizeof(*c));

        if (fscanf(f, "%s", c->f) == EOF) {
            free(c);
            break;
        }

        for (int i = 0; i < 3; i++) {
            fscanf(f, "%s", t);
            c->p_fake.values[i] = strtod(t, &ptr);
        }

        for (int i = 0; i < 3; i++) {
            fscanf(f, "%s", t);
            c->s.values[i] = strtod(t, &ptr);
        }

        for (int i = 0; i < 3; i++) {
            fscanf(f, "%s", t);
            c->r.values[i] = strtod(t, &ptr);
        }

        cl_1->cubes[cl_1_counter] = c;
        cl_1_counter += 1;
    }


    CubeList *cl_2 = newCubeList(cl_1_counter);
    for (size_t i = 0; i < cl_1_counter; i++) {
        cl_2->cubes[i] = cl_1->cubes[i];
    }

    freeCubeList(cl_1, false);

    return cl_2;
}


void printOutCubes(const CubeList* cl, FILE *f) {
    for (size_t i = 0; i < cl->length; i++) {
        fprintf(f, "%s\n", cl->cubes[i]->f);
    }
}


// The face remove algorithm


void removeFaces(CubeList* cl) {
    printf("|   Object #  |   Calcs    |  Hidden  |\n");
    printf("|-------------|------------|----------|\n");

    size_t face_count_old = 0;
    size_t face_count_new = 0;

    // populate fields for better performance
    for(size_t i = 0; i < cl->length ; i++) {
        computeCubeFields(cl->cubes[i]);
    }

    for(size_t i = 0; i < cl->length ; i++) {
        printf("|%5zu/%5zu", i + 1, cl->length);

        size_t checks = 0;
        int hidden = 0;

        Cube *c = cl->cubes[i];
        CubeList *cl_touching = getTouchingCubes(c, cl);

        for(int face = 0; face < 6 ; face++) {
            const int face_fake_index = getFakeFaceIndex(face);

            if (c->f[face_fake_index] == '0') {
                continue;  // face is disabled already; skip
            }

            // check if all four vertices are inside the same cube; easy wins
            Vector3List *vertex_points = getVertexPoints(c, face);
            bool covered_1 = false;
            for(size_t k = 0; k < cl_touching->length ; k++) {
                const Cube *c2 = cl_touching->cubes[k];
                bool all_inside = true;
                for (size_t j = 0; j < vertex_points->length; j++) {
                    if (!checkPointInsideCube(&vertex_points->vectors[j], c2)) {
                        all_inside = false;
                        break;
                    }
                }
                if (all_inside) {
                    covered_1 = true;
                    break;
                }
            }
            freeVectorList(vertex_points);
            if (covered_1) {
                cl->cubes[i]->f[face_fake_index] = '0';
                face_count_old += 1;
                hidden += 1;
                continue;
            }

            // check face points
            Vector3List *face_points = getFacePoints(c, face);
            bool covered_2 = true;
            for (size_t j = 0; j < face_points->length; j++) {
                bool covered_once = false;
                for(size_t k = 0; k < cl_touching->length ; k++) {
                    Cube *c2 = cl_touching->cubes[k];

                    if (checkPointInsideCube(&face_points->vectors[j], c2)) {
                        covered_once = true;
                        break;
                    }
                    checks += 1;
                }
                if (!covered_once) {
                    covered_2 = false;
                    break;
                }
            }
            freeVectorList(face_points);
            if (covered_2) {
                cl->cubes[i]->f[face_fake_index] = '0';
                face_count_old += 1;
                hidden += 1;
                continue;
            }

            face_count_old += 1;
            face_count_new += 1;
        }

        freeCubeList(cl_touching, false);

        if (checks < 1000) {
            printf("  |  %6zu    |   %d/6    |\n", checks, hidden);
        } else {
            printf("  |  %5.1fk    |   %d/6    |\n", checks/1000.0, hidden);
        }
    }

    printf("|-------------|------------|----------|\n");
    printf("|   Object #  |   Calcs    |  Hidden  |\n");
    printf("\n");
    printf("Old face count: %zu\n", face_count_old);
    printf("New face count: %zu\n", face_count_new);
    printf("Reduction: %.2f%%\n", (100.0 * (face_count_old - face_count_new) / face_count_old));
}


int main(int argc, char const *argv[]) {
    FILE *fptr_input = fopen("input.txt", "r");
    if (fptr_input == NULL) {
        printf("Couldn't open file input for reading! (Does it exist?)");
        return 1;
    }
    FILE *fptr_output = fopen("output.txt", "w");
    if (fptr_output == NULL) {
        printf("Couldn't open file output.txt for writing! (Do I have perms?)");
        return 2;
    }

    CubeList *cl = readInCubes(fptr_input);
    removeFaces(cl);
    printOutCubes(cl, fptr_output);
    freeCubeList(cl, true);

    fclose(fptr_input);
    fclose(fptr_output);
    return 0;
}
