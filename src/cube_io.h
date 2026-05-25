#include "cube.h"


#ifndef cube_io_h
#define cube_io


#define CL_SIZE_MAX 15000


CubeList* readInCubes(const char *s);
char* printOutCubes(const CubeList* cl);
int getFakeFaceIndex(int face_index);


#endif
