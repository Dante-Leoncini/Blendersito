//constantes
const TInt SpriteVertexSize = 4 * 3;
const TInt SpriteNormalsSize = 4 * 3;
const TInt SpriteFacesSize = 2 * 3;
const TInt SpriteUvSize = 4 * 2;

const TInt Cursor3DVertexSize = 12 * 3;
const TInt Cursor3DEdgesSize = 6 * 2;

static const GLshort Cursor3DVertices[Cursor3DVertexSize] = {
    //eje1
    0, 0, -750,
    0, 0, -225,
    0, 0, 750,
    0, 0, 225,
    //eje2
    0, -750, 0,
    0, -225, 0,
    0, 750, 0,
    0, 225, 0,
    //eje3
    -750, 0, 0,
    -225, 0, 0,
    750, 0, 0,
    225, 0, 0,
};

static const GLushort Cursor3DEdges[Cursor3DEdgesSize]={
	0, 1,
	2, 3,
	4, 5,
	6, 7,
	8, 9,
	10,11
};

/* Define vertice coordinates for the ground plane */
//en el sentido del reloj empezando de la esquina superior izquierda
static GLshort SpriteVertices[SpriteVertexSize] = {
	0,    0, 0,
	256,    0, 0,
    256, -256, 0,
    0, -256, 0
};

/* las normales de cada vertice */
static const GLbyte SpriteNormal[SpriteNormalsSize]={
    0,127,0,
    0,127,0,
    0,127,0,
    0,127,0
};

/** los dos triangulos */
static const GLushort SpriteFaces[SpriteFacesSize]={
	0,1,2,
	2,3,0
};

/* textura UV */
//en el sentido del reloj empezando de la esquina superior izquierda
static GLbyte SpriteUV[SpriteUvSize] ={
    -128,  -128,
    127,   -128,
    127,    127,
    -128,   127
};
//f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ...