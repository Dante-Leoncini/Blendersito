//constantes
#define CuboVertexSize 24 * 3
#define CuboNormalsSize 24 * 3
#define CuboFacesSize 12 * 3
#define CuboEdgesSize 12 * 6
#define CuboUvSize 24 * 2

/* Define normals for the cube */
static const GLbyte CuboNormals[CuboNormalsSize] = {
    /* top */
    0,0,127,
    0,0,127,
    0,0,127,
    0,0,127,

    /* front */
    127,0,0,
    127,0,0,
    127,0,0,
    127,0,0,

    /* right */
    0,127,0,
    0,127,0,
    0,127,0,
    0,127,0,

    /* left */
    0,-128,0,
    0,-128,0,
    0,-128,0,
    0,-128,0,

    /* back */
    -128,0,0,
    -128,0,0,
    -128,0,0,
    -128,0,0,

    /* bottom */
    0,0,-128,
    0,0,-128,
    0,0,-128,
    0,0,-128
};

/* Define indices for drawing the triangles ( 5 sides of the cube )
   Nasa Hubble texture is drawn on these sides. */
static const GLushort CuboTriangles[CuboFacesSize] = {
    /* top */
     1,0,3,
     1,3,2,

     /* front */
     5,4,7,
     5,7,6,

     /* right */
     9,8,11,
     9,11,10,

     /* left */
     13,12,15,
     13,15,14,

     /* back */
     17,16,19,
     17,19,18,
     
     /* bottom */
     21,22,23,
     21,23,20
};

/* Macro for changing the input texture coordinate values from
   GLubyte [0,255] to GLbyte [-128,127]. See more info below. */
//#define tex(u,v) (GLbyte)( (u) - 128 ) , (GLbyte)( (v) - 128 )

static const GLshort CuboVertices[CuboVertexSize] = {
	/* top */
	-2000,  2000,  2000,
	2000,  2000,  2000,
	2000, -2000,  2000,
	-2000, -2000,  2000,

	/* front */
	2000,  2000,  2000, //4
	2000,  2000, -2000,
	2000, -2000, -2000,
	2000, -2000,  2000,

	/* right */
	-2000,  2000,  2000, //8
	-2000,  2000, -2000,
	2000,  2000, -2000,
	2000,  2000,  2000, //11

	/* left */
	2000, -2000,  2000,
	2000, -2000, -2000,
	-2000, -2000, -2000,
	-2000, -2000,  2000,

	/* back */
	-2000, -2000,  2000,
	-2000, -2000, -2000,
	-2000,  2000, -2000,
	-2000,  2000,  2000, //19

	/* bottom */
	-2000,  2000, -2000,
	2000,  2000, -2000,
	2000, -2000, -2000,
	-2000, -2000, -2000
};

static const GLbyte CuboUV[CuboUvSize] = {
	//derecha ok
	-128,-128,
	127,-128,
	127,127,
	-128,127,

	//atras ok
	-128,-128,
	127,-128,
	127,127,
	-128,127,

	//top ok
	127,127,
	-128,127,
	-128,-128,
	127,-128,

	//abajo ok
	127,127,
	-128,127,
	-128,-128,
	127,-128,

	//frente
	127,127,
	-128,127,
	-128,-128,
	127,-128,

	//izquierda
	127,-128,
	-128,-128,
	-128,127,
	127,127,
};
