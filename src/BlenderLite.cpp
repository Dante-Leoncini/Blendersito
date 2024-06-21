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

#include "Animation.h"
#include "Mesh.h"
#include "mono.h" //modelo 3d
#include "Floor.h" //modelo 3d
#include "Primitivas.h" //sprite 2D
#include "GeometriaUI.h" //elementos de la UI

#include <utf.h>       // Para CnvUtfConverter

#include "blenderlite.rsg"
#include "blenderlite.hrh"
//#include <aknmessagequerydialog.h>		// DialogAlertL
#include <aknnotewrappers.h> 
#include <akncommondialogs.h>

//nuevo apra el nuevo dialogo con opciones
//#include <eikprogi.h>
//#include <aknlists.h>
//#include <aknpopup.h>
//#include <aknapp.h>
//#include <aknutils.h>
//#include <eikclbd.h>
//#include <aknselectionlist.h> // Para CAknListQueryDialog

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
static const GLfloat ListaColores[9][4] = {
		{ MATERIALCOLOR(1.0, 1.0, 1.0, 1.0)     },   //blanco
		{ MATERIALCOLOR(0.94, 0.59, 0.17, 1.0)  },   //naranja 	
		{ MATERIALCOLOR(0.0, 0.0, 0.0, 1.0)     },   //negro
		{ MATERIALCOLOR(0.12, 0.12, 0.12, 1.0)  },   //gris
		{ MATERIALCOLOR(0.94, 0.59, 0.17, 0.25f)},   //naranja transparente
		{ MATERIALCOLOR(0.22, 0.22, 0.22, 1.0)  },    //cabezera de la barra de herramientas
		{ MATERIALCOLOR(0.0, 0.0, 0.0, 0.25f)     },   //negroTransparente
		{ MATERIALCOLOR(0.278, 0.447, 0.702, 1.0)     },   //azul de los iconos seleccionados
		{ MATERIALCOLOR(0.757, 0.757, 0.757, 1.0)     },   //azul de los iconos seleccionados
};

enum{
	blanco,
	naranja,
	negro,
	gris,
	naranjaFace,
	headerColor,
	negroTransparente,
	azulUI,
	grisUI
};
TInt colorBordeSelect = 1;

//Piso
static const GLfloat LineaPiso[4]  =      { MATERIALCOLOR(0.29, 0.29, 0.29, 1.0) };
static const GLfloat LineaPisoRoja[4]  =  { MATERIALCOLOR(0.56, 0.23, 0.28, 1.0) };
static const GLfloat LineaPisoVerde[4]  = { MATERIALCOLOR(0.38, 0.53, 0.15, 1.0) };

//ejes transformacion
static const GLfloat ColorTransformX[4]  = { MATERIALCOLOR(0.88, 0.48, 0.54, 1.0) };
static const GLfloat ColorTransformY[4]  = { MATERIALCOLOR(0.65, 0.81, 0.38, 1.0) };
static const GLfloat ColorTransformZ[4]  = { MATERIALCOLOR(0.46, 0.67, 0.89, 1.0) };

/* Global ambient light. */
static const GLfloat globalAmbient[4]   = { LIGHTCOLOR(0.5, 0.5, 0.5, 1.0) };

GLfloat AmbientRender[4] = { LIGHTCOLOR(0.0, 0.0, 0.0, 1.0) };

/* Lamp parameters. */
static const GLfloat lightDiffuseLamp[4]   = { LIGHTCOLOR(0.8, 0.8, 0.8, 1.0) };
static const GLfloat lightAmbient[4]   = { LIGHTCOLOR(0.0, 0.0, 0.0, 1.0) };
static const GLfloat sunLightPosition[4]  = {-100, 1000, 1000, 0 }; // y, z, x, si es direccional o puntual
	
/* Spotlight parameters. */
//static const GLfloat lightDiffuseSpot[4]   = { LIGHTCOLOR(1.0, 1.0, 1.0, 1.0) };
//static const GLfloat lightSpecularSpot[4]  = { LIGHTCOLOR(1.0, 1.0, 1.0, 1.0) };
//static const GLfloat lightPositionSpot[4]  = {  0, -10, -1, 0 };
//static const GLfloat lightDirectionSpot[4] = {  0,  -10,  -1, 1 };

//para las luces puntuales
static const GLfloat positionPuntualLight[4] = {0, 0, 0, 1};

//camara
TBool ortografica = false;
GLfloat aspectRatio;
GLfloat rotX = 113.5;
GLfloat rotY = 20.0; //66.2
GLfloat posX = 0;
GLfloat posY = 0;
GLfloat posZ = 0;
GLfloat PivotX = 0;
GLfloat PivotY = 0;
GLfloat PivotZ = 0;

//vista 3d
GLshort mouseX = 0;
GLshort mouseY = 0;
TBool mouseVisible = false;
TBool showOverlays = true;
TBool ShowUi = true;
TBool show3DCursor = true;
TBool showFloor = true;
TBool showYaxis = true;
TBool showXaxis = true;
TBool showOutlineSelect = true;
TBool iWidescreenEnabled = false;
TBool showOrigins = true;
TBool SimularZBuffer = false;
GLfloat Cursor3DposX = 0.0f;
GLfloat Cursor3DposZ = 0.0f;
GLfloat Cursor3DposY = 0.0f;
TInt nextLightId = GL_LIGHT1;

//animacion
TInt StartFrame = 1;
TInt EndFrame = 10;
TInt CurrentFrame = 1;
TBool PlayAnimation = true;
TBool ShowTimeline = true;
TBool redibujar = true; //solo redibuja si este valor esta en true

//interpolacion
enum {lineal, closest};
enum {Solid, MaterialPreview, Wireframe, Rendered};

enum{
	navegacion,
	rotacion,
	escala,
	translacion, 
	translacionVertex,
	translacionEdge,
	translacionFace,
	edicion,
	timelineMove
};

enum{
	Orbit,
	Fly,
	Apuntar
};

enum{
	vertexSelect, edgeSelect, faceSelect
};

enum{
	cubo, esfera, cilindro, plano, vacio, camara,
    cad, luz, monkey,vertice
};

//CPrimitiva Primitivas;

typedef enum { X, Y, Z, XYZ } Axis;

TInt view = MaterialPreview;
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
RArray<TInt> Collection;
RArray<Object> Objects;
RArray<Material> Materials;
RArray<Light> Lights;
RArray<Mesh> Meshes;
RArray<Animation> Animations;
TInt objSelect = 0;
TInt tipoSelect = vertexSelect;
TInt EditSelect = 0;
GLshort estadoVertex[3]={0, 0, 0};

void CBlenderLite::changeSelect(){
	if (estado == navegacion){
		EditSelect = 0;
		if (objSelect+1 > Objects.Count()-1){
			objSelect = 0;
		}
		else {
			objSelect++;
		}
	}
	else if (estado == edicion){
		EditSelect++;
		Mesh& pMesh = Meshes[Objects[objSelect].Id];	
		//if (tipoSelect == vertexSelect && EditSelect >= pMesh.vertexGroupSize){
		if (tipoSelect == vertexSelect && EditSelect >= pMesh.vertexGroups.Count()){
			EditSelect = 0;			
		}	
		/*if (tipoSelect == edgeSelect && EditSelect >= pMesh.facesSize/2){
			EditSelect = 0;			
		}	
		else if (tipoSelect == faceSelect && EditSelect >= pMesh.facesSize/3){
			EditSelect = 0;			
		}	*/
	};
    redibujar = true;	
}

void CBlenderLite::SetTipoSelect(TInt tipo){
	if (estado != edicion){return;}
	tipoSelect = tipo;
	EditSelect = 0;	
    redibujar = true;
}

void CBlenderLite::SetNavigation(){
	if (navegacionMode == Orbit){
		navegacionMode = Fly;
	}
	else if (navegacionMode == Fly){
		navegacionMode = Apuntar;
		navegacionMode = Orbit;
	}
	else {
		navegacionMode = Orbit;
	}
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

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

GLfloat CBlenderLite::aSin(GLfloat value) {
    TReal result, input = static_cast<TReal>(value);
    if (Math::ASin(result, input) == KErrNone) {
        return static_cast<GLfloat>(result);
    }
    return 0;
}

GLfloat CBlenderLite::atan2(GLfloat y, GLfloat x) {
    TReal result;
    if (x > 0) {
        if (Math::ATan(result, y / x) == KErrNone) {
            return static_cast<GLfloat>(result);
        }
    } else if (x < 0) {
        if (Math::ATan(result, y / x) == KErrNone) {
            if (y >= 0) {
                return static_cast<GLfloat>(result) + M_PI;
            } else {
                return static_cast<GLfloat>(result) - M_PI;
            }
        }
    } else {
        if (y > 0) {
            return M_PI / 2.0;
        } else if (y < 0) {
            return -M_PI / 2.0;
        }
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
: iContainer( NULL ),CFiniteStateMachine()
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
	navegacionMode = Orbit;
	showOverlays = true;
	show3DCursor = true;
	showFloor = true;
	showYaxis = true;
	showXaxis = true;
	showOutlineSelect = true;
	showOrigins = true;
	PlayAnimation = false;
	ShowTimeline = true;

	//tiene que haber un material por defecto siempre
	NewMaterial();

	//debuger
	//console = Console::NewL(_L("Consola"),TSize(KConsFullScreen, KConsFullScreen));
	AddObject(camera);
	Objects[0].posX = -800*6.8;
	Objects[0].posY = -800*7.29;
	Objects[0].posZ = 800*4.91;
	Objects[0].rotZ = -45.0;
	Objects[0].rotY = -26.15;
	Objects[0].scaleX = Objects[0].scaleY = Objects[0].scaleZ = 40000;

	AddObject(light);
	Objects[1].posX = -3000;
	Objects[1].posY = 1500;
	Objects[1].posZ = 4500;
	//GLfloat positionLight[4] = {0, -10, 0, 0};
    //GLfloat lightPositionSpot[4]  = {  0, -10, -1, 0 };
	//glLightfv(  nextLightId-1, GL_POSITION, positionPuntualLight );
	//glLightfv(  nextLightId, GL_POSITION, lightPositionSpot );
	
	//AddMesh(monkey);
	AddMesh(cubo);	
	//ImportOBJ();

	/*Animation animNew;	
	Animations.Append(animNew);
	Animation& anim = Animations[0];	
	anim.Id = 2;

	AnimProperty propNew;
	anim.Propertys.Append(propNew);
	AnimProperty& prop = anim.Propertys[0];
	prop.Property = AnimPositionX;

	keyFrame key;
	key.frame = 1;
	key.value = 0;
	key.Interpolation = Linear;
	prop.keyframes.Append(key);

	key.frame = 251;
	key.value = 10000;
	prop.keyframes.Append(key);*/
}

void CBlenderLite::NewMaterial(){
	/*HBufC* inicialBuf = HBufC::NewLC(100);
	_LIT(Kinicial, "Material.%03d");
	inicialBuf->Des().Format(Kinicial, Materials.Count()+1);
	
	HBufC* tituloBuf = HBufC::NewLC(100);
	_LIT(Ktitulo, "Ingrese el nombre del Material");
	tituloBuf->Des().Copy(Ktitulo);
	TPtr nombre = DialogText(inicialBuf, tituloBuf);
	
	HBufC* noteBuf = HBufC::NewLC(100);//textoingresado.AllocLC();
	noteBuf->Des().Copy(textoingresado);
	DialogAlert(noteBuf);
	CleanupStack::PopAndDestroy(noteBuf);	
	CleanupStack::PopAndDestroy(tituloBuf);	
	CleanupStack::PopAndDestroy(inicialBuf);	*/
	
	Material mat;	
	mat.specular[0] = mat.specular[1] = mat.specular[2] = mat.specular[3] = 0.3;
	mat.diffuse[0] = mat.diffuse[1] = mat.diffuse[2] = mat.diffuse[3] = 1.0;
	mat.emission[0] = mat.emission[1] = mat.emission[2] = mat.emission[3] = 0.0;
	mat.textura = false;
	mat.color = false;
	mat.repeat = true;
	mat.lighting = true;
	mat.transparent = false;
	mat.interpolacion = lineal;
	mat.textureID = 0;
	if (Materials.Count() < 1){
		mat.name = HBufC::NewL(15);
		_LIT(KMatNameDefault, "DefaultMaterial");
		mat.name->Des().Format(KMatNameDefault, Materials.Count()+1);
	}
	else {
		mat.name = HBufC::NewL(12);
		_LIT(KMatName, "Material.%03d");
		mat.name->Des().Format(KMatName, Materials.Count()+1);
	}
	Materials.Append(mat);
}


void CBlenderLite::RemoveMaterial(){
	//si no hay objetos
	/*if (Objects.Count() < 1){return;}	
	Object& obj = Objects[objSelect];
	//si no es un mesh
	if (obj.type != mesh){return;}	
	Mesh& pMesh = Meshes[obj.Id];
	Material& mat = Materials[pMesh.materials[0]];

	Materials.Remove(objSelect);*/
};

void CBlenderLite::RemoveTexture(){

};

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
    delete iContainer;
}

// -----------------------------------------------------------------------------
// CBlenderLite::AppInit
// Initialize OpenGL ES, set the vertex and color arrays and pointers,
// and select the shading mode.
// -----------------------------------------------------------------------------
//
RArray<TTexture> Textures;
TInt NumTexturasBlendersito = 0;
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
    glLightfv(  GL_LIGHT0, GL_AMBIENT,  lightAmbient  );
    glLightfv(  GL_LIGHT0, GL_SPECULAR, lightDiffuseLamp  );
    glLightfv(  GL_LIGHT0, GL_POSITION, sunLightPosition );

    // Set up spot.  Initially spot is disabled.
    //glEnable( GL_LIGHT1 );
    /*glLightfv(  GL_LIGHT1, GL_DIFFUSE,  lightDiffuseSpot  );
    glLightfv(  GL_LIGHT1, GL_AMBIENT,  lightAmbient  );
    glLightfv(  GL_LIGHT1, GL_SPECULAR, lightSpecularSpot );
    glLightfv(  GL_LIGHT1, GL_POSITION, lightPositionSpot );

    glLightf(   GL_LIGHT1, GL_CONSTANT_ATTENUATION,  1.5  );
    glLightf(   GL_LIGHT1, GL_LINEAR_ATTENUATION,    0.5  );
    glLightf(   GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.5  );

    glLightf(   GL_LIGHT1, GL_SPOT_CUTOFF,   170.0                );
    glLightf(   GL_LIGHT1, GL_SPOT_EXPONENT,  20.0                );
    glLightfv(  GL_LIGHT1, GL_SPOT_DIRECTION, lightDirectionSpot );*/

    // Set shading mode
    glShadeModel( GL_SMOOTH );
    //glShadeModel( GL_FLAT );

    // Do not use perspective correction
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );
    
    RFs fs;
    User::LeaveIfError(fs.Connect());
    TFileName privateDir;
	
    TParsePtrC Parse( CEikonEnv::Static()->EikAppUi()->Application()->AppFullName() );
    TFileName RootDirectory = Parse.DriveAndPath().Left( 2 );

    User::LeaveIfError(fs.PrivatePath(privateDir));
    fs.Close();

	// Push the textures into the loading queue.
	_LIT( KOriginTexture, "origen.png" );
	_LIT( KMouseTexture, "cursor.png" );	
	_LIT( KLampTexture, "lamp.png" );		
	_LIT( KCursor3dTextura, "cursor3d.png" );	
	_LIT( KkeyframeTextura, "keyframe.png" );	
	
	NumTexturasBlendersito = 5;
	Textures.ReserveL(NumTexturasBlendersito); // Reservar espacio para las texturas
	for (TInt i = 0; i < NumTexturasBlendersito; ++i) {
	    TTexture texture;
	    Textures.Append(texture);
	}
	
    // Concatenar la ruta con el nombre del archivo
	TFileName fullFilePath = RootDirectory;
    fullFilePath.Append(privateDir);
	iTextureManager->RequestToLoad( KOriginTexture, fullFilePath, &Textures[0], false );
	iTextureManager->RequestToLoad( KMouseTexture, fullFilePath, &Textures[1], false );
	iTextureManager->RequestToLoad( KLampTexture, fullFilePath, &Textures[2], false );
	iTextureManager->RequestToLoad( KCursor3dTextura, fullFilePath, &Textures[3], false );
	iTextureManager->RequestToLoad( KkeyframeTextura, fullFilePath, &Textures[4], false );
	
	//Start to load the textures.
	iTextureManager->DoLoadL();
}


// -----------------------------------------------------------------------------
// CBlenderLite::AppExit
// Release any allocations made in AppInit.
// -----------------------------------------------------------------------------
//
void CBlenderLite::AppExit( void ){
	delete iTextureManager;
}


// -----------------------------------------------------------------------------
// CBlenderLite::AppCycle
// Draws and animates the objects
// -----------------------------------------------------------------------------
//

// Función para calcular las coordenadas de textura esféricas
void CBlenderLite::calculateReflectionUVs(Mesh& pMesh) {
    for (int i = 0; i < pMesh.vertexSize; ++i) {
        // Obtener la normal del vértice
        GLfloat nx = static_cast<GLfloat>(pMesh.normals[i * 3 + 0]) / 127.0f;
        GLfloat ny = static_cast<GLfloat>(pMesh.normals[i * 3 + 1]) / 127.0f;
        GLfloat nz = static_cast<GLfloat>(pMesh.normals[i * 3 + 2]) / 127.0f;

        // Calcular coordenadas UV esféricas
        GLfloat u = 0.5f + (atan2(nx, nz) / (2.0f * M_PI));
        GLfloat v = 0.5f - (asin(ny) / M_PI);

        pMesh.uv[i * 2 + 0] = u;
        pMesh.uv[i * 2 + 1] = v;
    }
}

void CBlenderLite::RenderMesh( TInt objId ){
	Mesh& pMesh = Meshes[objId];
		
	glColor4f(ListaColores[blanco][0],ListaColores[blanco][1],ListaColores[blanco][2],ListaColores[blanco][3]);
	glDisable(GL_COLOR_MATERIAL);
	glMaterialfv(   GL_FRONT_AND_BACK, GL_AMBIENT,  objAmbient  );
	glMaterialx( GL_FRONT_AND_BACK, GL_SHININESS,   12 << 16     );
	glMaterialfv(   GL_FRONT_AND_BACK, GL_EMISSION, ListaColores[negro] );

	glDisable(GL_POLYGON_OFFSET_FILL);
	// Set array pointers from mesh.
	glVertexPointer( 3, GL_SHORT, 0, pMesh.vertex );
	glColorPointer( 4, GL_UNSIGNED_BYTE, 0, pMesh.vertexColor );
	glNormalPointer( GL_BYTE, 0, pMesh.normals );
	//resetea las lienas a 1
	glLineWidth(1);	

	//glShadeModel( GL_SMOOTH );
	if (pMesh.smooth){glShadeModel( GL_SMOOTH );}
	else {glShadeModel( GL_FLAT );}

	//si usa culling
	if (pMesh.culling){glEnable( GL_CULL_FACE );}
	else {glDisable( GL_CULL_FACE );}	
	
	//modelo con textura
	if (SimularZBuffer){
		glColor4f(ListaColores[blanco][0],ListaColores[blanco][1],ListaColores[blanco][2],ListaColores[blanco][3]);
		glDisable( GL_LIGHTING );
		glDisable(GL_BLEND);
		glDisable( GL_TEXTURE_2D );
		for(int f=0; f < pMesh.materialsSize; f++){
			glDrawElements( GL_TRIANGLES, pMesh.facesGroupsSize[f]*3, GL_UNSIGNED_SHORT, pMesh.faces[f] );
		}
	}
	//material
	else if (view == MaterialPreview || view == Rendered){
		// Calcular coordenadas UV reflejadas
		//calculateReflectionUVs(pMesh);

		glTexCoordPointer( 2, GL_FLOAT, 0, pMesh.uv );
		
		for(int f=0; f < pMesh.materialsSize; f++){
			Material& mat = Materials[pMesh.materials[f]];	
			glMaterialfv(   GL_FRONT_AND_BACK, GL_DIFFUSE, mat.diffuse ); 
			glMaterialfv(   GL_FRONT_AND_BACK, GL_SPECULAR, mat.specular );

			//vertex color
			if (mat.color){
				glColor4f(ListaColores[negro][0],ListaColores[negro][1],ListaColores[negro][2],ListaColores[negro][3]);
				glEnableClientState( GL_COLOR_ARRAY );
			}
			else {glDisableClientState( GL_COLOR_ARRAY );}	 

			//si tiene iluminacion	
			if (mat.lighting){glEnable( GL_LIGHTING );}
			else {glDisable( GL_LIGHTING );}
			
			//transparent
			if (mat.transparent){glEnable(GL_BLEND);}
			else {glDisable(GL_BLEND);}
			
			//si tiene texturas
			if (mat.textura){
				glEnable( GL_TEXTURE_2D );
				glBindTexture(  GL_TEXTURE_2D, mat.textureID ); //selecciona la textura							
			
				//textura pixelada o suave
				if (mat.interpolacion == lineal){
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				}
				else if (mat.interpolacion == closest){
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				}	
				//si la textura se repite
				if (mat.repeat){
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				}
				else {
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				}	
			}
			else {glDisable( GL_TEXTURE_2D );}

			glMaterialfv(   GL_FRONT_AND_BACK, GL_EMISSION, mat.emission );

			glDrawElements( GL_TRIANGLES, pMesh.facesGroupsSize[f]*3, GL_UNSIGNED_SHORT, pMesh.faces[f] );	
		}
	}	
	//modelo sin textura
	else if (view == Solid){
		glEnable( GL_LIGHTING );
		Material& mat = Materials[pMesh.materials[0]];
		glDisableClientState( GL_COLOR_ARRAY );	  	
		glDisable( GL_TEXTURE_2D );
		glDisable(GL_BLEND);
		glMaterialfv(   GL_FRONT_AND_BACK, GL_DIFFUSE,  mat.diffuse   ); 
		//glDrawElements( GL_TRIANGLES, pMesh.facesSize*3, GL_UNSIGNED_SHORT, pMesh.faces );	
		for(int f=0; f < pMesh.materialsSize; f++){			
			glDrawElements( GL_TRIANGLES, pMesh.facesGroupsSize[f]*3, GL_UNSIGNED_SHORT, pMesh.faces[f] );	
		}
	}
	//wireframe view
	else if(objSelect != objId){    
		glDisableClientState( GL_COLOR_ARRAY );	  
		glDisable( GL_LIGHTING );
		glEnable(GL_COLOR_MATERIAL);
		glColor4f(ListaColores[gris][0],ListaColores[gris][1],ListaColores[gris][2],ListaColores[gris][3]);
		//glDrawElements( GL_LINES, pMesh.edgesSize, GL_UNSIGNED_SHORT, pMesh.edges );	
	}  
	
	//dibuja el borde seleccionado
	if(objSelect == objId && showOverlays && showOutlineSelect){
		glDisableClientState( GL_COLOR_ARRAY );	  
		glDisable( GL_LIGHTING );
		glEnable(GL_COLOR_MATERIAL);
		glDisable( GL_TEXTURE_2D );  
		glEnable(GL_POLYGON_OFFSET_FILL);
		
		//si se esta editando
		if (estado == edicion){ // || estado == translacionVertex
			//bordes
			glPolygonOffset(1.0, -1.0);
			glLineWidth(1);	 
			glColor4f(ListaColores[gris][0],
					ListaColores[gris][1],
					ListaColores[gris][2],
					ListaColores[gris][3]);
			//glDrawElements( GL_LINES, obj.edgesSize, GL_UNSIGNED_SHORT, obj.edges );
			//vertices
			if (tipoSelect == vertexSelect){
				glVertexPointer( 3, GL_SHORT, 0, pMesh.vertexGroupUI );
				glPolygonOffset(1.0, -4.0);
				glColor4f(ListaColores[negro][0], ListaColores[negro][1], ListaColores[negro][2], ListaColores[negro][3]);
				glPointSize(4);
				//glDrawElements( GL_POINTS, pMesh.vertexGroupSize, GL_UNSIGNED_SHORT, pMesh.vertexGroup);
				//glDrawElements( GL_POINTS, pMesh.vertexGroups.Count(), GL_UNSIGNED_SHORT, pMesh.vertexGroupUI);
				glDrawArrays( GL_POINTS, 0, pMesh.vertexGroups.Count() );
				//vertice seleccionado
				glPolygonOffset(1.0, -10.0);
				glColor4f(ListaColores[blanco][0],ListaColores[blanco][1],ListaColores[blanco][2],ListaColores[blanco][3]);
				//glDrawArrays( GL_POINTS, pMesh.vertexGroup[EditSelect], 1 );
				glDrawArrays( GL_POINTS, EditSelect, 1 );			
			}
			//borde seleccionado
			else if (tipoSelect == edgeSelect){		
				//vertice seleccionado
				/*if (obj.edgesSize > 0){		
					glPolygonOffset(1.0, -10.0);
					glColor4f(ListaColores[blanco][0],ListaColores[blanco][1],ListaColores[blanco][2],ListaColores[blanco][3]);
					glDrawElements( GL_LINES, 2, GL_UNSIGNED_SHORT, &obj.edges[EditSelect*2]);
				}	*/		
			}
			//cara seleccionado
			/*else if (tipoSelect == faceSelect){	
				//vertice seleccionado
				if (pMesh.facesSize > 0){	
					glEnable(GL_BLEND);
					glDisable(GL_LIGHTING);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glPolygonOffset(1.0, -10.0);
					glColor4f(ListaColores[naranjaFace][0],ListaColores[naranjaFace][1],ListaColores[naranjaFace][2],ListaColores[naranjaFace][3]);
					glDrawElements( GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, &pMesh.faces[EditSelect*3]);
					glDisable(GL_BLEND);
				}				
			}*/
		}
		else if (view != 2){
			glPolygonOffset(1.0, 200.0);
			glLineWidth(3);	 
			glColor4f(ListaColores[colorBordeSelect][0],ListaColores[colorBordeSelect][1],ListaColores[colorBordeSelect][2],ListaColores[colorBordeSelect][3]);
			//glDrawElements( GL_LINES, obj.edgesSize, GL_UNSIGNED_SHORT, obj.edges );	
		}	
		else { //bordes seleccionados en wireframe
			glDisable(GL_POLYGON_OFFSET_FILL);
			glLineWidth(1);		
			glColor4f(ListaColores[colorBordeSelect][0],ListaColores[colorBordeSelect][1],ListaColores[colorBordeSelect][2],ListaColores[colorBordeSelect][3]);
			//glDrawElements( GL_LINES, obj.edgesSize, GL_UNSIGNED_SHORT, obj.edges );	
		}
	};
}

// Función recursiva para renderizar un objeto y sus hijos
void CBlenderLite::RenderMeshAndChildren(Object& obj){
    // Guardar la matriz actual
    glPushMatrix();
    
    // Aplicar las transformaciones del objeto
    glTranslatef(obj.posX, obj.posZ, obj.posY);
    glRotatef(obj.rotX, 1, 0, 0); // ángulo, X Y Z
    glRotatef(obj.rotZ, 0, 1, 0); // ángulo, X Y Z
    glRotatef(obj.rotY, 0, 0, 1); // ángulo, X Y Z
    glScalex(obj.scaleX, obj.scaleZ, obj.scaleY);	

    // Si es visible y es un mesh, lo dibuja
    if (obj.visible && obj.type == mesh) {
        RenderMesh(obj.Id); // Ajusta el segundo parámetro si es necesario
    }
    
    // Procesar cada hijo
    for (int c = 0; c < obj.Childrens.Count(); c++) {
        Object& objChild = Objects[obj.Childrens[c]];
        RenderMeshAndChildren(objChild);
    }

    // Restaurar la matriz previa
    glPopMatrix();
}

// Función recursiva para renderizar un objeto y sus hijos
void CBlenderLite::RenderObjectAndChildrens(TInt objId){
	Object& obj = Objects[objId];
    // Guardar la matriz actual
    glPushMatrix();
    
    // Aplicar las transformaciones del objeto
    glTranslatef(obj.posX, obj.posZ, obj.posY);
    glRotatef(obj.rotX, 1, 0, 0); // ángulo, X Y Z
    glRotatef(obj.rotZ, 0, 1, 0); // ángulo, X Y Z
    glRotatef(obj.rotY, 0, 0, 1); // ángulo, X Y Z
    glScalex(obj.scaleX, obj.scaleZ, obj.scaleY);	

    // Si es visible y no es un mesh, lo dibuja
    if (obj.visible && obj.type != mesh) {
        RenderObject(objId); // Ajusta el segundo parámetro si es necesario
    }
    
    // Procesar cada hijo
    for (int c = 0; c < obj.Childrens.Count(); c++) {
        RenderObjectAndChildrens(obj.Childrens[c]);
    }

    // Restaurar la matriz previa
    glPopMatrix();
}

void CBlenderLite::RenderObject( TInt objId ){
	Object& obj = Objects[objId];

	glDisable( GL_TEXTURE_2D );
	//glPushMatrix(); //guarda la matrix

	//posicion, rotacion y escala del objeto
	//glTranslatef( obj.posX, obj.posZ, obj.posY);

	//color si esta seleccionado
	if (objSelect == objId){
		glColor4f(ListaColores[colorBordeSelect][0],ListaColores[colorBordeSelect][1],ListaColores[colorBordeSelect][2],ListaColores[colorBordeSelect][3]);
	}
	else {		
		glColor4f(ListaColores[negro][0],ListaColores[negro][1],ListaColores[negro][2],ListaColores[negro][3]);		
	}
	//si es un empty
	if (obj.type == empty){		
		glDisable( GL_TEXTURE_2D );	 
		glDisable( GL_BLEND );
		//glRotatef(obj.rotX, 1, 0, 0); //angulo, X Y Z
		//glRotatef(obj.rotZ, 0, 1, 0); //angulo, X Y Z
		//glRotatef(obj.rotY, 0, 0, 1); //angulo, X Y Z
		//glScalex( obj.scaleX, obj.scaleZ, obj.scaleY );		
		glLineWidth(1);	
		glVertexPointer( 3, GL_SHORT, 0, EmptyVertices );
		glDrawElements( GL_LINES, EmptyEdgesSize, GL_UNSIGNED_SHORT, EmptyEdges );
	}
	else if (obj.type == camera){			
		glDisable( GL_TEXTURE_2D ); 
		glDisable( GL_BLEND );
		//glRotatef(obj.rotX, 1, 0, 0); //angulo, X Y Z
		//glRotatef(obj.rotZ, 0, 1, 0); //angulo, X Y Z
		//glRotatef(obj.rotY, 0, 0, 1); //angulo, X Y Z
		//glScalex( obj.scaleX, obj.scaleZ, obj.scaleY );
		glLineWidth(1);	
		glVertexPointer( 3, GL_SHORT, 0, CameraVertices );
		glDrawElements( GL_LINES, CameraEdgesSize, GL_UNSIGNED_SHORT, CameraEdges );
	}
	else if (obj.type == light){				
		glEnable( GL_TEXTURE_2D ); 
		glEnable( GL_BLEND );
		glDepthMask(GL_FALSE); // Desactiva la escritura en el Z-buffer

		glEnable( GL_POINT_SPRITE_OES ); //activa el uso de sprites en los vertices
		glPointSize( 32 ); //tamaño del punto
		glVertexPointer( 3, GL_SHORT, 0, pointVertex );
		glBindTexture( GL_TEXTURE_2D, Textures[2].iID ); //selecciona la textura

		glTexEnvi( GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_TRUE );
		glDrawArrays( GL_POINTS, 0, 1 );
		glTexEnvi( GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_FALSE);

		glDisable( GL_POINT_SPRITE_OES );			
		glDisable( GL_TEXTURE_2D ); 

		glLineWidth(1);	
		//glScalex( 0, 10, 0 ); //4500
		LineaLightVertex[4] = (GLshort)-obj.posZ;
		glVertexPointer( 3, GL_SHORT, 0, LineaLightVertex );
		glDrawElements( GL_LINES, LineaEdgeSize, GL_UNSIGNED_SHORT, LineaEdge );

		glDisable( GL_BLEND );
		glDepthMask(GL_TRUE); // Reactiva la escritura en el Z-buffer
	}

	//glPopMatrix(); //reinicia la matrix a donde se guardo	
}


void CBlenderLite::InsertKeyframe(TInt propertySelect){
	ShowOptionsDialogL();
    // Crear una lista de opciones
    //CDesCArrayFlat* options = new (ELeave) CDesCArrayFlat(3);
    /*CleanupStack::PushL(options);
    options->AppendL(_L("Opción 1"));
    options->AppendL(_L("Opción 2"));
    options->AppendL(_L("Opción 3"));

    // Mostrar el diálogo de selección de opciones
    TInt selectedIndex = DialogSelectOption(_L("Selecciona una opción:"), *options);
    CleanupStack::PopAndDestroy(options);

	//si no se selecciono nada
    if (selectedIndex == -1) {return;}*/

	TBool encontrado = false;
	TInt index = 0;
    for(TInt a = 0; a < Animations.Count(); a++) {
		if (Animations[a].Id == objSelect){
			encontrado = true;
			index = a;
			break;
		}
	}	

	keyFrame key;
	key.frame = CurrentFrame;
	key.Interpolation = Linear;

	if (encontrado){
		for(TInt p = 0; p < Animations[index].Propertys.Count(); p++) {
			AnimProperty& animProp = Animations[index].Propertys[p];
			if(animProp.Property != propertySelect){continue;}
			switch (animProp.Property) {
				case AnimPosition:
					key.valueX = Objects[objSelect].posX;
					key.valueY =  Objects[objSelect].posY;
					key.valueZ =  Objects[objSelect].posZ;
					break;
				case AnimRotation:
					key.valueX = Objects[objSelect].rotX;
					key.valueY =  Objects[objSelect].rotY;
					key.valueZ =  Objects[objSelect].rotZ;
					break;
				case AnimScale:
					key.valueX = Objects[objSelect].scaleX;
					key.valueY =  Objects[objSelect].scaleY;
					key.valueZ =  Objects[objSelect].scaleZ;
					break;
				default:
					break;
			}

			TBool yaExiste = false;
			for(TInt k = 0; k < animProp.keyframes.Count(); k++) {
				//evita que se duplique. solo reemplaza
				if (animProp.keyframes[k].frame == CurrentFrame){
					animProp.keyframes[k].valueX = key.valueX;
					animProp.keyframes[k].valueY = key.valueY;
					animProp.keyframes[k].valueZ = key.valueZ;
					yaExiste = true;
					break;
				}
			};
			if (!yaExiste){
				animProp.keyframes.Append(key);	
				animProp.SortKeyFrames();
			}
			break;
		}
	}
	else {		
		Animation NewAnim;	
		Animations.Append(NewAnim);
		Animation& anim = Animations[Animations.Count()-1];	
		anim.Id = objSelect;
		
		AnimProperty propNew;
		anim.Propertys.Append(propNew);
		AnimProperty& prop = anim.Propertys[anim.Propertys.Count()-1];
		prop.Property = propertySelect;

		switch (propertySelect) {
			case AnimPosition:
				key.valueX = Objects[objSelect].posX;
				key.valueY =  Objects[objSelect].posY;
				key.valueZ =  Objects[objSelect].posZ;
				break;
			case AnimRotation:
				key.valueX = Objects[objSelect].rotX;
				key.valueY =  Objects[objSelect].rotY;
				key.valueZ =  Objects[objSelect].rotZ;
				break;
			case AnimScale:
				key.valueX = Objects[objSelect].scaleX;
				key.valueY =  Objects[objSelect].scaleY;
				key.valueZ =  Objects[objSelect].scaleZ;
				break;
			default:
				break;
		}
		prop.keyframes.Append(key);
	}
    redibujar = true;	
}

void CBlenderLite::RemoveKeyframes(){
    for(TInt a = 0; a < Animations.Count(); a++) {
		if (Animations[a].Id == objSelect){
        	if (Animations[a].Propertys.Count() > 0) {
				for(TInt p = 0; p < Animations[a].Propertys.Count(); p++) {
					AnimProperty& anim = Animations[a].Propertys[p];
					if (anim.keyframes.Count() > 0) {
						TInt firstFrame = 0;
						TInt firstFrameIndex = 0;
						// Encontrar el primer frame
						for(TInt f = 0; f < anim.keyframes.Count(); f++) {
							if (anim.keyframes[f].frame < CurrentFrame && anim.keyframes[f].frame > firstFrame) {
								firstFrameIndex = f;
							}
						}
						anim.keyframes.Remove(firstFrameIndex);
					}
				}
			}
			break;
		}
	}	
    redibujar = true;
}

void CBlenderLite::ClearKeyframes(){
    for(TInt a = 0; a < Animations.Count(); a++) {
		if (Animations[a].Id == objSelect){
			/*for(TInt p = 0; p < Animations[a].Propertys.Count(); p++){
				Animations[a].Propertys[p].keyframes.Close();
			}*/
			Animations[a].Propertys.Close();
			Animations.Remove(a);
			break;
		}
	}	
    redibujar = true;
}

void CBlenderLite::ReloadAnimation(){
    for(TInt a = 0; a < Animations.Count(); a++) {
		for(TInt p = 0; p < Animations[a].Propertys.Count(); p++){
			AnimProperty& anim = Animations[a].Propertys[p];
			if (anim.keyframes.Count() > 0){
				GLfloat valueX = 0;
				GLfloat valueY = 0;
				GLfloat valueZ = 0;
				TInt firstFrameIndex = 0;
				TInt lastFrameIndex = 0;
				// Encontrar el primer frame
				for(TInt f = 0; f < anim.keyframes.Count(); f++) {
					if (anim.keyframes[f].frame <= CurrentFrame) {
						firstFrameIndex = f;
					}
					if (anim.keyframes[f].frame >= CurrentFrame) {
						lastFrameIndex = f;
						break;
					}
				}

				// Si el CurrentFrame está fuera de los límites de los keyframes, usar el valor del primer o último frame
				if (CurrentFrame <= anim.keyframes[firstFrameIndex].frame) {
					valueX = anim.keyframes[firstFrameIndex].valueX;
					valueY = anim.keyframes[firstFrameIndex].valueY;
					valueZ = anim.keyframes[firstFrameIndex].valueZ;
				} 
				else if (CurrentFrame >= anim.keyframes[lastFrameIndex].frame) {
					valueX = anim.keyframes[anim.keyframes.Count() - 1].valueX;
					valueY = anim.keyframes[anim.keyframes.Count() - 1].valueY;
					valueZ = anim.keyframes[anim.keyframes.Count() - 1].valueZ;
				} 
				// Interpolación entre keyframes
				else {		
					// Interpolación entre keyframes
					TInt frame1 = anim.keyframes[firstFrameIndex].frame;
					GLfloat valueX1 = anim.keyframes[firstFrameIndex].valueX;
					GLfloat valueY1 = anim.keyframes[firstFrameIndex].valueY;
					GLfloat valueZ1 = anim.keyframes[firstFrameIndex].valueZ;
					TInt frame2 = anim.keyframes[lastFrameIndex].frame;
					GLfloat valueX2 = anim.keyframes[lastFrameIndex].valueX;
					GLfloat valueY2 = anim.keyframes[lastFrameIndex].valueY;
					GLfloat valueZ2 = anim.keyframes[lastFrameIndex].valueZ;

					switch (anim.keyframes[firstFrameIndex].Interpolation) {
						case Constant:
							valueX = valueX1;
							valueY = valueY1;
							valueZ = valueZ1;
							break;
						case Linear:
							valueX = valueX1 + ((valueX2 - valueX1) * (CurrentFrame - frame1)) / (frame2 - frame1);
							valueY = valueY1 + ((valueY2 - valueY1) * (CurrentFrame - frame1)) / (frame2 - frame1);
							valueZ = valueZ1 + ((valueZ2 - valueZ1) * (CurrentFrame - frame1)) / (frame2 - frame1);
							break;
						default:
							valueX = valueX1;
							valueY = valueY1;
							valueZ = valueZ1;
							break;
					}
				}

				// Asignar el valor calculado a la propiedad correspondiente del objeto
				switch (anim.Property) {
					case AnimPosition:
						Objects[Animations[a].Id].posX = valueX;
						Objects[Animations[a].Id].posY = valueY;
						Objects[Animations[a].Id].posZ = valueZ;
						break;
					// Otros casos según las propiedades
					default:
						// Manejar cualquier otro caso aquí si es necesario
						break;
				}
			}
		}
    }
}

void CBlenderLite::SetCurrentFrame(){
	Cancelar();
	estado = timelineMove;
    redibujar = true;
}

void CBlenderLite::SetEndFrame(){
	Cancelar();
	HBufC* noteBuf = HBufC::NewLC(100);
	noteBuf->Des().Copy(_L("Set Final Frame"));
	EndFrame = DialogNumber(EndFrame, 0, 2147483647, noteBuf);	
	CleanupStack::PopAndDestroy(noteBuf);
    redibujar = true;
}

void CBlenderLite::SetStartFrame(){
	Cancelar();
	HBufC* noteBuf = HBufC::NewLC(100);
	noteBuf->Des().Copy(_L("Set Start Frame"));
	StartFrame = DialogNumber(StartFrame, 0, 2147483647, noteBuf);
	CleanupStack::PopAndDestroy(noteBuf);
    redibujar = true;
}

TBool postProcesado = true;
void CBlenderLite::AppCycle( TInt iFrame, GLfloat aTimeSecs, GLfloat aDeltaTimeSecs ){
    // If texture loading is still going on, return from this method without doing anything.
	if ( GetState() == ELoadingTextures ){
        return;
    }
	GLfixed fixedDeltaTimeSecs = FLOAT_2_FIXED( aDeltaTimeSecs );
	// Controles
	Rotar( fixedDeltaTimeSecs );
	
	if ( !redibujar && !PlayAnimation ){	
		if (postProcesado){
			GLubyte* pixels = new GLubyte[iScreenWidth * iScreenHeight * 4]; // 4 para RGBA
			
			for (TInt i = 0; i < iScreenWidth * iScreenHeight * 4; ++i){
				pixels[i] = 0;
			}
			
			for (TInt i = iScreenWidth*30*4; i <iScreenWidth*40 * 4; ++i){
				pixels[i] = 255;
			}
			
	    	glBindTexture( GL_TEXTURE_2D, Textures[3].iID); //iCursor3dTextura.iID
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, iScreenWidth, iScreenHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
			//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, iScreenWidth, iScreenHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
			glFlush();	
			delete pixels;
			postProcesado = false;
		}
		return;
	}
	postProcesado = true;

	if (PlayAnimation){
		CurrentFrame++;
		if (CurrentFrame > EndFrame){
			CurrentFrame = StartFrame;
		}
		ReloadAnimation();
	}

	if (SimularZBuffer){
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Color negro
		glEnable(GL_FOG);
		glFogf(GL_FOG_MODE, GL_LINEAR); // Tipo de niebla lineal
		glFogf(GL_FOG_START, FRUSTUM_NEAR);  // Distancia inicial de la niebla
		glFogf(GL_FOG_END, FRUSTUM_FAR);     // Distancia final de la niebla
		GLfloat fogColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glFogfv(GL_FOG_COLOR, fogColor); // Color de la niebla
	}
	else {
		glDisable(GL_FOG);
		glClearColor( 0.23f, 0.23f, 0.23f, 1.f );
	}
	
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glLoadIdentity();
    glEnable( GL_DEPTH_TEST );	

	glTranslatef( posX, posZ, -cameraDistance+posY);
	glRotatef(rotY, 1, 0, 0); //angulo, X Y Z
	glRotatef(rotX, 0, 1, 0); //angulo, X Y Z
	// Use some magic numbers to scale the head model to fit the screen.
	glScalex( 1 << 10, 1 << 10, 1 << 10 );
	glTranslatef( PivotX, PivotZ, PivotY);

    // Habilitar la normalizaci�n de las normales
    glEnable(GL_NORMALIZE);	

	//primero hay que colocar las luces en caso de estar en modo render!
	if (!SimularZBuffer && view == MaterialPreview || view == Rendered){
		for(TInt o=0; o < Objects.Count(); o++){
			Object& obj = Objects[o];
			if(!obj.visible || obj.type != light ) {continue;}
			Light& light = Lights[obj.Id];

			glPushMatrix(); //guarda la matrix
			glTranslatef( obj.posX, obj.posZ, obj.posY);
			GLfloat lightPos[] = {0.0f, 0.0f, 0.0f, 1.0f}; // Luz puntual en la posici�n transformada
			glLightfv(light.lightId, GL_POSITION, lightPos);
			//glLightfv(  light.lightId, GL_POSITION, positionPuntualLight );
			glPopMatrix(); //reinicia la matrix a donde se guardo  
		}		
	}
	
	//bucle que dibuja cada objeto en orden
	if(Meshes.Count() > 0){
		// Función principal para iterar sobre la colección
		for (int o = 0; o < Collection.Count(); o++) {
			Object& obj = Objects[Collection[o]];
			RenderMeshAndChildren(obj);
		}
	}
	//fin del dibujado de objetos
	//si estaba simulando el zbuffer. el resto no hace falta
	if (SimularZBuffer){		
		redibujar = false;
		return;
	};

	//el resto de objetos no usan materiales ni luces
	glMaterialfv(   GL_FRONT_AND_BACK, GL_DIFFUSE,  ListaColores[negro] );
	glMaterialfv(   GL_FRONT_AND_BACK, GL_AMBIENT,  ListaColores[negro] );
	glMaterialfv(   GL_FRONT_AND_BACK, GL_SPECULAR, ListaColores[negro] );
	glDisable( GL_CULL_FACE ); // Enable back face culling.
	glDisable( GL_LIGHTING );
	glDisable(GL_POLYGON_OFFSET_FILL);
	glEnable(GL_COLOR_MATERIAL);
	glDisable( GL_TEXTURE_2D );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	//dibujar las lineas del piso y el piso
	if (showOverlays && (showFloor || showXaxis || showYaxis)){
		glEnable(GL_FOG);
		glFogf(GL_FOG_MODE, GL_LINEAR); // Tipo de niebla lineal
		glFogf(GL_FOG_START, FRUSTUM_NEAR);  // Distancia inicial de la niebla
		glFogf(GL_FOG_END, FRUSTUM_FAR);     // Distancia final de la niebla
		GLfloat fogColor[] = {0.23f, 0.23f, 0.23f, 1.f};
		glFogfv(GL_FOG_COLOR, fogColor); // Color de la niebla

		glVertexPointer( 3, GL_SHORT, 0, objVertexdataFloor );
		//glNormalPointer( GL_BYTE, 0, objNormaldataFloor );

		//dibuja el piso			
		glLineWidth(1);	
		if (showFloor){
			glColor4f(LineaPiso[0],LineaPiso[1],LineaPiso[2],LineaPiso[3]);
			glDrawElements( GL_LINES, objFacesFloor, GL_UNSIGNED_SHORT, objFacedataFloor );			
		}		
		//linea Verde
		if (showXaxis){
			glLineWidth(2);
			glColor4f(LineaPisoRoja[0],LineaPisoRoja[1],LineaPisoRoja[2],LineaPisoRoja[3]);
			glDrawElements( GL_LINES, 2, GL_UNSIGNED_SHORT, EjeRojo );
		}
		else if (showFloor){
			glLineWidth(1);
			glColor4f(LineaPiso[0],LineaPiso[1],LineaPiso[2],LineaPiso[3]);
			glDrawElements( GL_LINES, 2, GL_UNSIGNED_SHORT, EjeRojo );
		}
		//linea Roja	
		if (showYaxis){
			glLineWidth(2);
			glColor4f(LineaPisoVerde[0],LineaPisoVerde[1],LineaPisoVerde[2],LineaPisoVerde[3]);
			glDrawElements( GL_LINES, 2, GL_UNSIGNED_SHORT, EjeVerde );
		}
		else if (showFloor){
			glLineWidth(1);
			glColor4f(LineaPiso[0],LineaPiso[1],LineaPiso[2],LineaPiso[3]);
			glDrawElements( GL_LINES, 2, GL_UNSIGNED_SHORT, EjeVerde );
		}	
		glDisable(GL_FOG);
	}

	//dibujo de objetos nuevo!
	if (showOverlays){
		for (int o = 0; o < Collection.Count(); o++) {
			RenderObjectAndChildrens(Collection[o]);
		}
	}
	
    //dibuja los ejes de transformacion y el origen del objeto
	glDisable( GL_DEPTH_TEST );		
	if (Objects.Count() > 0 && (showOverlays && showOrigins) || estado == translacion || estado == translacionVertex || estado == rotacion || estado == escala) {
        for (TInt o = 0; o < Collection.Count(); o++) {
            TBool found = false;
            Object& obj = Objects[Collection[o]];
            SearchSelectObj(obj, Collection[o], found);
            if (found) break;  // Si ya encontró el objeto, salir del bucle
        }
    }

	//dibuja el cursor 3D	
	if (showOverlays && show3DCursor){
	    glPushMatrix(); //guarda la matrix
		glTranslatef( Cursor3DposX, Cursor3DposZ, Cursor3DposY);
		
		glEnable( GL_TEXTURE_2D );
		glEnable( GL_BLEND );
		glEnable( GL_POINT_SPRITE_OES ); // Enable point sprites.	
		glPointSize( 32 ); // Make the points bigger.
		glColor4f(ListaColores[blanco][0],ListaColores[blanco][1],ListaColores[blanco][2],ListaColores[blanco][3]);
	    glVertexPointer( 3, GL_SHORT, 0, pointVertex );
	    glBindTexture( GL_TEXTURE_2D, Textures[3].iID);//iCursor3dTextura.iID ); //selecciona la textura

	    glTexEnvi( GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_TRUE );
	    glDrawArrays( GL_POINTS, 0, 1 );
	    glTexEnvi( GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_FALSE);

		//dibuja lineas		
		glDisable( GL_TEXTURE_2D );
		glDisable( GL_POINT_SPRITE_OES );
		glDisable( GL_BLEND );

		glLineWidth(1);	
		glColor4f(ListaColores[negro][0],ListaColores[negro][1],ListaColores[negro][2],ListaColores[negro][3]);
	    glVertexPointer( 3, GL_SHORT, 0, Cursor3DVertices );
		glDrawElements( GL_LINES, Cursor3DEdgesSize, GL_UNSIGNED_SHORT, Cursor3DEdges );	

	    glPopMatrix(); //reinicia la matrix a donde se guardo	
	}

	if (ShowUi){
		dibujarUI();
	}

    //termino de dibujar
    redibujar = false;
}

