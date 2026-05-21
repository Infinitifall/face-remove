#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// #include <emscripten.h>

#include "face-remove.h"
#include "cube_io.h"
#include "file_io.h"


int getFakeFaceIndex(int face_index) {
    // assert((face_index >= 0) && (face_index < 6));
    const int lookup[6] = {4, 1, 2, 5, 0, 3};
    return lookup[face_index];
}


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

            // check if every vertex is inside the same cube; easy wins
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

            // check if every point in face points grid is covered
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


char* removeFacesWrapper(const char* s_input) {
    CubeList *cl = readInCubes(s_input);
    removeFaces(cl);
    char *s_output = printOutCubes(cl);
    freeCubeList(cl, true);
    return s_output;
}


int main() {
    char *s_input = slurpFile("data/input.txt");
    char *s_output = removeFacesWrapper(s_input);
    spitFile("data/output.txt", s_output);
    free(s_input);
    free(s_output);
    return 0;
}
