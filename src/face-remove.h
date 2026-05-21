#include "cube.h"


#ifndef face_remove_h
#define face_remove_h


int getFakeFaceIndex(int face_index);
void removeFaces(CubeList* cl);
char* removeFacesWrapper(const char* s_input);


int main();


#endif