void CBlenderLite::SearchSelectObj(Object& obj, TInt objIndex, TBool& found) {
    glPushMatrix();    
    glTranslatef(obj.posX, obj.posZ, obj.posY);
    
    if (objIndex == objSelect) {
		if (estado == rotacion || estado == escala){
			glRotatef(obj.rotX, 1, 0, 0); //angulo, X Y Z
			glRotatef(obj.rotZ, 0, 1, 0); //angulo, X Y Z
			glRotatef(obj.rotY, 0, 0, 1); //angulo, X Y Z
		}		
		//dibuja los ejes de transformacion
		if (estado == translacion || estado == translacionVertex || estado == rotacion || estado == escala){		
        	DrawTransformAxis(obj);
		}		
		//dibuja el origen
		if (showOverlays && showOrigins) {
			DibujarOrigen();
		}
        found = true;
    } else {
		glRotatef(obj.rotX, 1, 0, 0); //angulo, X Y Z
		glRotatef(obj.rotZ, 0, 1, 0); //angulo, X Y Z
		glRotatef(obj.rotY, 0, 0, 1); //angulo, X Y Z
		glScalex(obj.scaleX, obj.scaleZ, obj.scaleY);	
        for (int c = 0; c < obj.Childrens.Count(); c++) {
            if (found) break;  // Si ya lo encontró, salir del bucle
            Object& objChild = Objects[obj.Childrens[c]];
            SearchSelectObj(objChild, obj.Childrens[c], found);
        }
    }
    glPopMatrix();
}

