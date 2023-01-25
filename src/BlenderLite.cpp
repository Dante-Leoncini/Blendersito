/*
 * ==============================================================================
 *  Name        : BlenderLite.cpp
 *  Part of     : OpenGLEx / BlenderLite
 * ==============================================================================
 */

// INCLUDES
#include <e32std.h>
#include <e32math.h>

//para leer archivos
#include <s32file.h>

//debug
//#include <e32cons.h>
//LOCAL_D CConsoleBase* console;
#include "BlenderLite.h"
#include "Mesh.h"

#include "claude.h" //modelo 3d
#include "mono.h" //modelo 3d
//#include "marcus.h" //modelo 3d
#include "Floor.h" //modelo 3d
#include "sprite.h" //sprite 2D
#include "Primitivas.h" //sprite 2D

//#include "blenderlite.hrh"
#include <blenderlite.rsg>
#include <aknmessagequerydialog.h>		// DialogAlertL
#include <aknnotewrappers.h> 
#include <akncommondialogs.h>

typedef GLshort Edge[2][3];

// MACROS
#define MATERIAL_MAX 1
#define LIGHT_MAX    1

#define cameraDistance 270

#define MATERIALCOLOR(r, g, b, a)     \
       (GLfloat)(r * MATERIAL_MAX),   \
       (GLfloat)(g * MATERIAL_MAX),   \
       (GLfloat)(b * MATERIAL_MAX),   \
       (GLfloat)(a * MATERIAL_MAX)

#define LIGHTCOLOR(r, g, b, a)        \
       (GLfloat)(r * LIGHT_MAX),      \
       (GLfloat)(g * LIGHT_MAX),      \
       (GLfloat)(b * LIGHT_MAX),      \
       (GLfloat)(a * LIGHT_MAX)

// CONSTANTS
/* Materials for the Models object. */
static const GLfloat objDiffuse[4] = { MATERIALCOLOR(0.8, 0.8, 0.8, 1.0) };
static const GLfloat objAmbient[4] = { MATERIALCOLOR(0.4, 0.4, 0.4, 1.0) };

//color vertices
static const GLfloat colorVertice[4]  = { MATERIALCOLOR(0.87, 0.44, 0.06, 1.0) };
//color borde
static const GLfloat colorBorde[4]  = { MATERIALCOLOR(0.68, 0.45, 0.13, 1.0) };
//color borde Select
//GLfloat colorBordeSelect[4] = { MATERIALCOLOR(0.94, 0.59, 0.17, 1.0) };
//array de colores
static const GLfloat ListaColores[5][4] = {
		{ MATERIALCOLOR(1.0, 1.0, 1.0, 1.0) },   //blanco
		{ MATERIALCOLOR(0.94, 0.59, 0.17, 1.0)}, //naranja 	
		{ MATERIALCOLOR(0.0, 0.0, 0.0, 1.0) },    //negro
		{ MATERIALCOLOR(0.12, 0.12, 0.12, 1.0) },   //gris
		{ MATERIALCOLOR(0.94, 0.59, 0.17, 0.25f) }       //naranja transparente

};
enum{
	blanco,
	naranja,
	negro,
	gris,
	naranjaFace
};
int colorBordeSelect = 1;

//Piso
static const GLfloat LineaPiso[4]  =      { MATERIALCOLOR(0.29, 0.29, 0.29, 1.0) };
static const GLfloat LineaPisoRoja[4]  =  { MATERIALCOLOR(0.56, 0.23, 0.28, 1.0) };
static const GLfloat LineaPisoVerde[4]  = { MATERIALCOLOR(0.38, 0.53, 0.15, 1.0) };

//ejes transformacion
static const GLfloat ColorTransformX[4]  = { MATERIALCOLOR(0.88, 0.48, 0.54, 1.0) };
static const GLfloat ColorTransformY[4]  = { MATERIALCOLOR(0.65, 0.81, 0.38, 1.0) };
static const GLfloat ColorTransformZ[4]  = { MATERIALCOLOR(0.46, 0.67, 0.89, 1.0) };

/* Global ambient light. */
static const GLfloat globalAmbient[4]   = { LIGHTCOLOR(0.0, 0.0, 0.0, 1.0) };

/* Lamp parameters. */
static const GLfloat lightDiffuseLamp[4]   = { LIGHTCOLOR(0.8, 0.8, 0.8, 1.0) };
static const GLfloat lightAmbientLamp[4]   = { LIGHTCOLOR(0.4, 0.4, 0.4, 1.0) };
GLfloat lightPositionLamp[4]  = { -10, -5, 50, 0 }; // x, y, z

/* Spotlight parameters. */
static const GLfloat lightDiffuseSpot[4]   = { LIGHTCOLOR(0.0, 0.0, 0.0, 1.0) };
static const GLfloat lightAmbientSpot[4]   = { LIGHTCOLOR(0.0, 0.0, 0.0, 1.0) };
static const GLfloat lightSpecularSpot[4]  = { LIGHTCOLOR(0.0, 0.0, 3.0, 1.0) };
static const GLfloat lightPositionSpot[4]  = {  0, -10, -1, 0 };
static const GLfloat lightDirectionSpot[4] = {  0,  10,  1, 1 };

//camara
bool ortografica = false;
GLfloat aspectRatio;
GLfloat rotX = 153.5;
GLfloat rotY = 20.0; //66.2
GLfloat posX = 0;
GLfloat posY = 0;
GLfloat posZ = 0;

//solo redibuja si este valor esta en true
bool redibujar = true;

//interpolacion
enum {lineal, closest};
enum{
	Rendered,
	Texture,
	Solid,
	Wireframe
};

enum{
	navegacion,
	rotacion,
	escala,
	translacion, 
	translacionVertex,
	translacionEdge,
	translacionFace,
	edicion
};

enum{
	vertexSelect, edgeSelect, faceSelect
};

enum{
	cubo, esfera, cilindro, plano, vacio, camara,
    cad, luz, suzanne, claude, marcus, vertice
};

CPrimitiva Primitivas;

typedef enum { X, Y, Z, XYZ } Axis;

int estado = navegacion;
int view = Rendered;
int axisSelect = X;

class SaveState {
	public:
		GLfloat posX;
		GLfloat posY;
		GLfloat posZ;
		GLfloat rotX;
		GLfloat rotY;
		GLfloat rotZ;
		GLfixed scaleX;
		GLfixed scaleY;
		GLfixed scaleZ;
};

SaveState estadoObj;

//Crea un array de objetos
RArray<Mesh> Objetos;
TInt objSelect = 0;
TInt tipoSelect = vertexSelect;
TInt EditSelect = 0;
GLshort estadoVertex[3]={0, 0, 0};

void CBlenderLite::changeSelect(){
	if (estado == navegacion){
		EditSelect = 0;
		if (objSelect+1 > Objetos.Count()-1){
			objSelect = 0;
		}
		else {
			objSelect++;
		}
	}
	else if (estado == edicion){
		EditSelect++;
		if (tipoSelect == vertexSelect && EditSelect >= Objetos[objSelect].vertexGroupSize){
			EditSelect = 0;			
		}	
		else if (tipoSelect == edgeSelect && EditSelect >= Objetos[objSelect].facesSize/2){
			EditSelect = 0;			
		}	
		else if (tipoSelect == faceSelect && EditSelect >= Objetos[objSelect].facesSize/3){
			EditSelect = 0;			
		}	
	};
    redibujar = true;	
}

void CBlenderLite::SetTipoSelect(TInt tipo){
	if (estado != edicion){return;}
	tipoSelect = tipo;
	EditSelect = 0;	
    redibujar = true;
}

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// sin
// Help function to make the duck 'quak'.
// -----------------------------------------------------------------------------
//
GLfloat CBlenderLite::sin(GLfloat aRad){
    TReal trg, src = (TReal)aRad;
    if ( Math::Sin(trg, src) == KErrNone )
        {
        return (GLfloat)trg;
        }
    return 0;
}


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CBlenderLite::CBlenderLite
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CBlenderLite::CBlenderLite( TUint aWidth, TUint aHeight, CBlenderLiteInput* aInputHandler )
    : CFiniteStateMachine()
	{
    iScreenWidth  = aWidth;
    iScreenHeight = aHeight;
    iInputHandler = aInputHandler;
	}

// -----------------------------------------------------------------------------
// CBlenderLite::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
//TFixedArray<GLshort, 30> myArray;	
void CBlenderLite::ConstructL( void ){
	   
	//debuger
	//console = Console::NewL(_L("Consola"),TSize(KConsFullScreen, KConsFullScreen));
	//Objetos = new Mesh[cantObjetos];
	CrearObjeto(cubo);
}


// -----------------------------------------------------------------------------
// CBlenderLite::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CBlenderLite* CBlenderLite::NewL( TUint aWidth, TUint aHeight, CBlenderLiteInput* aInputHandler ){
    /* Symbian 2-phase constructor. Calls both the default
       C++ constructor and Symbian ConstructL methods */
    CBlenderLite* self = new (ELeave) CBlenderLite( aWidth, aHeight, aInputHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
}


// Destructor.
CBlenderLite::~CBlenderLite(){
}



// -----------------------------------------------------------------------------
// CBlenderLite::AppInit
// Initialize OpenGL ES, set the vertex and color arrays and pointers,
// and select the shading mode.
// -----------------------------------------------------------------------------
//
void CBlenderLite::AppInit( void ){
    // Construct a texture manager that uses the application's private
    // directory as the location for all textures.
	iTextureManager = CTextureManager::NewL ( iScreenWidth, iScreenHeight,
											  FRUSTUM_TOP, FRUSTUM_BOTTOM, FRUSTUM_RIGHT, FRUSTUM_LEFT, FRUSTUM_NEAR,
											  this );
  
    // Initialize viewport and projection.
	SetScreenSize( iScreenWidth, iScreenHeight );

    // Set the screen background color.
	glClearColor( 0.23f, 0.23f, 0.23f, 1.f );

    // Enable depth testing, texturing, back face culling, and lighting.
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE  );
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_LIGHTING   );
    /* Make OpenGL ES automatically normalize all normals after tranformations.
       This is important when making irregular xforms like scaling, or if we
       have specified nonunit-length normals. */
    glEnable( GL_NORMALIZE  );
    
    /* Initialize appropriate texture matrix. First we have to translate the
          input texture coordinate values to be within a range of [0,255]. Hence
          we translate the x- and y-coordinate values by 128. Recall that the
          values in nokTexCoords are between [-128,127], now they are in a range
          of [0,255]. After that we scale the values by 1/255 to make the values
          to be in range [0,1]. */
    glMatrixMode( GL_TEXTURE );
    glLoadIdentity();
    glScalef(     1.0f/255.0f, 1.0f/255.0f, 1.0f );
    glTranslatef( 128.0f,      128.0f,      0.0f );

    /* Remember to change the matrix mode to modelview. */
    glMatrixMode( GL_MODELVIEW );

    // Enable vertex and normal arrays.
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY );

    // Set up global ambient light.
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, globalAmbient );

    // Set up lamp.
    glEnable( GL_LIGHT0 );
    glLightfv(  GL_LIGHT0, GL_DIFFUSE,  lightDiffuseLamp  );
    glLightfv(  GL_LIGHT0, GL_AMBIENT,  lightAmbientLamp  );
    glLightfv(  GL_LIGHT0, GL_SPECULAR, lightDiffuseLamp  );
    glLightfv(  GL_LIGHT0, GL_POSITION, lightPositionLamp );

    // Set up spot.  Initially spot is disabled.
    glLightfv(  GL_LIGHT1, GL_DIFFUSE,  lightDiffuseSpot  );
    glLightfv(  GL_LIGHT1, GL_AMBIENT,  lightAmbientSpot  );
    glLightfv(  GL_LIGHT1, GL_SPECULAR, lightSpecularSpot );
    glLightfv(  GL_LIGHT1, GL_POSITION, lightPositionSpot );

    glLightf(   GL_LIGHT1, GL_CONSTANT_ATTENUATION,  1.5  );
    glLightf(   GL_LIGHT1, GL_LINEAR_ATTENUATION,    0.5  );
    glLightf(   GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.5  );

    glLightf(   GL_LIGHT1, GL_SPOT_CUTOFF,   17.0                );
    glLightf(   GL_LIGHT1, GL_SPOT_EXPONENT,  2.0                );
    glLightfv(  GL_LIGHT1, GL_SPOT_DIRECTION, lightDirectionSpot );

    // Set shading mode
    glShadeModel( GL_SMOOTH );
    //glShadeModel( GL_FLAT );

    // Do not use perspective correction
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );

    // Initialize menu state variables, Symbian menu stuff.
    iLightingEnabled = ETrue;          // Lighting is enabled
    iLampEnabled     = ETrue;          // Lamp is enabled
    iSpotEnabled     = EFalse;         // Spot is disabled

	// Push the textures into the loading queue.
	//_LIT( KModelTexture, "marcus.jpg" );
	_LIT( KModelTexture, "claude.jpg" );
	_LIT( KOriginTexture, "origen.png" );
	_LIT( KColorGridTexture, "color_grid.jpg" );
	_LIT( KMouseTexture, "cursor.png" );	
	_LIT( KLampTexture, "lamp.png" );	
	iTextureManager->RequestToLoad( KModelTexture, &iBaseColor, false );
	iTextureManager->RequestToLoad( KOriginTexture, &iOrigenTextura, false );
	iTextureManager->RequestToLoad( KColorGridTexture, &iColorGridTextura, false );
	iTextureManager->RequestToLoad( KMouseTexture, &iMouseTextura, false );
	iTextureManager->RequestToLoad( KLampTexture, &iLampTextura, false );
	
	//Start to load the textures.
	iTextureManager->DoLoadL();
	
	iParticleCoords = new GLfixed[1 * 3];
}


