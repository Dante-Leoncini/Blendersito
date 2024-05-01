/*
 * ==============================================================================
 *  Name        : BlenderLite.cpp
 *  Part of     : OpenGLEx / BlenderLite
 * ==============================================================================
 */

// INCLUDES
#include <e32std.h>
#include <e32math.h>
#include <e32base.h>

//para leer archivos
#include <s32file.h>

//debug
//#include <e32cons.h>
//LOCAL_D CConsoleBase* console;
#include "BlenderLite.h"
#include "BlenderLiteContainer.h"

#include "Mesh.h"
#include "mono.h" //modelo 3d
#include "Floor.h" //modelo 3d
//#include "sprite.h" //sprite 2D
#include "Primitivas.h" //sprite 2D
#include "GeometriaUI.h" //elementos de la UI

//#include "blenderlite.hrh"
#include <blenderlite.rsg>
#include <aknmessagequerydialog.h>		// DialogAlertL
#include <aknnotewrappers.h> 
#include <akncommondialogs.h>

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
GLshort mouseX = 0;
GLshort mouseY = 0;
TBool mouseVisible = true;

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
static const GLfloat ListaColores[7][4] = {
		{ MATERIALCOLOR(1.0, 1.0, 1.0, 1.0)     },   //blanco
		{ MATERIALCOLOR(0.94, 0.59, 0.17, 1.0)  },   //naranja 	
		{ MATERIALCOLOR(0.0, 0.0, 0.0, 1.0)     },   //negro
		{ MATERIALCOLOR(0.12, 0.12, 0.12, 1.0)  },   //gris
		{ MATERIALCOLOR(0.94, 0.59, 0.17, 0.25f)},   //naranja transparente
		{ MATERIALCOLOR(0.12, 0.12, 0.12, 1.0)  },   //gris
		{ MATERIALCOLOR(0.22, 0.22, 0.22, 1.0)  },    //cabezera de la barra de herramientas
};

enum{
	blanco,
	naranja,
	negro,
	gris,
	naranjaFace,
	headerColor
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
    cad, luz, suzanne,vertice
};

//CPrimitiva Primitivas;

typedef enum { X, Y, Z, XYZ } Axis;

TInt view = Rendered;
TInt axisSelect = X;

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
	iScreenHeightSplit = (GLfloat)aHeight/2;
    iInputHandler = aInputHandler;
	}