void CBlenderLite::DrawTransformAxis(Object& obj) {
	glPushMatrix();    
	glVertexPointer( 3, GL_SHORT, 0, objVertexdataFloor );
	glLineWidth(2);	
	if (estado == translacionVertex){
		Mesh& pMesh = Meshes[obj.Id];
		glTranslatef(pMesh.vertex[pMesh.vertexGroups[EditSelect].indices[0]*3]     *obj.scaleX/65000, 
						pMesh.vertex[pMesh.vertexGroups[EditSelect].indices[0]*3+1]*obj.scaleY/65000, 
						pMesh.vertex[pMesh.vertexGroups[EditSelect].indices[0]*3+2]*obj.scaleZ/65000
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
	glPopMatrix();
}

void CBlenderLite::DibujarOrigen(){
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_BLEND );
	// Enable point sprites.
	glEnable( GL_POINT_SPRITE_OES );
	// Make the points bigger.
	glPointSize( 8 );
	glColor4f(ListaColores[blanco][0],ListaColores[blanco][1],ListaColores[blanco][2],ListaColores[blanco][3]);
	glVertexPointer( 3, GL_SHORT, 0, pointVertex );
	//glBindTexture( GL_TEXTURE_2D, iOrigenTextura.iID ); //selecciona la textura
	glBindTexture( GL_TEXTURE_2D, Textures[0].iID ); //selecciona la textura
	glTexEnvi( GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_TRUE );
	glDrawArrays( GL_POINTS, 0, 1 );
	glTexEnvi( GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_FALSE);
	glDisable( GL_POINT_SPRITE_OES );
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

	//glDisable( GL_POINT_SPRITE_OES );
	//glTexEnvi( GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_FALSE);
	//glEnable( GL_TEXTURE_2D ); // Permite usar texturas
	glEnable( GL_BLEND ); //permite transparents
	//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //hace que sea pixelada
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //hace que sea pixelada
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	
    glVertexPointer( 3, GL_SHORT, 0, SpriteVertices );
	glTexCoordPointer( 2, GL_BYTE, 0, SpriteUV ); //SpriteUvSize
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//header	
	glDisable( GL_TEXTURE_2D ); // Permite usar texturas
	glColor4f(ListaColores[headerColor][0],ListaColores[headerColor][1],ListaColores[headerColor][2],0.8f);
	SetSpriteSize(iScreenWidth,24);
	DrawnRectangle();
	
	glEnable( GL_TEXTURE_2D ); // Permite usar texturas
	glColor4f(ListaColores[blanco][0],ListaColores[blanco][1],ListaColores[blanco][2],ListaColores[blanco][3]);	
	glBindTexture( GL_TEXTURE_2D, Textures[1].iID ); 

	//que editor esta abierto
	glPushMatrix();
	UiMoveTo(5,5);
	SetUvSprite(113*2,49*2,28,28);
	SetSpriteSize(14,14);
	DrawnRectangle();

	//icono de modo objeto
	UiMoveTo(25,0);
	if (estado != edicion){
		SetUvSprite(1*2,113*2,28,28);
	}
	//icono de edicion de mesh
	else {
		SetUvSprite(1*2,97*2,28,28);
	}
	DrawnRectangle();

	//icono de overlay
	UiMoveTo(23,-2);
	IconSelect(showOverlays);
	UiMoveTo(2,2);
	SetUvSprite(33*2,81*2,28,28);
	DrawnRectangle();
	
	//en caso de que overlay este desactivado. ese es el valor que tiene que estar
	glColor4f(ListaColores[blanco][0],ListaColores[blanco][1],ListaColores[blanco][2],ListaColores[blanco][3]);	
	glEnable( GL_TEXTURE_2D );

	//grupo de shaders ancho, alto,
	UiMoveTo(23,-3);
	SetUvBordes(1*2,56*2, 8*2,9*2, 4*2,4*2,5*2,4*2 );	
	DibujarRectanguloBordes(79,21, 4,4,5,4 );

	//wireframe
	UiMoveTo(1,1);
	IconSelect(view == Wireframe);
	UiMoveTo(2,2);
	SetUvSprite(65*2,113*2,28,28);
	SetSpriteSize(14,14);
	DrawnRectangle();

	//solid
	UiMoveTo(18,-2);
	IconSelect(view == Solid);
	UiMoveTo(2,2);
	SetUvSprite(81*2,113*2,28,28);
	DrawnRectangle();

	//material preview
	UiMoveTo(18,-2);
	IconSelect(view == MaterialPreview);
	UiMoveTo(2,2);
	SetUvSprite(97*2,113*2,28,28);
	DrawnRectangle(); 

	//Rendered
	UiMoveTo(18,-2);
	IconSelect(view == Rendered);
	UiMoveTo(2,2);
	SetUvSprite(113*2,113*2,28,28);
	DrawnRectangle();
	glPopMatrix(); //reinicia la matrix a donde se guardo	

	//dibuja el mouse por arriba de todo
	if (mouseVisible){
		UiMoveTo(mouseX,mouseY);
		SetUvSprite(1,1,20,23);
		SetSpriteSize(10,17);
		DrawnRectangle();
	}

	if (ShowTimeline){
		DrawnTimeline();
	}

	//resetea la perspectiva	
	ortografica = !ortografica;
	SetPerspectiva(false);
}

void CBlenderLite::DrawnTimeline(){
	glDisable( GL_TEXTURE_2D );	
	glDisable( GL_BLEND );

	UiMoveTo(0,iScreenHeight-32);
	/*glColor4f(0.114f,0.114f,0.114f,1.0f);
	SetSpriteSize(iScreenWidth,48);
	DrawnRectangle();

	UiMoveTo(0,24);*/
	glColor4f(0.188f,0.188f,0.188f,1.0f);
	SetSpriteSize(iScreenWidth,16);
	DrawnRectangle();

	glColor4f(0.086f,0.086f,0.086f,1.0f);
    GLshort distanciaEntreLineas = iScreenWidth / 5;
	glPushMatrix();
    for (int l = 0; l < 4; l++) {
		UiMoveTo(distanciaEntreLineas,0);
		DrawnLines(1, 1, LineaTimeline, LineaEdge);
    }
	glPopMatrix(); //reinicia la matrix a donde se guardo	

	glPushMatrix();
	glColor4f(0.149f,0.149f,0.149f,1.0f);
	UiMoveTo(distanciaEntreLineas/2,0);
    for (int l = 0; l < 4; l++) {
		DrawnLines(1, 1, LineaTimeline, LineaEdge);
		UiMoveTo(distanciaEntreLineas,0);
    }
	glPopMatrix(); //reinicia la matrix a donde se guardo	

	//linea azul del tiempo
	glColor4f(0.259f,0.427f,0.682f,1.0f); 
    GLshort posicionTiempo = CurrentFrame;
	UiMoveTo(posicionTiempo,0);
	DrawnLines(2, 1, LineaTimeline, LineaEdge);
}

void CBlenderLite::DrawnLines(TInt LineWidth, TInt cantidad, GLshort* vertexlines, GLushort* lineasIndices){
	glVertexPointer( 3, GL_SHORT, 0, vertexlines );
	glLineWidth(LineWidth);	
	glDrawElements( GL_LINES, cantidad*2, GL_UNSIGNED_SHORT, lineasIndices );
}

void CBlenderLite::DrawnLines(TInt LineWidth, TInt cantidad, const GLshort* vertexlines, const GLushort* lineasIndices) {
    glVertexPointer(3, GL_SHORT, 0, vertexlines);
    glLineWidth(LineWidth);
    glDrawElements(GL_LINES, cantidad * 2, GL_UNSIGNED_SHORT, lineasIndices);
}

void CBlenderLite::SetUvSprite(GLshort x, GLshort y, GLshort ancho, GLshort alto){
	SpriteUV[0] = SpriteUV[6] = (GLbyte)(-128+x);
	SpriteUV[2] = SpriteUV[4] = (GLbyte)(-128+x+ancho);
	SpriteUV[1] = SpriteUV[3] = (GLbyte)(-128+y);
	SpriteUV[5] = SpriteUV[7] = (GLbyte)(-128+y+alto);
}

void CBlenderLite::SetSpriteSize(GLshort ancho, GLshort alto){
	SpriteVertices[3] = SpriteVertices[6] = ancho+1;
	SpriteVertices[7] = SpriteVertices[10] = -(alto+1);
}

void CBlenderLite::DrawnRectangle(){
	glDrawElements( GL_TRIANGLES, 2*3, GL_UNSIGNED_SHORT, SpriteFaces );
}

void CBlenderLite::IconSelect(TBool activo){
	if (activo){
		glDisable( GL_TEXTURE_2D );
		glColor4f(ListaColores[azulUI][0],ListaColores[azulUI][1],ListaColores[azulUI][2],ListaColores[azulUI][3]);	
		SetSpriteSize(18,18);
		DrawnRectangle();
		SetSpriteSize(14,14);
		glColor4f(ListaColores[blanco][0],ListaColores[blanco][1],ListaColores[blanco][2],ListaColores[blanco][3]);	
		glEnable( GL_TEXTURE_2D );
	}
	else {
		glColor4f(ListaColores[grisUI][0],ListaColores[grisUI][1],ListaColores[grisUI][2],ListaColores[grisUI][3]);
	}
}

void CBlenderLite::UiMoveTo(GLshort x, GLshort y){
	glTranslatef( x, -y, 0);
}

void CBlenderLite::SetUvBordes(GLshort origenX, GLshort origenY, GLshort ancho, GLshort alto, GLshort top, GLshort right, GLshort bottom, GLshort left){
	//Filas en X
	SpriteUV[0]  = SpriteUV[6]  = SpriteUV[16] = SpriteUV[24] = (GLbyte)(-128+origenX);
	SpriteUV[2]  = SpriteUV[4]  = SpriteUV[18] = SpriteUV[26] = (GLbyte)(-128+origenX+left);
	SpriteUV[8]  = SpriteUV[10] = SpriteUV[20] = SpriteUV[28] = (GLbyte)(-128+origenX+ancho-right);	
	SpriteUV[12] = SpriteUV[14] = SpriteUV[22] = SpriteUV[30] = (GLbyte)(-128+origenX+ancho);
	//fila en Y
	SpriteUV[1]  = SpriteUV[3]  = SpriteUV[9]  = SpriteUV[13] =(GLbyte)(-128+origenY);
	SpriteUV[7]  = SpriteUV[5]  = SpriteUV[11] = SpriteUV[15] =(GLbyte)(-128+origenY+top);
	SpriteUV[17] = SpriteUV[19] = SpriteUV[21] = SpriteUV[23] =(GLbyte)(-128+origenY+alto-bottom);
	SpriteUV[25] = SpriteUV[27] = SpriteUV[29] = SpriteUV[31] =(GLbyte)(-128+origenY+alto);
}

void CBlenderLite::DibujarRectanguloBordes(GLshort ancho, GLshort alto, GLshort top, GLshort right, GLshort bottom, GLshort left ){
	//cambia el tamaño
	//Posicion en X
	SpriteVertices[3]  = SpriteVertices[6]  = SpriteVertices[27] = SpriteVertices[39] = left+1;
	SpriteVertices[12] = SpriteVertices[15] = SpriteVertices[30] = SpriteVertices[42] = ancho-right+1;
	SpriteVertices[18] = SpriteVertices[21] = SpriteVertices[33] = SpriteVertices[45] = ancho+1;	
	//Posicion en Y
	SpriteVertices[10] = SpriteVertices[7]  = SpriteVertices[16] = SpriteVertices[22] = -(top+1);
	SpriteVertices[25] = SpriteVertices[28] = SpriteVertices[31] = SpriteVertices[34] = -(alto-bottom+1);
	SpriteVertices[37] = SpriteVertices[40] = SpriteVertices[43] = SpriteVertices[46] = -(alto+1);

	glDrawElements( GL_TRIANGLES, 18*3, GL_UNSIGNED_SHORT, SpriteFaces );
}

void CBlenderLite::SetMouse(){
	mouseVisible = !mouseVisible;
	mouseX = iScreenWidth/2;
	mouseY = (GLshort)-iScreenHeightSplit;
    redibujar = true;
}

//invierte cualquier valor que se le manda, de verdadero a falso y viceversa
void CBlenderLite::ToggleValue(TBool& valueToUpdate){
    valueToUpdate = !valueToUpdate;
    redibujar = true;
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
			if (navegacionMode == Orbit){
				rotX-= 0.5;		
			}
			else if (navegacionMode == Fly){
				// Convertir el ángulo de rotX a radianes
				GLfloat radRotX = rotX * PI / 180.0;

				// Calcular el vector de dirección hacia la izquierda (90 grados a la izquierda del ángulo actual)
				GLfloat leftX = cos(radRotX);
				GLfloat leftY = sin(radRotX);

				// Mover hacia la izquierda
				PivotX += 30 * leftX;
				PivotY += 30 * leftY;
			}	
		}
		else if (estado == translacionVertex){	
			Mesh& pMesh = Meshes[Objects[objSelect].Id];	
			/*for(int g=0; g < pMesh.vertexGroupIndiceSize[EditSelect]; g++){
				if (axisSelect == X){
					pMesh.vertex[pMesh.vertexGroupIndice[EditSelect][g]*3] += 30;					
				}
				else if (axisSelect == Y){
					pMesh.vertex[pMesh.vertexGroupIndice[EditSelect][g]*3+2] -= 30;				
				}
				else if (axisSelect == Z){
					pMesh.vertex[pMesh.vertexGroupIndice[EditSelect][g]*3+1] -= 30;	
				}
			}*/
			for(int g=0; g < pMesh.vertexGroups[EditSelect].indices.Count(); g++){
				if (axisSelect == X){
					pMesh.vertex[pMesh.vertexGroups[EditSelect].indices[g]*3] += 30;					
				}
				else if (axisSelect == Y){
					pMesh.vertex[pMesh.vertexGroups[EditSelect].indices[g]*3+2] -= 30;				
				}
				else if (axisSelect == Z){
					pMesh.vertex[pMesh.vertexGroups[EditSelect].indices[g]*3+1] -= 30;	
				}
			}
		}
		else if (estado == translacion){
			if (axisSelect == X){
				Objects[objSelect].posX += 30;					
			}
			else if (axisSelect == Y){
				Objects[objSelect].posY -= 30;					
			}
			else if (axisSelect == Z){
				Objects[objSelect].posZ -= 30;					
			}
		}
		else if (estado == rotacion){
			if (axisSelect == X){
				Objects[objSelect].rotX -= 1;					
			}
			else if (axisSelect == Y){
				Objects[objSelect].rotY -= 1;					
			}
			else if (axisSelect == Z){
				Objects[objSelect].rotZ -= 1;					
			}
		}
		else if (estado == escala){
			if (axisSelect == X){Objects[objSelect].scaleX -= 1000;}
			else if (axisSelect == Y){Objects[objSelect].scaleY -= 1000;}
			else if (axisSelect == Z){Objects[objSelect].scaleZ -= 1000;}
			else if (axisSelect == XYZ){
				Objects[objSelect].scaleX -= 1000;
				Objects[objSelect].scaleY -= 1000;
				Objects[objSelect].scaleZ -= 1000;
			}
		}
		else if (estado == timelineMove){
			CurrentFrame--;
			if (!PlayAnimation){
				ReloadAnimation();
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
			if (navegacionMode == Orbit){
				rotX+= 0.5;			
			}
			else if (navegacionMode == Fly){
				// Convertir el ángulo de rotX a radianes
				GLfloat radRotX = rotX * PI / 180.0;

				// Calcular el vector de dirección hacia la izquierda (90 grados a la izquierda del ángulo actual)
				GLfloat leftX = cos(radRotX);
				GLfloat leftY = sin(radRotX);

				// Mover hacia la izquierda
				PivotX -= 30 * leftX;
				PivotY -= 30 * leftY;
			}
		}
		else if (estado == translacionVertex){	
			Mesh& pMesh = Meshes[Objects[objSelect].Id];		
			/*for(int g=0; g < pMesh.vertexGroupIndiceSize[EditSelect]; g++){
				if (axisSelect == X){
					pMesh.vertex[pMesh.vertexGroupIndice[EditSelect][g]*3] -= 30;					
				}
				else if (axisSelect == Y){
					pMesh.vertex[pMesh.vertexGroupIndice[EditSelect][g]*3+2] += 30;				
				}
				else if (axisSelect == Z){
					pMesh.vertex[pMesh.vertexGroupIndice[EditSelect][g]*3+1] += 30;	
				}
			}*/		
			for(int g=0; g < pMesh.vertexGroups[EditSelect].indices.Count(); g++){
				if (axisSelect == X){
					pMesh.vertex[pMesh.vertexGroups[EditSelect].indices[g]*3] -= 30;					
				}
				else if (axisSelect == Y){
					pMesh.vertex[pMesh.vertexGroups[EditSelect].indices[g]*3+2] += 30;				
				}
				else if (axisSelect == Z){
					pMesh.vertex[pMesh.vertexGroups[EditSelect].indices[g]*3+1] += 30;	
				}
			}			
		}
		else if (estado == translacion){
			if (axisSelect == X){
				Objects[objSelect].posX -= 30;					
			}
			else if (axisSelect == Y){
				Objects[objSelect].posY += 30;					
			}
			else if (axisSelect == Z){
				Objects[objSelect].posZ += 30;					
			}
		}
		else if (estado == rotacion){
			if (axisSelect == X){
				Objects[objSelect].rotX += 1;					
			}
			else if (axisSelect == Y){
				Objects[objSelect].rotY += 1;					
			}
			else if (axisSelect == Z){
				Objects[objSelect].rotZ += 1;					
			}
		}
		else if (estado == escala){
			if (axisSelect == X){Objects[objSelect].scaleX += 1000;}
			else if (axisSelect == Y){Objects[objSelect].scaleY += 1000;}
			else if (axisSelect == Z){Objects[objSelect].scaleZ += 1000;}
			else if (axisSelect == XYZ){
				Objects[objSelect].scaleX += 1000;
				Objects[objSelect].scaleY += 1000;
				Objects[objSelect].scaleZ += 1000;
			}			
		}
		else if (estado == timelineMove){
			CurrentFrame++;
			if (!PlayAnimation){
				ReloadAnimation();
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
			if (navegacionMode == Orbit){
				rotY-= 0.5;		
			}
			else if (navegacionMode == Fly){
				// Convertir el ángulo de rotX a radianes
				float radRotX = rotX * PI / 180.0;

				PivotY+= 30 * cos(radRotX);
				PivotX-= 30 * sin(radRotX);
			}		
		}
		else if (estado == translacionVertex){	
			Mesh& pMesh = Meshes[Objects[objSelect].Id];	
			/*for(int g=0; g < pMesh.vertexGroupIndiceSize[EditSelect]; g++){
				if (axisSelect == X){
					pMesh.vertex[pMesh.vertexGroupIndice[EditSelect][g]*3] -= 30;					
				}
				else if (axisSelect == Y){
					pMesh.vertex[pMesh.vertexGroupIndice[EditSelect][g]*3+2] += 30;				
				}
				else if (axisSelect == Z){
					pMesh.vertex[pMesh.vertexGroupIndice[EditSelect][g]*3+1] += 30;	
				}
			}*/	
			for(int g=0; g < pMesh.vertexGroups[EditSelect].indices.Count(); g++){
				if (axisSelect == X){
					pMesh.vertex[pMesh.vertexGroups[EditSelect].indices[g]*3] -= 30;					
				}
				else if (axisSelect == Y){
					pMesh.vertex[pMesh.vertexGroups[EditSelect].indices[g]*3+2] += 30;				
				}
				else if (axisSelect == Z){
					pMesh.vertex[pMesh.vertexGroups[EditSelect].indices[g]*3+1] += 30;	
				}
			}
		}
		else if (estado == translacion){
			if (axisSelect == X){
				Objects[objSelect].posX -= 30;					
			}
			else if (axisSelect == Y){
				Objects[objSelect].posY += 30;					
			}
			else if (axisSelect == Z){
				Objects[objSelect].posZ += 30;					
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
			if (navegacionMode == Orbit){
				rotY+= 0.5;		
			}
			else if (navegacionMode == Fly){
				// Convertir el ángulo de rotX a radianes
				float radRotX = rotX * PI / 180.0;

				PivotY-= 30 * cos(radRotX);
				PivotX+= 30 * sin(radRotX);
			}
		}
		else if (estado == translacionVertex){	
			Mesh& pMesh = Meshes[Objects[objSelect].Id];	
			/*for(int g=0; g < pMesh.vertexGroupIndiceSize[EditSelect]; g++){
				if (axisSelect == X){
					pMesh.vertex[pMesh.vertexGroupIndice[EditSelect][g]*3] += 30;					
				}
				else if (axisSelect == Y){
					pMesh.vertex[pMesh.vertexGroupIndice[EditSelect][g]*3+2] -= 30;				
				}
				else if (axisSelect == Z){
					pMesh.vertex[pMesh.vertexGroupIndice[EditSelect][g]*3+1] -= 30;	
				}
			}	*/
			for(int g=0; g < pMesh.vertexGroups[EditSelect].indices.Count(); g++){
				if (axisSelect == X){
					pMesh.vertex[pMesh.vertexGroups[EditSelect].indices[g]*3] += 30;					
				}
				else if (axisSelect == Y){
					pMesh.vertex[pMesh.vertexGroups[EditSelect].indices[g]*3+2] -= 30;				
				}
				else if (axisSelect == Z){
					pMesh.vertex[pMesh.vertexGroups[EditSelect].indices[g]*3+1] -= 30;	
				}
			}
		}
		else if (estado == translacion){
			if (axisSelect == X){
				Objects[objSelect].posX += 30;					
			}
			else if (axisSelect == Y){
				Objects[objSelect].posY -= 30;					
			}
			else if (axisSelect == Z){
				Objects[objSelect].posZ -= 30;					
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
	if (Objects.Count() < 1){return;}
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
	if (Objects.Count() < 1){return;}
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
	if (Objects.Count() < 1){return;}

	if (estado == navegacion){
		guardarEstado(objSelect);
		estado = translacion;
		if (axisSelect > 2){axisSelect = X;}
		colorBordeSelect = 0;
	}
	else if (estado == edicion){
		switch (tipoSelect) {
			case vertexSelect:
				estado = translacionVertex;
				break;
			case edgeSelect:
				estado = translacionEdge;
				break;
			case faceSelect:
				estado = translacionFace;
				break;
			default:
				// Manejar cualquier otro caso aquí si es necesario
				break;
		}
		guardarEstado(objSelect);
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
	if (estado == translacionVertex){
		ReestablecerEstado(objSelect);
		estado = edicion;		
	}
	else if (estado == translacion || estado == rotacion || estado == escala){
		ReestablecerEstado(objSelect);
		estado = navegacion;	
	}
    redibujar = true;
};

void CBlenderLite::Aceptar(){	
	//si no hay objetos
	if (Objects.Count() < 1){return;}	

	Object& obj = Objects[objSelect];	

	if ( estado == navegacion && obj.type == mesh ){
		estado = edicion;
		EditSelect = 0;
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
	}
	else if (estado == timelineMove){	
		HBufC* noteBuf = HBufC::NewLC(100);
		noteBuf->Des().Copy(_L("Set Current Frame"));
		CurrentFrame = DialogNumber(CurrentFrame, StartFrame-1, EndFrame+100, noteBuf);	
		CleanupStack::PopAndDestroy(noteBuf);
		Aceptar();
	};
};


void CBlenderLite::ReestablecerEstado(int indice){
	Object& obj = Objects[indice];

	if (estado == translacionVertex && obj.type == mesh ){
		Mesh& pMesh = Meshes[obj.Id];
		 /*for(int g=0; g < pMesh.vertexGroupIndiceSize[EditSelect]; g++){
			pMesh.vertex[pMesh.vertexGroupIndice[EditSelect][g]*3] = estadoVertex[0];
			pMesh.vertex[pMesh.vertexGroupIndice[EditSelect][g]*3+2] = estadoVertex[1];	
			pMesh.vertex[pMesh.vertexGroupIndice[EditSelect][g]*3+1] = estadoVertex[2];	
		}*/
		 for(int g=0; g < pMesh.vertexGroups[EditSelect].indices.Count(); g++){
			pMesh.vertex[pMesh.vertexGroups[EditSelect].indices[g]*3] = estadoVertex[0];
			pMesh.vertex[pMesh.vertexGroups[EditSelect].indices[g]*3+2] = estadoVertex[1];	
			pMesh.vertex[pMesh.vertexGroups[EditSelect].indices[g]*3+1] = estadoVertex[2];	
		}
	}
	else {
		obj.posX = estadoObj.posX;
		obj.posY = estadoObj.posY;
		obj.posZ = estadoObj.posZ;
		obj.rotX = estadoObj.rotX;
		obj.rotY = estadoObj.rotY;
		obj.rotZ = estadoObj.rotZ;
		obj.scaleX = estadoObj.scaleX;
		obj.scaleY = estadoObj.scaleY;
		obj.scaleZ = estadoObj.scaleZ;		
	}
};

void CBlenderLite::guardarEstado(int indice){	
	Object& obj = Objects[indice];

	/*if (estado == translacionVertex){
		Mesh& pMesh = Meshes[obj.Id];
		estadoVertex[0] = pMesh.vertex[pMesh.vertexGroupIndice[EditSelect][0]*3];
		estadoVertex[1] = pMesh.vertex[pMesh.vertexGroupIndice[EditSelect][0]*3+2];	
		estadoVertex[2] = pMesh.vertex[pMesh.vertexGroupIndice[EditSelect][0]*3+1];	
	}*/
	if (estado == translacionVertex){
		Mesh& pMesh = Meshes[obj.Id];
		if (pMesh.vertexGroups.Count() > 0 && pMesh.vertexGroups[EditSelect].indices.Count()){
			estadoVertex[0] = pMesh.vertex[pMesh.vertexGroups[EditSelect].indices[0]*3];
			estadoVertex[1] = pMesh.vertex[pMesh.vertexGroups[EditSelect].indices[0]*3+2];	
			estadoVertex[2] = pMesh.vertex[pMesh.vertexGroups[EditSelect].indices[0]*3+1];	
		}
		else {
			estado = edicion;
		}						
		/*HBufC* noteBuf = HBufC::NewLC(100);
		_LIT(KStaticErrorMessage, "pos: %d, %d, %d");
		//noteBuf->Des().Format(KStaticErrorMessage, pMesh.vertexGroups.Count(), pMesh.vertexGroups[EditSelect].indices.Count());
		noteBuf->Des().Format(KStaticErrorMessage, 
			pMesh.vertexGroupUI[pMesh.vertexGroups[EditSelect].indices[0]*3],
			pMesh.vertexGroupUI[pMesh.vertexGroups[EditSelect].indices[0]*3+1],
			pMesh.vertexGroupUI[pMesh.vertexGroups[EditSelect].indices[0]*3+2]
		);		
		DialogAlert(noteBuf);	
		CleanupStack::PopAndDestroy(noteBuf);*/
	}
	else {
		estadoObj.posX = obj.posX;
		estadoObj.posY = obj.posY;
		estadoObj.posZ = obj.posZ;
		estadoObj.rotX = obj.rotX;
		estadoObj.rotY = obj.rotY;
		estadoObj.rotZ = obj.rotZ;
		estadoObj.scaleX = obj.scaleX;
		estadoObj.scaleY = obj.scaleY;
		estadoObj.scaleZ = obj.scaleZ;		
	}
};

//cambie el shader
void CBlenderLite::SetShading(TInt valor){	
	for (int i = 0; i < Lights.Count(); i++) {
		//glDisable( Lights[i].lightId );			
		//glDisable( GL_LIGHT3 );			
	}
	switch (valor) {
		case Wireframe:
			glDisable( GL_LIGHT0 );
    		glLightModelfv( GL_LIGHT_MODEL_AMBIENT, globalAmbient );
			break;
		case Rendered:
			glDisable( GL_LIGHT0 );			
			for (int i = 0; i < Lights.Count(); i++) {
				//glEnable( Lights[i].lightId );		
				glEnable( GL_LIGHT3 );	
			}
    		glLightModelfv( GL_LIGHT_MODEL_AMBIENT, AmbientRender );
			break;
		case Solid:
			glEnable( GL_LIGHT0 );
    		glLightModelfv( GL_LIGHT_MODEL_AMBIENT, globalAmbient );
			break;
		case MaterialPreview:
			glEnable( GL_LIGHT0 );
    		glLightModelfv( GL_LIGHT_MODEL_AMBIENT, globalAmbient );
			break;
		default:
			glDisable( GL_LIGHT0 );
			break;
	};

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

void CBlenderLite::InsertarValor(){
	HBufC* buf = HBufC::NewLC( 20 );
	if (estado == translacion){
		if (axisSelect == X){
			buf->Des().Copy(_L("Mover en X"));
			TInt valorX = DialogNumber((TInt)(Objects[objSelect].posX-estadoObj.posX), -100000, 100000,buf);
			Objects[objSelect].posX = estadoObj.posX+valorX;			
		}
		else if (axisSelect == Y){
			buf->Des().Copy(_L("Mover en Y"));
			TInt valorY = DialogNumber((TInt)(Objects[objSelect].posY-estadoObj.posY), -100000, 100000,buf);
			Objects[objSelect].posY = estadoObj.posY+valorY;			
		}
		else if (axisSelect == Z){
			buf->Des().Copy(_L("Mover en Z"));
			TInt valorZ = DialogNumber((TInt)(Objects[objSelect].posZ-estadoObj.posZ), -100000, 100000,buf);
			Objects[objSelect].posZ = estadoObj.posZ+valorZ;			
		}
	}
	else if (estado == edicion){
		/*if (axisSelect == X){
			buf->Des().Copy(_L("Posicion en X"));
			TInt valorX = DialogNumber((TInt)Objects[objSelect].vertex[Objects[objSelect].vertexGroupIndice[EditSelect][0]*3], -1000000, 1000000,buf);
			for(int g=0; g < Objects[objSelect].vertexGroupIndiceSize[EditSelect]; g++){
				Objects[objSelect].vertex[Objects[objSelect].vertexGroupIndice[EditSelect][g]*3] = valorX;
			}
		}
		else if (axisSelect == Y){
			buf->Des().Copy(_L("Posicion en Y"));
			TInt valorY = DialogNumber((TInt)Objects[objSelect].vertex[Objects[objSelect].vertexGroupIndice[EditSelect][0]*3+2], -1000000, 1000000,buf);
			for(int g=0; g < Objects[objSelect].vertexGroupIndiceSize[EditSelect]; g++){
				Objects[objSelect].vertex[Objects[objSelect].vertexGroupIndice[EditSelect][g]*3+2] = valorY;
			}
		}
		else if (axisSelect == Z){
			buf->Des().Copy(_L("Posicion en Z"));
			TInt valorZ = DialogNumber((TInt)Objects[objSelect].vertex[Objects[objSelect].vertexGroupIndice[EditSelect][0]*3+1], -1000000, 1000000,buf);
			for(int g=0; g < Objects[objSelect].vertexGroupIndiceSize[EditSelect]; g++){
				Objects[objSelect].vertex[Objects[objSelect].vertexGroupIndice[EditSelect][g]*3+1] = valorZ;
			}
		}*/	
	}
	else if (estado == translacionVertex){
		/*if (axisSelect == X){
			buf->Des().Copy(_L("Posicion en X")); //(Objects[objSelect].vertex[Objects[objSelect].vertexGroupIndice[EditSelect][0]*3]-
			TInt valorX = DialogNumber((TInt)(Objects[objSelect].vertex[Objects[objSelect].vertexGroupIndice[EditSelect][0]*3]-estadoVertex[0]), -1000000, 1000000,buf);
			for(int g=0; g < Objects[objSelect].vertexGroupIndiceSize[EditSelect]; g++){
				Objects[objSelect].vertex[Objects[objSelect].vertexGroupIndice[EditSelect][g]*3] = valorX+estadoVertex[0]; //)*1;
			}
		}
		else if (axisSelect == Y){
			buf->Des().Copy(_L("Posicion en Y"));
			TInt valorY = DialogNumber((TInt)(Objects[objSelect].vertex[Objects[objSelect].vertexGroupIndice[EditSelect][0]*3+2]-estadoVertex[1]), -1000000, 1000000,buf);
			for(int g=0; g < Objects[objSelect].vertexGroupIndiceSize[EditSelect]; g++){
				Objects[objSelect].vertex[Objects[objSelect].vertexGroupIndice[EditSelect][g]*3+2] = valorY+estadoVertex[1];
			}
		}
		else if (axisSelect == Z){
			buf->Des().Copy(_L("Posicion en Z"));
			TInt valorZ = DialogNumber((TInt)(Objects[objSelect].vertex[Objects[objSelect].vertexGroupIndice[EditSelect][0]*3+1]-estadoVertex[2]), -1000000, 1000000,buf);
			for(int g=0; g < Objects[objSelect].vertexGroupIndiceSize[EditSelect]; g++){
				Objects[objSelect].vertex[Objects[objSelect].vertexGroupIndice[EditSelect][g]*3+1] = valorZ+estadoVertex[2];
			}
		}	
		Aceptar();*/
	}	
	//CleanupStack::PopAndDestroy(buf);
	redibujar = true;	
}

void CBlenderLite::TecladoNumerico(TInt numero){
	if (estado == translacion || estado == rotacion || estado == escala ){
		if (numero == 10){ //invertir
			if (axisSelect == X){
				Objects[objSelect].posX = estadoObj.posX-(Objects[objSelect].posX-estadoObj.posX);					
			}	
			else if (axisSelect == Y){
				Objects[objSelect].posY = estadoObj.posY-(Objects[objSelect].posY-estadoObj.posY);					
			}	
			else if (axisSelect == Z){
				Objects[objSelect].posZ = estadoObj.posZ-(Objects[objSelect].posZ-estadoObj.posZ);					
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
		SetShading(0);	
	}
	else if (numero == 5){
		SetShading(1);
	}
	else if (numero == 6){
		SetShading(2);
	}
};


void CBlenderLite::SetParent(){
	if (Objects.Count() < 1){return;}
	
	TInt ParentID = 1;
	HBufC* noteBuf = HBufC::NewLC(100);
	_LIT(KFormatString, "Set Parent 1 to %d");
	noteBuf->Des().Format(KFormatString, Objects.Count());
	ParentID = DialogNumber(1, 1, Objects.Count(), noteBuf);	
	ParentID --;

	//si se emparento a si mismo. falla
	if (ParentID == objSelect){
		noteBuf->Des().Copy(_L("El hijo y el padre son el mismo"));	
		MensajeError(noteBuf);
		CleanupStack::PopAndDestroy(noteBuf);
		return;
	}
	Objects[ParentID].Childrens.Append(objSelect);
	/*Objects[objSelect].posX = Objects[objSelect].posX - Objects[ParentID].posX* 65000;
	Objects[objSelect].posY = Objects[objSelect].posY - Objects[ParentID].posY* 65000;
	Objects[objSelect].posZ = Objects[objSelect].posZ - Objects[ParentID].posZ* 65000;*/
	/*Objects[objSelect].rotX = Objects[objSelect].rotX - Objects[ParentID].rotX;
	Objects[objSelect].rotY = Objects[objSelect].rotY - Objects[ParentID].rotY;
	Objects[objSelect].rotZ = Objects[objSelect].rotZ - Objects[ParentID].rotZ;*/
	/*Objects[objSelect].scaleX = Objects[ParentID].scaleX / Objects[objSelect].scaleX * 65000;
    Objects[objSelect].scaleY = Objects[ParentID].scaleY / Objects[objSelect].scaleY * 65000;
    Objects[objSelect].scaleZ = Objects[ParentID].scaleZ / Objects[objSelect].scaleZ * 65000;*/

	//si esta en la coleccion. lo borra
	for(int c=0; c < Collection.Count(); c++){
		if (Collection[c] == objSelect){
			Collection.Remove(c);
			break;			
		}
	}
	/*_LIT(KFormatString2, "coleccion %d");
	noteBuf->Des().Format(KFormatString2, Collection.Count());
	MensajeError(noteBuf);*/
	CleanupStack::PopAndDestroy(noteBuf);
	redibujar = true;
};

void CBlenderLite::ClearParent(){
	if (Objects.Count() < 1){return;}
	for(int c=0; c < Collection.Count(); c++){
		if (Collection[c] == objSelect){
			Collection.Remove(c);	
			break;
		}
	}
	Collection.Append(objSelect);

	//lo borra si quedo emparentado en algun objeto
	for(int o=0; o < Objects.Count(); o++){
		TBool salirBucle = false;
		for(int c=0; c < Objects[o].Childrens.Count(); c++){
			if (Objects[o].Childrens[c] == objSelect){
				Objects[o].Childrens.Remove(c);	
				salirBucle = true;
				break;
			}
		}
		if (salirBucle){break;}
	}
	redibujar = true;
};

void CBlenderLite::FlipNormals(){
	//si no hay objetos
	if (Objects.Count() < 1){return;}	
	Object& obj = Objects[objSelect];
	//si no es un mesh
	if (obj.type != mesh){return;}
	Mesh& pMesh = Meshes[obj.Id];	

	for(TInt a = 0; a < pMesh.vertexSize*3; a++) {
		pMesh.normals[a] = (GLbyte)-pMesh.normals[a];
	}
	redibujar = true;
}

void CBlenderLite::Borrar(){
	if (estado != navegacion && estado != edicion ){
		Cancelar();
	}
	else if (estado == navegacion){
		if (Objects.Count() < 1){return;}
		//pregunta de confirmacion
		HBufC* noteBuf = HBufC::NewLC(100);
		_LIT(KStaticErrorMessage, "Delete Object?");
		noteBuf->Des().Format(KStaticErrorMessage);
		if (!DialogAlert(noteBuf)){
			CleanupStack::PopAndDestroy(noteBuf);	
			return;
		}
		CleanupStack::PopAndDestroy(noteBuf);	
		Cancelar();

		//libera la memoria de los punteros primero	
		// Obtener el objeto seleccionado
		Object& obj = Objects[objSelect];

		// Liberar memoria de los punteros del objeto seleccionado
		if (obj.type == mesh){
			TInt links = 0;
			TInt id = obj.Id;
			
			for(int o=0; o < Objects.Count(); o++){
				if (Objects[o].type == mesh && Objects[o].Id == id){links++;};				
			}

			//si solo hay un objeto linkeado. borra la malla para ahorrar memoria			
			/*HBufC* noteBuf = HBufC::NewLC(100);
			_LIT(KFormatString, "hay %d objetos linkeados a la malla 3d (%d)");
			noteBuf->Des().Format(KFormatString, links, Meshes.Count());
			DialogAlert(noteBuf);*/
			if (links < 2){	
				for(int o=0; o < Objects.Count(); o++){
					if (Objects[o].type == mesh && Objects[o].Id > id){
						Objects[o].Id--;
					};				
				}
				Mesh& pMesh = Meshes[id];
				//primero se borran los objetos a los que apunta el mesh
				delete[] pMesh.vertex;
				delete[] pMesh.vertexColor;
				delete[] pMesh.normals;
				delete[] pMesh.uv;
				delete[] pMesh.facesGroupsSize;
				delete[] pMesh.materials;
				for(TInt i=0; i < pMesh.vertexGroups.Count(); i++){
					pMesh.vertexGroups[i].indices.Close();
				}
				pMesh.vertexGroups.Close();
				//delete[] pMesh.vertexGroup;
				//delete[] pMesh.vertexGroupIndiceSize;
				//for(int f=0; f < pMesh.vertexGroupSize; f++){
				//	delete[] pMesh.vertexGroupIndice[f];
				//}
				//delete[] pMesh.vertexGroupIndice;

				for(int f=0; f < pMesh.materialsSize; f++){
					delete[] pMesh.faces[f];
				}
				delete[] pMesh.faces;

				//ahora se borra el mesh
				Meshes.Remove(obj.Id);
			}
		}

		//si existe animaciones para ese objeto. las borra		
		for(TInt a = 0; a < Animations.Count(); a++) {
			if (Animations[a].Id == objSelect) {	
				for(TInt p = 0; p < Animations[a].Propertys.Count(); p++) {
					Animations[a].Propertys[p].keyframes.Close();
				}				
				Animations[a].Propertys.Close();
				Animations.Remove(a);
			}
			// Hace falta cambiar los índices
			else if (Animations[a].Id > objSelect) {
				Animations[a].Id--;
			}			
		}

		// Borrar de la colección
		for (int c = Collection.Count() - 1; c >= 0; c--) {
			if (Collection[c] == objSelect) {
				Collection.Remove(c);
			}
			// Hace falta cambiar los índices
			else if (Collection[c] > objSelect) {
				Collection[c]--;
			}
		}
		
		// Actualizar índices en los objetos
		for (int o = 0; o < Objects.Count(); o++) {
			for (int c = Objects[o].Childrens.Count() - 1; c >= 0; c--) {
				if (Objects[o].Childrens[c] == objSelect) {
					// Opcionalmente, puedes eliminar el hijo aquí si el objeto seleccionado es un hijo
					Objects[o].Childrens.Remove(c);
				} else if (Objects[o].Childrens[c] > objSelect) {
					Objects[o].Childrens[c]--;
				}
			}
		}

		Objects.Remove(objSelect);
		objSelect = Objects.Count()-1;		
		colorBordeSelect = 1;	
	}
	else if (estado == edicion){
		/*if (Objects[objSelect].vertexGroupSize < 1){return;}
		//pregunta de confirmacion
		_LIT(KStaticErrorMessage, "¿Eliminar Vertice?");
		if (!DialogAlert(KStaticErrorMessage)){return;}
		Mesh& obj = Objects[objSelect];
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
		}*/
	}
    redibujar = true;	
}

void CBlenderLite::CursorToSelect(){
	Cursor3DposX = Objects[objSelect].posX;
	Cursor3DposY = Objects[objSelect].posY;
	Cursor3DposZ = Objects[objSelect].posZ;
	redibujar = true;
}

void CBlenderLite::SelectToCursor(){
	Objects[objSelect].posX = Cursor3DposX;
	Objects[objSelect].posY = Cursor3DposY;
	Objects[objSelect].posZ = Cursor3DposZ;
	redibujar = true;
}

void CBlenderLite::AddObject( TInt tipo ){
	//Cancelar();
	Object obj;
	obj.type = tipo;
	obj.visible = true;
	obj.posX = Cursor3DposX;
	obj.posY = Cursor3DposY;
	obj.posZ = Cursor3DposZ;
	obj.rotX = obj.rotY = obj.rotZ = 0;
	obj.scaleX = obj.scaleY = obj.scaleZ = 45000;
	obj.Id = -0;
	Objects.Append(obj);
	Collection.Append(Objects.Count()-1);
	if (tipo == light){
		Light tempLight;
		tempLight.type = pointLight;
		tempLight.lightId = nextLightId;
		tempLight.color[0] = 1.0;
		tempLight.color[1] = 1.0;
		tempLight.color[2] = 1.0;
		tempLight.color[3] = 1.0;
		//tempLight.color  = { MATERIALCOLOR(1.0, 1.0, 1.0, 1.0) };

		glEnable( nextLightId );		
		GLfloat lightDiffuseSpot[4]   = { LIGHTCOLOR(1.0, 1.0, 1.0, 1.0) };
		GLfloat lightSpecularSpot[4]  = { LIGHTCOLOR(1.0, 1.0, 1.0, 1.0) };
		glLightfv(  nextLightId, GL_DIFFUSE,  lightDiffuseSpot  );
		glLightfv(  nextLightId, GL_AMBIENT,  lightAmbient  );
		glLightfv(  nextLightId, GL_SPECULAR, lightSpecularSpot );
		//glLightfv(  nextLightId, GL_POSITION, lightPositionSpot );
		glLightfv(  nextLightId, GL_POSITION, positionPuntualLight );
		

		glLightf(   nextLightId, GL_CONSTANT_ATTENUATION,  1.5  );
		glLightf(   nextLightId, GL_LINEAR_ATTENUATION,    0.5  );
		glLightf(   nextLightId, GL_QUADRATIC_ATTENUATION, 0.5  );

		/*glLightf(   nextLightId, GL_SPOT_CUTOFF,   170.0                );
		glLightf(   nextLightId, GL_SPOT_EXPONENT,  20.0                );
		glLightfv(  nextLightId, GL_SPOT_DIRECTION, lightDirectionSpot );*/
		nextLightId++;

		Lights.Append(tempLight);
		obj.Id = Lights.Count()-1;
	}
	objSelect = Objects.Count()-1;
    redibujar = true;
}

void CBlenderLite::AddMesh( int modelo ){
	//Cancelar();
	//creamos la mesh primero
	Object obj;	
	obj.type = mesh;
	obj.visible = true;
	obj.posX = Cursor3DposX;
	obj.posY = Cursor3DposY;
	obj.posZ = Cursor3DposZ;
	obj.rotX = obj.rotY = obj.rotZ = 0;
	obj.scaleX = obj.scaleY = obj.scaleZ = 45000;
	obj.Id = 0;
	
	Mesh tempMesh;
	tempMesh.materialsSize = 1;
	tempMesh.facesGroupsSize = new TInt[tempMesh.materialsSize];
	if (modelo == cubo){ 
    	tempMesh.vertexSize = 24;
		tempMesh.vertex = new GLshort[tempMesh.vertexSize*3];
		tempMesh.vertexColor = new GLubyte[tempMesh.vertexSize*4];
		tempMesh.normals = new GLbyte[tempMesh.vertexSize*3];
		tempMesh.uv = new GLfloat[tempMesh.vertexSize*2];

		for (int i = 0; i < tempMesh.vertexSize*3; i++) {
			tempMesh.vertex[i] = CuboVertices[i];
			tempMesh.normals[i] = CuboNormals[i];
		}
		for (int i = 0; i < tempMesh.vertexSize*4; i++) {
			tempMesh.vertexColor[i] = 255;
		}
		for (int i = 0; i < tempMesh.vertexSize*2; i++) {
			//tempMesh.uv[i] = (GLfloat)((CuboUV[i]+128)/255)*1280;
			tempMesh.uv[i] = (GLfloat)CuboUV[i];
		}

		tempMesh.facesGroupsSize[0] = 12;
		tempMesh.faces = new GLushort*[tempMesh.materialsSize];
		tempMesh.faces[0] = new GLushort[tempMesh.facesGroupsSize[0]*3];
		for (int i = 0; i < tempMesh.facesGroupsSize[0]*3; i++) {
			tempMesh.faces[0][i] = CuboTriangles[i];
		}
	}	
	else if (modelo == monkey){  
		obj.rotZ = 180;
    	tempMesh.vertexSize = MonkeyVertexSize;

		tempMesh.vertex = new GLshort[MonkeyVertexSize*3];
		tempMesh.normals = new GLbyte[MonkeyVertexSize*3];		
		for(int a=0; a < MonkeyVertexSize*3; a++){
			tempMesh.vertex[a] = MonkeyVertex[a];	
			tempMesh.normals[a] = MonkeyNormal[a];	
		}

		tempMesh.vertexColor = new GLubyte[MonkeyVertexSize*4];
		for(int a=0; a < MonkeyVertexSize*4; a++){
			tempMesh.vertexColor[a] = 255;
		}	
		
		tempMesh.facesGroupsSize[0] = MonkeyFaceSize;
		tempMesh.faces = new GLushort*[tempMesh.materialsSize];
		tempMesh.faces[0] = new GLushort[tempMesh.facesGroupsSize[0]*3];
		for(int a=0; a < tempMesh.facesGroupsSize[0]*3; a++){
			tempMesh.faces[0][a] = MonkeyFace[a];	
		}
		//tempMesh.edges = new GLushort[tempMesh.edgesSize];
		tempMesh.uv = new GLfloat[tempMesh.vertexSize*2];
		for(int a=0; a < tempMesh.vertexSize*2; a++){
			//tempMesh.uv[a] = (GLfloat)(MonkeyUV+128)[a]/255;	
			tempMesh.uv[a] = (GLfloat)MonkeyUV[a];			
		}
	}
	else {
		return;
	}
	tempMesh.smooth = true;
	tempMesh.culling = true;
	tempMesh.materials = new TInt[1];
	tempMesh.materials[0] = 0;
	tempMesh.vertexGroupUI = NULL;
	Meshes.Append(tempMesh);	

	//creamos el objeto y le asignamos la mesh
    
	//Objects[objSelect].RecalcularBordes();
	
	obj.Id = Meshes.Count()-1;
	Meshes[obj.Id].AgruparVertices();
	Objects.Append(obj);	
	Collection.Append(Objects.Count()-1);
	objSelect = Objects.Count()-1;
    redibujar = true;
}

void CBlenderLite::Extruir(){
	/*if (estado == edicion && Objects[objSelect].vertexGroupSize > 0){
		Object& obj = Objects[objSelect];
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
	}	*/
}

void CBlenderLite::ActivarTextura(){
	//si no hay objetos
	if (Objects.Count() < 1){return;}	
	Object& obj = Objects[objSelect];
	//si no es un mesh
	if (obj.type != mesh){return;}	
	Mesh& pMesh = Meshes[obj.Id];

	Cancelar();
	TInt MaterialID = 1;
	HBufC* noteBuf = HBufC::NewLC(100);
	if (pMesh.materialsSize > 1){
		_LIT(KFormatString, "Material 1 to %d");
		noteBuf->Des().Format(KFormatString, pMesh.materialsSize);
		MaterialID = DialogNumber(1, 1, pMesh.materialsSize, noteBuf);		
	}
	Material& mat = Materials[pMesh.materials[MaterialID-1]];

	Cancelar();
	//activa o desactiva las texturas
	noteBuf->Des().Copy(_L("Activar Textura?"));
	if (DialogAlert(noteBuf)){	
		mat.textura = true;
		
	}
	else {
		mat.textura = false;	
	}
	CleanupStack::PopAndDestroy(noteBuf);	
    redibujar = true;
}

void CBlenderLite::SetTransparencia(){
	//si no hay objetos
	if (Objects.Count() < 1){return;}	
	Object& obj = Objects[objSelect];
	//si no es un mesh
	if (obj.type != mesh){return;}	
	Mesh& pMesh = Meshes[obj.Id];

	Cancelar();
	TInt MaterialID = 1;
	HBufC* noteBuf = HBufC::NewLC(100);
	if (pMesh.materialsSize > 1){
		_LIT(KFormatString, "Material 1 to %d");
		noteBuf->Des().Format(KFormatString, pMesh.materialsSize);
		MaterialID = DialogNumber(1, 1, pMesh.materialsSize, noteBuf);		
	}

	Material& mat = Materials[pMesh.materials[MaterialID-1]];

	Cancelar();
	//activa o desactiva las Transparencias
	noteBuf->Des().Copy(_L("Activar Transparencia?"));
	if (DialogAlert(noteBuf)){	
		mat.transparent = true;
	}
	else {
		mat.transparent = false;	
	}
	CleanupStack::PopAndDestroy(noteBuf);	
    redibujar = true;
}

void CBlenderLite::SetTextureRepeat(){
	//si no hay objetos
	if (Objects.Count() < 1){return;}	
	Object& obj = Objects[objSelect];
	//si no es un mesh
	if (obj.type != mesh){return;}	
	Mesh& pMesh = Meshes[obj.Id];

	Cancelar();
	TInt MaterialID = 1;
	HBufC* noteBuf = HBufC::NewLC(100);
	if (pMesh.materialsSize > 1){
		_LIT(KFormatString, "Material 1 to %d");
		noteBuf->Des().Format(KFormatString, pMesh.materialsSize);
		MaterialID = DialogNumber(1, 1, pMesh.materialsSize, noteBuf);		
	}

	Material& mat = Materials[pMesh.materials[MaterialID-1]];

	Cancelar();
	//activa o desactiva las Transparencias
	noteBuf->Des().Copy(_L("Activar Repeticion de Textura?"));
	if (DialogAlert(noteBuf)){	
		mat.repeat = true;
	}
	else {
		mat.repeat = false;	
	}
	CleanupStack::PopAndDestroy(noteBuf);	
    redibujar = true;
}

void CBlenderLite::SetSmooth(){
	//si no hay objetos
	if (Objects.Count() < 1){return;}	
	Object& obj = Objects[objSelect];
	//si no es un mesh
	if (obj.type != mesh){return;}	
	Mesh& pMesh = Meshes[obj.Id];

	Cancelar();
	//activa o desactiva las Transparencias
	HBufC* buf = HBufC::NewLC( 22 );
	buf->Des().Copy(_L("Activar Smooth?"));
	if (DialogAlert(buf)){	
		pMesh.smooth = true;
	}
	else {
		pMesh.smooth = false;	
	}
    redibujar = true;
}

void CBlenderLite::SetCulling(){
	//si no hay objetos
	if (Objects.Count() < 1){return;}	
	Object& obj = Objects[objSelect];
	//si no es un mesh
	if (obj.type != mesh){return;}	
	Mesh& pMesh = Meshes[obj.Id];

	Cancelar();
	//activa o desactiva las Transparencias
	HBufC* noteBuf = HBufC::NewLC( 22 );
	noteBuf->Des().Copy(_L("Activar Culling?"));
	if (DialogAlert(noteBuf)){	
		pMesh.culling = true;
	}
	else {
		pMesh.culling = false;	
	}
	CleanupStack::PopAndDestroy(noteBuf);	
    redibujar = true;
}

void CBlenderLite::SetLighting(){
	//si no hay objetos
	if (Objects.Count() < 1){return;}	
	Object& obj = Objects[objSelect];
	//si no es un mesh
	if (obj.type != mesh){return;}	
	Mesh& pMesh = Meshes[obj.Id];

	TInt MaterialID = 1;
	HBufC* noteBuf = HBufC::NewLC(100);
	if (pMesh.materialsSize > 1){
		_LIT(KFormatString, "Material 1 to %d");
		noteBuf->Des().Format(KFormatString, pMesh.materialsSize);
		MaterialID = DialogNumber(1, 1, pMesh.materialsSize, noteBuf);	
	}
	Material& mat = Materials[pMesh.materials[MaterialID-1]];

	Cancelar();
	//activa o desactiva las Transparencias
	noteBuf->Des().Copy(_L("Activar Iluminacion?"));
	if (DialogAlert(noteBuf)){	
		mat.lighting = true;
	}
	else {
		mat.lighting = false;	
	}
	CleanupStack::PopAndDestroy(noteBuf);	
    redibujar = true;
}

void CBlenderLite::SetVertexColor(){
	//si no hay objetos
	if (Objects.Count() < 1){return;}	
	Object& obj = Objects[objSelect];
	//si no es un mesh
	if (obj.type != mesh){return;}	
	Mesh& pMesh = Meshes[obj.Id];

	TInt MaterialID = 1;
	HBufC* noteBuf = HBufC::NewLC(100);
	if (pMesh.materialsSize > 1){
		_LIT(KFormatString, "Material 1 to %d");
		noteBuf->Des().Format(KFormatString, pMesh.materialsSize);
		MaterialID = DialogNumber(1, 1, pMesh.materialsSize, noteBuf);	
	}
	Material& mat = Materials[pMesh.materials[MaterialID-1]];

	Cancelar();
	//activa o desactiva las Transparencias
	noteBuf->Des().Copy(_L("Activar Vertex Color?"));
	if (DialogAlert(noteBuf)){	
		mat.color = true;
	}
	else {
		mat.color = false;	
	}
	CleanupStack::PopAndDestroy(noteBuf);	
    redibujar = true;
}

void CBlenderLite::SetInterpolation(){
	//si no hay objetos
	if (Objects.Count() < 1){return;}	
	Object& obj = Objects[objSelect];
	//si no es un mesh
	if (obj.type != mesh){return;}	
	Mesh& pMesh = Meshes[obj.Id];

	TInt MaterialID = 1;
	HBufC* noteBuf = HBufC::NewLC(100);
	if (pMesh.materialsSize > 1){
		_LIT(KFormatString, "Material 1 to %d");
		noteBuf->Des().Format(KFormatString, pMesh.materialsSize);
		MaterialID = DialogNumber(1, 1, pMesh.materialsSize, noteBuf);
	}
	Material& mat = Materials[pMesh.materials[MaterialID-1]];

	Cancelar();
	//activa o desactiva las Transparencias
	noteBuf->Des().Copy(_L("Interpolacion Lineal?"));
	if (DialogAlert(noteBuf)){	
		mat.interpolacion = lineal;
	}
	else {
		mat.interpolacion = closest;	
	}
	CleanupStack::PopAndDestroy(noteBuf);	
    redibujar = true;
}

void CBlenderLite::SetTexture(){
	//si solo estan las texturas de blender
	if (Textures.Count() < NumTexturasBlendersito+1){		
		HBufC* noteBuf = HBufC::NewLC(100);	
		noteBuf->Des().Copy(_L("No hay texturas cargadas"));	
		MensajeError(noteBuf);
		CleanupStack::PopAndDestroy(noteBuf);
		return;
	}	
	//si no hay objetos
	if (Objects.Count() < 1){return;}	
	Object& obj = Objects[objSelect];
	//si no es un mesh
	if (obj.type != mesh){return;}	
	Mesh& pMesh = Meshes[obj.Id];

	Cancelar();
	TInt MaterialID = 1;
	HBufC* noteBuf = HBufC::NewLC(100);
	if (pMesh.materialsSize > 1){
		_LIT(KFormatString, "Material 1 to %d");
		noteBuf->Des().Format(KFormatString, pMesh.materialsSize);
		MaterialID = DialogNumber(1, 1, pMesh.materialsSize, noteBuf);		
	}

	Material& mat = Materials[pMesh.materials[MaterialID-1]];

	_LIT(KFormatString2, "Select Texture 1 to %d");
	noteBuf->Des().Format(KFormatString2, Textures.Count()-NumTexturasBlendersito);
	TInt textureID = DialogNumber(1, 1, Textures.Count()-NumTexturasBlendersito, noteBuf);
	CleanupStack::PopAndDestroy(noteBuf);
	
	mat.textura = true;
	mat.textureID = textureID+NumTexturasBlendersito;
	//mat.textureID = Textures[textureID-5].iID;
    redibujar = true;
}

void CBlenderLite::SetMaterial(){
	//si no hay objetos
	if (Objects.Count() < 1){return;}	
	Object& obj = Objects[objSelect];
	//si no es un mesh
	if (obj.type != mesh){return;}	
	Mesh& pMesh = Meshes[obj.Id];

	Cancelar();
	HBufC* noteBuf = HBufC::NewLC(100);
	_LIT(KFormatString, "Old Material 1 to %d");
	noteBuf->Des().Format(KFormatString, pMesh.materialsSize);
	TInt OldMaterialID = DialogNumber(1, 1, pMesh.materialsSize, noteBuf);
	
	if (OldMaterialID > pMesh.materialsSize || 1 > OldMaterialID){		
		noteBuf->Des().Copy(_L("Indice invalido"));	
		MensajeError(noteBuf);
	}
	else {
		_LIT(KFormatString, "New Material 1 to %d");
		noteBuf->Des().Format(KFormatString, Materials.Count());
		TInt MaterialID = DialogNumber(1, 1, Materials.Count(), noteBuf);
		
		if (MaterialID > Materials.Count() || 1 > MaterialID){		
			noteBuf->Des().Copy(_L("Indice invalido"));	
			MensajeError(noteBuf);
		}
		else {
			pMesh.materials[OldMaterialID-1] = MaterialID-1;
		}
	}
	CleanupStack::PopAndDestroy(noteBuf);
    redibujar = true;
}

void CBlenderLite::SetEditMode(){
	//si no hay objetos
	if (Objects.Count() < 1){return;}	
	Object& obj = Objects[objSelect];
	//si no es un mesh
	if (obj.type != mesh){return;}	
	
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

void CBlenderLite::EnfocarObject(){
	//si no hay objetos
	if (Objects.Count() < 1){return;}
    redibujar = true;
	PivotX = -Objects[objSelect].posX;
	PivotY = -Objects[objSelect].posY;
	PivotZ = -Objects[objSelect].posZ;
}


void CBlenderLite::DuplicatedObject(){	
	//si no hay objetos
	if (Objects.Count() < 1){return;}	
	Object& obj = Objects[objSelect];
	//si no es un mesh
	if (obj.type == mesh){
		Mesh& tempMesh = Meshes[obj.Id];
		Meshes.Append(tempMesh);
		Mesh& tempMesh2 = Meshes[Meshes.Count()-1];

		Objects.Append(obj);
		objSelect = Objects.Count()-1;
		Collection.Append(objSelect);
		Objects[objSelect].Id = Meshes.Count()-1;

		//los punteros apuntan a la misma memoria que el mesh original. hay que cambiarlo
		tempMesh2.vertex = new GLshort[tempMesh.vertexSize*3];
		tempMesh2.normals = new GLbyte[tempMesh.vertexSize*3];
		tempMesh2.vertexColor = new GLubyte[tempMesh.vertexSize*4];
		//tempMesh2.uv = new GLbyte[tempMesh.vertexSize*2];
		tempMesh2.uv = new GLfloat[tempMesh.vertexSize*2];
		tempMesh2.materials = new TInt[tempMesh.materialsSize];
		tempMesh2.faces = new GLushort*[tempMesh.materialsSize];
		
		tempMesh2.facesGroupsSize = new TInt[tempMesh2.materialsSize];
		for(int m=0; m < tempMesh2.materialsSize; m++){		
			tempMesh2.materials[m] = tempMesh.materials[m];
			tempMesh2.facesGroupsSize[m] = tempMesh.facesGroupsSize[m];   
			tempMesh2.faces[m] = new GLushort[tempMesh2.facesGroupsSize[m]*3];
			for(int f=0; f < tempMesh2.facesGroupsSize[m]*3; f++){		
				tempMesh2.faces[m][f] = tempMesh.faces[m][f];	
			}
		}
		
		for(TInt a=0; a < tempMesh2.vertexSize*3; a++){
			tempMesh2.vertex[a] = tempMesh.vertex[a];
			tempMesh2.normals[a] = tempMesh.normals[a];
		}		
		for(TInt a=0; a < tempMesh2.vertexSize*4; a++){
			tempMesh2.vertexColor[a] = tempMesh.vertexColor[a];
		}
		for(TInt a=0; a < tempMesh2.vertexSize*2; a++){
			tempMesh2.uv[a] = tempMesh.uv[a];
		}
	}
	else {
		Objects.Append(obj);	
		objSelect = Objects.Count()-1;
		Collection.Append(objSelect);
	}

    redibujar = true;
}

void CBlenderLite::DuplicatedLinked(){
	//si no hay objetos
	if (Objects.Count() < 1){return;}	
	Object& obj = Objects[objSelect];
	Objects.Append(obj);	
	objSelect = Objects.Count()-1;
	Collection.Append(objSelect);
    redibujar = true;
}

void CBlenderLite::SetSpecular(){	
	//si no hay objetos
	if (Objects.Count() < 1){return;}	
	Object& obj = Objects[objSelect];
	//si no es un mesh
	if (obj.type != mesh){return;}	
	Mesh& pMesh = Meshes[obj.Id];

	Cancelar();
	TInt MaterialID = 1;
	HBufC* noteBuf = HBufC::NewLC(100);
	if (pMesh.materialsSize > 1){
		_LIT(KFormatString, "Material 1 to %d");
		noteBuf->Des().Format(KFormatString, pMesh.materialsSize);
		MaterialID = DialogNumber(1, 1, pMesh.materialsSize, noteBuf);		
	}
	Material& mat = Materials[pMesh.materials[MaterialID-1]];

	noteBuf->Des().Copy(_L("Valor Specular (0 - 100)"));
	TInt valor = DialogNumber((TInt)(mat.specular[0]*100.f), 0, 100, noteBuf);
	GLfloat resultado = valor/100.0f;

	mat.specular[0] = resultado;
	mat.specular[1] = resultado;
	mat.specular[2] = resultado;
	mat.specular[3] = resultado;
	CleanupStack::PopAndDestroy(noteBuf);
    redibujar = true;
}

void CBlenderLite::SetEmission(){
	//si no hay objetos
	if (Objects.Count() < 1){return;}	
	Object& obj = Objects[objSelect];
	//si no es un mesh
	if (obj.type != mesh){return;}	
	Mesh& pMesh = Meshes[obj.Id];

	Cancelar();
	TInt MaterialID = 1;
	HBufC* noteBuf = HBufC::NewLC(100);
	if (pMesh.materialsSize > 1){
		_LIT(KFormatString, "Material 1 to %d");
		noteBuf->Des().Format(KFormatString, pMesh.materialsSize);
		MaterialID = DialogNumber(1, 1, pMesh.materialsSize, noteBuf);		
	}
	Material& mat = Materials[pMesh.materials[MaterialID-1]];
	
	noteBuf->Des().Copy(_L("Emission Roja (0 - 100)"));
	TInt valor = DialogNumber((TInt)(mat.emission[0]*100.f), 0, 100, noteBuf);
	mat.emission[0] = valor/100.0f;
    redibujar = true;

	noteBuf->Des().Copy(_L("Emission Verde (0 - 100)"));
    valor = DialogNumber((TInt)(mat.emission[1]*100.f), 0, 100, noteBuf);
    mat.emission[1] = valor/100.0f;
	redibujar = true;	

	noteBuf->Des().Copy(_L("Emission Verde (0 - 100)"));
    valor = DialogNumber((TInt)(mat.emission[2]*100.f), 0, 100, noteBuf);
    mat.emission[2] = valor/100.0f;
	CleanupStack::PopAndDestroy(noteBuf);
	redibujar = true;
}

void CBlenderLite::SetAmbientLight(){
	HBufC* buf = HBufC::NewLC( 25 );
	buf->Des().Copy(_L("Rojo (0 - 100)"));
	TInt valorR = DialogNumber((TInt)(AmbientRender[0]*100.f), 0, 100, buf);
	AmbientRender[0] = (GLfloat)valorR/100.0f;
    redibujar = true;	
	buf->Des().Copy(_L("Verde (0 - 100)"));
	TInt valorG = DialogNumber((TInt)(AmbientRender[1]*100.f), 0, 100, buf);
	AmbientRender[1] = (GLfloat)valorG/100.0f;
    redibujar = true;	
	buf->Des().Copy(_L("Azul (0 - 100)"));
	TInt valorB = DialogNumber((TInt)(AmbientRender[2]*100.f), 0, 100, buf);
	AmbientRender[2] = (GLfloat)valorB/100.0f;
	CleanupStack::PopAndDestroy(buf);
	if (view == Rendered){
		glLightModelfv( GL_LIGHT_MODEL_AMBIENT, AmbientRender );
	}
    redibujar = true;
}

void CBlenderLite::SetDiffuse(){
	//si no hay objetos
	if (Objects.Count() < 1){return;}	
	Object& obj = Objects[objSelect];
	//si no es un mesh
	if (obj.type != mesh){return;}	
	Mesh& pMesh = Meshes[obj.Id];

	Cancelar();
	TInt MaterialID = 1;
	HBufC* noteBuf = HBufC::NewLC(100);
	if (pMesh.materialsSize > 1){
		_LIT(KFormatString, "Material 1 to %d");
		noteBuf->Des().Format(KFormatString, pMesh.materialsSize);
		MaterialID = DialogNumber(1, 1, pMesh.materialsSize, noteBuf);		
	}
	Material& mat = Materials[pMesh.materials[MaterialID-1]];

	noteBuf->Des().Copy(_L("Rojo (0 - 100)"));
	TInt valor = DialogNumber((TInt)(mat.diffuse[0]*100.f), 0, 100, noteBuf);
	mat.diffuse[0] = (GLfloat)valor/100.0f;
    redibujar = true;	
	noteBuf->Des().Copy(_L("Verde (0 - 100)"));
	valor = DialogNumber((TInt)(mat.diffuse[1]*100.f), 0, 100, noteBuf);
	mat.diffuse[1] = (GLfloat)valor/100.0f;
    redibujar = true;	
	noteBuf->Des().Copy(_L("Azul (0 - 100)"));
	valor = DialogNumber((TInt)(mat.diffuse[2]*100.f), 0, 100, noteBuf);
	mat.diffuse[2] = (GLfloat)valor/100.0f;
    redibujar = true;	
	noteBuf->Des().Copy(_L("Alpha (0 - 100)"));	
	valor = DialogNumber((TInt)(mat.diffuse[3]*100.f), 0, 100, noteBuf);
	//setea la transparencia deacuerdo al alpha
	if (valor < 100){mat.transparent = true;}
	else {mat.transparent = false;}
	mat.diffuse[3] = (GLfloat)valor/100.0f;
	
	CleanupStack::PopAndDestroy(noteBuf);
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
	/*Mesh& obj = Objects[objSelect];
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
			TempVertexGroup[a+obj.vertexGroupSize] = obj.vertexSize/3+obj.vertexGroup[a];*/
			/*for(TInt s=0; s < obj.vertexGroupIndiceSize[a]; s++){
				TempVertexGroupIndices[a][s] = obj.vertexGroupIndice[a][s];
				TempVertexGroupIndices[a+obj.vertexGroupSize][s] = obj.vertexSize/3+obj.vertexGroupIndice[a][s];			
			}*/
		/*}
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
		TempVertexGroupIndices[3][0] = 44;*/
		/*HBufC* noteBuf = HBufC::NewLC(30); //TInt::Length(obj.vertexGroupSize)
		_LIT(KFormatString, "indices %d %d %d %d");
		noteBuf->Des().Format(KFormatString,  
				TempVertexGroupIndices[0][0],
				TempVertexGroupIndices[1][0],
				TempVertexGroupIndices[2][0],
				TempVertexGroupIndices[3][0]);
		Mensaje(noteBuf);*/
		//libera memoria
		/*delete[] TempVertex;
		delete[] TempNormals;
		delete[] TempEdges;
		delete[] TempUv;
		delete[] TempVertexGroup;
		delete[] TempVertexGroupIndicesSize;
		delete[] TempVertexGroupIndices;*/
		//CleanupStack::PopAndDestroy(noteBuf);
	//}
	/*redibujar = true;*/
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
	HBufC* noteBuf = HBufC::NewLC(100);	
	_LIT(KFormatString, "Posicion: x: %d, y: %d, z: %d");
	noteBuf->Des().Format(KFormatString, Objects[objSelect].posX, Objects[objSelect].posY, Objects[objSelect].posZ);
	DialogAlert(noteBuf);	
	
	_LIT(KFormatString2, "Rotacion: x: %d, y: %d, z: %d");
	noteBuf->Des().Format(KFormatString2, Objects[objSelect].rotX, Objects[objSelect].rotY, Objects[objSelect].rotZ);
	DialogAlert(noteBuf);	

	_LIT(KFormatString3, "Escala: x: %d, y: %d, z: %d");
	noteBuf->Des().Format(KFormatString3, Objects[objSelect].scaleX, Objects[objSelect].scaleY, Objects[objSelect].scaleZ);
	DialogAlert(noteBuf);

	CleanupStack::PopAndDestroy(noteBuf);	
	/*if (opciones == 1){
		HBufC* noteBuf = HBufC::NewLC(30); //TInt::Length(obj.vertexGroupSize)
		_LIT(KFormatString, "size %d Indices %d");
		noteBuf->Des().Format(KFormatString,  Objects[objSelect].vertexGroupIndiceSize[EditSelect],
				              Objects[objSelect].vertexGroupIndice[EditSelect][0]);
				                                                                 //,
				              //Objects[objSelect].vertexGroupIndice[EditSelect][1],
				              //Objects[objSelect].vertexGroupIndice[EditSelect][2]);
		Mensaje(noteBuf);	
		CleanupStack::PopAndDestroy(noteBuf);
	}
	else if (opciones == 1){ //cantidad de vertices
		HBufC* noteBuf = HBufC::NewLC(30); //TInt::Length(obj.vertexGroupSize)
		_LIT(KFormatString, "obj: %d Vertices: %d");
		noteBuf->Des().Format(KFormatString, objSelect+1, Objects[objSelect].vertexSize/3);
		Mensaje(noteBuf);	
		CleanupStack::PopAndDestroy(noteBuf);	
	}	
	else if (opciones == 2){ //cantidad de vertices
		HBufC* noteBuf = HBufC::NewLC(35); //TInt::Length(obj.vertexGroupSize)
		_LIT(KFormatString, "VertexGroup: %d group: %d");
		noteBuf->Des().Format(KFormatString, Objects[objSelect].vertexGroupSize, Objects[objSelect].vertexGroupIndiceSize[EditSelect]);
		Mensaje(noteBuf);	
		CleanupStack::PopAndDestroy(noteBuf);	
	}	*/
}; 


void CBlenderLite::Mensaje(HBufC* noteBuf){	        	
	CAknInformationNote* note = new (ELeave) CAknInformationNote();
	note->ExecuteLD(*noteBuf);
	//CleanupStack::PopAndDestroy(noteBuf);
};

void CBlenderLite::MensajeError(HBufC* noteBuf){
    CAknErrorNote* note = new (ELeave) CAknErrorNote();
    note->ExecuteLD(*noteBuf);
    //CleanupStack::PopAndDestroy(noteBuf);
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

TPtr CBlenderLite::DialogText(HBufC* textBuf, HBufC* noteBuf) {
    TPtr textPtr = textBuf->Des();
    
    CAknTextQueryDialog* dlg = CAknTextQueryDialog::NewL(textPtr);
    dlg->PrepareLC(R_AVKON_DIALOG_QUERY_VALUE_TEXT);
    dlg->SetPromptL(*noteBuf);    
    dlg->RunLD();
	return textPtr;
}

TInt CBlenderLite::ShowOptionsDialogL() {	
	
	///iContainer->ShowProgressNoteUnderSingleProcessL(R_BLENDERLITE_PROGRESS_NOTE, EAknExNoteCtrlIdProgressNote);
    iContainer->ShowWaitNoteL(R_BLENDERLITE_PROGRESS_NOTE, EAknExNoteCtrlIdWaitNote);
    //R_BLENDERLITE_WAIT_NOTE_SOFTKEY_CANCEL r_blenderlite_wait_note_softkey_cancel
	
	TInt blee = 1;
	return blee;
    /*TInt index = 0; // Variable para almacenar el �ndice seleccionado
    CAknListQueryDialog* dialog = new (ELeave) CAknListQueryDialog(&index); // Crear una instancia de CAknListQueryDialog

    const TInt KNumberOfItems = 3;
    CDesC16ArrayFlat* itemTextArray = new (ELeave) CDesC16ArrayFlat(KNumberOfItems);
    CleanupStack::PushL(itemTextArray);

    // Agregar los textos de las opciones al array de texto
    itemTextArray->AppendL(_L("Option 1"));
    itemTextArray->AppendL(_L("Option 2"));
    itemTextArray->AppendL(_L("Option 3"));

    dialog->PrepareLC(R_AVKON_DIALOG_POPUP_LIST); // Preparar el di�logo
    dialog->SetItemTextArray(itemTextArray); // Establecer el array de texto como opciones
    dialog->SetOwnershipType(ELbmOwnsItemArray);

    dialog->RunLD(); // Ejecutar el di�logo

    CleanupStack::PopAndDestroy(itemTextArray); // Limpiar el array de texto

    return index;*/
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

// Función para obtener el directorio raíz de un filePath
TFileName CBlenderLite::GetRootDirectory(const TDesC& aFilePath)
{
    TParse parse;
    parse.Set(aFilePath, NULL, NULL);
    return parse.DriveAndPath();
}

//import
void CBlenderLite::NewTexture(){
	//si no hay objetos
	if (Objects.Count() < 1){return;}	
	Object& obj = Objects[objSelect];
	//si no es un mesh
	if (obj.type != mesh){return;}	
	Mesh& pMesh = Meshes[obj.Id];

	Cancelar();
	TInt MaterialID = 1;
	if (pMesh.materialsSize > 1){
		HBufC* noteBuf = HBufC::NewLC(100);
		_LIT(KFormatString, "Material 1 to %d");
		noteBuf->Des().Format(KFormatString, pMesh.materialsSize);
		MaterialID = DialogNumber(1, 1, pMesh.materialsSize, noteBuf);	
		CleanupStack::PopAndDestroy(noteBuf);	
	}
	Material& mat = Materials[pMesh.materials[MaterialID-1]];

    _LIT(KTitle, "Selecciona la Textura");
    TFileName filePath;
    if (AknCommonDialogs::RunSelectDlgLD(filePath, R_BLENDERLITE_SELECT_DIALOG, KTitle)) {
		
		iTextureManager = CTextureManager::NewL ( iScreenWidth, iScreenHeight,
												FRUSTUM_TOP, FRUSTUM_BOTTOM, FRUSTUM_RIGHT, FRUSTUM_LEFT, FRUSTUM_NEAR,
												this );
		TTexture newTexture;

		// Crear un objeto TParse para analizar la ruta del archivo
        TParse parse;
        parse.Set(filePath, NULL, NULL);

        // Obtener el nombre del archivo seleccionado
        //const TFileName& selectedFileName = parse.Name();
        //const TFileName& selectedFileName = parse.FullName();
        const TFileName& selectedFileName = parse.NameAndExt();
        
		newTexture.iTextureName = selectedFileName;
		
		Textures.Append(newTexture);
		mat.textura = true;
		//mat.textureID = Textures[Textures.Count()-1].iID;
		mat.textureID = Textures.Count();
    	redibujar = true;

		// Agregar la nueva textura a la cola de carga del TextureManager
		TFileName directorioRaiz = GetRootDirectory(filePath);
		iTextureManager->RequestToLoad(newTexture.iTextureName, directorioRaiz, &Textures[Textures.Count()-1], false);
		//iTextureManager->RequestToLoad(KTextureName, &Textures[Textures.Count()-1], false);

		// Iniciar la carga de texturas
		iTextureManager->DoLoadL();
    }
}

void CBlenderLite::ImportOBJ(){    
    _LIT(KTitle, "Import Wavefront (.obj)");
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
		/*if (file){
			_LIT(KFormatString, "Formato no valido");
			HBufC* noteBuf = HBufC::NewLC(100);
			noteBuf->Des().Format(KFormatString, &file);
			MensajeError(noteBuf); 
			return;
		}			*/
		//crea el objeto
		Cancelar();
		Object obj;
		obj.visible = true;
		obj.posX = Cursor3DposX;
		obj.posY = Cursor3DposY;
		obj.posZ = Cursor3DposZ;
		obj.rotX = obj.rotY = obj.rotZ = 0;
		obj.rotX = obj.rotY = obj.rotZ = 0;	
		obj.scaleX = 65000;
		obj.scaleY = 65000;
		obj.scaleZ = 65000;
		obj.Id = 0;		
		obj.type = mesh;
		
		//los obj tienen una lista de normales. a veces las normales se pueden repetir y esa es la logica
		//tambien se puede repetir coordenadas de texturas asi que en vez de tener los uv y normals vertice por vertices.. tienen un listado

		RArray<GLshort> ListVertices;
		RArray<GLubyte> ListColors;
		RArray<GLbyte> ListNormals;
		RArray<GLfloat> ListUVs;
		RArray<TInt> ListCaras;
		RArray<TInt> MaterialesNuevos;
		RArray<TInt> MeshsGroups;
		//TInt materiales = 0;	

		TBool continuarLeyendo = ETrue; // Variable para controlar la lectura del archivo
		TBuf8<2048> buffer;
		TInt pos = 0;
		TInt64 startPos = 0; // Variable para mantener la posición de lectura en el archivo
		TInt fileSize;
		rFile.Size(fileSize);
 
		while (startPos < fileSize) {
			// Leer una línea del archivo desde la posición actual
			err = rFile.Read(startPos, buffer, buffer.MaxLength());
			if (err != KErrNone) {
				//tarde o temprano va a fallar la lectura y va a parar
				// Manejar error al leer
				_LIT(KFormatString, "Error al leer linea");
				HBufC* noteBuf = HBufC::NewLC(100);
				noteBuf->Des().Format(KFormatString);
				MensajeError(noteBuf);
				continuarLeyendo = EFalse; // Salir del bucle
				break;
			}
			// Procesar la línea hasta que no haya más caracteres en buffer
			while ((pos = buffer.Locate('\n')) != KErrNotFound || (pos = buffer.Locate('\r')) != KErrNotFound) {
				TPtrC8 line = buffer.Left(pos);
			
				// Contador para almacenar la cantidad de "strings" separados por espacios
				TInt contador = 0;
				if (line.Length() > 0) {
					if (line.Left(2) == _L8("v ")) {
						contador = 0;		
						ListVertices.ReserveL(ListVertices.Count() +3); // Reservar espacio para los elementos
						ListColors.ReserveL(ListColors.Count() +3); // Reservar espacio para los elementos

						TLex8 lex(line.Mid(2));  // Inicializa TLex con la subcadena a partir del tercer carácter
						// Iterar mientras no se llegue al final del descriptor y se haya alcanzado el límite de 8 strings
						while (!lex.Eos() && contador < 6) {		
							TPtrC8 currentString = lex.NextToken(); // Mostrar el mensaje con el valor actual del "string" y el contador					
							TLex8 testLex(currentString);// Crear un nuevo objeto TLex para la prueba
							
							// Convertir el string en un número TInt
							TReal number = 0.0;
							TInt err = testLex.Val(number, '.');
							if (err == KErrNone && contador < 3) {
								number = number*2000;								
								GLshort glNumber = static_cast<GLshort>(number); // Conversión a GLbyte
								ListVertices.Append(glNumber);
							}
							else if (contador < 3){
								ListVertices.Append(0);
							}							
							else if (contador < 6){
								number = number*255.0;	
								if (number > 255.0){number = 255.0;}	
								GLshort glNumber = static_cast<GLubyte>(number); // Conversión a GLbyte
								ListColors.Append(glNumber);
							}

							// Avanzar al siguiente "string" que no sea espacio en blanco
							lex.SkipSpace();

							// Incrementar el contador para llevar la cuenta de los strings procesados
							contador++;
						}
						while (contador < 6) {		
							if (contador < 3){
								ListVertices.Append(0);
							}							
							else if (contador < 6){
								ListColors.Append(255);
							}
							contador++;
						}
					}
					else if (line.Left(3) == _L8("vn ")) {
						contador = 0;
						ListNormals.ReserveL(ListNormals.Count() +3); // Reservar espacio para los elementos

						TLex8 lex(line.Mid(3));  // Inicializa TLex con la subcadena a partir del tercer carácter
						// Iterar mientras no se llegue al final del descriptor y se haya alcanzado el límite de 8 strings
						while (!lex.Eos() && contador < 3) {							
							TPtrC8 currentString = lex.NextToken(); // Mostrar el mensaje con el valor actual del "string" y el contador
							TLex8 testLex(currentString); // Crear un nuevo objeto TLex para la prueba
							
							TReal number = 0.0;
							TInt err = testLex.Val(number, '.');
							if (err == KErrNone) {
								number = ((number +1)/2)* 255.0 - 128.0;
								if (number > 127.0){number = 127.0;}
								else if (number < -128.0){number = -128.0;}
								GLbyte glNumber = static_cast<GLbyte>(number); // Conversión a GLbyte
								ListNormals.Append(glNumber);
							}
							else if (contador < 3){
								ListNormals.Append(0);
							}
							lex.SkipSpace();
							contador++;
						}				
					}
					else if (line.Left(3) == _L8("vt ")) {					
						contador = 0;
						ListUVs.ReserveL(ListUVs.Count() +2); // Reservar espacio para los elementos

						TLex8 lex(line.Mid(3));  // Inicializa TLex con la subcadena a partir del tercer carácter
						// Iterar mientras no se llegue al final del descriptor y se haya alcanzado el límite de 8 strings
						while (!lex.Eos() && contador < 2) {							
							TPtrC8 currentString = lex.NextToken(); // Mostrar el mensaje con el valor actual del "string" y el contador
							TLex8 testLex(currentString); // Crear un nuevo objeto TLex para la prueba
							
							TReal number = 0.0;
							TInt err = testLex.Val(number, '.');
							if (err == KErrNone) {		
								if (contador == 1) {
									number = -number+1;
								}		
								number = number * 255.0 - 128.0;
								/*if (number > 127.0){number = 127.0;}
								else if (number < -128.0){number = -128.0;}*/
								//GLbyte glNumber = static_cast<GLbyte>(number); // Conversión a GLbyte
								GLfloat glNumber = static_cast<GLfloat>(number); // Conversión a GLbyte
								ListUVs.Append(glNumber);	
							}
							else if (contador < 2){
								ListUVs.Append(0);
							}
							contador++;
							lex.SkipSpace();
						}			
					}
					else if (line.Left(2) == _L8("f ")) {
						contador = 0;
						ListCaras.ReserveL(ListCaras.Count() +9); // Reservar espacio para los elementos
						TInt conTBarras = 0;

						TLex8 lex(line.Mid(2));  // Inicializa TLex con la subcadena a partir del tercer carácter

						// Iterar mientras no se llegue al final del descriptor y se haya alcanzado el límite de 8 strings
						while (!lex.Eos() && contador < 3) {				
							TPtrC8 currentString = lex.NextToken(); // Mostrar el mensaje con el valor actual del "string" y el contador

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
								TPtrC8 token = currentString.Mid(startPos, tokenLength);
								TLex8 testLex(token); // Crear un nuevo objeto TLex para la prueba

								// Convertir el string en un número TInt
								TInt number = 0;
								TInt err = testLex.Val(number);
								if (err == KErrNone && conTBarras < 3) {
									ListCaras.Append(number-1);
								}

								// Actualiza la posición inicial para el próximo token
								startPos += tokenLength + 1; // Suma 1 para omitir la barra diagonal
								conTBarras++;
							}	
							lex.SkipSpace();
							contador++;
						}
						MeshsGroups[MeshsGroups.Count()-1]++;
					}
					else if (line.Left(7) == _L8("usemtl ")) {	
						MaterialesNuevos[MaterialesNuevos.Count()-1] = Materials.Count();
						
						Material mat;	
						mat.specular[0] = mat.specular[1] = mat.specular[2] = mat.specular[3] = 0.3;
						mat.diffuse[0] = mat.diffuse[1] = mat.diffuse[2] = mat.diffuse[3] = 1.0;
						mat.emission[0] = mat.emission[1] = mat.emission[2] = mat.emission[3] = 0.0;
						mat.textura = false;
						mat.color = false;
						mat.repeat = true;
						mat.lighting = true;
						mat.transparent = false;
						mat.interpolacion = lineal;
						mat.textureID = 0;

						TLex8 lex(line.Mid(7));
						if (!lex.Eos()){
							TPtrC8 currentString = lex.NextToken();	
							mat.name = HBufC::NewL(currentString.Length());
							mat.name->Des().Copy(currentString);
						}
						else {	
							mat.name = HBufC::NewL(100);						
							_LIT(KMatName, "Material.%03d");
							mat.name->Des().Format(KMatName, Materials.Count()+1);
						}

						/*HBufC* noteBuf3 = HBufC::NewLC(180);
						_LIT(KFormatString3, "Material: %d\nNombre: %S");
						noteBuf3->Des().Format(KFormatString3, Materials.Count(), mat.name);
						DialogAlert(noteBuf3);
						CleanupStack::PopAndDestroy(noteBuf3);*/

						Materials.Append(mat);
					}
				}

				// Actualizar la posición de inicio para la próxima lectura
				startPos += pos + 1;

				// Eliminar la parte de la línea ya procesada y el carácter de salto de línea
				buffer.Delete(0, pos + 1);
				buffer.TrimLeft(); // Eliminar espacios en blanco iniciales
			}
			//continuarLeyendo = (buffer.Length() > 0);
		}
		
		// cuantos vertices tiene
		/*HBufC* noteBuf3 = HBufC::NewLC(180);
		_LIT(KFormatString3, "Vertexs: %d Faces: %d Norm: %d UVs: %d, Mat: %d");
		noteBuf3->Des().Format(KFormatString3, tempMesh.vertexSize, tempMesh.facesSize, ListNormals.Count()/3, ListUVs.Count()/2, materiales);
		DialogAlert(noteBuf3);*/

		// Cerrar el archivo
		rFile.Close();
		fsSession.Close();

		//liberar memoria			
		//parece que no ahce falta ya que hago "close". pero no estoy seguro con la pila de limpieza
		//CleanupStack::PopAndDestroy(&rFile);
		//CleanupStack::PopAndDestroy(&fsSession);

		//obj.edges = new GLushort[obj.edgesSize];
		/*for(TInt v=0; v < ListColors.Count()/3; v++){
			HBufC* noteBuf = HBufC::NewLC(180);
			_LIT(KFormatString, "indice: %d, color: %d, %d, %d");
			noteBuf->Des().Format(KFormatString, v+1, ListColors[v*3],ListColors[v*3+1],ListColors[v*3+2]);
			DialogAlert(noteBuf);
		}*/
		
		// Arregla los errores que puede haber
		TInt errores = 0;
		TInt ultimoIndice = 0;

		RArray<GLshort> NewListVertices;
		RArray<GLubyte> NewListColors;
		RArray<GLbyte> NewListNormals;
		RArray<GLfloat> NewListUVs;
		RArray<TInt> NewListCaras;		

		NewListCaras.ReserveL(ListCaras.Count()/3);
		for (TInt m = 0; m < MeshsGroups.Count(); m++) {
			TInt limite = ultimoIndice + MeshsGroups[m];
			//TInt ultimoIndiceinicio = ultimoIndice;
			for (TInt a = ultimoIndice; a < limite; a++) {
				for (TInt f = 0; f < 3; f++) {
					TInt indiceCara = a * 9 + f * 3;
					TInt positionIndex = ListCaras[indiceCara];
					TInt normalIndex = ListCaras[indiceCara + 2];
					TInt uvIndex = ListCaras[indiceCara + 1];

					// Detecta que el vértice del triángulo tiene errores
					if (positionIndex != normalIndex || positionIndex != uvIndex) {
						// Comprobar si ya existe un vértice con estas propiedades
						TInt foundIndex = -1;
						for (TInt i = 0; i < NewListVertices.Count()/3; i++) {
							if (NewListVertices[i * 3] == ListVertices[positionIndex * 3] &&
								NewListVertices[i * 3 + 1] == ListVertices[positionIndex * 3 + 1] &&
								NewListVertices[i * 3 + 2] == ListVertices[positionIndex * 3 + 2] &&
								NewListNormals[i * 3] == ListNormals[normalIndex * 3] &&
								NewListNormals[i * 3 + 1] == ListNormals[normalIndex * 3 + 1] &&
								NewListNormals[i * 3 + 2] == ListNormals[normalIndex * 3 + 2] &&
								NewListUVs[i * 2] == ListUVs[uvIndex * 2] &&
								NewListUVs[i * 2 + 1] == ListUVs[uvIndex * 2 + 1]) {
								foundIndex = i;
								break;
							}
						}
						if (foundIndex == -1) {
							NewListVertices.ReserveL(NewListVertices.Count() +3);
							NewListNormals.ReserveL(NewListNormals.Count() +3);
							NewListColors.ReserveL(NewListColors.Count() +3);
							NewListUVs.ReserveL(NewListUVs.Count() +2);
							// Añadir nueva posición, normal y color
							for (TInt v = 0; v < 3; v++) {
								NewListVertices.Append(ListVertices[positionIndex*3+v]);
								NewListNormals.Append(ListNormals[normalIndex*3+v]);
								NewListColors.Append(ListColors[positionIndex*3+v]);
							}
							// Añadir nuevas UVs
							for (TInt uv = 0; uv < 2; uv++) {
								NewListUVs.Append(ListUVs[uvIndex*2+uv]);
							}
							foundIndex = (NewListVertices.Count()/3)-1;
						}

						NewListCaras.Append(foundIndex);
						errores++;
					}
				}
				ultimoIndice++;
			}
		}
		
		HBufC* noteBuf22 = HBufC::NewLC(180);
		_LIT(KFormatString22, "Num. Vert. Original %d\nNum. Vert. GLES1.1: %d");
		noteBuf22->Des().Format(KFormatString22, ListVertices.Count(), NewListColors.Count());
		DialogAlert(noteBuf22);
		CleanupStack::PopAndDestroy(noteBuf22);	
		
		//ya no hacen falta
		ListVertices.Close();
		ListColors.Close();
		ListNormals.Close();
		ListUVs.Close();	
		ListCaras.Close();		
		//fin del arreglo de vertices
		
		ultimoIndice = 0;
		Mesh tempMesh;
		tempMesh.materialsSize = MeshsGroups.Count();
		tempMesh.materials = new TInt[tempMesh.materialsSize];
		tempMesh.facesGroupsSize = new TInt[tempMesh.materialsSize];
		tempMesh.vertexSize = NewListVertices.Count()/3;
		tempMesh.vertex = new GLshort[tempMesh.vertexSize*3];
		tempMesh.vertexColor = new GLubyte[tempMesh.vertexSize*4];
		tempMesh.normals = new GLbyte[tempMesh.vertexSize*3];
		tempMesh.uv = new GLfloat[tempMesh.vertexSize*2];
		tempMesh.smooth = true;
		tempMesh.culling = true;	
		tempMesh.vertexGroupUI = NULL;		

		//valores defecto
		for (int i = 0; i < tempMesh.vertexSize*3; i++) {
			tempMesh.vertex[i] = NewListVertices[i];
			tempMesh.normals[i] = NewListNormals[i];
		}
		for (int i = 0; i < tempMesh.vertexSize; i++) {
			tempMesh.vertexColor[i*4] = NewListColors[i*3];
			tempMesh.vertexColor[i*4+1] = NewListColors[i*3+1];
			tempMesh.vertexColor[i*4+2] = NewListColors[i*3+2];
			tempMesh.vertexColor[i*4+3] = 255;
		}
		for (int i = 0; i < tempMesh.vertexSize*2; i++) {
			tempMesh.uv[i] = NewListUVs[i];
		}

		tempMesh.faces = new GLushort*[tempMesh.materialsSize];
		for(TInt m=0; m < MeshsGroups.Count(); m++){	
			tempMesh.materials[m] = MaterialesNuevos[m];	
			tempMesh.facesGroupsSize[m] = MeshsGroups[m];
			tempMesh.faces[m] = new GLushort[tempMesh.facesGroupsSize[m]*3];
			
			//valores reales
			TInt limite = ultimoIndice + MeshsGroups[m];			
			TInt ultimoIndiceinicio = ultimoIndice;
			for(TInt a=ultimoIndice; a < limite; a++){			
				for(TInt f=0; f < 3; f++){
					TInt indiceCara = a*3+f;
					TInt indiceTodos = NewListCaras[indiceCara];
					tempMesh.faces[m][(a-ultimoIndiceinicio)*3+f] = indiceTodos;
				}	
				ultimoIndice++;		
			}
		}

		Meshes.Append(tempMesh);
		
		obj.Id = Meshes.Count()-1;
		Objects.Append(obj);
		Collection.Append(Objects.Count()-1);
		objSelect = Objects.Count()-1;
		Meshes[obj.Id].VaciarGrupoVertices();
		//Meshes[obj.Id].AgruparVertices();

		//libero memoria	
		NewListVertices.Close();
		NewListColors.Close();
		NewListNormals.Close();
		NewListUVs.Close();	
		NewListCaras.Close();	

		//para separar las mesh	
		MaterialesNuevos.Close();
		MeshsGroups.Close();
		//Objects[objSelect].RecalcularBordes();

		// Intenta leer el archivo .mtl
		TFileName mtlFile = file;
		mtlFile.Replace(file.Length() - 4, 4, _L(".mtl"));

		RFs fs;
	    User::LeaveIfError(fs.Connect()); // Asegurarse de que fs se conecta correctamente
	    CleanupClosePushL(fs);
	    
		TEntry entry;
		err = fs.Entry(mtlFile, entry);

		//si el archivo existe. no tendria que marcar error
		if (err == KErrNone) {
			TRAP(err, LeerMTL(mtlFile));
			//si ocurrio algun error al leerlo
			if (err != KErrNone) {
				_LIT(KFormatString, "Error al leer el archivo .mtl");
				HBufC* noteBuf = HBufC::NewLC(100);
				noteBuf->Des().Format(KFormatString);
				MensajeError(noteBuf);
				CleanupStack::PopAndDestroy(noteBuf);
			}
		} else {
            // El archivo no existe, manejar el error
            _LIT(KFileNotFound, "El archivo .mtl no existe");
            HBufC* noteBuf = HBufC::NewLC(100);
            noteBuf->Des().Format(KFileNotFound);
            MensajeError(noteBuf);
            CleanupStack::PopAndDestroy(noteBuf);
        }
		fs.Close();

		redibujar = true;
    }	
    else {		
    	_LIT(KFormatString, "Error al leer el Archivo");
		HBufC* noteBuf = HBufC::NewLC(24);
		noteBuf->Des().Format(KFormatString);
		MensajeError(noteBuf);
        CleanupStack::PopAndDestroy(noteBuf);
    }
};

void CBlenderLite::LeerMTL(const TFileName& aFile) {
	RFs fsSession2;	
	User::LeaveIfError(fsSession2.Connect());

	RFile rFile;
	TInt err;
	
	TRAP(err,rFile.Open(fsSession2, aFile, EFileRead));
	if (err != KErrNone){
		_LIT(KFormatString, "Error al abrir: %S");
        HBufC* noteBuf = HBufC::NewLC(aFile.Length() + 16);
        noteBuf->Des().Format(KFormatString, &aFile);
        MensajeError(noteBuf);
        return;
    }

	TBuf8<2048> buffer;
	TInt pos = 0;
	TInt64 startPos = 0; // Variable para mantener la posición de lectura en el archivo
	TInt fileSize;
	rFile.Size(fileSize);

	//necesario para modificar el material correcto	
	Object& obj = Objects[objSelect];
	Mesh& pMesh = Meshes[obj.Id];
	Material* mat = NULL; 
	HBufC* materialName16 = HBufC::NewLC(180);
	HBufC* noteBuf3 = HBufC::NewLC(180);
	TBool encontrado = false;

	while (startPos < fileSize) {
		// Leer una línea del archivo desde la posición actual
		err = rFile.Read(startPos, buffer, buffer.MaxLength());
		if (err != KErrNone) {
			_LIT(KFormatString, "Error al leer linea");
			HBufC* noteBuf = HBufC::NewLC(100);
			noteBuf->Des().Format(KFormatString);
			MensajeError(noteBuf);
    		rFile.Close();	
    		fsSession2.Close();	
            //CleanupStack::PopAndDestroy(&fsSession);
			break;
		}        

        while ((pos = buffer.Locate('\n')) != KErrNotFound || (pos = buffer.Locate('\r')) != KErrNotFound) {
			//TInt indice = 0;
            TPtrC8 line = buffer.Left(pos);

            if (line.Length() > 0) {
                if (line.Left(7) == _L8("newmtl ")) {
                    TLex8 lex(line.Mid(7));
                    TPtrC8 materialName = lex.NextToken();

					// Convertir materialName de TPtrC8 (8 bits) a HBufC (16 bits)
					//delete materialName16;
					materialName16 = CnvUtfConverter::ConvertToUnicodeFromUtf8L(materialName);

					/*_LIT(KFormatString3, "newmtl %S");
					noteBuf3->Des().Format(KFormatString3, materialName16);
					DialogAlert(noteBuf3);*/

					//buscar el material con el mismo nombre
					encontrado = false;
					for(int f=0; f < pMesh.materialsSize; f++){
						//no se puede usar el material 0. ese el que es por defecto y no se toca por mas que se llame igual
						if (Materials[pMesh.materials[f]].name->Compare(*materialName16) == 0 && pMesh.materials[f] != 0){
							mat = &Materials[pMesh.materials[f]];
							/*_LIT(KFormatString3, "newmtl %S encontrado\nMaterial: %d");
							noteBuf3->Des().Format(KFormatString3, materialName16, pMesh.materials[f]+1);
							DialogAlert(noteBuf3);*/
							encontrado = true;
							break;
						}
					}	
                } 
				else if (encontrado){
					//specular
					if (line.Left(3) == _L8("Ns ")) {
						TLex8 lex(line.Mid(3));
						TReal nsValue;
						lex.Val(nsValue, '.');
						GLfloat resultado = nsValue/1000.0f;
						mat->specular[0] = resultado;
						mat->specular[1] = resultado;
						mat->specular[2] = resultado;
						mat->specular[3] = resultado;
						
						/*_LIT(KFormatString3, "newmtl %S encontrado\nSpecular: %f");
						noteBuf3->Des().Format(KFormatString3, materialName16, resultado);
						DialogAlert(noteBuf3);*/
					} 
					//difusso, Aquí manejas el color ambiental Ka (kaR, kaG, kaB)			
					else if (line.Left(3) == _L8("Kd ")) {
						TLex8 lex(line.Mid(3));
						TReal kdR, kdG, kdB;
						lex.Val(kdR, '.');
						lex.SkipSpace();
						lex.Val(kdG, '.');
						lex.SkipSpace();
						lex.Val(kdB, '.');

						mat->diffuse[0] = (GLfloat)kdR;
						mat->diffuse[1] = (GLfloat)kdG;
						mat->diffuse[2] = (GLfloat)kdB;
					} 
					/*else if (line.Left(3) == _L8("Ks ")) {
						TLex8 lex(line.Mid(3));
						TReal ksR, ksG, ksB;
						lex.Val(ksR, '.');
						lex.SkipSpace();
						lex.Val(ksG, '.');
						lex.SkipSpace();
						lex.Val(ksB, '.');
						// Aquí manejas el color especular Ks (ksR, ksG, ksB)
					}*/
					// Aquí manejas el color de emisión Ke (keR, keG, keB)
					else if (line.Left(3) == _L8("Ke ")) {
						TLex8 lex(line.Mid(3));
						TReal keR, keG, keB;
						lex.Val(keR, '.');
						lex.SkipSpace();
						lex.Val(keG, '.');
						lex.SkipSpace();
						lex.Val(keB, '.');	
						mat->emission[0] = (GLfloat)keR;
						mat->emission[1] = (GLfloat)keG;
						mat->emission[2] = (GLfloat)keB;
					} 
					/*else if (line.Left(3) == _L8("Ni ")) {
						TLex8 lex(line.Mid(3));
						TReal niValue;
						lex.Val(niValue, '.');
						// Aquí manejas el índice de refracción Ni
					}*/
					//opacidad 
					else if (line.Left(2) == _L8("d ")) {
						TLex8 lex(line.Mid(2));
						TReal dValue;
						lex.Val(dValue, '.');					
						mat->diffuse[3] = (GLfloat)dValue;
						//setea la transparencia deacuerdo al alpha
						if (dValue < 1.f){mat->transparent = true;}
						else {mat->transparent = false;}
					} 
					/*else if (line.Left(6) == _L8("illum ")) {
						TLex8 lex(line.Mid(6));
						TInt illumValue;
						lex.Val(illumValue);
						// Aquí manejas el modelo de iluminación illum
					}*/
					// Aquí manejas la textura difusa map_Kd
					else if (line.Left(7) == _L8("map_Kd ")) {
						TLex8 lex(line.Mid(7));
						TPtrC8 texturePath = lex.NextToken();

						// Convertir la ruta relativa a una ruta absoluta
						HBufC* texturePath16 = HBufC::NewLC(180);
						texturePath16 = CnvUtfConverter::ConvertToUnicodeFromUtf8L(texturePath);
						TParse fileParser;
						fileParser.Set(aFile, NULL, NULL);
						TFileName absolutePath = fileParser.DriveAndPath();
						absolutePath.Append(*texturePath16);

						// Comprobar si la textura existe
						RFs fs;
						fs.Connect();
						//User::LeaveIfError(fs.Connect());
						TEntry entry;
						TInt err = fs.Entry(absolutePath, entry);
						if (err == KErrNone) {						
							// Cargar la textura desde la ruta absoluta
							iTextureManager = CTextureManager::NewL(iScreenWidth, iScreenHeight,
																	FRUSTUM_TOP, FRUSTUM_BOTTOM, FRUSTUM_RIGHT, FRUSTUM_LEFT, FRUSTUM_NEAR,
																	this);
							TTexture newTexture;
							newTexture.iTextureName = *texturePath16;
							Textures.Append(newTexture);
							mat->textura = true;
							mat->textureID = Textures.Count();

							iTextureManager->RequestToLoad(newTexture.iTextureName, fileParser.DriveAndPath(), &Textures[Textures.Count() - 1], false);
							iTextureManager->DoLoadL();
						} else {
							// El archivo no existe, manejar el error
							_LIT(KFileNotFound, "No existe la textura '%S'");
							noteBuf3->Des().Format(KFileNotFound, texturePath16);
							MensajeError(noteBuf3);
						}
						fs.Close();
						CleanupStack::PopAndDestroy(texturePath16);
					}				
				}
            }

            startPos += pos + 1;
            buffer.Delete(0, pos + 1);
            buffer.TrimLeft();
        }
    }
	CleanupStack::PopAndDestroy(materialName16);
	CleanupStack::PopAndDestroy(noteBuf3);
    rFile.Close();	
    fsSession2.Close();	
    //CleanupStack::PopAndDestroy(&fsSession);
}

void CBlenderLite::OldImportOBJ(){    
    _LIT(KTitle, "Old Import Wavefront (.obj)");
    TFileName file(KNullDesC);
    if (AknCommonDialogs::RunSelectDlgLD(file, R_BLENDERLITE_SELECT_DIALOG, KTitle)){		
    	RFs fsSession;	
    	User::LeaveIfError(fsSession.Connect());
    	CleanupClosePushL(fsSession);

		// Revisar la extensión del archivo
		TPtrC extension = file.Right(4);  // Obtiene las últimas 4 letras del nombre del archivo
		if (extension.CompareF(_L(".obj")) != 0) {
			_LIT(KExtensionError, "Error: El archivo seleccionado no tiene la extensión .obj");
			HBufC* noteBuf = HBufC::NewLC(180);
			noteBuf->Des().Format(KExtensionError);
			MensajeError(noteBuf);
			CleanupStack::PopAndDestroy(noteBuf);
			fsSession.Close();
			return;
		}

    	RFile rFile;
    	TInt err;	

    	TRAP(err,rFile.Open(fsSession, file, EFileRead));
		if (err != KErrNone){
			_LIT(KFormatString, "Error al abrir: %S");
			HBufC* noteBuf = HBufC::NewLC(file.Length()+16);
			noteBuf->Des().Format(KFormatString, &file);
			MensajeError(noteBuf);
			CleanupStack::PopAndDestroy(noteBuf); 
			rFile.Close();
			fsSession.Close();
			return;
		}	
		/*if (file){
			_LIT(KFormatString, "Formato no valido");
			HBufC* noteBuf = HBufC::NewLC(100);
			noteBuf->Des().Format(KFormatString, &file);
			MensajeError(noteBuf); 
			return;
		}			*/
		//crea el objeto
		Cancelar();

		TInt64 startPos = 0; // Variable para mantener la posición de lectura en el archivo
		//esto se hace para no cerrar el archivo y por cada nuevo obj encontrado simplemente volvemos a usar leerOBJ con el archivo donde quedo
		//TBool hayMasObjetos;
		TInt objetosCargados = 0;		
		TInt acumuladoVertices = 0;
		TInt acumuladoNormales = 0;
		TInt acumuladoUVs = 0;
		while (LeerOBJ(&fsSession, &rFile, &file, &startPos, &acumuladoVertices, &acumuladoNormales, &acumuladoUVs)) { // && objetosCargados < 1
			objetosCargados++;
		}

		// Cerrar el archivo
		rFile.Close();
		fsSession.Close();

		TFileName mtlFile = file;
		mtlFile.Replace(file.Length() - 4, 4, _L(".mtl"));

		RFs fs;
	    User::LeaveIfError(fs.Connect()); // Asegurarse de que fs se conecta correctamente
	    CleanupClosePushL(fs);
	    
		TEntry entry;
		err = fs.Entry(mtlFile, entry);

		//si el archivo existe. no tendria que marcar error
		if (err == KErrNone) {
			TRAP(err, LeerMTL(mtlFile));
			//si ocurrio algun error al leerlo
			if (err != KErrNone) {
				_LIT(KFormatString, "Error al leer el archivo .mtl");
				HBufC* noteBuf = HBufC::NewLC(100);
				noteBuf->Des().Format(KFormatString);
				MensajeError(noteBuf);
				CleanupStack::PopAndDestroy(noteBuf);
			}
		} else {
            // El archivo no existe, manejar el error
            _LIT(KFileNotFound, "El archivo .mtl no existe");
            HBufC* noteBuf = HBufC::NewLC(100);
            noteBuf->Des().Format(KFileNotFound);
            MensajeError(noteBuf);
            CleanupStack::PopAndDestroy(noteBuf);
        }
		fs.Close();

		redibujar = true;
	}	
    else {
    	_LIT(KFormatString, "Error al leer el Archivo");
		HBufC* noteBuf = HBufC::NewLC(24);
		noteBuf->Des().Format(KFormatString);
		MensajeError(noteBuf);  
    }
}


TBool CBlenderLite::LeerOBJ(RFs* fsSession, RFile* rFile, TFileName* file, TInt64* startPos,
	TInt* acumuladoVertices,
	TInt* acumuladoNormales,
	TInt* acumuladoUVs
	){
	TInt err;
	Object obj;
	TBool NombreEncontrado = false;
	TBool hayMasObjetos = false;
	obj.visible = true;
	obj.posX = obj.posY = obj.posZ = 0;
	obj.rotX = obj.rotY = obj.rotZ = 0;
	obj.rotX = obj.rotY = obj.rotZ = 0;	
	obj.scaleX = obj.scaleY = obj.scaleZ = 65000;
	obj.Id = 0;		
	obj.type = mesh;
	
	//los obj tienen una lista de normales. a veces las normales se pueden repetir y esa es la logica
	//tambien se puede repetir coordenadas de texturas asi que en vez de tener los uv y normals vertice por vertices.. tienen un listado

	RArray<GLshort> ListVertices;
	RArray<GLubyte> ListColors;
	RArray<GLbyte> ListNormals;
	RArray<GLfloat> ListUVs;
	RArray<TInt> ListCaras;
	RArray<TInt> MaterialesNuevos;
	RArray<TInt> MeshsGroups;
	//TInt materiales = 0;	

	TBool continuarLeyendo = true; // Variable para controlar la lectura del archivo
	TBuf8<2048> buffer;
	TInt pos = 0;
	TInt fileSize;
	rFile->Size(fileSize);

	while (continuarLeyendo && *startPos < fileSize ) {
		// Leer una línea del archivo desde la posición actual
		err = rFile->Read(*startPos, buffer, buffer.MaxLength());
		if (err != KErrNone) {
			//tarde o temprano va a fallar la lectura y va a parar
			// Manejar error al leer
			_LIT(KFormatString, "Error al leer linea");
			HBufC* noteBuf = HBufC::NewLC(100);
			noteBuf->Des().Format(KFormatString);
			MensajeError(noteBuf);
			continuarLeyendo = false; // Salir del bucle
			break;
		}
		// Procesar la línea hasta que no haya más caracteres en buffer
		while (continuarLeyendo && (pos = buffer.Locate('\n')) != KErrNotFound || (pos = buffer.Locate('\r')) != KErrNotFound) {
			TPtrC8 line = buffer.Left(pos);
		
			// Contador para almacenar la cantidad de "strings" separados por espacios
			TInt contador = 0;
			if (line.Length() > 0) {
				if (!NombreEncontrado && line.Left(2) == _L8("o ")) {
					//evita el crasheo en caso de que no tenga materiales
					//se le asigna el material por defecto
					//MaterialesNuevos.Append(0);
					//MeshsGroups.Append(0);

					TLex8 lex(line.Mid(2));
					if (!lex.Eos()){
						TPtrC8 currentString = lex.NextToken();							
						obj.name = HBufC::NewL(currentString.Length());
						obj.name->Des().Copy(currentString);
					}
					
					/*HBufC* noteBuf3 = HBufC::NewLC(180);
					_LIT(KFormatString3, "Nombre de objeto: %S");
					noteBuf3->Des().Format(KFormatString3, obj.name);
					DialogAlert(noteBuf3);
					CleanupStack::PopAndDestroy(noteBuf3);*/

					NombreEncontrado = true;
				}
				//si encuentra otro objeto para
				else if (NombreEncontrado && line.Left(2) == _L8("o ")) {
					continuarLeyendo = false; // Salir del bucle
					hayMasObjetos = true;
				}
				else {
					if (line.Left(2) == _L8("v ")) {
						contador = 0;		
						ListVertices.ReserveL(ListVertices.Count() +3); // Reservar espacio para los elementos
						ListVertices.ReserveL(ListVertices.Count() +3); // Reservar espacio para los elementos

						TLex8 lex(line.Mid(2));  // Inicializa TLex con la subcadena a partir del tercer carácter
						// Iterar mientras no se llegue al final del descriptor y se haya alcanzado el límite de 8 strings
						while (!lex.Eos() && contador < 6) {		
							TPtrC8 currentString = lex.NextToken(); // Mostrar el mensaje con el valor actual del "string" y el contador					
							TLex8 testLex(currentString);// Crear un nuevo objeto TLex para la prueba
							
							// Convertir el string en un número TInt
							TReal number = 0.0;
							TInt err = testLex.Val(number, '.');
							if (err == KErrNone && contador < 3) {
								number = number*2000;								
								GLshort glNumber = static_cast<GLshort>(number); // Conversión a GLbyte
								ListVertices.Append(glNumber);
							}
							else if (contador < 3){
								ListVertices.Append(0);
							}							
							else if (contador < 6){
								number = number*255.0;	
								if (number > 255.0){number = 255.0;}	
								GLshort glNumber = static_cast<GLubyte>(number); // Conversión a GLbyte
								ListColors.Append(glNumber);
							}

							// Avanzar al siguiente "string" que no sea espacio en blanco
							lex.SkipSpace();

							// Incrementar el contador para llevar la cuenta de los strings procesados
							contador++;
						}
						while (contador < 6) {		
							if (contador < 3){
								ListVertices.Append(0);
							}							
							else if (contador < 6){
								ListColors.Append(255);
							}
							contador++;
						}
					}
					else if (line.Left(3) == _L8("vn ")) {
						contador = 0;
						ListNormals.ReserveL(ListNormals.Count() +3); // Reservar espacio para los elementos

						TLex8 lex(line.Mid(3));  // Inicializa TLex con la subcadena a partir del tercer carácter
						// Iterar mientras no se llegue al final del descriptor y se haya alcanzado el límite de 8 strings
						while (!lex.Eos() && contador < 3) {							
							TPtrC8 currentString = lex.NextToken(); // Mostrar el mensaje con el valor actual del "string" y el contador
							TLex8 testLex(currentString); // Crear un nuevo objeto TLex para la prueba
							
							TReal number = 0.0;
							TInt err = testLex.Val(number, '.');
							if (err == KErrNone) {
								number = ((number +1)/2)* 255.0 - 128.0;
								if (number > 127.0){number = 127.0;}
								else if (number < -128.0){number = -128.0;}
								GLbyte glNumber = static_cast<GLbyte>(number); // Conversión a GLbyte
								ListNormals.Append(glNumber);
							}
							else if (contador < 3){
								ListNormals.Append(0);
							}
							lex.SkipSpace();
							contador++;
						}				
					}
					else if (line.Left(3) == _L8("vt ")) {					
						contador = 0;
						ListUVs.ReserveL(ListUVs.Count() +2); // Reservar espacio para los elementos

						TLex8 lex(line.Mid(3));  // Inicializa TLex con la subcadena a partir del tercer carácter
						// Iterar mientras no se llegue al final del descriptor y se haya alcanzado el límite de 8 strings
						while (!lex.Eos() && contador < 2) {							
							TPtrC8 currentString = lex.NextToken(); // Mostrar el mensaje con el valor actual del "string" y el contador
							TLex8 testLex(currentString); // Crear un nuevo objeto TLex para la prueba
							
							TReal number = 0.0;
							TInt err = testLex.Val(number, '.');
							if (err == KErrNone) {		
								if (contador == 1) {
									number = -number+1;
								}		
								number = number * 255.0 - 128.0;
								/*if (number > 127.0){number = 127.0;}
								else if (number < -128.0){number = -128.0;}*/
								//GLbyte glNumber = static_cast<GLbyte>(number); // Conversión a GLbyte
								GLfloat glNumber = static_cast<GLfloat>(number); // Conversión a GLbyte
								ListUVs.Append(glNumber);	
							}
							else if (contador < 2){
								ListUVs.Append(0);
							}
							contador++;
							lex.SkipSpace();
						}			
					}
					else if (line.Left(2) == _L8("f ")) {
						contador = 0;
						TInt conTBarras = 0;

						TLex8 lex(line.Mid(2));  // Inicializa TLex con la subcadena a partir del tercer carácter

						// Lista temporal para almacenar los índices de los vértices
    					RArray<TInt> tempIndices;	
						tempIndices.ReserveL(9);				
						
						HBufC* noteBuf3 = HBufC::NewLC(180);
						// Iterar mientras no se llegue al final del descriptor y se haya alcanzado el límite de 8 strings
						//while (!lex.Eos() && contador < 3) {			
						while (!lex.Eos()) {		
							TPtrC8 currentString = lex.NextToken(); // Mostrar el mensaje con el valor actual del "string" y el contador

							TInt startPos2 = 0; // Posición inicial
							TInt tokenLength = 0; // Longitud del token
							conTBarras = 0;

							// Recorre la cadena hasta encontrar el final
							while (startPos2 < currentString.Length()) {
								// Busca la posición de la siguiente barra diagonal
								TInt nextSlashPos = currentString.Mid(startPos2).Locate('/');

								// Si no se encuentra una barra diagonal, asume que es el último token
								if (nextSlashPos == KErrNotFound) {
									tokenLength = currentString.Length() - startPos2;
								} else {
									tokenLength = nextSlashPos; // Longitud del token hasta la barra diagonal
								}

								// Extrae el token utilizando la posición y longitud calculadas
								TPtrC8 token = currentString.Mid(startPos2, tokenLength);
								TLex8 testLex(token); // Crear un nuevo objeto TLex para la prueba

								// Convertir el string en un número TInt
								TInt number = 0;
								TInt err = testLex.Val(number);
								if (err == KErrNone && conTBarras < 3) {
                					tempIndices.Append(number-1);
								}

								// Actualiza la posición inicial para el próximo token
								startPos2 += tokenLength + 1; // Suma 1 para omitir la barra diagonal
								conTBarras++;
							}	

							lex.SkipSpace();
							contador++;				
						}

						// Crear triángulos de la lista temporal de índices
						TInt numIndices = tempIndices.Count();
						contador -= 2;

						/*_LIT(KFormatString4, "Num. indices: %d\nCaras: %d");
						noteBuf3->Des().Format(KFormatString4, numIndices, contador);
						DialogAlert(noteBuf3);*/

						// Esto resuelve tanto triangulos, quads como ngones
						ListCaras.ReserveL(ListCaras.Count() +3*contador); // Reservar espacio para los elementos
						for (TInt c = 0; c < contador; ++c) {
							for (TInt i = 0; i < 3; ++i) {
								//el primer vertice de los triangulos es el primero
								if (i == 0){
									ListCaras.Append(tempIndices[i*3  ]);
									ListCaras.Append(tempIndices[i*3+1]);
									ListCaras.Append(tempIndices[i*3+2]);
								}
								else {									
									ListCaras.Append(tempIndices[3*c+ i*3  ]);
									ListCaras.Append(tempIndices[3*c+ i*3+1]);
									ListCaras.Append(tempIndices[3*c+ i*3+2]);
								}

								/*_LIT(KFormatString4, "Face %d: %d/%d/%d");
								TInt contC= ListCaras.Count();
								noteBuf3->Des().Format(KFormatString4, i+1, ListCaras[contC-3]+1, ListCaras[contC-2]+1, ListCaras[contC-1]+1);
								DialogAlert(noteBuf3);*/
							}
						}	
						// Limpiar la lista temporal de índices
						tempIndices.Close();

						// Actualizar el contador de caras en el último grupo de mallas
						//MeshsGroups[MeshsGroups.Count()-1] += (contador - 2);
						if (MeshsGroups.Count() < 1){
							MeshsGroups.Append(0);
						}
						MeshsGroups[MeshsGroups.Count()-1] += contador;

						/*_LIT(KFormatString5, "Terminado.\nCaras indices: %d\nmeshsGroups: %d");
						noteBuf3->Des().Format(KFormatString5, ListCaras.Count(), MeshsGroups[MeshsGroups.Count()-1]);
						DialogAlert(noteBuf3);
						CleanupStack::PopAndDestroy(noteBuf3);*/

						//MeshsGroups[MeshsGroups.Count()-1]++;
					}
					else if (line.Left(7) == _L8("usemtl ")) {	
						MaterialesNuevos.Append(0);
						MeshsGroups.Append(0);					
						Material mat;	
						mat.specular[0] = mat.specular[1] = mat.specular[2] = mat.specular[3] = 0.3;
						mat.diffuse[0] = mat.diffuse[1] = mat.diffuse[2] = mat.diffuse[3] = 1.0;
						mat.emission[0] = mat.emission[1] = mat.emission[2] = mat.emission[3] = 0.0;
						mat.textura = false;
						mat.color = false;
						mat.repeat = true;
						mat.lighting = true;
						mat.transparent = false;
						mat.interpolacion = lineal;
						mat.textureID = 0;
						HBufC* materialName16 = HBufC::NewLC(180);

						TInt encontrado = -1;	
						TLex8 lex(line.Mid(7));
						if (!lex.Eos()){
							TPtrC8 currentString = lex.NextToken();							
							mat.name = HBufC::NewL(currentString.Length());
							mat.name->Des().Copy(currentString);

							//busca si existe el material
							materialName16 = CnvUtfConverter::ConvertToUnicodeFromUtf8L(currentString);
							for(int f=0; f < Materials.Count(); f++){
								if (Materials[f].name->Compare(*materialName16) == 0 && f != 0){
									encontrado = f;
									break;
								}
							}
						}
						else {	
							mat.name = HBufC::NewL(100);						
							_LIT(KMatName, "Material.%03d");
							mat.name->Des().Format(KMatName, Materials.Count()+1);
						}

						/*HBufC* noteBuf3 = HBufC::NewLC(180);
						_LIT(KFormatString3, "Material: %d\nNombre: %S");
						noteBuf3->Des().Format(KFormatString3, Materials.Count(), mat.name);
						DialogAlert(noteBuf3);
						CleanupStack::PopAndDestroy(noteBuf3);*/

						//si se encontro no crea el nuevo material
						if (encontrado > -1){
							/*HBufC* noteBuf3 = HBufC::NewLC(180);
							_LIT(KFormatString3, "newmtl %S\nencontrado %d\nmateriales num: %d");
							noteBuf3->Des().Format(KFormatString3, materialName16, encontrado, Materials.Count());
							DialogAlert(noteBuf3);*/
							MaterialesNuevos[MaterialesNuevos.Count()-1] = encontrado;
						}
						else {
							/*HBufC* noteBuf3 = HBufC::NewLC(180);
							_LIT(KFormatString3, "no encontrado %S");
							noteBuf3->Des().Format(KFormatString3, materialName16);
							DialogAlert(noteBuf3);*/
							Materials.Append(mat);
							MaterialesNuevos[MaterialesNuevos.Count()-1] = Materials.Count()-1;
						}
						CleanupStack::PopAndDestroy(materialName16);
					}
				}
			}
			if (continuarLeyendo){
				// Actualizar la posición de inicio para la próxima lectura
				*startPos += pos + 1;

				// Eliminar la parte de la línea ya procesada y el carácter de salto de línea
				buffer.Delete(0, pos + 1);
				buffer.TrimLeft(); // Eliminar espacios en blanco iniciales
			}
		}
	}
	
	TInt ultimoIndice = 0;
	Mesh tempMesh;
	tempMesh.materialsSize = MeshsGroups.Count();
	tempMesh.materials = new TInt[tempMesh.materialsSize];
	tempMesh.facesGroupsSize = new TInt[tempMesh.materialsSize];
	tempMesh.vertexSize = ListVertices.Count()/3;
	tempMesh.vertex = new GLshort[tempMesh.vertexSize*3];
	tempMesh.vertexColor = new GLubyte[tempMesh.vertexSize*4];
	tempMesh.normals = new GLbyte[tempMesh.vertexSize*3];
	tempMesh.uv = new GLfloat[tempMesh.vertexSize*2];
	tempMesh.smooth = true;
	tempMesh.culling = true;	
	tempMesh.vertexGroupUI = NULL;

	//valores defecto
	for (int i = 0; i < tempMesh.vertexSize*3; i++) {
		tempMesh.vertex[i] = ListVertices[i];
		tempMesh.normals[i] = 0;
	}
	for (int i = 0; i < tempMesh.vertexSize*4; i++) {
		tempMesh.vertexColor[i] = 255;
	}
	for (int i = 0; i < tempMesh.vertexSize*2; i++) {
		tempMesh.uv[i] = 0;
	}	

	/*HBufC* noteBuf3 = HBufC::NewLC(180);
	_LIT(KFormatString3, "vertices: %d\nfaces: %d\nnormals: %d\nUVs: %d\nColors: %d");
	noteBuf3->Des().Format(KFormatString3, ListVertices.Count()/3, ListCaras.Count()/9, ListNormals.Count()/3, ListUVs.Count()/2, ListColors.Count()/3);
	DialogAlert(noteBuf3);*/	

	
	/*HBufC* noteBuf3 = HBufC::NewLC(180);
	_LIT(KFormatString33, "Num Vertices: %d\nNormales: %d\nUVs: %d"); //\nvalores: %d/%f/%d"
	noteBuf3->Des().Format(KFormatString33, ListVertices.Count()/3, ListNormals.Count()/3, ListUVs.Count()/2);
	DialogAlert(noteBuf3);*/

	tempMesh.faces = new GLushort*[tempMesh.materialsSize];
	for(TInt m=0; m < MeshsGroups.Count(); m++){	
		//se asigna el material por defecto en caso de no existir materiales en el obj
		if (MaterialesNuevos.Count() < 1){
			tempMesh.materials[m] = 0;
		}
		else {
			tempMesh.materials[m] = MaterialesNuevos[m];
		}
		tempMesh.facesGroupsSize[m] = MeshsGroups[m];

		//valores defecto caras
		tempMesh.faces[m] = new GLushort[tempMesh.facesGroupsSize[m]*3];
		for (int i = 0; i < tempMesh.facesGroupsSize[m]*3; i++) {
			tempMesh.faces[m][i] = 0;
		}
		
		//valores reales
		TInt limite = ultimoIndice + MeshsGroups[m];			
		TInt ultimoIndiceinicio = ultimoIndice;

		for(TInt a=ultimoIndice; a < limite; a++){			
			for(TInt f=0; f < 3; f++){
				TInt indiceCara = a*9+f*3;

				TInt indiceVertice = (ListCaras[indiceCara]-*acumuladoVertices)*3;
				TInt indiceNormales = (ListCaras[indiceCara]-*acumuladoNormales)*3;
				TInt indiceUV = (ListCaras[indiceCara]-*acumuladoUVs)*2;
				TInt indiceColor = (ListCaras[indiceCara]-*acumuladoVertices)*4;

				/*HBufC* noteBuf3 = HBufC::NewLC(180);
				_LIT(KFormatString3, "indiceCara: %d\nNuevoIndiceCara: %d\nindiceVertice: %d\nindiceUV: %d\nindiceColor: %d");
				noteBuf3->Des().Format(KFormatString3, ListCaras[indiceCara]+1, ListCaras[indiceCara]-*acumuladoVertices+1,indiceVertice, indiceUV, indiceColor);
				DialogAlert(noteBuf3);*/				

				/*HBufC* noteBuf3 = HBufC::NewLC(180);
				_LIT(KFormatString3, "cara: %d vertice: %d\nindices viejo: %d\nindice nuevo: %d");
				noteBuf3->Des().Format(KFormatString3, a+1, f+1, ListCaras[indiceCara]-*acumuladoVertices, ListCaras[indiceCara]-*acumuladoVertices);*/
				/*_LIT(KFormatString3, "cara: %d vertice: %d\nindices: %d/%d/%d\nvertice: %f\n%f\n%f");//\nuv: %f/%f\nnormal: %d/%d/%d"); //\nvalores: %d/%f/%d"
				noteBuf3->Des().Format(KFormatString3, a+1, f+1, 
					ListCaras[indiceCara]-*acumuladoVertices+1, ListCaras[indiceCara+1]-*acumuladoUVs+1, ListCaras[indiceCara+2]-*acumuladoNormales+1, 
					(GLfloat)ListVertices[(ListCaras[indiceCara]-*acumuladoVertices)*3]/2000,
					(GLfloat)ListVertices[(ListCaras[indiceCara]-*acumuladoVertices)*3+1]/2000,
					(GLfloat)ListVertices[(ListCaras[indiceCara]-*acumuladoVertices)*3+2]/2000
					ListUVs[(ListCaras[indiceCara+1]-*acumuladoUVs)*3],
					ListUVs[(ListCaras[indiceCara+1]-*acumuladoUVs)*3+1],
					ListNormals[(ListCaras[indiceCara+2]-*acumuladoNormales)*3],
					ListNormals[(ListCaras[indiceCara+2]-*acumuladoNormales)*3+1],
					ListNormals[(ListCaras[indiceCara+2]-*acumuladoNormales)*3+2]
				);
				DialogAlert(noteBuf3);*/

				tempMesh.faces[m][(a-ultimoIndiceinicio)*3+f] = ListCaras[indiceCara]-*acumuladoVertices;				
				for(TInt v=0; v < 3; v++){
					//a*9 es que ListCaras tiene 9 valores por cara, 3 vertices, 3 normales y 3 UV
					//f*3 es para ir por las distintas "/" 1/1/1
					tempMesh.vertex[indiceVertice+v]  = ListVertices[(ListCaras[indiceCara]-*acumuladoVertices)*3+v];	
					tempMesh.normals[indiceNormales+v] = ListNormals[(ListCaras[indiceCara+2]-*acumuladoNormales)*3+v];
					tempMesh.vertexColor[indiceColor+v] = ListColors[(ListCaras[indiceCara]-*acumuladoVertices)*3+v];
				}
				for(TInt uv=0; uv < 2; uv++){
					tempMesh.uv[indiceUV+uv] = ListUVs[(ListCaras[indiceCara+1]-*acumuladoUVs)*2+uv];
				}
			}	
			ultimoIndice++;		
		}
	}

	Meshes.Append(tempMesh);	

	/*HBufC* noteBuf = HBufC::NewLC(180);
	_LIT(KFormatString, "se creo la malla 3D");
	noteBuf->Des().Format(KFormatString);
	DialogAlert(noteBuf);
	CleanupStack::PopAndDestroy(noteBuf);*/
	
	obj.Id = Meshes.Count()-1;
	Objects.Append(obj);
	Collection.Append(Objects.Count()-1);
	objSelect = Objects.Count()-1;
	//Meshes[obj.Id].VaciarGrupoVertices();
	Meshes[obj.Id].AgruparVertices();

	//libero memoria		
	*acumuladoVertices += ListVertices.Count()/3;
	*acumuladoNormales += ListNormals.Count()/3;
	*acumuladoUVs += ListUVs.Count()/2;
	ListVertices.Close();
	ListColors.Close();
	ListCaras.Close();
	ListNormals.Close();
	ListUVs.Close();

	//para separar las mesh	
	MaterialesNuevos.Close();
	MeshsGroups.Close();
	//Objects[objSelect].RecalcularBordes();

	return hayMasObjetos;
};


void CBlenderLite::SetOrigen( TInt opcion ){
	if (estado != edicion){
		return;
	}
	//a la geometria
	else if (opcion == 0){
		/*if (tipoSelect == vertexSelect){
			//guarda el centro de la geometria
			GLshort NuevoOrigen[3] = {0,0,0};
			for(TInt i = 0; i < Objects[objSelect].vertexGroupSize; i++) {
			    NuevoOrigen[0] += Objects[objSelect].vertex[Objects[objSelect].vertexGroup[i]*3];
			    NuevoOrigen[1] += Objects[objSelect].vertex[Objects[objSelect].vertexGroup[i]*3+1];
			    NuevoOrigen[2] += Objects[objSelect].vertex[Objects[objSelect].vertexGroup[i]*3+2];
			}
			NuevoOrigen[0] /= Objects[objSelect].vertexGroupSize;
			NuevoOrigen[1] /= Objects[objSelect].vertexGroupSize;
			NuevoOrigen[2] /= Objects[objSelect].vertexGroupSize;

			for(TInt i = 0; i < Objects[objSelect].vertexSize/3; i++) {
				Objects[objSelect].vertex[i*3] -= NuevoOrigen[0];
				Objects[objSelect].vertex[i*3+1] -= NuevoOrigen[1];
				Objects[objSelect].vertex[i*3+2] -= NuevoOrigen[2];
			}			
			Objects[objSelect].posX += NuevoOrigen[0]*Objects[objSelect].scaleX/65000;
			Objects[objSelect].posY += NuevoOrigen[2]*Objects[objSelect].scaleY/65000;
			Objects[objSelect].posZ += NuevoOrigen[1]*Objects[objSelect].scaleZ/65000;
		}	*/
	}	
	//al vertice seleccionado
	else if (opcion == 1){
		/*if (tipoSelect == vertexSelect){
			//guarda el valor del vertice seleccionado
			GLshort NuevoOrigen[3] = {
				Objects[objSelect].vertex[Objects[objSelect].vertexGroup[EditSelect]*3],
				Objects[objSelect].vertex[Objects[objSelect].vertexGroup[EditSelect]*3+1],
				Objects[objSelect].vertex[Objects[objSelect].vertexGroup[EditSelect]*3+2]
			};
			for(TInt i = 0; i < Objects[objSelect].vertexSize/3; i++) {
				Objects[objSelect].vertex[i*3] -= NuevoOrigen[0];
				Objects[objSelect].vertex[i*3+1] -= NuevoOrigen[1];
				Objects[objSelect].vertex[i*3+2] -= NuevoOrigen[2];
			}
			Objects[objSelect].posX += NuevoOrigen[0]*Objects[objSelect].scaleX/65000;
			Objects[objSelect].posY += NuevoOrigen[2]*Objects[objSelect].scaleY/65000;
			Objects[objSelect].posZ += NuevoOrigen[1]*Objects[objSelect].scaleZ/65000;
		}//, edgeSelect, faceSelect  EditSelect		*/
	}	
    redibujar = true;
}

struct __attribute__((packed)) TBMPFileHeader {
    TUint16 bfType;
    TUint32 bfSize;
    TUint16 bfReserved1;
    TUint16 bfReserved2;
    TUint32 bfOffBits;
};

struct TBMPInfoHeader {
    TUint32 biSize;
    TInt32 biWidth;
    TInt32 biHeight;
    TUint16 biPlanes;
    TUint16 biBitCount;
    TUint32 biCompression;
    TUint32 biSizeImage;
    TInt32 biXPelsPerMeter;
    TInt32 biYPelsPerMeter;
    TUint32 biClrUsed;
    TUint32 biClrImportant;
};

void CBlenderLite::SaveAsBMP(int width, int height, const GLubyte* pixels, const TDesC& fileName) {
    RFs fsSession;
    RFile file;
    TInt err = fsSession.Connect();
    if (err != KErrNone) {
        return;
    }

    // Crear la carpeta si no existe
    _LIT(KDirName, "E:\\blendersito\\");
    err = fsSession.MkDirAll(KDirName);
    if (err != KErrNone && err != KErrAlreadyExists) {
		fsSession.Close();
		return;
    }

	err = file.Replace(fsSession, fileName, EFileWrite);
    if (err != KErrNone) {
        fsSession.Close();
        return;
    }

    TBMPFileHeader fileHeader;
    TBMPInfoHeader infoHeader;

    TInt32 rowSize = ((24 * width + 31) / 32) * 4;  // Row size with padding
    TUint32 imageSize = rowSize * height;  // Image size in bytes
    fileHeader.bfType = 0x4D42;  // 'BM'
    fileHeader.bfSize = sizeof(TBMPFileHeader) + sizeof(TBMPInfoHeader) + imageSize;
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
    fileHeader.bfOffBits = sizeof(TBMPFileHeader) + sizeof(TBMPInfoHeader);

	/*HBufC* noteBuf = HBufC::NewLC(200);
	_LIT(KFormatString, "TBMPFileHeader: %d\nTBMPInfoHeader:%d");
	noteBuf->Des().Format(KFormatString, sizeof(TBMPFileHeader), sizeof(TBMPInfoHeader));

	_LIT(KFormatString2, "rowSize: %d\nimage size:%d\nbfSize: %d\nbfReserved1: %d\nbfReserved2: %d");
	noteBuf->Des().Format(KFormatString2, rowSize, imageSize, fileHeader.bfSize, fileHeader.bfReserved1, fileHeader.bfReserved2);
	DialogAlert(noteBuf);	
	CleanupStack::PopAndDestroy(noteBuf);*/

    infoHeader.biSize = sizeof(TBMPInfoHeader);
    infoHeader.biWidth = width;
    infoHeader.biHeight = height;
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = 24;  // 24 bits per pixel
    infoHeader.biCompression = 0;
    infoHeader.biSizeImage = imageSize;
    infoHeader.biXPelsPerMeter = 0;
    infoHeader.biYPelsPerMeter = 0;
    infoHeader.biClrUsed = 0;
    infoHeader.biClrImportant = 0;

    TPtrC8 fileHeaderPtr(reinterpret_cast<const TUint8*>(&fileHeader), sizeof(fileHeader));
    file.Write(fileHeaderPtr);
    TPtrC8 infoHeaderPtr(reinterpret_cast<const TUint8*>(&infoHeader), sizeof(infoHeader));
    file.Write(infoHeaderPtr);

    // Convert from RGBA to RGB
    GLubyte* rgbPixels = new GLubyte[imageSize];
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int srcIndex = (y * width + x) * 4;  // Source RGBA index
            int destIndex = y * rowSize + x * 3;  // Destination RGB index with padding
            rgbPixels[destIndex] = pixels[srcIndex+2];        // R
            rgbPixels[destIndex + 1] = pixels[srcIndex + 1]; // G
            rgbPixels[destIndex + 2] = pixels[srcIndex ]; // B
        }
    }

    TPtrC8 pixelsPtr(reinterpret_cast<const TUint8*>(rgbPixels), imageSize);
    file.Write(pixelsPtr);

    delete[] rgbPixels;
    file.Close();
    fsSession.Close();
}

void CBlenderLite::SaveCanvasToImage(TBool secuencia, TBool showUi)  {
    // Redibuja el canvas sin los overlays
    TBool originalShowOverlays = showOverlays;
    TBool originalShowUi = ShowUi;
    ShowUi = showUi;    
    showOverlays = false;

    // Tamaño temporal del canvas
    /*TUint temScreenWidth  = iScreenWidth;
    TUint temScreenHeight = iScreenHeight;

    // Nuevo tamaño del canvas
    TUint ScreenWidth  = 320; // Puedes cambiar esto al tamaño que desees
    TUint ScreenHeight = 320; // Puedes cambiar esto al tamaño que desees

    SetScreenSize(ScreenWidth, ScreenHeight, false);*/

    // Captura los píxeles de la pantalla
    GLubyte* pixels = new GLubyte[iScreenWidth * iScreenHeight * 4]; // 4 para RGBA

    _LIT(KFileName, "E:\\blendersito\\render_%04d.bmp");
    _LIT(KFileNameZbuffer, "E:\\blendersito\\renderZbuffer_%04d.bmp");
    TFileName fileName;	
	TBuf<256> buffer;

	if (secuencia){
		CurrentFrame = StartFrame;
		ReloadAnimation();
		for (int r = 0; r < EndFrame; ++r) {	
			redibujar = true;
			AppCycle(0, 0, 0);		
    		glReadPixels(0, 0, iScreenWidth, iScreenHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
			buffer.Format(KFileName, CurrentFrame);
			fileName.Copy(buffer);
			SaveAsBMP(iScreenWidth, iScreenHeight, pixels, fileName);
			CurrentFrame++;
			ReloadAnimation();
		}
	}
	else {   
		redibujar = true;
		AppCycle(0, 0, 0);
		glReadPixels(0, 0, iScreenWidth, iScreenHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		//postprocesado
		//applyBlur(pixels, iScreenWidth, iScreenHeight, 3);

		buffer.Format(KFileName, CurrentFrame);
		fileName.Copy(buffer);
		SaveAsBMP(iScreenWidth, iScreenHeight, pixels, fileName);

		SimularZBuffer = true;
		redibujar = true;
		AppCycle(0, 0, 0);	
		glReadPixels(0, 0, iScreenWidth, iScreenHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		buffer.Format(KFileNameZbuffer, CurrentFrame);
		fileName.Copy(buffer);
		SaveAsBMP(iScreenWidth, iScreenHeight, pixels, fileName);
		SimularZBuffer = false;
	}

    // Restaura el estado original
    showOverlays = originalShowOverlays;
    ShowUi = originalShowUi;
    delete[] pixels;

    // Restaura el tamaño original del canvas
    //SetScreenSize(temScreenWidth, temScreenHeight, false);
    redibujar = true;
}

void CBlenderLite::applyBlur(GLubyte* pixels, int width, int height, int radius) {
    // Calcula el tamaño del área del kernel
    int kernelSize = 2 * radius + 1;
    
    // Itera sobre cada píxel de la imagen
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float r = 0.0f, g = 0.0f, b = 0.0f;
            
            // Itera sobre cada píxel en el área del kernel
            for (int ky = -radius; ky <= radius; ++ky) {
                for (int kx = -radius; kx <= radius; ++kx) {
                    int pixelX = clamp(x + kx, 0, width - 1);
                    int pixelY = clamp(y + ky, 0, height - 1);
                    
                    // Obtiene el valor del píxel original
                    int index = (pixelY * width + pixelX) * 4;
                    r += pixels[index];
                    g += pixels[index + 1];
                    b += pixels[index + 2];
                }
            }
            
            // Calcula el promedio de los valores RGB en el área del kernel
            int index = (y * width + x) * 4;
            pixels[index] = static_cast<GLubyte>(r / (kernelSize * kernelSize));
            pixels[index + 1] = static_cast<GLubyte>(g / (kernelSize * kernelSize));
            pixels[index + 2] = static_cast<GLubyte>(b / (kernelSize * kernelSize));
        }
    }
}

int CBlenderLite::clamp(int value, int min, int max) {
    if (value < min) {
        return min;
    } else if (value > max) {
        return max;
    } else {
        return value;
    }
}

void CBlenderLite::SetWidescreen(){
	iWidescreenEnabled = !iWidescreenEnabled;
	SetScreenSize( iScreenWidth, iScreenHeight, iWidescreenEnabled );
}

//  End of File
