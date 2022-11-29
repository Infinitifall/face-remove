#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define DIVS 20
#define OBJ_COUNT_MAX 10000
#define ERROR 0.01


// Structs

typedef struct matrix {
    double cells[3][3];
} Matrix;


typedef struct vector {
    double cells[3];
} Vector;


typedef struct vector_list {
    int length;
    Vector list[(DIVS + 1) * (DIVS + 1)];
} VectorList;


typedef struct cube {
    Vector p;
    Vector s;
    Vector r;
    char f[6];
} Cube;


typedef struct cube_list {
    int length;
    Cube list[OBJ_COUNT_MAX];
} CubeList;


// Matrix functions


Matrix newMatrix() {
    Matrix result;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result.cells[i][j] = 0;
        }
    }
    return result;
}


Matrix newMatrixValues(double cells[3][3]) {
    Matrix result;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result.cells[i][j] = cells[i][j];
        }
    }
    return result;
}


Matrix matrixMultiply(Matrix matrix_1, Matrix matrix_2) {
    Matrix result = newMatrix();
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                result.cells[i][j] += matrix_1.cells[i][k] * matrix_2.cells[k][j];
            }
        }
    }
    return result;
}


// Vector functions


Vector newVector() {
    Vector result;
    for (int i = 0; i < 3; i++) {
        result.cells[i] = 0;
    }
    return result;
}


Vector newVectorValues(double cells[3]) {
    Vector result;
    for (int i = 0; i < 3; i++) {
        result.cells[i] = cells[i];
    }
    return result;
}


VectorList newVectorList() {
    VectorList vl;
    vl.length = 0;
    return vl;
}


Vector scaleVector(Vector vector, double scale) {
    Vector result;
    for (int i = 0; i < 3; i++) {
        result.cells[i] = vector.cells[i] * scale;
    }
    return result;
}

Vector addVectors(Vector vector_1, Vector vector_2) {
    Vector result = newVector();
    for (int i = 0; i < 3; i++) {
        result.cells[i] = vector_1.cells[i] + vector_2.cells[i];
    }
    return result;
}

Vector matrixVectorMultiply(Matrix matrix_1, Vector matrix_2) {
    Vector result = newVector();
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result.cells[i] += matrix_1.cells[i][j] * matrix_2.cells[j];
        }
    }
    return result;
}


Vector rotateVector3D(Vector vector, Vector r) {
    Vector result = newVector();
    for (int i = 0; i < 3; i++) {
        result.cells[i] = vector.cells[i];
    }

    double rX[3][3] = {
        {1, 0, 0},
        {0, cos(r.cells[0]), -sin(r.cells[0])},
        {0, sin(r.cells[0]), cos(r.cells[0])}
    };

    double rY[3][3] = {
        {cos(r.cells[1]), 0, sin(r.cells[1])},
        {0, 1, 0},
        {-sin(r.cells[1]), 0, cos(r.cells[1])}
    };

    double rZ[3][3] = {
        {cos(r.cells[2]), -sin(r.cells[2]), 0},
        {sin(r.cells[2]), cos(r.cells[2]), 0},
        {0, 0, 1}
    };

    Matrix temp_1 = matrixMultiply(newMatrixValues(rX), newMatrixValues(rY));
    Matrix temp_2 = matrixMultiply(temp_1, newMatrixValues(rZ));
    result = matrixVectorMultiply(temp_2, result);

    return result;
}


Vector rotateVector3DInverse(Vector vector, Vector r) {
    Vector result = newVector();
    for (int i = 0; i < 3; i++) {
        result.cells[i] = vector.cells[i];
    }

    double rX[3][3] = {
        {1, 0, 0},
        {0, cos(r.cells[0]), sin(r.cells[0])},
        {0, -sin(r.cells[0]), cos(r.cells[0])}
    };

    double rY[3][3] = {
        {cos(r.cells[1]), 0, -sin(r.cells[1])},
        {0, 1, 0},
        {sin(r.cells[1]), 0, cos(r.cells[1])}
    };

    double rZ[3][3] = {
        {cos(r.cells[2]), sin(r.cells[2]), 0},
        {-sin(r.cells[2]), cos(r.cells[2]), 0},
        {0, 0, 1}
    };

    Matrix temp_1 = matrixMultiply(newMatrixValues(rZ), newMatrixValues(rY));
    Matrix temp_2 = matrixMultiply(temp_1, newMatrixValues(rX));
    result = matrixVectorMultiply(temp_2, result);
    
    return result;
}


// Cube functions


Cube newCube() {
    Cube c;
    strcpy(c.f, "");
    c.p = newVector();
    c.s = newVector();
    c.r = newVector();
    return c;
}


CubeList newCubeList() {
    CubeList cl;
    cl.length = 0;
    return cl;
}


VectorList getSideVectors(Cube c) {
    double s1[] = {c.s.cells[0], 0, 0};
    Vector temp_1 = rotateVector3D(newVectorValues(s1), c.r);

    double s2[] = {0, c.s.cells[1], 0};
    Vector temp_2 = rotateVector3D(newVectorValues(s2), c.r);

    double s3[] = {0, 0, c.s.cells[2]};
    Vector temp_3 = rotateVector3D(newVectorValues(s3), c.r);

    VectorList result;
    result.list[0] = temp_1;
    result.list[1] = temp_2;
    result.list[2] = temp_3;
    result.length = 3;

    return result;
}