// -----------------------------------------------------------------------------
// CBlenderLite::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CBlenderLite::ConstructL( void ){
	estado = navegacion;

	//debuger
	//console = Console::NewL(_L("Consola"),TSize(KConsFullScreen, KConsFullScreen));
	//Objetos = new Mesh[cantObjetos];
	AddMesh(cubo);
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
	_LIT( KOriginTexture, "origen.png" );
	_LIT( KColorGridTexture, "color_grid.png" );
	_LIT( KMouseTexture, "cursor.png" );	
	_LIT( KLampTexture, "lamp.png" );	
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
		glColor4f(ListaColores[blanco][0],ListaColores[blanco][1],ListaColores[blanco][2],ListaColores[blanco][3]);
		glDisable(GL_COLOR_MATERIAL);
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
			glMaterialfv(   GL_FRONT_AND_BACK, GL_DIFFUSE,  Objetos[o].diffuse   ); 
			glDrawElements( GL_TRIANGLES, Objetos[o].facesSize, GL_UNSIGNED_SHORT, Objetos[o].faces );
		}
		//wireframe view
		else if(objSelect != o){    
		    glDisable( GL_LIGHTING );
			glEnable(GL_COLOR_MATERIAL);
			glColor4f(ListaColores[gris][0],ListaColores[gris][1],ListaColores[gris][2],ListaColores[gris][3]);
			glDrawElements( GL_LINES, Objetos[o].edgesSize, GL_UNSIGNED_SHORT, Objetos[o].edges );	
		}  
		
		//dibuja el borde seleccionado
		if(objSelect == o){
		    glDisable( GL_LIGHTING );
			glEnable(GL_COLOR_MATERIAL);
			glDisable( GL_TEXTURE_2D );  
			glEnable(GL_POLYGON_OFFSET_FILL);
			//si se esta editando
			if (estado == edicion || estado == translacionVertex){
				//bordes
				glPolygonOffset(1.0, -1.0);
				glLineWidth(1);	 
				glColor4f(ListaColores[gris][0],
						  ListaColores[gris][1],
						  ListaColores[gris][2],
						  ListaColores[gris][3]);
				glDrawElements( GL_LINES, Objetos[o].edgesSize, GL_UNSIGNED_SHORT, Objetos[o].edges );
				//vertices
				if (tipoSelect == vertexSelect){
					glPolygonOffset(1.0, -4.0);
					glColor4f(ListaColores[negro][0],
							  ListaColores[negro][1],
							  ListaColores[negro][2],
							  ListaColores[negro][3]);
					glPointSize(4);
					glDrawElements( GL_POINTS, Objetos[o].vertexGroupSize, GL_UNSIGNED_SHORT, Objetos[o].vertexGroup);
					//vertice seleccionado
					if (Objetos[o].vertexGroupSize > 0){
						glPolygonOffset(1.0, -10.0);
						glColor4f(ListaColores[blanco][0],ListaColores[blanco][1],ListaColores[blanco][2],ListaColores[blanco][3]);
					    glDrawArrays( GL_POINTS, Objetos[o].vertexGroup[EditSelect], 1 );
						//glMaterialfv(   GL_FRONT_AND_BACK, GL_EMISSION, ListaColores[gris] );					
					}					
				}
				//borde seleccionado
				else if (tipoSelect == edgeSelect){		
					//vertice seleccionado
					if (Objetos[o].edgesSize > 0){		
						glPolygonOffset(1.0, -10.0);
						glColor4f(ListaColores[blanco][0],ListaColores[blanco][1],ListaColores[blanco][2],ListaColores[blanco][3]);
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
						glColor4f(ListaColores[naranjaFace][0],ListaColores[naranjaFace][1],ListaColores[naranjaFace][2],ListaColores[naranjaFace][3]);
						glDrawElements( GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, &Objetos[o].faces[EditSelect*3]);
						glDisable(GL_BLEND);
					}				
				}
			}
			else if (view != 2){
				glPolygonOffset(1.0, 200.0);
				glLineWidth(3);	 
				glColor4f(ListaColores[colorBordeSelect][0],ListaColores[colorBordeSelect][1],ListaColores[colorBordeSelect][2],ListaColores[colorBordeSelect][3]);
				glDrawElements( GL_LINES, Objetos[o].edgesSize, GL_UNSIGNED_SHORT, Objetos[o].edges );	
			}	
			else { //bordes seleccionados en wireframe
				glDisable(GL_POLYGON_OFFSET_FILL);
				glLineWidth(1);		
				glColor4f(ListaColores[colorBordeSelect][0],ListaColores[colorBordeSelect][1],ListaColores[colorBordeSelect][2],ListaColores[colorBordeSelect][3]);
				glDrawElements( GL_LINES, Objetos[o].edgesSize, GL_UNSIGNED_SHORT, Objetos[o].edges );	
			}
		};	
		
	    glPopMatrix(); //reinicia la matrix a donde se guardo  
	}

	//dibujar las lineas del piso
	glLineWidth(1);	
	glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable( GL_TEXTURE_2D );
    glDisable( GL_LIGHTING );
	glEnable(GL_COLOR_MATERIAL);
	glMaterialfv(   GL_FRONT_AND_BACK, GL_DIFFUSE,  ListaColores[negro]  );
	glMaterialfv(   GL_FRONT_AND_BACK, GL_AMBIENT,  ListaColores[negro]  );
	glMaterialfv(   GL_FRONT_AND_BACK, GL_SPECULAR, ListaColores[negro] );
    glVertexPointer( 3, GL_SHORT, 0, objVertexdataFloor );
	glNormalPointer( GL_BYTE, 0, objNormaldataFloor );
	

	//dibuja el piso
	glPushMatrix(); //guarda la matrix
	glTranslatef( 0, -5000, 0);
	glColor4f(LineaPiso[0],LineaPiso[1],LineaPiso[2],LineaPiso[3]);
	glDrawElements( GL_LINES, objFacesFloor, GL_UNSIGNED_SHORT, objFacedataFloor );	 
	//linea Verde
	glLineWidth(2);
	glColor4f(LineaPisoRoja[0],LineaPisoRoja[1],LineaPisoRoja[2],LineaPisoRoja[3]);
	glDrawElements( GL_LINES, 2, GL_UNSIGNED_SHORT, EjeRojo );	
	//linea Roja
	glColor4f(LineaPisoVerde[0],LineaPisoVerde[1],LineaPisoVerde[2],LineaPisoVerde[3]);
	glDrawElements( GL_LINES, 2, GL_UNSIGNED_SHORT, EjeVerde ); 
    glPopMatrix(); //reinicia la matrix a donde se guardo	
	
    //dibuja los ejes de transformacion    
	glDisable( GL_DEPTH_TEST );
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
		if (axisSelect == X){
			glColor4f(ColorTransformX[0],ColorTransformX[1],ColorTransformX[2],ColorTransformX[3]);
			glDrawElements( GL_LINES, 2, GL_UNSIGNED_SHORT, EjeRojo );				
		}
		else if (axisSelect == Y){
			glColor4f(ColorTransformY[0],ColorTransformY[1],ColorTransformY[2],ColorTransformY[3]);
			glDrawElements( GL_LINES, 2, GL_UNSIGNED_SHORT, EjeVerde ); 				
		}
		else if (axisSelect == Z){
			glColor4f(ColorTransformZ[0],ColorTransformZ[1],ColorTransformZ[2],ColorTransformZ[3]);
			glDrawElements( GL_LINES, 2, GL_UNSIGNED_SHORT, EjeAzul ); 				
		}	
		else if (axisSelect == XYZ){
			glColor4f(ColorTransformX[0],ColorTransformX[1],ColorTransformX[2],ColorTransformX[3]);
			glDrawElements( GL_LINES, 2, GL_UNSIGNED_SHORT, EjeRojo );	
			glColor4f(ColorTransformY[0],ColorTransformY[1],ColorTransformY[2],ColorTransformY[3]);
			glDrawElements( GL_LINES, 2, GL_UNSIGNED_SHORT, EjeVerde ); 
			glColor4f(ColorTransformZ[0],ColorTransformZ[1],ColorTransformZ[2],ColorTransformZ[3]);
			glDrawElements( GL_LINES, 2, GL_UNSIGNED_SHORT, EjeAzul ); 				
		}	
	    glPopMatrix(); //reinicia la matrix a donde se guardo	
	}
	
	//dibuja el origen del objeto
	if (Objetos.Count() > 0){
	    glPopMatrix(); //reinicia la matrix a donde se guardo
		glTranslatef( Objetos[objSelect].posX, Objetos[objSelect].posZ, Objetos[objSelect].posY);
		glEnable( GL_CULL_FACE ); // Enable back face culling.
		// Enable point sprites.
		glEnable( GL_TEXTURE_2D );
		glEnable( GL_POINT_SPRITE_OES );
		// Enable blending for transparency.
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		// Make the points bigger.
		glPointSize( 8 );
		glColor4f(ListaColores[blanco][0],ListaColores[blanco][1],ListaColores[blanco][2],ListaColores[blanco][3]);
		GLshort posicionPunto[3]={0, 0, 0};
	    glVertexPointer( 3, GL_SHORT, 0, posicionPunto );
	    glBindTexture( GL_TEXTURE_2D, iOrigenTextura.iID ); //selecciona la textura
	    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	    glTexEnvi( GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_TRUE );
	    glDrawArrays( GL_POINTS, 0, 1 );
	    glTexEnvi( GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_FALSE);
	    glPopMatrix(); //reinicia la matrix a donde se guardo	
	}

	dibujarUI();

    //termino de dibujar
    redibujar = false;
}

//Se encarga de la nueva UI 3d
void CBlenderLite::dibujarUI(){
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
    GLfloat aspectRatioNuevo = (GLfloat)(iScreenWidth) / (GLfloat)(iScreenHeight);
	glOrthof(-iScreenHeightSplit * aspectRatioNuevo, iScreenHeightSplit * aspectRatioNuevo, -iScreenHeightSplit, iScreenHeightSplit, 0.0f, 1000.0f); 
    glMatrixMode( GL_MODELVIEW );

	glLoadIdentity();
	glTranslatef( -iScreenHeightSplit* aspectRatioNuevo, iScreenHeightSplit, -100);
	glScalex( 71400, 71400, 71400 );	

	glDisable( GL_DEPTH_TEST ); //quita el Zbuffer
	glDisable( GL_CULL_FACE ); // Enable back face culling.
	//glDisable( GL_POINT_SPRITE_OES );
	//glTexEnvi( GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_FALSE);
	//glEnable( GL_TEXTURE_2D ); // Permite usar texturas
	glEnable( GL_BLEND ); //permite transparencias
	//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glDisable( GL_LIGHTING ); //quita las luces
	glDisable(GL_POLYGON_OFFSET_FILL); //quita el offset
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //hace que sea pixelada
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //hace que sea pixelada
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
    glVertexPointer( 3, GL_SHORT, 0, SpriteVertices );
	glNormalPointer( GL_BYTE, 0, SpriteNormal );
	glTexCoordPointer( 2, GL_BYTE, 0, SpriteUV ); //SpriteUvSize
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glBindTexture( GL_TEXTURE_2D, iColorGridTextura.iID ); //selecciona la textura
	//SetSprite(255,255,0,0);

	//header	
	glDisable( GL_TEXTURE_2D ); // Permite usar texturas
	glColor4f(ListaColores[headerColor][0],ListaColores[headerColor][1],ListaColores[headerColor][2],0.5f);
	DibujarRectangulo(iScreenWidth,24, 0,0);
	
	glEnable( GL_TEXTURE_2D ); // Permite usar texturas
	glColor4f(ListaColores[blanco][0],ListaColores[blanco][1],ListaColores[blanco][2],ListaColores[blanco][3]);	
	glBindTexture( GL_TEXTURE_2D, iMouseTextura.iID ); 

	//que editor esta abierto
	SetSprite(14,14,113*2,49*2,28,28,5,-5);

	if (estado == navegacion){
		SetSprite(14,14,1*2,113*2,28,28,26,-5);
	}
	else if (estado == edicion){
		SetSprite(14,14,1*2,97*2,28,28,26,-5);
	}
	//dibuja el mouse por arriba de todo
	if (mouseVisible){
		SetSprite(10,17,1,1,20,32,mouseX,mouseY);
	}

	//resetea la perspectiva	
	ortografica = !ortografica;
	SetPerspectiva(false);
}

void CBlenderLite::SetSprite(GLshort ancho, GLshort alto, GLshort origenX, GLshort origenY, GLshort U, GLshort V, GLshort x, GLshort y){
	glPushMatrix(); //guarda la matrix
	glTranslatef( x, y, 0);
	//cambia el tamaño
	SpriteVertices[3] = SpriteVertices[6] = ancho+1;
	SpriteVertices[7] = SpriteVertices[10] = -(alto+1);
	//recalcula uv
	SpriteUV[0] = (GLbyte)(-128+origenX);
	SpriteUV[1] = (GLbyte)(-128+origenY);
	SpriteUV[2] = (GLbyte)(-128+U+origenX);
	SpriteUV[3] = (GLbyte)(-128+origenY);
	SpriteUV[4] = (GLbyte)(-128+U+origenX);
	SpriteUV[5] = (GLbyte)(-128+V+origenY);
	SpriteUV[6] = (GLbyte)(-128+origenX);
	SpriteUV[7] = (GLbyte)(-128+V+origenY);
	glDrawElements( GL_TRIANGLES, SpriteFacesSize, GL_UNSIGNED_SHORT, SpriteFaces );
	glPopMatrix(); //reinicia la matrix a donde se guardo	
}

void CBlenderLite::DibujarRectangulo(GLshort ancho, GLshort alto, GLshort x, GLshort y){
	glPushMatrix(); //guarda la matrix
	glTranslatef( x, y, 0);
	//cambia el tamaño
	SpriteVertices[3] = SpriteVertices[6] = ancho+1;
	SpriteVertices[7] = SpriteVertices[10] = -(alto+1);
	glDrawElements( GL_TRIANGLES, SpriteFacesSize, GL_UNSIGNED_SHORT, SpriteFaces );
	glPopMatrix(); //reinicia la matrix a donde se guardo	
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
		//mueve el mouse
		if (mouseVisible){
			mouseX--;
			if (mouseX < 0){mouseX = 0;};
		}

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
		//mueve el mouse
		if (mouseVisible){
			mouseX++;
			if (mouseX > iScreenWidth-11){mouseX = iScreenWidth-11;};
		}

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
		//mueve el mouse
		if (mouseVisible){
			mouseY++;
			if (mouseY > 0){mouseY = 0;};
		}

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
		//mueve el mouse
		if (mouseVisible){
			mouseY--;
			if (mouseY < -iScreenHeight-17){mouseY = -iScreenHeight-17;};
		}

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
		_LIT(KStaticErrorMessage, "¿Eliminar el Objeto?");
		if (!DialogAlert(KStaticErrorMessage)){return;}
		Cancelar();
		//estado = navegacion;
		//libera la memoria de los punteros primero		

		// Obtener el objeto Mesh seleccionado
		Mesh* mesh = &Objetos[objSelect];

		// Liberar memoria de los punteros del objeto seleccionado
		delete[] mesh->vertex;
		delete[] mesh->normals;
		delete[] mesh->faces;
		delete[] mesh->edges;
		delete[] mesh->uv;
		delete[] mesh->vertexGroup;
		delete[] mesh->vertexGroupIndiceSize;
		for (TInt j = 0; j < mesh->vertexGroupSize; ++j) {
			delete[] mesh->vertexGroupIndice[j];
		}
		delete[] mesh->vertexGroupIndice;

		// Limpiar NewFaces y liberar memoria
		mesh->NewFaces.Reset();

		Objetos.Remove(objSelect);
		if (Objetos.Count()-1 < objSelect){
			objSelect = Objetos.Count()-1;		
		}
	}
	else if (estado == edicion){
		if (Objetos[objSelect].vertexGroupSize < 1){return;}
		//pregunta de confirmacion
		_LIT(KStaticErrorMessage, "¿Eliminar Vertice?");
		if (!DialogAlert(KStaticErrorMessage)){return;}
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
	}
    redibujar = true;	
}

