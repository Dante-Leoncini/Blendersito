// MACROS
//#define MATERIAL_MAX 1
#define spriteVerticesModel    4
#define spriteFacesModel       6

/* Define vertice coordinates for the ground plane */
static const GLshort spriteVertices[4 * 3] = {
    +500,0,-500,
    +500,0,+500,
    -500,0,+500,
    -500,0,-500
};

/* las normales de cada vertice */
static const GLbyte spriteNormal[4 * 3]={
    0,127,0,
    0,127,0,
    0,127,0,
    0,127,0
};

/** los dos triangulos */
static const GLushort spriteFaces[2 * 3]={
	0,2,1,
	0,3,2
};

/* textura UV */
static const GLbyte spriteUV[4 * 2] ={
     127,-127,
     127, 127,
    -127, 127,
    -127,-127
};
