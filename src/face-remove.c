#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "face-remove.h"
#include "cube_io.h"
#ifndef __EMSCRIPTEN__
#include "file_io.h"
#else
#include <emscripten.h>  // for EMSCRIPTEN_KEEPALIVE; alternatively use -s "EXPORTED_FUNCTIONS
#endif


void removeFaces(CubeList* cl) {
    printf("| Obj # |  Old   |  New   |\n");
    printf("|-------|--------|--------|\n");

    // populate fields for better performance
    for(size_t i = 0; i < cl->length ; i++) {
        computeCubeFields(cl->cubes[i]);
    }

    for(size_t i = 0; i < cl->length ; i++) {
        Cube *c = cl->cubes[i];
        CubeList *cl_touching = getTouchingCubes(c, cl);

        for(int face = 0; face < 6 ; face++) {
            const int face_fake_index = getFakeFaceIndex(face);

            if (c->f_new[face_fake_index] == '0') {
                // face is disabled already; skip
                continue;
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
                c->f_new[face_fake_index] = '0';
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
                }
                if (!covered_once) {
                    covered_2 = false;
                    break;
                }
            }
            freeVectorList(face_points);
            if (covered_2) {
                c->f_new[face_fake_index] = '0';
                continue;
            }
        }

        freeCubeList(cl_touching, false);

        if (strcmp(c->f_old, c->f_new)) {
            printf("|%6zu | %s | %s |\n", i + 1, c->f_old, c->f_new);
        }
    }

    size_t face_count_old = faceCount(cl, true);
    size_t face_count_new = faceCount(cl, false);

    printf("|-------|--------|--------|\n");
    printf("| Obj # |  Old   |  New   |\n");
    printf("\n");
    printf(
        "Faces disabled: %.2f%%  (%zu out of %zu)\n",
        (100.0 * (face_count_old - face_count_new) / face_count_old),
        face_count_old - face_count_new,
        face_count_old
    );
}


#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif
char* removeFacesWrapper(const char* s_input) {
    CubeList *cl = readInCubes(s_input);
    removeFaces(cl);
    char *s_output = printOutCubes(cl);
    freeCubeList(cl, true);
    return s_output;
}


#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif
double test_wasm() {
    printf("Hello World from C :)\n");  // end \n is important!
    return 3.14;
}


#ifndef __EMSCRIPTEN__
int main() {
    char *s_input = slurpFile("data/input.txt");
    char *s_output = removeFacesWrapper(s_input);
    spitFile("data/output.txt", s_output);
    free(s_input);
    free(s_output);
    return 0;
}
#endif