// -----------------------------------------------------------------------------
// CBlenderLite::AppExit
// Release any allocations made in AppInit.
// -----------------------------------------------------------------------------
//
void CBlenderLite::AppExit( void ){
	delete iTextureManager;
    delete[] iParticleCoords;
}


// -----------------------------------------------------------------------------
// CBlenderLite::AppCycle
// Draws and animates the objects
// -----------------------------------------------------------------------------
//

void CBlenderLite::AppCycle( TInt iFrame, GLfloat aTimeSecs, GLfloat aDeltaTimeSecs ){
    // If texture loading is still going on, return from this method without doing anything.
	if ( GetState() == ELoadingTextures ){
        return;
    }
	GLfixed fixedDeltaTimeSecs = FLOAT_2_FIXED( aDeltaTimeSecs );
	// Controles
	Rotar( fixedDeltaTimeSecs );
	
	if ( !redibujar ){
		return;
	}
	
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glLoadIdentity();
    glEnable( GL_DEPTH_TEST );		

	//posicion lamparas
	/*lightPositionLamp[0] = 0;
	lightPositionLamp[1] = 0;
	lightPositionLamp[2] = 0;
    glLightfv(  GL_LIGHT0, GL_POSITION, lightPositionLamp );*/
    
	glTranslatef( posX, posZ, -cameraDistance+posY);
	glRotatef(rotY, 1, 0, 0); //angulo, X Y Z
	glRotatef(rotX, 0, 1, 0); //angulo, X Y Z
	// Use some magic numbers to scale the head model to fit the screen.
	glScalex( 1 << 10, 1 << 10, 1 << 10 );
	
	
	//bucle que dibuja cada objeto en orden
	for(int o=0; o < Objetos.Count(); o++){
		if (Objetos[o].vertexGroupSize <= 0){
			continue;
		}
		glPushMatrix(); //guarda la matrix
		// Set head front material
		glMaterialfv(   GL_FRONT_AND_BACK, GL_AMBIENT,  objAmbient  );
		glMaterialfv(   GL_FRONT_AND_BACK, GL_SPECULAR, Objetos[o].specular );
		glMaterialx( GL_FRONT_AND_BACK, GL_SHININESS,   12 << 16     );
		glMaterialfv(   GL_FRONT_AND_BACK, GL_EMISSION, ListaColores[negro] );
			
		//posicion, rotacion y escala del objeto
		glTranslatef( Objetos[o].posX, Objetos[o].posZ, Objetos[o].posY);
		glRotatef(Objetos[o].rotX, 1, 0, 0); //angulo, X Y Z
		glRotatef(Objetos[o].rotY, 0, 0, 1); //angulo, X Y Z
		glRotatef(Objetos[o].rotZ, 0, 1, 0); //angulo, X Y Z
		glScalex( Objetos[o].scaleX, Objetos[o].scaleZ, Objetos[o].scaleY );	

		glEnable( GL_LIGHTING );
		glDisable( GL_TEXTURE_2D );
		glDisable(GL_POLYGON_OFFSET_FILL);
		// Set array pointers from mesh.
		glVertexPointer( 3, GL_SHORT, 0, Objetos[o].vertex );
		glNormalPointer( GL_BYTE, 0, Objetos[o].normals );
		//resetea las lienas a 1
		glLineWidth(1);	

		if (Objetos[o].smooth){glShadeModel( GL_SMOOTH );}
		else {glShadeModel( GL_FLAT );}
		
		//modelo con textura
		if (view == 0){
			glTexCoordPointer( 2, GL_BYTE, 0, Objetos[o].uv );
			if (Objetos[o].textura){
				glEnable( GL_TEXTURE_2D );
			};			 
			glMaterialfv(   GL_FRONT_AND_BACK, GL_DIFFUSE, Objetos[o].diffuse );        
			glBindTexture(  GL_TEXTURE_2D, Objetos[o].texturaID ); //selecciona la textura	
			//transparencia
			if (Objetos[o].transparencia){glEnable(GL_BLEND);}
			else {glDisable(GL_BLEND);}
			//textura pixelada o suave
			if (Objetos[o].interpolacion == lineal){
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
			else if (Objetos[o].interpolacion == closest){
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}
			glMaterialfv(   GL_FRONT_AND_BACK, GL_EMISSION, Objetos[o].emission );
			// Obtiene el puntero del arreglo
			//const GLushort* myArrayPointer = &Objetos[o].NewFaces[0];
			//glDrawElements( GL_TRIANGLES, Objetos[o].NewFaces.Count(), GL_UNSIGNED_SHORT, &Objetos[o].NewFaces[0] );	
			glDrawElements( GL_TRIANGLES, Objetos[o].facesSize, GL_UNSIGNED_SHORT, Objetos[o].faces );		
		}
		//modelo sin textura
		else if (view == 1){
			//modelo simple	
			glMaterialfv(   GL_FRONT_AND_BACK, GL_DIFFUSE,  Objetos[o].diffuse   ); 
			glDrawElements( GL_TRIANGLES, Objetos[o].facesSize, GL_UNSIGNED_SHORT, Objetos[o].faces );
		}
		else if(objSelect != o){    
			glMaterialfv(   GL_FRONT_AND_BACK, GL_DIFFUSE,  ListaColores[negro]  );
			glMaterialfv(   GL_FRONT_AND_BACK, GL_AMBIENT,  ListaColores[negro]  );
			glMaterialfv(   GL_FRONT_AND_BACK, GL_SPECULAR, ListaColores[negro] );
			glMaterialfv(   GL_FRONT_AND_BACK, GL_EMISSION, ListaColores[gris]);	
			glDrawElements( GL_LINES, Objetos[o].edgesSize, GL_UNSIGNED_SHORT, Objetos[o].edges );	
		}  
		
		//dibuja el borde seleccionado
		if(objSelect == o){
			glDisable( GL_TEXTURE_2D );  
			glEnable(GL_POLYGON_OFFSET_FILL);
			glMaterialfv(   GL_FRONT_AND_BACK, GL_DIFFUSE,  ListaColores[negro] );
			glMaterialfv(   GL_FRONT_AND_BACK, GL_AMBIENT,  ListaColores[negro] );
			glMaterialfv(   GL_FRONT_AND_BACK, GL_SPECULAR, ListaColores[negro] );	
			//si se esta editando
			if (estado == edicion || estado == translacionVertex){
				//bordes
				glPolygonOffset(1.0, -1.0);
				glLineWidth(1);	 
				glMaterialfv(   GL_FRONT_AND_BACK, GL_EMISSION, ListaColores[negro] );
				glMaterialfv(   GL_FRONT_AND_BACK, GL_EMISSION, ListaColores[gris] );	
				glDrawElements( GL_LINES, Objetos[o].edgesSize, GL_UNSIGNED_SHORT, Objetos[o].edges );
				//vertices
				if (tipoSelect == vertexSelect){
					glPolygonOffset(1.0, -4.0);
					glMaterialfv(   GL_FRONT_AND_BACK, GL_EMISSION, ListaColores[negro] );	
					glPointSize(4);
					glDrawElements( GL_POINTS, Objetos[o].vertexGroupSize, GL_UNSIGNED_SHORT, Objetos[o].vertexGroup);
					//vertice seleccionado
					if (Objetos[o].vertexGroupSize > 0){
						glPolygonOffset(1.0, -10.0);
						glMaterialfv(   GL_FRONT_AND_BACK, GL_EMISSION, ListaColores[blanco] );
					    glDrawArrays( GL_POINTS, Objetos[o].vertexGroup[EditSelect], 1 );
						//glMaterialfv(   GL_FRONT_AND_BACK, GL_EMISSION, ListaColores[gris] );					
					}					
				}
				//borde seleccionado
				else if (tipoSelect == edgeSelect){		
					//vertice seleccionado
					if (Objetos[o].edgesSize > 0){		
						glPolygonOffset(1.0, -10.0);
						glMaterialfv(   GL_FRONT_AND_BACK, GL_EMISSION, ListaColores[blanco] );	
						glDrawElements( GL_LINES, 2, GL_UNSIGNED_SHORT, &Objetos[o].edges[EditSelect*2]);
					}			
				}
				//cara seleccionado
				else if (tipoSelect == faceSelect){	
					//vertice seleccionado
					if (Objetos[o].facesSize > 0){	
						glEnable(GL_BLEND);
						glDisable(GL_LIGHTING);
						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						glPolygonOffset(1.0, -10.0);
						glEnable(GL_COLOR_MATERIAL);
						//glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0);
						glColor4f(ListaColores[naranjaFace][0],
								  ListaColores[naranjaFace][1],
								  ListaColores[naranjaFace][2],
								  ListaColores[naranjaFace][3]);
						glDrawElements( GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, &Objetos[o].faces[EditSelect*3]);
						glDisable(GL_COLOR_MATERIAL);
					}				
				}
			}
			else if (view != 2){
				glPolygonOffset(1.0, 200.0);
				glLineWidth(3);	 
				glMaterialfv(   GL_FRONT_AND_BACK, GL_EMISSION, ListaColores[colorBordeSelect] );
				glDrawElements( GL_LINES, Objetos[o].edgesSize, GL_UNSIGNED_SHORT, Objetos[o].edges );	
			}	
			else { //bordes seleccionados en wireframe
				glDisable(GL_POLYGON_OFFSET_FILL);
				glLineWidth(1);		
				glMaterialfv(   GL_FRONT_AND_BACK, GL_EMISSION, ListaColores[colorBordeSelect] );
				glDrawElements( GL_LINES, Objetos[o].edgesSize, GL_UNSIGNED_SHORT, Objetos[o].edges );	
			}
		};	
		
	    glPopMatrix(); //reinicia la matrix a donde se guardo  
	}

	//dibujar las lineas del piso
	glLineWidth(1);	
	glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable( GL_TEXTURE_2D );
	glEnable( GL_LIGHTING );
	glMaterialfv(   GL_FRONT_AND_BACK, GL_DIFFUSE,  ListaColores[negro]  );
	glMaterialfv(   GL_FRONT_AND_BACK, GL_AMBIENT,  ListaColores[negro]  );
	glMaterialfv(   GL_FRONT_AND_BACK, GL_SPECULAR, ListaColores[negro] );
    glVertexPointer( 3, GL_SHORT, 0, objVertexdataFloor );
	glNormalPointer( GL_BYTE, 0, objNormaldataFloor );
    //dibuja los ejes de transformacion    
	if (estado != navegacion && estado != edicion){		
		glPushMatrix(); //guarda la matrix
		//posicion, rotacion y escala del objeto seleccionado
		glTranslatef( Objetos[objSelect].posX, Objetos[objSelect].posZ, Objetos[objSelect].posY);
		if (estado != translacion){
			glRotatef(Objetos[objSelect].rotX, 1, 0, 0); //angulo, X Y Z
			glRotatef(Objetos[objSelect].rotY, 0, 0, 1); //angulo, X Y Z
			glRotatef(Objetos[objSelect].rotZ, 0, 1, 0); //angulo, X Y Z			
		}
		if (estado == translacionVertex){
			glTranslatef(Objetos[objSelect].vertex[Objetos[objSelect].vertexGroup[EditSelect]*3]  *Objetos[objSelect].scaleX/65000, 
					     Objetos[objSelect].vertex[Objetos[objSelect].vertexGroup[EditSelect]*3+1]*Objetos[objSelect].scaleY/65000, 
						 Objetos[objSelect].vertex[Objetos[objSelect].vertexGroup[EditSelect]*3+2]*Objetos[objSelect].scaleZ/65000
			);		
		}
	    glDisable( GL_DEPTH_TEST );
		if (axisSelect == X){
			glMaterialfv(   GL_FRONT_AND_BACK, GL_EMISSION, ColorTransformX ); 
			glDrawElements( GL_LINES, 2, GL_UNSIGNED_SHORT, EjeRojo );				
		}
		else if (axisSelect == Y){
			glMaterialfv(   GL_FRONT_AND_BACK, GL_EMISSION, ColorTransformY );
			glDrawElements( GL_LINES, 2, GL_UNSIGNED_SHORT, EjeVerde ); 				
		}
		else if (axisSelect == Z){
			glMaterialfv(   GL_FRONT_AND_BACK, GL_EMISSION, ColorTransformZ );
			glDrawElements( GL_LINES, 2, GL_UNSIGNED_SHORT, EjeAzul ); 				
		}	
		else if (axisSelect == XYZ){
			glMaterialfv(   GL_FRONT_AND_BACK, GL_EMISSION, ColorTransformX ); 
			glDrawElements( GL_LINES, 2, GL_UNSIGNED_SHORT, EjeRojo );	
			glMaterialfv(   GL_FRONT_AND_BACK, GL_EMISSION, ColorTransformY );
			glDrawElements( GL_LINES, 2, GL_UNSIGNED_SHORT, EjeVerde ); 
			glMaterialfv(   GL_FRONT_AND_BACK, GL_EMISSION, ColorTransformZ );
			glDrawElements( GL_LINES, 2, GL_UNSIGNED_SHORT, EjeAzul ); 				
		}	
	    glEnable( GL_DEPTH_TEST );	
	    glPopMatrix(); //reinicia la matrix a donde se guardo	
	}
	//dibuja el piso
	glShadeModel( GL_SMOOTH );
	glPushMatrix(); //guarda la matrix
	glTranslatef( 0, -5000, 0);
	glMaterialfv(   GL_FRONT_AND_BACK, GL_EMISSION, LineaPiso );  
	glDrawElements( GL_LINES, objFacesFloor, GL_UNSIGNED_SHORT, objFacedataFloor );	 
	//linea Verde
	glLineWidth(2);
	glMaterialfv(   GL_FRONT_AND_BACK, GL_EMISSION, LineaPisoRoja ); 
	glDrawElements( GL_LINES, 2, GL_UNSIGNED_SHORT, EjeRojo );	
	//linea Roja
	glMaterialfv(   GL_FRONT_AND_BACK, GL_EMISSION, LineaPisoVerde );
	glDrawElements( GL_LINES, 2, GL_UNSIGNED_SHORT, EjeVerde ); 
	
	//dibuja el origen del objeto
	if (Objetos.Count() > 0){
	    glPopMatrix(); //reinicia la matrix a donde se guardo
		glTranslatef( Objetos[objSelect].posX, Objetos[objSelect].posZ, Objetos[objSelect].posY);
		glEnable( GL_CULL_FACE ); // Enable back face culling.
		// Enable point sprites.
		glEnable( GL_TEXTURE_2D );
		glEnable( GL_POINT_SPRITE_OES );
		// We don't need a depth buffer in this example
		glDisable( GL_DEPTH_TEST );
		// Enable blending for transparency.
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		// Make the points bigger.
		glPointSize( 8 );
		// Disable lighting
		glDisable( GL_LIGHTING );
		GLshort posicionPunto[3]={0, 0, 0};
	    glVertexPointer( 3, GL_SHORT, 0, posicionPunto );
	    glBindTexture( GL_TEXTURE_2D, iOrigenTextura.iID ); //selecciona la textura
	    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	    glTexEnvi( GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_TRUE );
	    glDrawArrays( GL_POINTS, 0, 1 );
	    glTexEnvi( GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_FALSE);
	}

    //termino de dibujar
    redibujar = false;
}

// -------------------------------------------------------------------------------------------------------
// CBlenderLite::OnStartLoadingTextures()
// Called for a MTextureLoadingListener by the texture manager when texture loading operation starts
// -------------------------------------------------------------------------------------------------------

void CBlenderLite::OnStartLoadingTexturesL(){
    SetStateL( ELoadingTextures );
}

// ------------------------------------------------------------------------------------------------------------
// CBlenderLite::OnEndLoadingTextures()
// Called for a MTextureLoadingListener by the texture manager when texture loading operation is completed
// ------------------------------------------------------------------------------------------------------------
void CBlenderLite::OnEndLoadingTexturesL(){
	if ( GetState() == ELoadingTextures ){
		SetStateL( ERunning );
	}
}

void CBlenderLite::Rotar(GLfixed aDeltaTimeSecs){
	if( iInputHandler->IsInputPressed( EJoystickLeft ) ){
		//rotX += fixedMul( 0.1, aDeltaTimeSecs );
		if (estado == navegacion || estado == edicion){
			rotX-= 0.5;			
		}
		else if (estado == translacionVertex){		
			for(int g=0; g < Objetos[objSelect].vertexGroupIndiceSize[EditSelect]; g++){
				if (axisSelect == X){
					Objetos[objSelect].vertex[Objetos[objSelect].vertexGroupIndice[EditSelect][g]*3] += 30;					
				}
				else if (axisSelect == Y){
					Objetos[objSelect].vertex[Objetos[objSelect].vertexGroupIndice[EditSelect][g]*3+2] -= 30;				
				}
				else if (axisSelect == Z){
					Objetos[objSelect].vertex[Objetos[objSelect].vertexGroupIndice[EditSelect][g]*3+1] -= 30;	
				}
			}
		}
		else if (estado == translacion){
			if (axisSelect == X){
				Objetos[objSelect].posX += 30;					
			}
			else if (axisSelect == Y){
				Objetos[objSelect].posY -= 30;					
			}
			else if (axisSelect == Z){
				Objetos[objSelect].posZ -= 30;					
			}
		}
		else if (estado == rotacion){
			if (axisSelect == X){
				Objetos[objSelect].rotX -= 1;					
			}
			else if (axisSelect == Y){
				Objetos[objSelect].rotY -= 1;					
			}
			else if (axisSelect == Z){
				Objetos[objSelect].rotZ -= 1;					
			}
		}
		else if (estado == escala){
			if (axisSelect == X){Objetos[objSelect].scaleX -= 1000;}
			else if (axisSelect == Y){Objetos[objSelect].scaleY -= 1000;}
			else if (axisSelect == Z){Objetos[objSelect].scaleZ -= 1000;}
			else if (axisSelect == XYZ){
				Objetos[objSelect].scaleX -= 1000;
				Objetos[objSelect].scaleY -= 1000;
				Objetos[objSelect].scaleZ -= 1000;
			}
		}
		/*else if (estado == movimientoView){
			posX-= 0.5;					
		}
		else if (estado == movimientoProfundidad){
			posX-= 0.5;			
		}
		else if (estado == edicion){
			objVertexdataModel[0]-= 10;	
			objVertexdataModel[1]-= 10;
			objVertexdataModel[2]-= 10;
			objVertexdataModel[3]-= 10;
			objVertexdataModel[4]-= 10;		
		}	*/
	    redibujar = true;
	}
	if( iInputHandler->IsInputPressed( EJoystickRight ) ){
		//rotX -= fixedMul( 1, aDeltaTimeSecs );
		if (estado == navegacion || estado == edicion){
			rotX+= 0.5;			
		}
		else if (estado == translacionVertex){		
			for(int g=0; g < Objetos[objSelect].vertexGroupIndiceSize[EditSelect]; g++){
				if (axisSelect == X){
					Objetos[objSelect].vertex[Objetos[objSelect].vertexGroupIndice[EditSelect][g]*3] -= 30;					
				}
				else if (axisSelect == Y){
					Objetos[objSelect].vertex[Objetos[objSelect].vertexGroupIndice[EditSelect][g]*3+2] += 30;				
				}
				else if (axisSelect == Z){
					Objetos[objSelect].vertex[Objetos[objSelect].vertexGroupIndice[EditSelect][g]*3+1] += 30;	
				}
			}
		}
		else if (estado == translacion){
			if (axisSelect == X){
				Objetos[objSelect].posX -= 30;					
			}
			else if (axisSelect == Y){
				Objetos[objSelect].posY += 30;					
			}
			else if (axisSelect == Z){
				Objetos[objSelect].posZ += 30;					
			}
		}
		else if (estado == rotacion){
			if (axisSelect == X){
				Objetos[objSelect].rotX += 1;					
			}
			else if (axisSelect == Y){
				Objetos[objSelect].rotY += 1;					
			}
			else if (axisSelect == Z){
				Objetos[objSelect].rotZ += 1;					
			}
		}
		else if (estado == escala){
			if (axisSelect == X){Objetos[objSelect].scaleX += 1000;}
			else if (axisSelect == Y){Objetos[objSelect].scaleY += 1000;}
			else if (axisSelect == Z){Objetos[objSelect].scaleZ += 1000;}
			else if (axisSelect == XYZ){
				Objetos[objSelect].scaleX += 1000;
				Objetos[objSelect].scaleY += 1000;
				Objetos[objSelect].scaleZ += 1000;
			}			
		}
		/*
		else if (estado == movimientoView){
			posX+= 0.5;					
		}
		else if (estado == movimientoProfundidad){
			posX+= 0.5;			
		}
		else if (estado == edicion){
			objVertexdataModel[0]+= 10;	
			objVertexdataModel[1]+= 10;
			objVertexdataModel[2]+= 10;
			objVertexdataModel[3]+= 10;
			objVertexdataModel[4]+= 10;		
		}	*/	
	    redibujar = true;
	}
	if( iInputHandler->IsInputPressed( EJoystickUp ) ){
		if (estado == navegacion || estado == edicion){
			rotY-= 0.5;			
		}
		else if (estado == translacionVertex){		
			for(int g=0; g < Objetos[objSelect].vertexGroupIndiceSize[EditSelect]; g++){
				if (axisSelect == X){
					Objetos[objSelect].vertex[Objetos[objSelect].vertexGroupIndice[EditSelect][g]*3] -= 30;					
				}
				else if (axisSelect == Y){
					Objetos[objSelect].vertex[Objetos[objSelect].vertexGroupIndice[EditSelect][g]*3+2] += 30;				
				}
				else if (axisSelect == Z){
					Objetos[objSelect].vertex[Objetos[objSelect].vertexGroupIndice[EditSelect][g]*3+1] += 30;	
				}
			}
		}
		else if (estado == translacion){
			if (axisSelect == X){
				Objetos[objSelect].posX -= 30;					
			}
			else if (axisSelect == Y){
				Objetos[objSelect].posY += 30;					
			}
			else if (axisSelect == Z){
				Objetos[objSelect].posZ += 30;					
			}
		}/*
		else if (estado == movimientoView){
			posZ+= 0.5;					
		}
		else if (estado == movimientoProfundidad){
			posY -= 0.5;			
		}*/
	    redibujar = true;
	}
	if( iInputHandler->IsInputPressed( EJoystickDown ) ){
		if (estado == navegacion || estado == edicion){
			rotY+= 0.5;			
		}
		else if (estado == translacionVertex){		
			for(int g=0; g < Objetos[objSelect].vertexGroupIndiceSize[EditSelect]; g++){
				if (axisSelect == X){
					Objetos[objSelect].vertex[Objetos[objSelect].vertexGroupIndice[EditSelect][g]*3] += 30;					
				}
				else if (axisSelect == Y){
					Objetos[objSelect].vertex[Objetos[objSelect].vertexGroupIndice[EditSelect][g]*3+2] -= 30;				
				}
				else if (axisSelect == Z){
					Objetos[objSelect].vertex[Objetos[objSelect].vertexGroupIndice[EditSelect][g]*3+1] -= 30;	
				}
			}
		}
		else if (estado == translacion){
			if (axisSelect == X){
				Objetos[objSelect].posX += 30;					
			}
			else if (axisSelect == Y){
				Objetos[objSelect].posY -= 30;					
			}
			else if (axisSelect == Z){
				Objetos[objSelect].posZ -= 30;					
			}
		}
	    redibujar = true;
	}
	//if( iInputHandler->IsInputPressed( EVolumenUp ) ){
		//posY += 0.8;		
		//}
	//if( iInputHandler->IsInputPressed( EVolumenDown ) ){
		//posY -= 0.8;		
	//}
	//if( iInputHandler->IsInputPressed( EDelete ) ){view++;}
	//if( iInputHandler->IsInputPressed( EKeyLeftShift ) ){view--;}
	//if( iInputHandler->IsInputPressed( EKeyIncVolume ) ){view++;}
	//if( iInputHandler->IsInputPressed( EKeyDecVolume ) ){view--;}
	//if( iInputHandler->IsInputPressed( EKeyMenu ) ){view++;}
}

void CBlenderLite::SetRotacion(){
	//si no hay objetos
	if (Objetos.Count() < 1){return;}
	else if (estado == navegacion){
		guardarEstado(objSelect);
		estado = rotacion;	
		colorBordeSelect = 0;
		axisSelect = X;
	}	
	else if (axisSelect+1 > 2){axisSelect = X;}
	else {axisSelect++;}
    redibujar = true;	
};
void CBlenderLite::SetEscala(){
	//XYZ tiene escala
	//si no hay objetos
	if (Objetos.Count() < 1){return;}
	else if (estado == navegacion){
		guardarEstado(objSelect);
		estado = escala;
		colorBordeSelect = 0;
		axisSelect = XYZ;	
	}	
	else if (axisSelect+1 > 3){axisSelect = X;}
	else {axisSelect++;}
    redibujar = true;	
};
void CBlenderLite::SetPosicion(){
	//si no hay objetos
	if (Objetos.Count() < 1){return;}
	else if (estado == navegacion){
		guardarEstado(objSelect);
		estado = translacion;
		colorBordeSelect = 0;
	}
	else if (estado == edicion){
		if (tipoSelect == vertexSelect){
			estado = translacionVertex;
			guardarEstado(objSelect);	
		}
		else if (tipoSelect == edgeSelect){
			estado = translacionEdge;
			guardarEstado(objSelect);				
		}
		else if (tipoSelect == faceSelect){
			estado = translacionFace;
			guardarEstado(objSelect);				
		}
	}	
	else if (axisSelect+1 > 2){axisSelect = X;}
	else {axisSelect++;}
    redibujar = true;	
};

void CBlenderLite::SetEje(int eje){
	if (estado != navegacion){
		axisSelect = eje;
	}	
    redibujar = true;	
};

void CBlenderLite::Cancelar(){
	if (estado != navegacion && estado != edicion){
		ReestablecerEstado(objSelect);
		colorBordeSelect = 1;
	}
	else if (estado == translacionVertex){
		estado = edicion;		
	}
	else if (estado == translacion || estado == rotacion || estado == escala){
		estado = navegacion;	
	}
    redibujar = true;
};

void CBlenderLite::Aceptar(){
	if (estado == navegacion){
		estado = edicion;	
		colorBordeSelect = 1;	
	}
	else if (estado == edicion){
		estado = navegacion;		
		colorBordeSelect = 1;
	}
	else if (estado == translacionVertex){
		estado = edicion;		
	}
	else if (estado != navegacion){
		guardarEstado(objSelect);
		estado = navegacion;	
		colorBordeSelect = 1;
	};
    redibujar = true;
};

void CBlenderLite::Tab(){
	if (estado == navegacion){
		changeSelect();
	}
	else if (estado == edicion){
		changeSelect();
	}
	else if (estado == translacion || estado == rotacion || estado == escala || estado == translacionVertex){
		InsertarValor();
	};
};


void CBlenderLite::ReestablecerEstado(int indice){
	if (estado == translacionVertex){
		 for(int g=0; g < Objetos[objSelect].vertexGroupIndiceSize[EditSelect]; g++){
			Objetos[objSelect].vertex[Objetos[objSelect].vertexGroupIndice[EditSelect][g]*3] = estadoVertex[0];
			Objetos[objSelect].vertex[Objetos[objSelect].vertexGroupIndice[EditSelect][g]*3+2] = estadoVertex[1];	
			Objetos[objSelect].vertex[Objetos[objSelect].vertexGroupIndice[EditSelect][g]*3+1] = estadoVertex[2];	
		}
	}
	else {
		Objetos[indice].posX = estadoObj.posX;
		Objetos[indice].posY = estadoObj.posY;
		Objetos[indice].posZ = estadoObj.posZ;
		Objetos[indice].rotX = estadoObj.rotX;
		Objetos[indice].rotY = estadoObj.rotY;
		Objetos[indice].rotZ = estadoObj.rotZ;
		Objetos[indice].scaleX = estadoObj.scaleX;
		Objetos[indice].scaleY = estadoObj.scaleY;
		Objetos[indice].scaleZ = estadoObj.scaleZ;		
	}
};

void CBlenderLite::guardarEstado(int indice){
	if (estado == translacionVertex){
		estadoVertex[0] = Objetos[objSelect].vertex[Objetos[indice].vertexGroupIndice[EditSelect][0]*3];
		estadoVertex[1] = Objetos[objSelect].vertex[Objetos[indice].vertexGroupIndice[EditSelect][0]*3+2];	
		estadoVertex[2] = Objetos[objSelect].vertex[Objetos[indice].vertexGroupIndice[EditSelect][0]*3+1];	
	}
	else {
		estadoObj.posX = Objetos[indice].posX;
		estadoObj.posY = Objetos[indice].posY;
		estadoObj.posZ = Objetos[indice].posZ;
		estadoObj.rotX = Objetos[indice].rotX;
		estadoObj.rotY = Objetos[indice].rotY;
		estadoObj.rotZ = Objetos[indice].rotZ;
		estadoObj.scaleX = Objetos[indice].scaleX;
		estadoObj.scaleY = Objetos[indice].scaleY;
		estadoObj.scaleZ = Objetos[indice].scaleZ;		
	}
};

//cambie el shader
void CBlenderLite::SetShader(TInt valor){
	view = valor;
    redibujar = true;	
}

// ---------------------------------------------------------
// CBlenderLite::OnEnterState( TInt aState )
// Called by TFiniteStateMachine when the f.s.m enters a new state
// ---------------------------------------------------------

void CBlenderLite::OnEnterStateL( TInt /*aState*/ ){
	// Nothing to do here...
}

//----------------------------------------------------------
// The following methods are called by the CBlenderLiteAppUi
// class when handling the incoming menu events.
//----------------------------------------------------------


// -----------------------------------------------------------------------------
// CBlenderLite::ToggleLighting
// Enable/Disable lighting from the application menu.
// -----------------------------------------------------------------------------
//
void CBlenderLite::ToggleLighting( void ){
    // If texture loading is still going on, return from this method without doing anything.
	if ( GetState() == ELoadingTextures ){
        return;
    }
    if ( iLightingEnabled){
        iLightingEnabled = EFalse;
    	glShadeModel( GL_FLAT );  // Don't need smooth shading when no lights
        glDisable( GL_LIGHTING );
        
        /* Set both textures environments to GL_REPLACE. */
		glBindTexture( GL_TEXTURE_2D, iBaseColor.iID );
		glTexEnvx(  GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        }
    else
        {
        iLightingEnabled = ETrue;
    	glShadeModel( GL_SMOOTH ); // For the lighting to work properly
        glEnable( GL_LIGHTING );

    	/* Set both textures environments to GL_MODULATE. */
    	glBindTexture( GL_TEXTURE_2D, iBaseColor.iID );
    	glTexEnvx(  GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        }
    }


// -----------------------------------------------------------------------------
// CBlenderLite::ToggleLamp
// Enable/Disable lamp from the application menu.
// -----------------------------------------------------------------------------
//
void CBlenderLite::ToggleLamp( void )
    {
    if ( iLampEnabled )
        {
        iLampEnabled = EFalse;
        glDisable( GL_LIGHT0 );
        }
    else
        {
        iLampEnabled = ETrue;
        glEnable( GL_LIGHT0 );
        }
    }


// -----------------------------------------------------------------------------
// CBlenderLite::ToggleSpot
// Enable/Disable spot from the application menu.
// -----------------------------------------------------------------------------
//
void CBlenderLite::ToggleSpot( void ){
    if ( iSpotEnabled ){
		iSpotEnabled = EFalse;
		glDisable( GL_LIGHT1 );
	}
    else{
		iSpotEnabled = ETrue;
		glEnable( GL_LIGHT1 );
	}
}

void CBlenderLite::InsertarValor(){
	HBufC* buf = HBufC::NewLC( 20 );
	if (estado == translacion){
		if (axisSelect == X){
			buf->Des().Copy(_L("Mover en X"));
			TInt valorX = DialogNumber((TInt)(Objetos[objSelect].posX-estadoObj.posX), -100000, 100000,buf);
			Objetos[objSelect].posX = estadoObj.posX+valorX;			
		}
		else if (axisSelect == Y){
			buf->Des().Copy(_L("Mover en Y"));
			TInt valorY = DialogNumber((TInt)(Objetos[objSelect].posY-estadoObj.posY), -100000, 100000,buf);
			Objetos[objSelect].posY = estadoObj.posY+valorY;			
		}
		else if (axisSelect == Z){
			buf->Des().Copy(_L("Mover en Z"));
			TInt valorZ = DialogNumber((TInt)(Objetos[objSelect].posZ-estadoObj.posZ), -100000, 100000,buf);
			Objetos[objSelect].posZ = estadoObj.posZ+valorZ;			
		}
	}
	else if (estado == edicion){
		if (axisSelect == X){
			buf->Des().Copy(_L("Posicion en X"));
			TInt valorX = DialogNumber((TInt)Objetos[objSelect].vertex[Objetos[objSelect].vertexGroupIndice[EditSelect][0]*3], -1000000, 1000000,buf);
			for(int g=0; g < Objetos[objSelect].vertexGroupIndiceSize[EditSelect]; g++){
				Objetos[objSelect].vertex[Objetos[objSelect].vertexGroupIndice[EditSelect][g]*3] = valorX;
			}
		}
		else if (axisSelect == Y){
			buf->Des().Copy(_L("Posicion en Y"));
			TInt valorY = DialogNumber((TInt)Objetos[objSelect].vertex[Objetos[objSelect].vertexGroupIndice[EditSelect][0]*3+2], -1000000, 1000000,buf);
			for(int g=0; g < Objetos[objSelect].vertexGroupIndiceSize[EditSelect]; g++){
				Objetos[objSelect].vertex[Objetos[objSelect].vertexGroupIndice[EditSelect][g]*3+2] = valorY;
			}
		}
		else if (axisSelect == Z){
			buf->Des().Copy(_L("Posicion en Z"));
			TInt valorZ = DialogNumber((TInt)Objetos[objSelect].vertex[Objetos[objSelect].vertexGroupIndice[EditSelect][0]*3+1], -1000000, 1000000,buf);
			for(int g=0; g < Objetos[objSelect].vertexGroupIndiceSize[EditSelect]; g++){
				Objetos[objSelect].vertex[Objetos[objSelect].vertexGroupIndice[EditSelect][g]*3+1] = valorZ;
			}
		}	
	}
	else if (estado == translacionVertex){
		if (axisSelect == X){
			buf->Des().Copy(_L("Posicion en X")); //(Objetos[objSelect].vertex[Objetos[objSelect].vertexGroupIndice[EditSelect][0]*3]-
			TInt valorX = DialogNumber((TInt)(Objetos[objSelect].vertex[Objetos[objSelect].vertexGroupIndice[EditSelect][0]*3]-estadoVertex[0]), -1000000, 1000000,buf);
			for(int g=0; g < Objetos[objSelect].vertexGroupIndiceSize[EditSelect]; g++){
				Objetos[objSelect].vertex[Objetos[objSelect].vertexGroupIndice[EditSelect][g]*3] = valorX+estadoVertex[0]; //)*1;
			}
		}
		else if (axisSelect == Y){
			buf->Des().Copy(_L("Posicion en Y"));
			TInt valorY = DialogNumber((TInt)(Objetos[objSelect].vertex[Objetos[objSelect].vertexGroupIndice[EditSelect][0]*3+2]-estadoVertex[1]), -1000000, 1000000,buf);
			for(int g=0; g < Objetos[objSelect].vertexGroupIndiceSize[EditSelect]; g++){
				Objetos[objSelect].vertex[Objetos[objSelect].vertexGroupIndice[EditSelect][g]*3+2] = valorY+estadoVertex[1];
			}
		}
		else if (axisSelect == Z){
			buf->Des().Copy(_L("Posicion en Z"));
			TInt valorZ = DialogNumber((TInt)(Objetos[objSelect].vertex[Objetos[objSelect].vertexGroupIndice[EditSelect][0]*3+1]-estadoVertex[2]), -1000000, 1000000,buf);
			for(int g=0; g < Objetos[objSelect].vertexGroupIndiceSize[EditSelect]; g++){
				Objetos[objSelect].vertex[Objetos[objSelect].vertexGroupIndice[EditSelect][g]*3+1] = valorZ+estadoVertex[2];
			}
		}	
		Aceptar();
	}	
	//CleanupStack::PopAndDestroy(buf);
	redibujar = true;	
}

void CBlenderLite::TecladoNumerico(TInt numero){
	if (estado == translacion || estado == rotacion || estado == escala ){
		if (numero == 10){ //invertir
			if (axisSelect == X){
				Objetos[objSelect].posX = estadoObj.posX-(Objetos[objSelect].posX-estadoObj.posX);					
			}	
			else if (axisSelect == Y){
				Objetos[objSelect].posY = estadoObj.posY-(Objetos[objSelect].posY-estadoObj.posY);					
			}	
			else if (axisSelect == Z){
				Objetos[objSelect].posZ = estadoObj.posZ-(Objetos[objSelect].posZ-estadoObj.posZ);					
			}		
		}
		else {
			//InsertarValor();			
		}
	}	
	else if (estado == edicion){
		if (numero == 1){SetPosicion();}
		else if (numero == 7){
			Extruir();
		}
	}
	else if (estado == translacionVertex){
		//InsertarValor();
	}
	else if (numero == 1){SetPosicion();}
	else if (numero == 2){SetRotacion();}
	else if (numero == 3){SetEscala();}
	else if (numero == 4){
		SetShader(0);	
	}
	else if (numero == 5){
		SetShader(1);
	}
	else if (numero == 6){
		SetShader(2);
	}
};

void CBlenderLite::Borrar(){
	if (estado != navegacion && estado != edicion ){
		Cancelar();
	}
	else if (estado == navegacion){
		if (Objetos.Count() < 1){return;}
		//pregunta de confirmacion
		HBufC* buf = HBufC::NewLC( 20 );
		buf->Des().Copy(_L("Eliminar el Objeto?"));
		if (!DialogAlert(buf)){return;}
		Cancelar();
		estado = navegacion;
		Objetos.Remove(objSelect);
		if (Objetos.Count()-1 < objSelect){
			objSelect = Objetos.Count()-1;		
		}
		//CleanupStack::PopAndDestroy(buf);
	}
	else if (estado == edicion){
		if (Objetos[objSelect].vertexGroupSize < 1){return;}
		//pregunta de confirmacion
		HBufC* buf = HBufC::NewLC( 20 );
		buf->Des().Copy(_L("Eliminar Vertice?"));
		if (!DialogAlert(buf)){return;}	
		Mesh& obj = Objetos[objSelect];
		//busca las caras que contengan algun vertices del grupo de vertices
		RArray<GLushort> faces;
		for(int f=0; f < obj.facesSize/3; f++){
			for(int v=0; v < obj.vertexGroupIndiceSize[EditSelect]; v++){
				if (obj.vertexGroupIndice[EditSelect][v] == obj.faces[f*3] ||
					obj.vertexGroupIndice[EditSelect][v] == obj.faces[f*3+1] ||
					obj.vertexGroupIndice[EditSelect][v] == obj.faces[f*3+2]){
					faces.Append(f);
					break;
				}				
			}
		};		
		//busca los bordes
		RArray<GLushort> edges;
		for(int e=0; e < obj.edgesSize/2; e++){
			for(int g=0; g < obj.vertexGroupIndiceSize[EditSelect]; g++){
				if (obj.vertexGroupIndice[EditSelect][g] == obj.edges[e*2] ||
					obj.vertexGroupIndice[EditSelect][g] == obj.edges[e*2+1]){
					edges.Append(e);
					break;
				}				
			}
		};		
		
		obj.RemoveFaces(faces, EditSelect);
		obj.RemoveEdges(edges, EditSelect);
		obj.RemoveVertex(EditSelect);
		faces.Close();
		edges.Close();
		if (obj.vertexGroupSize < EditSelect+1){
			EditSelect = obj.vertexGroupSize-1;			
		}
		/*HBufC* noteBuf = HBufC::NewLC(128);
		_LIT(KFormatString, "Select %d Group %d Vertices %d");
		noteBuf->Des().Format(KFormatString, EditSelect, obj.vertexGroupSize,  obj.vertexSize/3);
		Mensaje(noteBuf);*/
				
		//CleanupStack::PopAndDestroy(noteBuf);
		//CleanupStack::PopAndDestroy(buf);
	}
    redibujar = true;	
}

void CBlenderLite::CrearObjeto( int modelo ){
	Cancelar();
	Mesh obj;
	obj.visible = true;
	obj.posX = 0;
	obj.posY = 0;
	obj.posZ = 0;
	obj.rotX = 0;
	obj.rotY = 0;
	obj.rotZ = 0;
	obj.specular[0] = obj.specular[1] = obj.specular[2] = obj.specular[3] = 0.3;
	obj.diffuse[0] = obj.diffuse[1] = obj.diffuse[2] = obj.diffuse[3] = 1.0;
	obj.emission[0] = obj.emission[1] = obj.emission[2] = obj.emission[3] = 0.0;
	obj.smooth = true;
	obj.textura = false;
	obj.transparencia = false;
	obj.interpolacion = lineal; //interpolacion lineal
	
    if (modelo == cubo){ 
    	obj.vertexSize = 24 * 3;
    	obj.normalsSize = 24 * 3;
		obj.facesSize = 12 * 3;
		obj.edgesSize = 12 * 6;
		obj.uvSize = 24 * 2;
		obj.texturaID = 1;
		obj.scaleX = 45000;
		obj.scaleY = 45000;
		obj.scaleZ = 45000;			

		obj.vertex = new GLshort[24 * 3];
		obj.normals = new GLbyte[24 * 3];
		obj.faces = new GLushort[12 * 3];
		obj.edges = new GLushort[12 * 6];
		obj.uv = new GLbyte[24 * 2];
		obj.vertex = Primitivas.CuboVertices(1,1,1);
		//GLshort *vertex =  new GLshort[24 * 3];
		//obj.vertex.ReserveL(24 * 3);
		//vertex = Primitivas.CuboVertices(1,1,1);
		obj.NewFaces.ReserveL(12*3);
		for (int i = 0; i < 12*3; i++) {
			obj.NewFaces.Append(obj.faces[i]);
		}
		//delete[] vertex;
		obj.normals = Primitivas.CuboNormals();
		obj.faces = Primitivas.CuboFaces();
		obj.uv = Primitivas.CuboUV();
	}	
    else if (modelo == vertice){
    	obj.vertexSize = 1 * 3;
    	obj.normalsSize = 1 * 3;
		obj.facesSize = 0 * 3;
		obj.edgesSize = 0 * 6;
		obj.uvSize = 1 * 2;
		obj.texturaID = 1;
		obj.scaleX = 65000;
		obj.scaleY = 65000;
		obj.scaleZ = 65000;		

		obj.vertex = new GLshort[1 * 3];
		obj.normals = new GLbyte[1 * 3];
		obj.faces = new GLushort[0 * 3];
		obj.edges = new GLushort[0 * 6];
		obj.uv = new GLbyte[1 * 2];
		obj.vertex[0] = obj.vertex[1] = obj.vertex[2] = 0;
		obj.normals[0] = obj.normals[1] = 0;
		obj.normals[2] = 1;
		obj.uv[0] = obj.uv[0] = 0;
    }
    else if (modelo == suzanne){  
    	obj.vertexSize = SuzzaneVertexSize;
    	obj.normalsSize = SuzzaneNormalSize;
		obj.facesSize = SuzzaneFaceSize;
		obj.edgesSize = SuzzaneFaceSize*2;
		obj.uvSize = SuzzaneUVSize;  	
		obj.vertex = new GLshort[SuzzaneVertexSize];
		obj.texturaID = 1;
		obj.scaleX = 45000;
		obj.scaleY = 45000;
		obj.scaleZ = 45000;	
		obj.rotZ = 180;
		for(int a=0; a < SuzzaneVertexSize; a++){
			obj.vertex[a] = SuzzaneVertex[a];	
			//obj.vertex.Append(SuzzaneVertex[a]);		
		}
		obj.normals = new GLbyte[SuzzaneNormalSize];
		for(int a=0; a < SuzzaneNormalSize; a++){
			obj.normals[a] = SuzzaneNormal[a];			
		}
		obj.faces = new GLushort[SuzzaneFaceSize];
		for(int a=0; a < SuzzaneFaceSize; a++){
			obj.faces[a] = SuzzaneFace[a];			
		}
		obj.edges = new GLushort[obj.edgesSize];
		obj.uv = new GLbyte[SuzzaneUVSize];
		for(int a=0; a < SuzzaneUVSize; a++){
			obj.uv[a] = SuzzaneUV[a];			
		}
	}	
    else if (modelo == claude){
    	obj.vertexSize = objVertexdataModelSize;
    	obj.normalsSize = objNormaldataModelSize;
		obj.facesSize = objFacedataModelSize;
		obj.edgesSize = objFacedataModelSize*2;
		obj.uvSize = objTexdataModelSize;  	
		obj.texturaID = iBaseColor.iID;
		obj.scaleX = 65000;
		obj.scaleY = 65000;
		obj.scaleZ = 65000;
		obj.vertex = new GLshort[objVertexdataModelSize];
		for(int a=0; a < objVertexdataModelSize; a++){
			obj.vertex[a] = objVertexdataModel[a];	
			//obj.vertex.Append(objVertexdataModel[a]);
		}
		obj.normals = new GLbyte[objNormaldataModelSize];
		for(int a=0; a < objNormaldataModelSize; a++){
			obj.normals[a] = objNormaldataModel[a];			
		}
		obj.faces = new GLushort[objFacedataModelSize];
		for(int a=0; a < objFacedataModelSize; a++){
			obj.faces[a] = objFacedataModel[a];			
		}
		obj.edges = new GLushort[obj.edgesSize];
		obj.uv = new GLbyte[objTexdataModelSize];
		for(int a=0; a < objTexdataModelSize; a++){
			obj.uv[a] = objTexdataModel[a];			
		}	
    }
    
    //agrupar vertices
    GLshort* TempVertexPos = new GLshort[obj.vertexSize];
    GLushort* TempVertexIndice = new GLushort[obj.vertexSize];
    TInt* TempVertexIndiceGroupSize = new TInt[obj.vertexSize];
    GLushort** TempVertexIndiceGroup = new GLushort*[obj.vertexSize];
	for(TInt a=0; a < obj.vertexSize; a++){
		TempVertexPos[a] = 0;//obj.vertex[a];
		//TempVertexIndiceGroup[a] = new GLushort[obj.vertexSize/3];
		TempVertexIndiceGroup[a] = new GLushort[5]; //lo ideal seria el maximo posible. pero problemas de memoria
	}	
    obj.vertexGroupSize = 0;
    bool iguales = false;
	for(TInt a=0; a < obj.vertexSize/3; a++){
		iguales = false;
		//busca copias
		for(TInt s=0; s < obj.vertexGroupSize; s++){
			if (TempVertexPos[s*3] == obj.vertex[a*3] &&
				TempVertexPos[s*3+1] == obj.vertex[a*3+1] &&
				TempVertexPos[s*3+2] == obj.vertex[a*3+2]){
				iguales = true;
				//aumenta el tamaño del array y le agrega el indice que coincide
				//GLushort* temp = new GLushort[TempVertexIndiceGroupSize[s]+1];
				//for(int t=0; t < TempVertexIndiceGroupSize[s]; t++){
				//	temp[t] = TempVertexIndiceGroup[s][t];			
				//}	
				//temp[TempVertexIndiceGroupSize[s]] = a;	
				//TempVertexIndiceGroupSize[s]++;
				//TempVertexIndiceGroup[s] = new GLushort[TempVertexIndiceGroupSize[s]]; //agrega el primer indice
				//for(int t=0; t < TempVertexIndiceGroupSize[s]; t++){
				//	TempVertexIndiceGroup[s][t] = temp[t];			
				//}
				//delete[] temp;
				TempVertexIndiceGroup[s][TempVertexIndiceGroupSize[s]] = a;
				TempVertexIndiceGroupSize[s]++;
				break;
			}			
		};
		//si no se encontro el vertice. lo guarda y aumenta el size
		if (!iguales){
			TempVertexPos[obj.vertexGroupSize*3] = obj.vertex[a*3];
			TempVertexPos[obj.vertexGroupSize*3+1] = obj.vertex[a*3+1];
			TempVertexPos[obj.vertexGroupSize*3+2] = obj.vertex[a*3+2];
			TempVertexIndice[obj.vertexGroupSize] = a;	
			
			//TempVertexIndiceGroup[obj.vertexGroupSize] = new GLushort[1]; //agrega el primer indice
			TempVertexIndiceGroupSize[obj.vertexGroupSize] = 1; //arranca en uno
			TempVertexIndiceGroup[obj.vertexGroupSize][0] = a;
			obj.vertexGroupSize++;
		}
	}
	//copia los indices de los vertices a dibujar
	obj.vertexGroup = new GLushort[obj.vertexGroupSize];
	for(int a=0; a < obj.vertexGroupSize; a++){
		obj.vertexGroup[a] = TempVertexIndice[a];
	}	
	obj.vertexGroupIndice = new GLushort*[obj.vertexGroupSize];
	obj.vertexGroupIndiceSize = new TInt[obj.vertexGroupSize];
	for(int a=0; a < obj.vertexGroupSize; a++){
		obj.vertexGroupIndiceSize[a] = TempVertexIndiceGroupSize[a];
		obj.vertexGroupIndice[a] = new GLushort[obj.vertexGroupIndiceSize[a]];
		for(int i=0; i < obj.vertexGroupIndiceSize[a]; i++){
			obj.vertexGroupIndice[a][i] = TempVertexIndiceGroup[a][i];	
		}
	}	
	delete[] TempVertexPos;
	delete[] TempVertexIndice;
	delete[] TempVertexIndiceGroup;
	delete[] TempVertexIndiceGroupSize;
	
    //crear bordes
	Edge* TempEdgesVertex = new Edge[obj.edgesSize];
	for(int a=0; a < obj.facesSize/3; a++){
		obj.edges[a*6]   = obj.faces[a*3];	
		obj.edges[a*6+1] = obj.faces[a*3+1];		
		obj.edges[a*6+2] = obj.faces[a*3+1];	
		obj.edges[a*6+3] = obj.faces[a*3+2];	
		obj.edges[a*6+4] = obj.faces[a*3+2];	
		obj.edges[a*6+5] = obj.faces[a*3];
		
		/*TempEdgesVertex[a][0][0] = obj.vertex[obj.faces[a*3]];
		TempEdgesVertex[a][0][1] = obj.vertex[obj.faces[a*3+2]];
		TempEdgesVertex[a][0][2] = obj.vertex[obj.faces[a*3+4]];
		TempEdgesVertex[a][1][0] = obj.vertex[obj.faces[a*3+1]];
		TempEdgesVertex[a][1][1] = obj.vertex[obj.faces[a*3+3]];
		TempEdgesVertex[a][1][2] = obj.vertex[obj.faces[a*3]];*/
	};
	/*obj.edgesSize = 0;
	for(int a=0; a < obj.facesSize/3; a++){
		iguales = false;
		//busca copias
		for(int s=0; s < obj.vertexGroupSize; s++){
			if (TempVertexPos[s*3] == obj.vertex[a*3] &&
				TempVertexPos[s*3+1] == obj.vertex[a*3+1] &&
				TempVertexPos[s*3+2] == obj.vertex[a*3+2]){
				iguales = true;
				break;
			}			
		};
		//si no se encontro el borde. lo guarda y aumenta el size
		if (!iguales){
			TempEdgesIndice[a*6]   = obj.faces[a*3];	
			TempEdgesIndice[a*6+1] = obj.faces[a*3+1];		
			TempEdgesIndice[a*6+2] = obj.faces[a*3+1];	
			TempEdgesIndice[a*6+3] = obj.faces[a*3+2];	
			TempEdgesIndice[a*6+4] = obj.faces[a*3+2];	
			TempEdgesIndice[a*6+5] = obj.faces[a*3];	
			obj.edgesSize++;
		}
	};*/

	/*obj.vertextemp = new GLshort[obj.vertexSize];
	for(int a=0; a < obj.vertexSize; a++){
		obj.vertextemp[a] = obj.vertex[a];
	}*/
	
	delete[] TempEdgesVertex;
	Objetos.Append(obj);
	objSelect = Objetos.Count()-1;
    redibujar = true;
}

void CBlenderLite::Extruir(){
	if (estado == edicion && Objetos[objSelect].vertexGroupSize > 0){
		Mesh& obj = Objetos[objSelect];
		//primero crea los array temporales y les suma el espacio del nuevo vertice
		GLshort* TempVertex = new GLshort[obj.vertexSize+3];
		GLbyte* TempNormals = new GLbyte[obj.normalsSize+3];
		GLushort* TempEdges = new GLushort[obj.edgesSize+2];
		GLbyte* TempUv = new GLbyte[obj.uvSize+2];
	    GLushort* TempVertexGroup = new GLushort[obj.vertexGroupSize+1];
		GLushort** TempVertexGroupIndices = new GLushort*[obj.vertexGroupSize+1];
		TInt* TempVertexGroupIndicesSize =  new TInt[obj.vertexGroupSize+1];
		//copia los valores originales al array temporal
		for(int a=0; a < obj.vertexSize; a++){
			TempVertex[a] = obj.vertex[a];
			TempNormals[a] = obj.normals[a];	
		}
		for(int a=0; a < obj.edgesSize; a++){
			TempEdges[a] = obj.edges[a];			
		}
		for(int a=0; a < obj.uvSize; a++){
			TempUv[a] = obj.uv[a];			
		}
		for(int a=0; a < obj.vertexGroupSize; a++){
			TempVertexGroup[a] = obj.vertexGroup[a];
			TempVertexGroupIndicesSize[a] = obj.vertexGroupIndiceSize[a];
			TempVertexGroupIndices[a] = new GLushort[obj.vertexGroupIndiceSize[a]];
			for(int s=0; s < obj.vertexGroupIndiceSize[a]; s++){
				TempVertexGroupIndices[a][s] = obj.vertexGroupIndice[a][s];			
			}
		}

		//copia el vertice seleccionado al nuevo vertice
		TempVertex[obj.vertexSize] = obj.vertex[obj.vertexGroup[EditSelect]*3];
		TempVertex[obj.vertexSize+1] = obj.vertex[obj.vertexGroup[EditSelect]*3+1];
		TempVertex[obj.vertexSize+2] = obj.vertex[obj.vertexGroup[EditSelect]*3+2];
	    TempNormals[obj.vertexSize] = obj.normals[obj.vertexGroup[EditSelect]*3];
	    TempNormals[obj.vertexSize+1] = obj.normals[obj.vertexGroup[EditSelect]*3+1];
	    TempNormals[obj.vertexSize+2] = obj.normals[obj.vertexGroup[EditSelect]*3+2];
	    TempUv[obj.uvSize] = obj.uv[obj.vertexGroup[EditSelect]*2];
	    TempUv[obj.uvSize+1] = obj.uv[obj.vertexGroup[EditSelect]*2+1];	    
	    TempEdges[obj.edgesSize] =   obj.vertexGroup[EditSelect];
	    TempEdges[obj.edgesSize+1] = obj.vertexSize/3;
	    TempVertexGroup[obj.vertexGroupSize] = obj.vertexSize/3; //ultimo indice creado
	    TempVertexGroupIndices[obj.vertexGroupSize] = new GLushort[1]; //le agrega a la memoria
	    TempVertexGroupIndices[obj.vertexGroupSize][0] = obj.vertexSize/3; //ultimo indice creado
	    TempVertexGroupIndicesSize[obj.vertexGroupSize] = 1; //ultimo indice creado
		//suma el nuevo tamaño
		obj.vertexSize += 3;
		obj.normalsSize += 3;
		obj.edgesSize += 2;
		obj.uvSize += 2;
		obj.vertexGroupSize++;
		//asigna el nuevo tamaño
		obj.vertex =  new GLshort[obj.vertexSize];
		obj.normals = new GLbyte[obj.normalsSize];
		obj.edges =   new GLushort[obj.edgesSize];
		obj.uv =      new GLbyte[obj.uvSize];
		obj.vertexGroup = new GLushort[obj.vertexGroupSize];
		obj.vertexGroupIndiceSize = new TInt[obj.vertexGroupSize];
		obj.vertexGroupIndice = new GLushort*[obj.vertexGroupSize];
		//agrega los valores temporales al objeto
		for(int a=0; a < obj.vertexSize; a++){
			obj.vertex[a] = TempVertex[a];
			obj.normals[a] = TempNormals[a];			
		}
		for(int a=0; a < obj.edgesSize; a++){
			obj.edges[a] = TempEdges[a];			
		}
		for(int a=0; a < obj.uvSize; a++){
			obj.uv[a]= TempUv[a];			
		}
		for(int a=0; a < obj.vertexGroupSize; a++){
			obj.vertexGroup[a] = TempVertexGroup[a];	
			obj.vertexGroupIndiceSize[a] = TempVertexGroupIndicesSize[a];
			obj.vertexGroupIndice[a] = new GLushort[obj.vertexGroupIndiceSize[a]];
			for(int s=0; s < obj.vertexGroupIndiceSize[a]; s++){
				obj.vertexGroupIndice[a][s] = TempVertexGroupIndices[a][s];			
			}
		}
		
		EditSelect = obj.vertexGroupSize-1;
		estado = translacionVertex;
		guardarEstado(objSelect);
		//libera memoria
		delete[] TempVertex;
		delete[] TempNormals;
		delete[] TempEdges;
		delete[] TempUv;
		delete[] TempVertexGroup;
		delete[] TempVertexGroupIndicesSize;
		delete[] TempVertexGroupIndices;
		redibujar = true;
	}	
}

void CBlenderLite::ActivarTextura(){
	//si no hay objetos
	if (Objetos.Count() < 1){return;}
	Cancelar();
	//activa o desactiva las texturas
	HBufC* buf = HBufC::NewLC( 20 );
	buf->Des().Copy(_L("Activar Textura?"));
	if (DialogAlert(buf)){	
		Objetos[objSelect].textura = true;
		
	}
	else {
		Objetos[objSelect].textura = false;	
	}
	//CleanupStack::PopAndDestroy(buf);	
    redibujar = true;
}

void CBlenderLite::SetTransparencia(){
	//si no hay objetos
	if (Objetos.Count() < 1){return;}
	Cancelar();
	//activa o desactiva las Transparencias
	HBufC* buf = HBufC::NewLC( 22 );
	buf->Des().Copy(_L("Activar Transparencia?"));
	if (DialogAlert(buf)){	
		Objetos[objSelect].transparencia = true;
	}
	else {
		Objetos[objSelect].transparencia = false;	
	}
	//CleanupStack::PopAndDestroy(buf);	
    redibujar = true;
}

void CBlenderLite::SetInterpolation(){
	//si no hay objetos
	if (Objetos.Count() < 1){return;}
	Cancelar();
	//activa o desactiva las Transparencias
	HBufC* buf = HBufC::NewLC( 21 );
	buf->Des().Copy(_L("Interpolacion Lineal?"));
	if (DialogAlert(buf)){	
		Objetos[objSelect].interpolacion = lineal;
	}
	else {
		Objetos[objSelect].interpolacion = closest;	
	}
	//CleanupStack::PopAndDestroy(buf);	
    redibujar = true;
}

void CBlenderLite::SetTexture(){
	//si no hay objetos
	if (Objetos.Count() < 1){return;}
	Cancelar();
	HBufC* buf = HBufC::NewLC( 30 );
	buf->Des().Copy(_L("ID de Textura (0-4)"));	
	TInt texturaID = DialogNumber(0, 0, 100, buf);
	Objetos[objSelect].textura = true;
	Objetos[objSelect].texturaID = texturaID;
	/*if ( texturaID == 0 ){
		Objetos[objSelect].texturaID = iBaseColor.iID;
	}	
	else if ( texturaID == 1 ){
		Objetos[objSelect].texturaID = iColorGridTextura.iID;
	}*/
	//CleanupStack::PopAndDestroy(buf);
    redibujar = true;
}


void CBlenderLite::SetEditMode(){
	//si no hay objetos
	if (Objetos.Count() < 1){return;}
	Cancelar();
	if ( estado != edicion ){
		EditSelect = 0;
		estado = edicion;
	}	
    else {
    	estado = navegacion;
	}
    redibujar = true;
}

void CBlenderLite::SetSmooth(){
	//si no hay objetos
	if (Objetos.Count() < 1){return;}
	Cancelar();
	if ( Objetos[objSelect].smooth ){
    	Objetos[objSelect].smooth = false;
	}	
    else {
    	Objetos[objSelect].smooth = true;
	}
    redibujar = true;
}

void CBlenderLite::EnfocarObjeto(){
	//si no hay objetos
	if (Objetos.Count() < 1){return;}
	HBufC* buf = HBufC::NewLC( 25 );
	buf->Des().Copy(_L("Posicion X de Camara"));
	posX = DialogNumber((TInt)posX, -100000, 100000, buf);
	CleanupStack::PopAndDestroy(buf);
    redibujar = true;
	//posX = Objetos[objSelect].posX;
	//posY = Objetos[objSelect].posY;
	//posZ = Objetos[objSelect].posZ;
}

void CBlenderLite::SetSpecular(){
	HBufC* buf = HBufC::NewLC( 25 );
	buf->Des().Copy(_L("Valor Specular (0 - 100)"));
	TInt valor = DialogNumber((TInt)(Objetos[objSelect].specular[0]*100.f), 0, 100, buf);
	GLfloat resultado = valor/100.0f;

	Objetos[objSelect].specular[0] = resultado;
	Objetos[objSelect].specular[1] = resultado;
	Objetos[objSelect].specular[2] = resultado;
	Objetos[objSelect].specular[3] = resultado;
	CleanupStack::PopAndDestroy(buf);
    redibujar = true;	
}

void CBlenderLite::SetEmission(){
	HBufC* buf = HBufC::NewLC( 30 );
	
	buf->Des().Copy(_L("Emission Roja (0 - 100)"));
	TInt valor = DialogNumber((TInt)(Objetos[objSelect].emission[0]*100.f), 0, 100, buf);
	Objetos[objSelect].emission[0] = valor/100.0f;
    redibujar = true;

	buf->Des().Copy(_L("Emission Verde (0 - 100)"));
    valor = DialogNumber((TInt)(Objetos[objSelect].emission[1]*100.f), 0, 100, buf);
    Objetos[objSelect].emission[1] = valor/100.0f;
	redibujar = true;	

	buf->Des().Copy(_L("Emission Verde (0 - 100)"));
    valor = DialogNumber((TInt)(Objetos[objSelect].emission[2]*100.f), 0, 100, buf);
    Objetos[objSelect].emission[2] = valor/100.0f;
	CleanupStack::PopAndDestroy(buf);
	redibujar = true;	
}

void CBlenderLite::SetDiffuse(){
	HBufC* buf = HBufC::NewLC( 25 );
	buf->Des().Copy(_L("Rojo (0 - 100)"));
	TInt valorR = DialogNumber((TInt)(Objetos[objSelect].diffuse[0]*100.f), 0, 100, buf);
	Objetos[objSelect].diffuse[0] = (GLfloat)valorR/100.0f;
    redibujar = true;	
	buf->Des().Copy(_L("Verde (0 - 100)"));
	TInt valorG = DialogNumber((TInt)(Objetos[objSelect].diffuse[1]*100.f), 0, 100, buf);
	Objetos[objSelect].diffuse[1] = (GLfloat)valorG/100.0f;
    redibujar = true;	
	buf->Des().Copy(_L("Azul (0 - 100)"));
	TInt valorB = DialogNumber((TInt)(Objetos[objSelect].diffuse[2]*100.f), 0, 100, buf);
	Objetos[objSelect].diffuse[2] = (GLfloat)valorB/100.0f;
	CleanupStack::PopAndDestroy(buf);
    redibujar = true;	
}

void CBlenderLite::SetPerspectiva(){
	// Reinitialize viewport and projection.
	//glViewport( 0, 0, iScreenWidth, iScreenHeight );
	// Recalculate the view frustrum
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
    if ( ortografica ){
    	ortografica = false;
		glFrustumf( FRUSTUM_LEFT * aspectRatio, FRUSTUM_RIGHT * aspectRatio,
					FRUSTUM_BOTTOM, FRUSTUM_TOP,
					FRUSTUM_NEAR, FRUSTUM_FAR );
    }
    else {
    	ortografica = true;
    	glOrthof(-90.0f * aspectRatio, 90.0f * aspectRatio, -90.0f, 90.0f, -0.0f, 1000.0f);    	
    }
    glMatrixMode( GL_MODELVIEW );
    redibujar = true;
}

/*void CBlenderLite::DialogAlert(){
	_LIT( KAlerta, "origen.png" );
	_LIT( KAlerta2, "origen.png2" );
	CAknMessageQueryDialog* dialog = CAknMessageQueryDialog::NewL( KAlerta );

    dialog->PrepareLC( R_AVKON_MESSAGE_QUERY_DIALOG );
    dialog->ButtonGroupContainer().MakeCommandVisible( 
													EAknSoftkeyCancel, EFalse );

    dialog->QueryHeading()->SetTextL( KAlerta2 );

    dialog->RunLD();
	
    HBufC* buf = HBufC::NewLC( 4 );
    buf->Des().Copy(_L("Hola"));
    CAknMessageQueryDialog* dialog = 
                                CAknMessageQueryDialog::NewL( *buf );
    dialog->PrepareLC( R_AVKON_MESSAGE_QUERY_DIALOG );
    //dialog->ButtonGroupContainer().MakeCommandVisible(EAknSoftkeyCancel, EFalse );
	dialog->QueryHeading()->SetTextL(_L("cabeza"));
    dialog->SetHeaderTextL(_L("Example"));
    dialog->SetMessageTextL(_L("Do you want to continue?"));
    dialog->RunLD();
    delete buf;
}*/

enum{
	top,
	front,
	right
};

enum{
	array,
	mirror,
	screw
};

void CBlenderLite::AddModificador(TInt opcion){
	Mesh& obj = Objetos[objSelect];
	if (opcion == mirror){
		//primero crea los array temporales y les suma el espacio del nuevo vertice
		TInt copias = 2;
		GLshort* TempVertex = new GLshort[obj.vertexSize*copias];
		GLbyte* TempNormals = new GLbyte[obj.normalsSize*copias];
		GLushort* TempEdges = new GLushort[obj.edgesSize*copias];
		GLbyte* TempUv = new GLbyte[obj.uvSize*copias];
	    GLushort* TempVertexGroup = new GLushort[obj.vertexGroupSize*copias];
		GLushort** TempVertexGroupIndices = new GLushort*[obj.vertexGroupSize*copias];
		TInt* TempVertexGroupIndicesSize =  new TInt[obj.vertexGroupSize*copias];
		//copia los valores originales al array temporal
		for(TInt a=0; a < obj.vertexSize; a++){
			TempVertex[a] = TempVertex[a+obj.vertexSize] = obj.vertex[a];
			TempNormals[a] = TempNormals[a+obj.vertexSize] = obj.normals[a];
		}
		for(TInt a=0; a < obj.vertexSize/3; a++){
			TempVertex[a*3+obj.vertexSize] = -obj.vertex[a*3];
			//TempNormals[a*3+obj.vertexSize] = obj.normals[a];
		}
		for(TInt a=0; a < obj.edgesSize; a++){
			TempEdges[a] = obj.edges[a];	
			TempEdges[a+obj.edgesSize] = obj.vertexSize/3+obj.edges[a];			
		}
		for(TInt a=0; a < obj.uvSize; a++){
			TempUv[a] = TempUv[a+obj.uvSize] = obj.uv[a];
		}
		for(TInt a=0; a < obj.vertexGroupSize; a++){
			TempVertexGroupIndicesSize[a] = TempVertexGroupIndicesSize[a+obj.vertexGroupSize] = obj.vertexGroupIndiceSize[a];
			TempVertexGroupIndices[a] = TempVertexGroupIndices[a+obj.vertexGroupSize] = new GLushort[obj.vertexGroupIndiceSize[a]];
			TempVertexGroup[a] = obj.vertexGroup[a];
			TempVertexGroup[a+obj.vertexGroupSize] = obj.vertexSize/3+obj.vertexGroup[a];
			/*for(TInt s=0; s < obj.vertexGroupIndiceSize[a]; s++){
				TempVertexGroupIndices[a][s] = obj.vertexGroupIndice[a][s];
				TempVertexGroupIndices[a+obj.vertexGroupSize][s] = obj.vertexSize/3+obj.vertexGroupIndice[a][s];			
			}*/
		}
		TempVertexGroupIndices[0][0] = 11;
		TempVertexGroupIndices[1][0] = 22;
		TempVertexGroupIndices[2][0] = 33;
		TempVertexGroupIndices[3][0] = 44;
		//suma el nuevo tamaño
		obj.vertexSize = obj.vertexSize*copias;
		obj.normalsSize = obj.normalsSize*copias;
		obj.edgesSize = obj.edgesSize*copias;
		obj.uvSize = obj.uvSize*copias;
		obj.vertexGroupSize = obj.vertexGroupSize*copias;
		//asigna el nuevo tamaño
		obj.vertex =  new GLshort[obj.vertexSize];
		obj.normals = new GLbyte[obj.normalsSize];
		obj.edges =   new GLushort[obj.edgesSize];
		obj.uv =      new GLbyte[obj.uvSize];
		obj.vertexGroup = new GLushort[obj.vertexGroupSize];
		obj.vertexGroupIndiceSize = new TInt[obj.vertexGroupSize];
		obj.vertexGroupIndice = new GLushort*[obj.vertexGroupSize];
		//agrega los valores temporales al objeto
		for(TInt a=0; a < obj.vertexSize; a++){
			obj.vertex[a] = TempVertex[a];
			obj.normals[a] = TempNormals[a];			
		}
		for(TInt a=0; a < obj.edgesSize; a++){
			obj.edges[a] = TempEdges[a];			
		}
		for(TInt a=0; a < obj.uvSize; a++){
			obj.uv[a]= TempUv[a];			
		}
		for(TInt a=0; a < obj.vertexGroupSize; a++){
			obj.vertexGroup[a] = TempVertexGroup[a];	
			obj.vertexGroupIndiceSize[a] = TempVertexGroupIndicesSize[a];
			obj.vertexGroupIndice[a] = new GLushort[obj.vertexGroupIndiceSize[a]];
			for(TInt s=0; s < obj.vertexGroupIndiceSize[a]; s++){
				obj.vertexGroupIndice[a][s] = TempVertexGroupIndices[a][s];	
			}
		}

		TempVertexGroupIndices[0][0] = 11;
		TempVertexGroupIndices[1][0] = 22;
		TempVertexGroupIndices[2][0] = 33;
		TempVertexGroupIndices[3][0] = 44;
		HBufC* noteBuf = HBufC::NewLC(30); //TInt::Length(obj.vertexGroupSize)
		_LIT(KFormatString, "indices %d %d %d %d");
		noteBuf->Des().Format(KFormatString,  
				TempVertexGroupIndices[0][0],
				TempVertexGroupIndices[1][0],
				TempVertexGroupIndices[2][0],
				TempVertexGroupIndices[3][0]);
		Mensaje(noteBuf);
		//libera memoria
		delete[] TempVertex;
		delete[] TempNormals;
		delete[] TempEdges;
		delete[] TempUv;
		delete[] TempVertexGroup;
		delete[] TempVertexGroupIndicesSize;
		delete[] TempVertexGroupIndices;
		CleanupStack::PopAndDestroy(noteBuf);
	}
	redibujar = true;
}

void CBlenderLite::SetViewpoint(TInt opcion){
	if (opcion == top){
		rotX = -180.0;
		rotY = 90.0;
	}
	else if (opcion == front){
		rotX = -180.0;
		rotY = 0.0;	
	}
	else if (opcion == right){
		rotX = 90.0;
		rotY = 0.0;		
	}
	redibujar = true;
}

void CBlenderLite::InfoObject(TInt opciones){
	if (opciones == 1){
		HBufC* noteBuf = HBufC::NewLC(30); //TInt::Length(obj.vertexGroupSize)
		_LIT(KFormatString, "size %d Indices %d");
		noteBuf->Des().Format(KFormatString,  Objetos[objSelect].vertexGroupIndiceSize[EditSelect],
				              Objetos[objSelect].vertexGroupIndice[EditSelect][0]);
				                                                                 //,
				              //Objetos[objSelect].vertexGroupIndice[EditSelect][1],
				              //Objetos[objSelect].vertexGroupIndice[EditSelect][2]);
		Mensaje(noteBuf);	
		CleanupStack::PopAndDestroy(noteBuf);
	}
	else if (opciones == 1){ //cantidad de vertices
		HBufC* noteBuf = HBufC::NewLC(30); //TInt::Length(obj.vertexGroupSize)
		_LIT(KFormatString, "obj: %d Vertices: %d");
		noteBuf->Des().Format(KFormatString, objSelect+1, Objetos[objSelect].vertexSize/3);
		Mensaje(noteBuf);	
		CleanupStack::PopAndDestroy(noteBuf);	
	}	
	else if (opciones == 2){ //cantidad de vertices
		HBufC* noteBuf = HBufC::NewLC(35); //TInt::Length(obj.vertexGroupSize)
		_LIT(KFormatString, "VertexGroup: %d group: %d");
		noteBuf->Des().Format(KFormatString, Objetos[objSelect].vertexGroupSize, Objetos[objSelect].vertexGroupIndiceSize[EditSelect]);
		Mensaje(noteBuf);	
		CleanupStack::PopAndDestroy(noteBuf);	
	}	
}; 


void CBlenderLite::Mensaje(HBufC* noteBuf){	        	
	CAknInformationNote* note = new (ELeave) CAknInformationNote();
	note->ExecuteLD(*noteBuf);
	CleanupStack::PopAndDestroy(noteBuf);	
};

void CBlenderLite::MensajeError(HBufC* noteBuf){
    CAknErrorNote* note = new (ELeave) CAknErrorNote();
    note->ExecuteLD(*noteBuf);
    CleanupStack::PopAndDestroy(noteBuf);
}

TBool CBlenderLite::DialogAlert(HBufC* noteBuf){	  
    TBool retVal( EFalse );
    CAknQueryDialog* query = CAknQueryDialog::NewL();
    if( query->ExecuteLD( R_ACCEPT_INVITATION_DLG, *noteBuf )){
        retVal = ETrue;
    }

    CleanupStack::PopAndDestroy( noteBuf );
    return retVal;
}

//
//HBufC* buf = HBufC::NewLC( 20 );
//buf->Des().Copy(_L("Activar Textura?"));

TInt CBlenderLite::DialogNumber(TInt valor, TInt min, TInt max, HBufC* noteBuf ){ //TPtrC etiqueta
	TInt number = valor;
	CAknNumberQueryDialog* dlg = CAknNumberQueryDialog::NewL( number );
	dlg->PrepareLC( R_AVKON_DIALOG_QUERY_VALUE_NUMBER );
	dlg->SetPromptL(*noteBuf);
	dlg->SetMinimumAndMaximum( min, max );
	if (dlg->RunLD()){
		number = number;
	}    
    return number;
}

// -----------------------------------------------------------------------------
// CBlenderLite::SetScreenSize
// Reacts to the dynamic screen size change during execution of this program.
// -----------------------------------------------------------------------------
//
void CBlenderLite::SetScreenSize( TUint aWidth, TUint aHeight ){
    iScreenWidth  = aWidth;
    iScreenHeight = aHeight;
    
    // Notify the texture manager of screen size change
    iTextureManager->SetScreenSize( aWidth, aHeight );

    // Reinitialize viewport and projection.
    glViewport( 0, 0, iScreenWidth, iScreenHeight );

    // Recalculate the view frustrum
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    aspectRatio = (GLfloat)(iScreenWidth) / (GLfloat)(iScreenHeight);
    if ( !ortografica ){
		glFrustumf( FRUSTUM_LEFT * aspectRatio, FRUSTUM_RIGHT * aspectRatio,
					FRUSTUM_BOTTOM, FRUSTUM_TOP,
					FRUSTUM_NEAR, FRUSTUM_FAR );
    }
    else {
    	glOrthof(-90.0f * aspectRatio, 90.0f * aspectRatio, -90.0f, 90.0f, -5.0f, 1000.0f);    	
    }
    glMatrixMode( GL_MODELVIEW );
}

void CBlenderLite::DisplayWarningL( const TDesC &aDescr, TInt aErr) const{
    ASSERT( aErr != KErrNone );

    _LIT(KFailed,"%S: %d");
    CAknWarningNote* note = new (ELeave) CAknWarningNote(ETrue);
    TBuf<128> text;
    text.Format(KFailed, &aDescr, aErr);
    note->ExecuteLD( text );
}

//import
void CBlenderLite::NewTexture(){
    // use dialog
    _LIT(KTitle, "Selecciona la Textura");
    TFileName file(KNullDesC);
    if (AknCommonDialogs::RunSelectDlgLD(file, R_BLENDERLITE_SELECT_DIALOG, KTitle)){
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
        TRAPD(err, LoadFile( file, mainPaneRect.Size() ));
        if (err){
            _LIT(KFileLoadFailed,"Opening image file failed");
            DisplayWarningL(KFileLoadFailed, err);
        }
        /*else{
            // this is a blocking call
            if (! ExecuteWaitNoteL()){
                // cancelled
                iHandler->Cancel();
            }
        }*/
    }
}

void CBlenderLite::LoadFile(const TFileName& aFileName,
                                      const TSize &aSize,
                                      TInt aSelectedFrame){
    /*__ASSERT_ALWAYS(!IsActive(),User::Invariant());
    iSize = aSize;
    iScaledBitmap->Reset();
    iScaledBitmap->Create(aSize, EColor16M);
    LoadFileL(aFileName, aSelectedFrame);*/

	/*_LIT( KNewTexture, "color_grid.jpg" );
	iTextureManager->RequestToLoad( KNewTexture, &iColorGridTextura, false );
	
	//Start to load the textures.
	iTextureManager->DoLoadL();*/
}

void CBlenderLite::ImportOBJ(){
    _LIT(KTitle, "Importar Modelo OBJ");
    TFileName file(KNullDesC);
    if (AknCommonDialogs::RunSelectDlgLD(file, R_BLENDERLITE_SELECT_DIALOG, KTitle)){		
    	RFs fsSession;	
    	User::LeaveIfError(fsSession.Connect());
    	CleanupClosePushL(fsSession);

    	RFile rFile;
    	TInt err;
    	TRAP(err,rFile.Open(fsSession, file, EFileRead));
		if (err != KErrNone){
			_LIT(KFormatString, "Error al abrir: %S");
			HBufC* noteBuf = HBufC::NewLC(file.Length()+16);
			noteBuf->Des().Format(KFormatString, &file);
			MensajeError(noteBuf);  
			CleanupStack::PopAndDestroy(noteBuf);
		}
		else {	
			
			//RFileReadStream inputFileStream(rFile);
			//CleanupClosePushL(inputFileStream);
			
			//_LIT(KHelloString, "Hola");
			HBufC8* lineBuf = HBufC8::NewLC(256);
			TPtr8 line = lineBuf->Des();
			rFile.Read(line);
			rFile.Close();
			
			HBufC* buf = HBufC::NewL(lineBuf->Length());
			buf->Des().Copy(*lineBuf);
			
			//_LIT(KFormatString, "linea: %S");
			//HBufC* noteBuf = HBufC::NewLC(256);
			//noteBuf->Des().Format(KFormatString, &line);
			Mensaje(buf);
			CleanupStack::PopAndDestroy(lineBuf);

            // HBufC descriptor is created from the RFileStream object.
            //HBufC* fileData = HBufC::NewLC(inputFileStream, 32);
			//Mensaje(fileData);

            // Pop loaded resources from the cleanup stack:
            // filedata, inputFileStream, rFile, fsSession
            //CleanupStack::PopAndDestroy(4, &fsSession);   
		} 
    }	
    else {
    	_LIT(KFormatString, "Error al leer el Archivo");
		HBufC* noteBuf = HBufC::NewLC(24);
		noteBuf->Des().Format(KFormatString);
		MensajeError(noteBuf);    	
    }
};
void CBlenderLite::CloseWaitNoteL(){
    // Close and delete the wait note dialog,
    // if it has not been dismissed yet
	if( iWaitDialog ){
		iWaitDialog->ProcessFinishedL();
	}
}

void CBlenderLite::OpenWaitNoteL( TFileName file ){
    //CloseWaitNoteL();

    // Create and view the wait note dialog
	//iWaitDialog = new (ELeave) CAknWaitDialog(reinterpret_cast<CEikDialog**>( &iWaitDialog ), ETrue);
	//iWaitDialog->SetTextL(_L("Cargando archivo: "));
	//iWaitDialog->SetTextL(file.Name());
	//iWaitDialog->RunLD();
	//iWaitDialog->TryExitL(ETrue);
}

//  End of File