VectorList getFacePoints(Cube c, int face) {
    VectorList side_vectors = getSideVectors(c);

    Vector vector_1 = side_vectors.list[face % 3];
    Vector vector_2 = side_vectors.list[(face + 1) % 3];
    Vector vector_3 = side_vectors.list[(face + 2) % 3];
    int sign = (face % 2) ? -1 : 1;

    Vector offset = addVectors(scaleVector(vector_1, -0.5), scaleVector(vector_2, -0.5));
    offset = addVectors(offset, scaleVector(vector_3, 0.5 * sign));
    offset = addVectors(offset, c.p);
    Vector vector_1_div = scaleVector(vector_1, 1.0 / DIVS);
    Vector vector_2_div = scaleVector(vector_2, 1.0 / DIVS);

    VectorList points = newVectorList();
    for (int i = 0; i <= DIVS; i++) {
        for (int j = 0; j <= DIVS; j++) {
            Vector temp_vector = addVectors(scaleVector(vector_1_div, i), scaleVector(vector_2_div, j));
            temp_vector = addVectors(temp_vector, offset);
            points.list[points.length] = temp_vector;
            points.length += 1;
        }   
    }
    return points;
}


int checkPointInsideCube(Vector v, Cube c) {
    Vector new_point = rotateVector3DInverse(v, c.r);
    Vector new_p = rotateVector3DInverse(c.p, c.r);
    new_point = addVectors(new_point, scaleVector(new_p, -1));
    if (
        (-c.s.cells[0]/2 <= (new_point.cells[0] + ERROR)) && ((new_point.cells[0] - ERROR) <= c.s.cells[0]/2) &&
        (-c.s.cells[1]/2 <= (new_point.cells[1] + ERROR)) && ((new_point.cells[1] - ERROR) <= c.s.cells[1]/2) &&
        (-c.s.cells[2]/2 <= (new_point.cells[2] + ERROR)) && ((new_point.cells[2] - ERROR) <= c.s.cells[2]/2)
    ) {
        return 1;
    } else {
        return 0;
    }
}


// Housekeeping functions


Cube getRealP(Cube c) {
    double s2[3] = {0, c.s.cells[1] / 2,0};
    c.p = addVectors(c.p, rotateVector3D(newVectorValues(s2), c.r));
    return c;
}


int getFakeFaceIndex(int face_index) {
    switch(face_index) {
        case 0:
            return 4;
        case 1:
            return 1;
        case 2:
            return 2;
        case 3:
            return 5;
        case 4:
            return 0;
        case 5:
            return 3;
        default:
            return 0;
    }
}


// Read/write functions


CubeList readInCubes(FILE *f) {
    CubeList cl;
    cl.length = 0;

    char *ptr;
    char t[100];
    while(1) {
        Cube c;

        if (fscanf(f, "%s", c.f) == EOF) {
            break;
        }

        for (int i = 0; i < 3; i++) {
            fscanf(f, "%s", t);
            c.p.cells[i] = strtod(t, &ptr);
        }

        for (int i = 0; i < 3; i++) {
            fscanf(f, "%s", t);
            c.s.cells[i] = strtod(t, &ptr);
        }

        for (int i = 0; i < 3; i++) {
            fscanf(f, "%s", t);
            c.r.cells[i] = strtod(t, &ptr);
        }

        cl.list[cl.length] = c;
        cl.length += 1;
    }
    return cl;
}


void printOutCubes(CubeList cl, FILE *f) {
    for (int i = 0; i < cl.length; i++) {
        fprintf(f, "%s\n", cl.list[i].f);
    }
}


// The face remove algorithm


CubeList removeFaces(CubeList cl) {
    int face_count_old = 0;
    int face_count_new = 0;
    printf("   Object    Checks    Hidden\n");
    for(int i = 0; i < cl.length ; i++) {
        printf("%4d/%4d", i + 1, cl.length);
        int checks = 0;
        int hidden = 0;
        Cube c = cl.list[i];
        c = getRealP(c);
        for(int face = 0; face < 6 ; face++) {
            int face_fake = getFakeFaceIndex(face);
            if (c.f[face_fake] == '0') {
                continue;
            }
            VectorList face_points = getFacePoints(c, face);
            int covered = 1;
            for (int j = 0; j < face_points.length; j++) {
                int covered_once = 0;
                for(int k = 0; k < cl.length ; k++) {
                    if (i == k) {
                        continue;
                    }
                    Cube c2 = cl.list[k];
                    c2 = getRealP(c2);
                    if (checkPointInsideCube(face_points.list[j], c2)) {
                        covered_once = 1;
                        break;
                    }
                    checks += 1;
                }
                if (covered_once == 0) {
                    covered = 0;
                    break;
                }
            }
            if (covered) {
                cl.list[i].f[face_fake] = '0';
                face_count_old += 1;
                hidden += 1;
            } else {
                face_count_old += 1;
                face_count_new += 1;
            }
        }
        printf("    %6.1fk    %d/6\n", checks/1000.0, hidden);
    }
    printf("\n");
    printf("Old face count: %d\n", face_count_old);
    printf("New face count: %d\n", face_count_new);
    printf("Reduction: %.2f%%\n", (100.0 * (face_count_old - face_count_new) / face_count_old));
    return cl;
}


int main(int argc, char const *argv[]) {
    FILE *fptr_input = fopen("input.txt", "r");
    FILE *fptr_output = fopen("output.txt", "w");

    CubeList cl = readInCubes(fptr_input);
    cl = removeFaces(cl);
    printOutCubes(cl, fptr_output);

    fclose(fptr_input);
    fclose(fptr_output);
    return 0;
}
