#include "vertices.h"


// prostiremo se od -1 do 1 x,  0 do 1 y,  -0.6 do 0.6 z

float floor_vertices[48] =
{
    //X     Y      Z       NX    NY     NZ  TX  TY
   -1.0f, -0.2f, -2.0f,  0.0f,  1.0f, 0.0f, 0.0f, 0.0f,
   -1.0f, -0.4f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, -0.4f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, 1.0f,

   -1.0f, -0.2f, -2.0f,  0.0f,  1.0f, 0.0f, 0.0f, 0.0f,
    1.0f, -0.4f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, -0.2f, -2.0f,  0.0f,  1.0f, 0.0f, 1.0f, 0.0f,
};


unsigned int floor_stride = (3 + 3 + 2) * sizeof(float);

float name_vertices_2d[] = {
    // X      Y      Z      NX    NY    NZ    TX    TY
    -0.98,  0.4,   0.0,   0.0,  0.0,  1.0,  0.0,  1.0, // vertex 1
    -0.7,   0.4,   0.0,   0.0,  0.0,  1.0,  1.0,  1.0, // vertex 2
    -0.98, -0.2,   0.0,   0.0,  0.0,  1.0,  0.0,  0.0, // vertex 3
    -0.7,  -0.2,   0.0,   0.0,  0.0,  1.0,  1.0,  0.0, // vertex 4
};

float name_vertices[] = {
    //X     Y      Z       NX    NY     NZ  TX  TY
   -0.5f, -0.9f, -0.8f,  0.0f,  1.0f, 0.0f, 0.0f, 1.0f,
   -0.5f, -0.9f,  0.8f,  0.0f,  1.0f, 0.0f, 0.0f, 0.0f,
    0.5f, -0.9f,  0.8f,  0.0f,  1.0f, 0.0f, 1.0f, 0.0f,
   -0.5f, -0.9f, -0.8f,  0.0f,  1.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.9f,  0.8f,  0.0f,  1.0f, 0.0f, 1.0f, 0.0f,
    0.5f, -0.9f, -0.8f,  0.0f,  1.0f, 0.0f, 1.0f, 1.0f,
};





//float name_vertices[] = {
//-0.9,  0.4, 0.0, 1.0, // Bottom-left
//-0.6,  0.4, 1.0, 1.0, // Bottom-right
//-0.9,  0.8, 0.0, 0.0, // Top-left
//-0.6,  0.8, 1.0, 0.0  // Top-right
//};


//unsigned int green_shield_stride = (2 + 2) * sizeof(float);

unsigned int name_stride = (3 + 3 + 2) * sizeof(float);

unsigned int name_indices[] =
{
    0, 1, 2,
    1, 2, 3,
};

float cube_vertices[] = {
1.0f, 1.0f, 0.5f, 0.0f, 0.0f, 1.0f,// L D
0.5f, 1.0f, 0.5f, 0.0f, 0.0f, 1.0f,// R D
1.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,// L U
0.5f, 1.0f, 0.5f, 0.0f, 0.0f, 1.0f,// R D
0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,// R U
1.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,// L U
// LEFT SIDE
1.0f, 1.0f, -0.5f, -1.0f, 0.0f, 0.0f, // L D
1.0f, 1.0f, 0.5f, -1.0f, 0.0f, 0.0f, // R D
1.0f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, // L U
1.0f, 1.0f, 0.5f, -1.0f, 0.0f, 0.0f, // R D
1.0f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, // R U
1.0f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, // L U
// RIGHT SIDE
0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 0.0f,// L D
0.5f, 1.0f, -0.5f, 1.0f, 0.0f, 0.0f,// R D
0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,// L U
0.5f, 1.0f, -0.5f, 1.0f, 0.0f, 0.0f,// R D
0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,// R U
0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,// L U
// BOTTOM SIDE
1.0f, 1.0f, -0.5f, 0.0f, -1.0f, 0.0f,// L D
0.5f, 1.0f, -0.5f, 0.0f, -1.0f, 0.0f,// R D
1.0f, 1.0f, 0.5f, 0.0f, -1.0f, 0.0f,// L U
0.5f, 1.0f, -0.5f, 0.0f, -1.0f, 0.0f,// R D
0.5f, 1.0f, 0.5f, 0.0f, -1.0f, 0.0f,// R U
1.0f, 1.0f, 0.5f, 0.0f, -1.0f, 0.0f,// L U
// TOP SIDE
1.0f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, // L D
0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, // R D
1.0f, 0.5f,-0.5f, 0.0f, 1.0f, 0.0f, // L U
0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, // R D
0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // R U
1.0f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // L U
// BACK SIDE
0.5f, 1.0f, -0.5f, 0.0f, 0.0f, -1.0f,// L D
1.0f, 1.0f, -0.5f, 0.0f, 0.0f, -1.0f, // R D
0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, // L U
1.0f, 1.0f, -0.5f, 0.0f, 0.0f, -1.0f, // R D
1.0f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, // R U
0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f // L U
};

unsigned int stride = (3 + 3) * sizeof(float);