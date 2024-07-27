//para los sprites
static const GLshort pointVertex[3]={0, 0, 0};

static const TInt LineaLightVertexSize = 2 * 3;
static const TInt LineaEdgeSize = 1 * 2;

static GLshort LineaLightVertex[LineaLightVertexSize] = {    
    0, 0, 0,
    0, -30000, 0,
};

static const GLshort LineaTimeline[6] = {    
    0, 0, 0,
    0, 16, 0,
};

// Define los índices de las líneas
static const GLushort LineaEdge[2] = {
    0, 1
};

static const TInt CameraVertexSize = 8 * 3;
static const TInt CameraEdgesSize = 11 * 2;

static const GLshort CameraVertices[CameraVertexSize] = {    
    0, 0, 0, //origen
    3500, 1440, 1920,
    3500, -1440, 1920,
    3500, -1440, -1920,
    3500, 1440, -1920,
    //tiangulo
    3500, 1700, 1280,
    3500, 1700, -1280,
    3500, 3100, 0,
};

static const GLushort CameraEdges[CameraEdgesSize]={
    //cono
	0, 1,
	0, 2,
	0, 3,
	0, 4,
    //tapa de enfrente    
	1, 2,
	2, 3,
	3, 4,
	4, 1,
    //triangulo
	5, 6,
	6, 7,
	7, 5,
};

static const TInt EmptyVertexSize = 6 * 3;
static const TInt EmptyEdgesSize = 3 * 2;

static const GLshort EmptyVertices[EmptyVertexSize] = {
    //eje1
    0, 0, -2000,
    0, 0, 2000,
    //eje2
    0, -2000, 0,
    0, 2000, 0,
    //eje3
    -2000, 0, 0,
    2000, 0, 0,
};

static const GLushort EmptyEdges[EmptyEdgesSize]={
	0, 1,
	2, 3,
	4, 5
};

static const TInt Cursor3DVertexSize = 12 * 3;
static const TInt Cursor3DEdgesSize = 6 * 2;

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

static const TInt SpriteVertexSize = 16 * 3;
static const TInt SpriteFacesSize = 18 * 3;
static const TInt SpriteUvSize = 16 * 2;

/* Define vertice coordinates for the ground plane */
//en el sentido del reloj empezando de la esquina superior izquierda
static GLshort SpriteVertices[SpriteVertexSize] = {
	0,    0, 0,
	256,    0, 0,
    256, 256, 0,
    0, 256, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
};

/** los dos triangulos */
static const GLushort SpriteFaces[SpriteFacesSize]={
    //1
	0,1,2,
	2,3,0,
    //2
	1,4,2,
	2,5,4,
    //3
    4,5,6,
    5,6,7,
    //4
    3,2,8,
    8,2,9,
    //5
    2,9,5,
    9,10,5,
    //6
    10,5,7,
    10,11,7,
    //7
    8,12,9,
    12,9,13,
    //8
    13,9,10,
    13,10,14,
    //9
    14,10,11,
    14,11,15
};

/* textura UV */
//en el sentido del reloj empezando de la esquina superior izquierda
static GLbyte SpriteUV[SpriteUvSize] ={
    -128,  -128,
    127,   -128,
    127,    127,
    -128,   127,
    0,0,
    0,0,
    0, 0, 
    0, 0, 
    0, 0, 
    0, 0, 
    0, 0, 
    0, 0, 
    0, 0, 
    0, 0, 
    0, 0, 
    0, 0 
};
//f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ...