/* Define normals for the cube */
GLbyte CuboNormals[24 * 3] = {
    /* top */
    0,0,1,
    0,0,1,
    0,0,1,
    0,0,1,

    /* front */
    1,0,0,
    1,0,0,
    1,0,0,
    1,0,0,

    /* right */
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,

    /* left */
    0,-1,0,
    0,-1,0,
    0,-1,0,
    0,-1,0,

    /* back */
    -1,0,0,
    -1,0,0,
    -1,0,0,
    -1,0,0,

    /* bottom */
    0,0,-1,
    0,0,-1,
    0,0,-1,
    0,0,-1
};

/* Define indices for drawing the triangles ( 5 sides of the cube )
   Nasa Hubble texture is drawn on these sides. */
GLushort CuboTriangles[12 * 3] = {
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
#define tex(u,v) (GLbyte)( (u) - 128 ) , (GLbyte)( (v) - 128 )

/*class CPrimitiva {
    public:
        //GLshort* CuboVertices(TInt x, TInt y, TInt z);
        GLushort* CuboFaces();
        GLbyte* CuboNormals();
        GLbyte* CuboUV();
};*/

GLshort CuboVertices[24 * 3] = {
	/* top */
	-2500,  2500,  2500,
	2500,  2500,  2500,
	2500, -2500,  2500,
	-2500, -2500,  2500,

	/* front */
	2500,  2500,  2500, //4
	2500,  2500, -2500,
	2500, -2500, -2500,
	2500, -2500,  2500,

	/* right */
	-2500,  2500,  2500, //8
	-2500,  2500, -2500,
	2500,  2500, -2500,
	2500,  2500,  2500, //11

	/* left */
	2500, -2500,  2500,
	2500, -2500, -2500,
	-2500, -2500, -2500,
	-2500, -2500,  2500,

	/* back */
	-2500, -2500,  2500,
	-2500, -2500, -2500,
	-2500,  2500, -2500,
	-2500,  2500,  2500, //19

	/* bottom */
	-2500,  2500, -2500,
	2500,  2500, -2500,
	2500, -2500, -2500,
	-2500, -2500, -2500
};

GLbyte CuboUV[24 * 2] = {
	/* top, whole texture nasa_hubble.h */
	tex(0,0),
	tex(255,0),
	tex(255,255),
	tex(0,255),

	/* front, spiral with tail */
	tex(0,255),
	tex(127,255),
	tex(127,127),
	tex(0,127),

	/* right, red nebulae */
	tex(127,255),
	tex(255,255),
	tex(255,127),
	tex(127,127),

	/* left, plasma cloud */
	tex(0,127),
	tex(127,127),
	tex(127,0),
	tex(0,0),

	/* back, 2 spirals */
	tex(127,127),
	tex(255,127),
	tex(255,0),
	tex(127,0),

	/* bottom, whole texture ogles.jpg */
	tex(255,255),
	tex(255,0),
	tex(0,0),
	tex(0,255)
};
