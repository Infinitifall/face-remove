#include <stdio.h>
#include <stdlib.h>

#include "cube_io.h"


CubeList* readInCubes(const char *s) {
    CubeList *cl_1 = newCubeList(CL_SIZE_MAX);
    size_t cl_1_counter = 0;

    char *ptr;
    char t[100];
    int consumed;
    const char *p = s;
    while(true) {
        Cube *c = malloc(sizeof(*c));

        if (sscanf(p, "%s%n", c->f_old, &consumed) != 1) {
            free(c);
            break;
        }
        p += consumed;

        for (int i = 0; i < 6; i++) {
            c->f_new[i] = c->f_old[i];
        }

        c->f_old[6] = '\0';
        c->f_new[6] = '\0';

        for (int i = 0; i < 3; i++) {
            sscanf(p, "%s%n", t, &consumed);
            p += consumed;
            c->p_fake.values[i] = strtod(t, &ptr);
        }

        for (int i = 0; i < 3; i++) {
            sscanf(p, "%s%n", t, &consumed);
            p += consumed;
            c->s.values[i] = strtod(t, &ptr);
        }

        for (int i = 0; i < 3; i++) {
            sscanf(p, "%s%n", t, &consumed);
            p += consumed;
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


char* printOutCubes(const CubeList* cl) {
    const int face_string_size = 6 + 1;  // 6 face chars and newline
    char *s = malloc((cl->length * face_string_size + 1) * sizeof(*s));
    char *p = s;
    for (size_t i = 0; i < cl->length; i++) {
        p += sprintf(p, "%s\n", cl->cubes[i]->f_new);
    }
    *p = '\0';
    return s;
}


int getFakeFaceIndex(int face_index) {
    // assert((face_index >= 0) && (face_index < 6));
    const int lookup[6] = {4, 1, 2, 5, 0, 3};
    return lookup[face_index];
}