void CBlenderLite::AddMesh( int modelo ){
	Cancelar();
	Mesh obj;
	obj.visible = true;
	obj.posX = obj.posY = obj.posZ = obj.rotX = obj.rotY = obj.rotZ = 0;
	obj.specular[0] = obj.specular[1] = obj.specular[2] = obj.specular[3] = 0.3;
	obj.diffuse[0] = obj.diffuse[1] = obj.diffuse[2] = obj.diffuse[3] = 1.0;
	obj.emission[0] = obj.emission[1] = obj.emission[2] = obj.emission[3] = 0.0;
	obj.smooth = true;
	obj.textura = false;
	obj.transparencia = false;
	obj.interpolacion = lineal;
	
    if (modelo == cubo){ 
    	obj.vertexSize = obj.normalsSize = 24 * 3;
		obj.facesSize = 12 * 3;
		obj.edgesSize = 12 * 6;
		obj.uvSize = 24 * 2;
		obj.texturaID = 1;
		obj.scaleX = obj.scaleY = obj.scaleZ = 45000;		

		obj.vertex = new GLshort[24 * 3];
		obj.normals = new GLbyte[24 * 3];
		obj.faces = new GLushort[12 * 3];
		obj.edges = new GLushort[12 * 6];
		obj.uv = new GLbyte[24 * 2];
		for (int i = 0; i < 24*3; i++) {
			obj.vertex[i] = CuboVertices[i];
			obj.normals[i] = CuboNormals[i];
		}
		for (int i = 0; i < 24*2; i++) {
			//obj.uv[i] = CuboUV[i];
			obj.uv[i] = CuboNormals[i];
		}
		for (int i = 0; i < 12*3; i++) {
			obj.faces[i] = CuboTriangles[i];
		}
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
	else {return;} 
    
	Objetos.Append(obj);	
		
	objSelect = Objetos.Count()-1;
	Objetos[objSelect].AgruparVertices();
	Objetos[objSelect].RecalcularBordes();
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
		//suma el nuevo tama�o
		obj.vertexSize += 3;
		obj.normalsSize += 3;
		obj.edgesSize += 2;
		obj.uvSize += 2;
		obj.vertexGroupSize++;
		//asigna el nuevo tama�o
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

void CBlenderLite::SetPerspectiva(TBool redibuja ){
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
    redibujar = redibuja;
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
		//suma el nuevo tama�o
		obj.vertexSize = obj.vertexSize*copias;
		obj.normalsSize = obj.normalsSize*copias;
		obj.edgesSize = obj.edgesSize*copias;
		obj.uvSize = obj.uvSize*copias;
		obj.vertexGroupSize = obj.vertexGroupSize*copias;
		//asigna el nuevo tama�o
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
		/*HBufC* noteBuf = HBufC::NewLC(30); //TInt::Length(obj.vertexGroupSize)
		_LIT(KFormatString, "indices %d %d %d %d");
		noteBuf->Des().Format(KFormatString,  
				TempVertexGroupIndices[0][0],
				TempVertexGroupIndices[1][0],
				TempVertexGroupIndices[2][0],
				TempVertexGroupIndices[3][0]);
		Mensaje(noteBuf);*/
		//libera memoria
		delete[] TempVertex;
		delete[] TempNormals;
		delete[] TempEdges;
		delete[] TempUv;
		delete[] TempVertexGroup;
		delete[] TempVertexGroupIndicesSize;
		delete[] TempVertexGroupIndices;
		//CleanupStack::PopAndDestroy(noteBuf);
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

/*TBool CBlenderLite::DialogAlert(HBufC* noteBuf){	  
    TBool retVal( EFalse );
    CAknQueryDialog* query = CAknQueryDialog::NewL();
    if( query->ExecuteLD( R_ACCEPT_INVITATION_DLG, *noteBuf )) {
        retVal = ETrue;
    }
	CleanupStack::PopAndDestroy( noteBuf );

    return retVal;
}*/

TBool CBlenderLite::DialogAlert(HBufC* noteBuf) {
    TBool retVal(EFalse);
	CAknQueryDialog* query = CAknQueryDialog::NewL();
	if (query->ExecuteLD(R_ACCEPT_INVITATION_DLG, *noteBuf)) {
		retVal = ETrue;
	}
	CleanupStack::PopAndDestroy( noteBuf );
    return retVal;
}


TBool CBlenderLite::DialogAlert(const TDesC& noteBuf) {
    TBool retVal(EFalse);
	CAknQueryDialog* query = CAknQueryDialog::NewL();
	if (query->ExecuteLD(R_ACCEPT_INVITATION_DLG, noteBuf)) {
		retVal = ETrue;
	}

    return retVal;
}

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
void CBlenderLite::SetScreenSize( TUint aWidth, TUint aHeight, TBool widescreen = false ){
    iScreenWidth  = aWidth;
    iScreenHeight = aHeight;
	iScreenHeightSplit = (GLfloat)aHeight/2;
    
    // Notify the texture manager of screen size change
    iTextureManager->SetScreenSize( aWidth, aHeight );

    // Reinitialize viewport and projection.
    glViewport( 0, 0, iScreenWidth, iScreenHeight );

    // Recalculate the view frustrum
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    aspectRatio = (GLfloat)(iScreenWidth) / (GLfloat)(iScreenHeight);
    if (widescreen && iScreenWidth > iScreenHeight){
        aspectRatio = (GLfloat)(16) / (GLfloat)(9);    	
    };
    if ( !ortografica ){
		glFrustumf( FRUSTUM_LEFT * aspectRatio, FRUSTUM_RIGHT * aspectRatio,
					FRUSTUM_BOTTOM, FRUSTUM_TOP,
					FRUSTUM_NEAR, FRUSTUM_FAR );
    }
    else {
    	glOrthof(-90.0f * aspectRatio, 90.0f * aspectRatio, -90.0f, 90.0f, -5.0f, 1000.0f);    	
    }
    glMatrixMode( GL_MODELVIEW );
	redibujar = true;
}

void CBlenderLite::SetScreenSize( TUint aWidth, TUint aHeight ){
    iScreenWidth  = aWidth;
    iScreenHeight = aHeight;
	iScreenHeightSplit = (GLfloat)aHeight/2;
    
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
	redibujar = true;
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
			return;
		}		
		//crea el objeto
		Cancelar();
		Mesh obj;
		obj.visible = true;
		obj.posX = obj.posY = obj.posZ = 0;
		obj.rotX = obj.rotY = obj.rotZ = 0;
		obj.specular[0] = obj.specular[1] = obj.specular[2] = obj.specular[3] = 0.3;
		obj.diffuse[0] = obj.diffuse[1] = obj.diffuse[2] = obj.diffuse[3] = 1.0;
		obj.emission[0] = obj.emission[1] = obj.emission[2] = obj.emission[3] = 0.0;
		obj.smooth = true;
		obj.textura = false;
		obj.transparencia = false;
		obj.interpolacion = lineal; //interpolacion lineal

		TInt vertices = 0;
		TInt caras = 0;
		TInt normales = 0;
		TInt textura = 0;			
		TInt pos = 0;			

		TBool continuarLeyendo = ETrue; // Variable para controlar la lectura del archivo
		TBuf8<512> lineBuf;
		TBuf8<512> tempBuf; // Búfer temporal para almacenar parte de una línea si es necesario
		
		//los obj tienen una lista de normales. a veces las normales se pueden repetir y esa es la logica
		//tambien se puede repetir coordenadas de texturas asi que en vez de tener los uv y normals vertice por vertices.. tienen un listado
		GLshort* ListVertices = new GLshort[0];
		GLbyte* ListNormals = new GLbyte[0];
		GLbyte* ListUVs = new GLbyte[0];
		TInt* ListCaras = new TInt[0];

		// Leer línea por línea
		//while (rFile.Read(lineBuf) == KErrNone) {
		while (continuarLeyendo) {
			// Leer una línea del archivo
			err = rFile.Read(lineBuf);
			if (err != KErrNone) {
				// Error al leer o final del archivo, salir del bucle
				break;
			}

			// Concatenar el contenido actual de lineBuf con el contenido anterior de tempBuf
			tempBuf.Append(lineBuf);
			lineBuf.Copy(tempBuf); // Copiar la combinación al búfer de línea para su procesamiento

			// Procesar la línea hasta que no haya más saltos de línea o se llegue al final del archivo
			while ((pos = lineBuf.Locate('\n')) != KErrNotFound || (pos = lineBuf.Locate('\r')) != KErrNotFound) {
				//lineas++;
				// Obtener la línea hasta el salto de línea
				TPtrC8 line = lineBuf.Left(pos);

				// Convertir a descriptor de 16 bits si es necesario
				HBufC* line16 = HBufC::NewLC(line.Length());
				line16->Des().Copy(line);

				//es para revisar linea por linea
				//DialogAlert(line16);

				// Revisar si empieza con "v " vertices, caras y normales

				// Contador para almacenar la cantidad de "strings" separados por espacios
				TInt contador = 0;
				if (line16->Left(2) == _L("v ")) {
					contador = 0;						
					//copia los vertices temporalmente
					GLshort* temVertices = new GLshort[vertices*3];
					for(TInt a=0; a < vertices*3; a++){
						temVertices[a] = ListVertices[a];
					}

					TLex lex(line16->Des().Mid(2));  // Inicializa TLex con la subcadena a partir del tercer carácter
					// Iterar mientras no se llegue al final del descriptor y se haya alcanzado el límite de 8 strings
					while (!lex.Eos() && contador < 8) {							
						TPtrC currentString = lex.NextToken(); // Mostrar el mensaje con el valor actual del "string" y el contador						
						TLex testLex(currentString);// Crear un nuevo objeto TLex para la prueba
						
						// Convertir el string en un número TInt
						TReal number = 0.0;
						TInt err = testLex.Val(number, '.');
						if (err == KErrNone && contador < 3) {
							number = number*1000;								
							GLshort glNumber = static_cast<GLshort>(number); // Conversión a GLbyte
							temVertices[vertices*3+contador] = glNumber;

							/*HBufC* noteBuf = HBufC::NewLC(100);
							_LIT(KFormatString, "vertice: %d-%d Valor: %d");
							noteBuf->Des().Format(KFormatString, vertices+1, contador+1, temVertices[vertices*3+contador]);
							DialogAlert(noteBuf);*/
						}
						else {
							temVertices[vertices*3+contador] = 0;
						}

						// Avanzar al siguiente "string" que no sea espacio en blanco
						lex.SkipSpace();

						// Incrementar el contador para llevar la cuenta de los strings procesados
						contador++;
					}
					delete[] ListVertices;
					ListVertices = temVertices;
					vertices++;
				}
				//orientacion de las normales
				else if (line16->Left(3) == _L("vn ")){
					contador = 0;
					//copia los vertices temporalmente
					GLbyte* temNormales = new GLbyte[normales*3];
					for(TInt a=0; a < normales*3; a++){
						temNormales[a] = ListNormals[a];
					}

					TLex lex(line16->Des().Mid(3));  // Inicializa TLex con la subcadena a partir del tercer carácter

					// Iterar mientras no se llegue al final del descriptor y se haya alcanzado el límite de 8 strings
					while (!lex.Eos() && contador < 3) {							
						TPtrC currentString = lex.NextToken(); // Mostrar el mensaje con el valor actual del "string" y el contador
						TLex testLex(currentString); // Crear un nuevo objeto TLex para la prueba
						
						TReal number = 0.0;
						TInt err = testLex.Val(number, '.');
						if (err == KErrNone) {
							number = ((number +1)/2)* 255.0 - 128.0;
							if (number > 127.0){number = 127.0;}
							else if (number < -128.0){number = -128.0;}
							GLbyte glNumber = static_cast<GLbyte>(number); // Conversión a GLbyte
							temNormales[normales*3+contador] = glNumber;
						}
						else {
							temNormales[normales*3+contador] = 0;
						}
						lex.SkipSpace();
						contador++;
					}						
					delete[] ListNormals;
					ListNormals = temNormales;
					normales++;
				}
				//coordenadas UV para texturas
				else if (line16->Left(3) == _L("vt ")){						
					contador = 0;
					//copia los vertices temporalmente
					GLbyte* temUVs = new GLbyte[textura*2];
					for(int a=0; a < textura*2; a++){
						temUVs[a] = ListUVs[a];
					}
					TLex lex(line16->Des().Mid(3));  // Inicializa TLex con la subcadena a partir del tercer carácter
					// Iterar mientras no se llegue al final del descriptor y se haya alcanzado el límite de 8 strings
					while (!lex.Eos() && contador < 2) {							
						TPtrC currentString = lex.NextToken(); // Mostrar el mensaje con el valor actual del "string" y el contador
						TLex testLex(currentString); // Crear un nuevo objeto TLex para la prueba
						
						TReal number = 0.0;
						TInt err = testLex.Val(number, '.');
						if (err == KErrNone) {		
							// Invertir coordenadas X multiplicando por -1
							if (contador == 0) {
								number = -number+1;
							}					
							number = number * 255.0 - 128.0;
							if (number > 127.0){number = 127.0;}
							else if (number < -128.0){number = -128.0;}
							GLbyte glNumber = static_cast<GLbyte>(number); // Conversión a GLbyte
							temUVs[textura*2+contador] = glNumber;

							/*HBufC* noteBuf = HBufC::NewLC(100);
							_LIT(KFormatString, "tex: %d c: %d st: %S TReal: %f GLbyte %d");
							noteBuf->Des().Format(KFormatString, textura+1, contador, &currentString, number, temUVs[textura*2+contador]);
							DialogAlert(noteBuf);*/
						}
						else {
							temUVs[textura*2+contador] = 0;
						}
						contador++;
						lex.SkipSpace();
					}
					
					delete[] ListUVs;
					ListUVs = temUVs;

					/*TBuf<128> unicodeLine;  // Buffer para almacenar la versión Unicode de 'line'
					unicodeLine.Copy(line);
					HBufC* noteBuf = HBufC::NewLC(100);
					_LIT(KFormatString, "tex: %d st: %S UV: %d, %d");
					noteBuf->Des().Format(KFormatString, textura+1, &unicodeLine, ListUVs[textura*2], ListUVs[textura*2+1]);
					DialogAlert(noteBuf);*/

					textura++;
				}
				//las caras
				else if (line16->Left(2) == _L("f ")){
					contador = 0;
					TInt conTBarras = 0;
					//copia los vertices temporalmente
					TInt* temCaras = new TInt[(caras+1)*9];
					for(TInt a=0; a < caras*9; a++){
						temCaras[a] = ListCaras[a];
					}

					TLex lex(line16->Des().Mid(2));  // Inicializa TLex con la subcadena a partir del tercer carácter

					// Iterar mientras no se llegue al final del descriptor y se haya alcanzado el límite de 8 strings
					while (!lex.Eos() && contador < 3) {				
						TPtrC currentString = lex.NextToken(); // Mostrar el mensaje con el valor actual del "string" y el contador

						TInt startPos = 0; // Posición inicial
						TInt tokenLength = 0; // Longitud del token
						conTBarras = 0;

						// Recorre la cadena hasta encontrar el final
						while (startPos < currentString.Length()) {
							// Busca la posición de la siguiente barra diagonal
							TInt nextSlashPos = currentString.Mid(startPos).Locate('/');

							// Si no se encuentra una barra diagonal, asume que es el último token
							if (nextSlashPos == KErrNotFound) {
								tokenLength = currentString.Length() - startPos;
							} else {
								tokenLength = nextSlashPos; // Longitud del token hasta la barra diagonal
							}

							// Extrae el token utilizando la posición y longitud calculadas
							TPtrC token = currentString.Mid(startPos, tokenLength);
							TLex testLex(token); // Crear un nuevo objeto TLex para la prueba

							// Convertir el string en un número TInt
							TInt number = 0;
							TInt err = testLex.Val(number);
							if (err == KErrNone && conTBarras < 3) {
								temCaras[caras*9+contador*3+conTBarras] = number-1;
							}

							// Actualiza la posición inicial para el próximo token
							startPos += tokenLength + 1; // Suma 1 para omitir la barra diagonal

							/*HBufC* noteBuf = HBufC::NewLC(100);
							_LIT(KFormatString, "f: %d ct: %d st: %S num: %d");
							noteBuf->Des().Format(KFormatString, contador, conTBarras, &currentString, temCaras[caras*9+contador*3+conTBarras]);
							DialogAlert(noteBuf);*/

							conTBarras++;
						}	

						/*HBufC* noteBuf = HBufC::NewLC(100);
						_LIT(KFormatString, "f: %d st: %S v: %d n: %d uv: %d");
						noteBuf->Des().Format(KFormatString, (contador+1), &currentString, temCaras[caras*9+contador*3+0]+1, temCaras[caras*9+contador*3+1]+1, temCaras[caras*9+contador*3+2]+1);
						DialogAlert(noteBuf);*/					

						lex.SkipSpace();
						contador++;
					}
					
					delete[] ListCaras;
					ListCaras = temCaras;

					/*HBufC* noteBuf = HBufC::NewLC(100);
					_LIT(KFormatString, "face: %d v: %d, %d, %d");
					noteBuf->Des().Format(KFormatString, caras+1, ListCaras[caras*9]+1, ListCaras[caras*9+3]+1, ListCaras[caras*9+6]+1);
					DialogAlert(noteBuf);	*/				
					caras++;
				}

				// Eliminar la parte de la línea ya procesada y el carácter de salto de línea
				lineBuf.Delete(0, pos + 1);

				// Eliminar también los espacios en blanco iniciales
				lineBuf.TrimLeft();
			}	

			/*if (caras > 1){
				HBufC* noteBuf = HBufC::NewLC(100);
				_LIT(KFormatString, "v: %d, %d, %d, %d, %d, %d");
				noteBuf->Des().Format(KFormatString, ListCaras[0]+1, ListCaras[3]+1, ListCaras[6]+1, ListCaras[9]+1, ListCaras[12]+1, ListCaras[15]+1);
				DialogAlert(noteBuf);
			}*/				

			// Almacenar el contenido restante de lineBuf en tempBuf para la próxima iteración
			tempBuf.Copy(lineBuf);

			// Verificar si quedan más caracteres en la línea actual
			continuarLeyendo = (lineBuf.Length() > 0);   
		}

		/*HBufC* noteBuf = HBufC::NewLC(100);
		_LIT(KFormatString, "v: %d, %d, %d, %d, %d, %d");
		noteBuf->Des().Format(KFormatString, ListCaras[0]+1, ListCaras[3]+1, ListCaras[6]+1, ListCaras[9]+1, ListCaras[12]+1, ListCaras[15]+1);
		DialogAlert(noteBuf);*/

		/*HBufC* noteBuf = HBufC::NewLC(75); //TInt::Length(obj.vertexGroupSize)
		_LIT(KFormatString, "Termino de leer");
		noteBuf->Des().Format(KFormatString);
		DialogAlert(noteBuf);*/

		// cuantos vertices tiene
		/*HBufC* noteBuf = HBufC::NewLC(100); //TInt::Length(obj.vertexGroupSize)
		_LIT(KFormatString, "Vertices: %d Caras: %d Normales %d UVs %d");
		noteBuf->Des().Format(KFormatString, vertices, caras, normales, textura);
		DialogAlert(noteBuf);
		//Mensaje(noteBuf);*/

		// Cerrar el archivo
		rFile.Close();

		//liberar memoria			
		CleanupStack::PopAndDestroy(&rFile);
		CleanupStack::PopAndDestroy(&fsSession);

		obj.vertexSize = vertices*3;
		obj.normalsSize = vertices*3;
		obj.facesSize = caras*3;
		obj.edgesSize = caras*6;
		obj.uvSize = vertices*2; //UV  	
		obj.texturaID = 1; //defecto
		obj.scaleX = 65000;
		obj.scaleY = 65000;
		obj.scaleZ = 65000;

		obj.edges = new GLushort[obj.edgesSize];
		obj.normals = new GLbyte[obj.normalsSize];
		obj.vertex = new GLshort[obj.vertexSize];
		obj.uv = new GLbyte[obj.uvSize];
		obj.faces = new GLushort[obj.facesSize];
		
		for(TInt v=0; v < obj.vertexSize; v++){
			obj.vertex[v] = ListVertices[v];
		}
		for(TInt a=0; a < caras; a++){
			for(TInt f=0; f < 3; f++){
				TInt indice = a*9+f*3;
				obj.faces[a*3+f] = ListCaras[indice];
				/*for(TInt v=0; v < 3; v++){
					//a*9 es que ListCaras tiene 9 valores por cara, 3 vertices, 3 normales y 3 UV
					//f*3 es para ir por las distintas "/" 1/1/1
					//obj.vertex[ListCaras[a*9+f*3]*3+v] = ListVertices[ListCaras[a*9+f*3]*3+v];
					HBufC* noteBuf = HBufC::NewLC(100);
					_LIT(KFormatString, "face %d v%d-%d: %d valor: %d");
					noteBuf->Des().Format(KFormatString, a+1, f+1, v+1, ListCaras[a*9+f*3]+1, ListVertices[ListCaras[a*9+f*3]*3+v]);
					DialogAlert(noteBuf);
				}*/
				/*HBufC* noteBuf = HBufC::NewLC(100);
				_LIT(KFormatString, "face %d v%d: %d, Vt: %d %d %d");
				noteBuf->Des().Format(KFormatString, a+1, f+1, ListCaras[a*9+f*3]+1, ListVertices[ListCaras[a*9+f*3]+0],ListVertices[ListCaras[a*9+f*3]+1],ListVertices[ListCaras[a*9+f*3]+2]);
				DialogAlert(noteBuf);*/

				for(TInt uv=0; uv < 2; uv++){
					obj.uv[ListCaras[indice]*2+uv] = ListUVs[ListCaras[indice+1]*2+uv];
				}
				/*HBufC* noteBuf = HBufC::NewLC(100);
				_LIT(KFormatString, "face %d v%d: %d, UV: %d %d");
				noteBuf->Des().Format(KFormatString, a+1, f+1, ListCaras[a*9+f*3]+1, obj.uv[ListCaras[a*9+f*3]*2], obj.uv[ListCaras[a*9+f*3]*2+1]);
				DialogAlert(noteBuf);*/

				for(TInt vn=0; vn < 3; vn++){
					obj.normals[ListCaras[indice]*3+vn] = ListNormals[ListCaras[indice+1]*3+vn];
				}
				/*HBufC* noteBuf = HBufC::NewLC(100);
				_LIT(KFormatString, "face %d v%d: %d, VN: %d %d %d");
				noteBuf->Des().Format(KFormatString, a+1, f+1, ListCaras[a*9+f*3]+1, obj.uv[ListCaras[a*9+f*3]*3], obj.uv[ListCaras[a*9+f*3]*3+1], obj.uv[ListCaras[a*9+f*3]*3+2]);
				DialogAlert(noteBuf);*/
			}			
		}	

		//libero memoria		
		delete[] ListVertices;	
		delete[] ListCaras;
		delete[] ListNormals;
		delete[] ListUVs;
		
		Objetos.Append(obj);
		objSelect = Objetos.Count()-1;
		Objetos[objSelect].AgruparVertices();
		Objetos[objSelect].RecalcularBordes();

		redibujar = true;
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

void CBlenderLite::SetOrigen( TInt opcion ){
	if (estado != edicion){
		return;
	}
	//a la geometria
	else if (opcion == 0){
		if (tipoSelect == vertexSelect){
			//guarda el centro de la geometria
			GLshort NuevoOrigen[3] = {0,0,0};
			for(TInt i = 0; i < Objetos[objSelect].vertexGroupSize; i++) {
			    NuevoOrigen[0] += Objetos[objSelect].vertex[Objetos[objSelect].vertexGroup[i]*3];
			    NuevoOrigen[1] += Objetos[objSelect].vertex[Objetos[objSelect].vertexGroup[i]*3+1];
			    NuevoOrigen[2] += Objetos[objSelect].vertex[Objetos[objSelect].vertexGroup[i]*3+2];
			}
			NuevoOrigen[0] /= Objetos[objSelect].vertexGroupSize;
			NuevoOrigen[1] /= Objetos[objSelect].vertexGroupSize;
			NuevoOrigen[2] /= Objetos[objSelect].vertexGroupSize;

			for(TInt i = 0; i < Objetos[objSelect].vertexSize/3; i++) {
				Objetos[objSelect].vertex[i*3] -= NuevoOrigen[0];
				Objetos[objSelect].vertex[i*3+1] -= NuevoOrigen[1];
				Objetos[objSelect].vertex[i*3+2] -= NuevoOrigen[2];
			}			
			Objetos[objSelect].posX += NuevoOrigen[0]*Objetos[objSelect].scaleX/65000;
			Objetos[objSelect].posY += NuevoOrigen[2]*Objetos[objSelect].scaleY/65000;
			Objetos[objSelect].posZ += NuevoOrigen[1]*Objetos[objSelect].scaleZ/65000;
		}	
	}	
	//al vertice seleccionado
	else if (opcion == 1){
		if (tipoSelect == vertexSelect){
			//guarda el valor del vertice seleccionado
			GLshort NuevoOrigen[3] = {
				Objetos[objSelect].vertex[Objetos[objSelect].vertexGroup[EditSelect]*3],
				Objetos[objSelect].vertex[Objetos[objSelect].vertexGroup[EditSelect]*3+1],
				Objetos[objSelect].vertex[Objetos[objSelect].vertexGroup[EditSelect]*3+2]
			};
			for(TInt i = 0; i < Objetos[objSelect].vertexSize/3; i++) {
				Objetos[objSelect].vertex[i*3] -= NuevoOrigen[0];
				Objetos[objSelect].vertex[i*3+1] -= NuevoOrigen[1];
				Objetos[objSelect].vertex[i*3+2] -= NuevoOrigen[2];
			}
			Objetos[objSelect].posX += NuevoOrigen[0]*Objetos[objSelect].scaleX/65000;
			Objetos[objSelect].posY += NuevoOrigen[2]*Objetos[objSelect].scaleY/65000;
			Objetos[objSelect].posZ += NuevoOrigen[1]*Objetos[objSelect].scaleZ/65000;
		}//, edgeSelect, faceSelect  EditSelect		
	}	
    redibujar = true;
}


//  End of File
