// A simple demonstration application using Scintilla
#include <stdio.h>

#include <windows.h>
#include <richedit.h>

#include "Scintilla.h"
#include "SciLexer.h"
#include "resource.h"

const char appName[] = "DMApp";
const char className[] = "DMAppWindow";
const int blockSize = 128 * 1024;

const COLORREF black = RGB(0,0,0);
const COLORREF white = RGB(0xff,0xff,0xff);

int Scintilla_RegisterClasses(void *hInstance);
int Scintilla_ReleaseResources();

#include "gl/ScintillaGL.h"

struct DMApp {
	MyEditor myEd;
	HINSTANCE hInstance;
	HWND currentDialog;
	HWND wMain;
	HWND wEditor;
	bool isDirty;
	char fullPath[MAX_PATH];

	LRESULT SendEditor(UINT Msg, WPARAM wParam=0, LPARAM lParam=0) {
		return myEd.WndProc(Msg, wParam, lParam);
	}

	void SetAStyle(int style, COLORREF fore, COLORREF back=white, int size=-1, const char *face=0);
	void InitialiseEditor();
};

static DMApp app;

void DMApp::SetAStyle(int style, COLORREF fore, COLORREF back, int size, const char *face) {
	SendEditor(SCI_STYLESETFORE, style, fore);
	SendEditor(SCI_STYLESETBACK, style, back);
	if (size >= 1)
		SendEditor(SCI_STYLESETSIZE, style, size);
	if (face) 
		SendEditor(SCI_STYLESETFONT, style, reinterpret_cast<LPARAM>(face));
}

const char glslKeyword[] =
	"discard struct if else switch case default break goto return for while do continue";

const char glslType[] = 
	"attribute const in inout out uniform varying invariant "
	"centroid flat smooth noperspective layout patch sample "
	"subroutine lowp mediump highp precision "
	"void float vec2 vec3 vec4 bvec2 bvec3 bvec4 ivec2 ivec3 ivec4 "
	"uvec2 uvec3 uvec4 dvec2 dvec3 dvec4 "
	"sampler1D sampler2D sampler3D isampler2D isampler1D isampler3D "
	"usampler1D usampler2D usampler3D "
	"sampler1DShadow sampler2DShadow sampler1DArray sampler2DArray "
	"sampler1DArrayShadow sampler2DArrayShadow "
	"samplerCube samperCubeShadow samperCubeArrayShadow ";

const char glslBuiltin[] = 
	"radians degrees sin cos tan asin acos atan sinh "
	"cosh tanh asinh acosh atanh pow exp log exp2 "
	"log2 sqrt inversesqrt abs sign floor trunc round "
	"roundEven ceil fract mod modf min max clamp mix "
	"step smoothstep isnan isinf floatBitsToInt floatBitsToUint "
	"intBitsToFloat uintBitsToFloat fma frexp ldexp packUnorm2x16 "
	"packUnorm4x8 packSnorm4x8 unpackUnorm2x16 unpackUnorm4x8 "
	"unpackSnorm4x8 packDouble2x32 unpackDouble2x32 length distance "
	"dot cross normalize ftransform faceforward reflect "
	"refract matrixCompMult outerProduct transpose determinant "
	"inverse lessThan lessThanEqual greaterThan greaterThanEqual "
	"equal notEqual any all not uaddCarry usubBorrow "
	"umulExtended imulExtended bitfieldExtract bitfildInsert "
	"bitfieldReverse bitCount findLSB findMSB textureSize "
	"textureQueryLOD texture textureProj textureLod textureOffset "
	"texelFetch texelFetchOffset textureProjOffset textureLodOffset "
	"textureProjLod textureProjLodOffset textureGrad textureGradOffset "
	"textureProjGrad textureProjGradOffset textureGather "
	"textureGatherOffset texture1D texture2D texture3D texture1DProj "
	"texture2DProj texture3DProj texture1DLod texture2DLod "
	"texture3DLod texture1DProjLod texture2DProjLod texture3DProjLod "
	"textureCube textureCubeLod shadow1D shadow2D shadow1DProj "
	"shadow2DProj shadow1DLod shadow2DLod shadow1DProjLod "
	"shadow2DProjLod dFdx dFdy fwidth interpolateAtCentroid "
	"interpolateAtSample interpolateAtOffset noise1 noise2 noise3 "
	"noise4 EmitStreamVertex EndStreamPrimitive EmitVertex "
	"EndPrimitive barrier "
	"gl_VertexID gl_InstanceID gl_Position gl_PointSize "
	"gl_ClipDistance gl_PrimitiveIDIn gl_InvocationID gl_PrimitiveID "
	"gl_Layer gl_PatchVerticesIn gl_TessLevelOuter gl_TessLevelInner "
	"gl_TessCoord gl_FragCoord gl_FrontFacing gl_PointCoord "
	"gl_SampleID gl_SamplePosition gl_FragColor gl_FragData "
	"gl_FragDepth gl_SampleMask gl_ClipVertex gl_FrontColor "
	"gl_BackColor gl_FrontSecondaryColor gl_BackSecondaryColor "
	"gl_TexCoord gl_FogFragCoord gl_Color gl_SecondaryColor "
	"gl_Normal gl_Vertex gl_MultiTexCoord0 gl_MultiTexCoord1 "
	"gl_MultiTexCoord2 gl_MultiTexCoord3 gl_MultiTexCoord4 "
	"gl_MultiTexCoord5 gl_MultiTexCoord6 gl_MultiTexCoord7 gl_FogCoord "
	"gl_MaxVertexAttribs gl_MaxVertexUniformComponents gl_MaxVaryingFloats "
	"gl_MaxVaryingComponents gl_MaxVertexOutputComponents "
	"gl_MaxGeometryInputComponents gl_MaxGeometryOutputComponents "
	"gl_MaxFragmentInputComponents gl_MaxVertexTextureImageUnits "
	"gl_MaxCombinedTextureImageUnits gl_MaxTextureImageUnits "
	"gl_MaxFragmentUniformComponents gl_MaxDrawBuffers gl_MaxClipDistances "
	"gl_MaxGeometryTextureImageUnits gl_MaxGeometryOutputVertices "
	"gl_MaxGeometryTotalOutputComponents gl_MaxGeometryUniformComponents "
	"gl_MaxGeometryVaryingComponents gl_MaxTessControlInputComponents "
	"gl_MaxTessControlOutputComponents gl_MaxTessControlTextureImageUnits "
	"gl_MaxTessControlUniformComponents "
	"gl_MaxTessControlTotalOutputComponents "
	"gl_MaxTessEvaluationInputComponents gl_MaxTessEvaluationOutputComponents "
	"gl_MaxTessEvaluationTextureImageUnits "
	"gl_MaxTessEvaluationUniformComponents gl_MaxTessPatchComponents "
	"gl_MaxPatchVertices gl_MaxTessGenLevel gl_MaxTextureUnits "
	"gl_MaxTextureCoords gl_MaxClipPlanes "
	"gl_DepthRange gl_ModelViewMatrix gl_ProjectionMatrix "
	"gl_ModelViewProjectionMatrix gl_TextureMatrix gl_NormalMatrix "
	"gl_ModelViewMatrixInverse gl_ProjectionMatrixInverse "
	"gl_ModelViewProjectionMatrixInverse gl_TextureMatrixInverse "
	"gl_ModelViewMatrixTranspose gl_ProjectionMatrixTranspose "
	"gl_ModelViewProjectionMatrixTranspose gl_TextureMatrixTranspose "
	"gl_ModelViewMatrixInverseTranspose gl_ProjectionMatrixInverseTranspose "
	"gl_ModelViewProjectionMatrixInverseTranspose "
	"gl_TextureMatrixInverseTranspose gl_NormalScale gl_ClipPlane "
	"gl_Point gl_FrontMaterial gl_BackMaterial gl_LightSource "
	"gl_LightModel gl_FrontLightModelProduct gl_BackLightModelProduct "
	"gl_FrontLightProduct gl_BackLightProduct gl_TextureEnvColor "
	"gl_EyePlaneS gl_EyePlaneT gl_EyePlaneR gl_EyePlaneQ "
	"gl_ObjectPlaneS gl_ObjectPlaneT gl_ObjectPlaneR gl_ObjectPlaneQ "
	"gl_Fog";

void DMApp::InitialiseEditor() {
	//SendEditor(SCI_SETLEXER, SCLEX_HTML);
	myEd.ls.SetLexer(SCLEX_CPP);

	SendEditor(SCI_SETSTYLEBITS, 7);

	myEd.ls.SetWordList(0, glslKeyword);
	myEd.ls.SetWordList(1, glslType);
	myEd.ls.SetWordList(4, glslBuiltin);

	// Set up the global default style. These attributes are used wherever no explicit choices are made.
	SetAStyle(STYLE_DEFAULT, 0xFFFFFFFF, 0xFF000000, 20, "c:/windows/fonts/cour.ttf");
	SendEditor(SCI_STYLECLEARALL);	// Copies global style to all others
	SetAStyle(STYLE_INDENTGUIDE, 0xFFC0C0C0, 0xFF000000, 20, "c:/windows/fonts/cour.ttf");
	SetAStyle(STYLE_BRACELIGHT, 0xFF00FF00, 0xFF000000, 20, "c:/windows/fonts/cour.ttf");
	SetAStyle(STYLE_BRACEBAD, 0xFF0000FF, 0xFF000000, 20, "c:/windows/fonts/cour.ttf");
	SetAStyle(STYLE_LINENUMBER, 0xFFC0C0C0, 0xFF333333, 20, "c:/windows/fonts/cour.ttf");
	SendEditor(SCI_SETFOLDMARGINCOLOUR, 1, 0xFF1A1A1A);
	SendEditor(SCI_SETSELBACK, 1, 0xFFCC9966);
	SendEditor(SCI_SETCARETFORE, 0xFFFFFFFF, 0);
	SendEditor(SCI_SETCARETLINEVISIBLE, 1);
	SendEditor(SCI_SETCARETLINEBACK, 0xFF333333);
	
	SendEditor(SCI_SETUSETABS, 1);
	SendEditor(SCI_SETTABWIDTH, 4);
	//SendEditor(SCI_SETINDENTATIONGUIDES, SC_IV_LOOKBOTH);

	SetAStyle(SCE_C_DEFAULT, 0xFFFFFFFF, 0xFF000000, 16, "c:/windows/fonts/cour.ttf");
	SetAStyle(SCE_C_WORD, 0xFF0066FF, 0xFF000000);
	SetAStyle(SCE_C_WORD2, 0xFFFFFF00, 0xFF000000);
	//WTF??? SetAStyle(SCE_C_GLOBALCLASS, 0xFF0000FF, 0xFF000000);
	SetAStyle(SCE_C_PREPROCESSOR, 0xFFC0C0C0, 0xFF000000);
	SetAStyle(SCE_C_NUMBER, 0xFF0080FF, 0xFF000000);
	SetAStyle(SCE_C_OPERATOR, 0xFF00CCFF, 0xFF000000);
	SetAStyle(SCE_C_COMMENT, 0xFF00FF00, 0xFF000000);
	SetAStyle(SCE_C_COMMENTLINE, 0xFF00FF00, 0xFF000000);
}

#include <SDL.h>
#include <gl/glee.h>

void Platform_Initialise();
void Platform_Finalise();

int main(int /*argc*/, char** /*argv*/)
{
	SDL_Surface* mScreen;

	if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER)<0)											// Init The SDL Library, The VIDEO Subsystem
	{
		return 0;															// Get Out Of Here. Sorry.
	}

	uint32_t flags = SDL_HWSURFACE|SDL_OPENGLBLIT;									// We Want A Hardware Surface And Special OpenGLBlit Mode

	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );								// In order to use SDL_OPENGLBLIT we have to
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );							// set GL attributes first
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
	SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, TRUE );							// colors and doublebuffering
	//SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1 );
	//SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, 0 );
	//SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 1 );

	mScreen = SDL_SetVideoMode(800, 600, 32, flags);
	if (!mScreen)
	{
		SDL_Quit();
		return 0;															// And Exit
	}
	
	SDL_EnableUNICODE(TRUE);
	SDL_EnableKeyRepeat(500, 100);

	Platform_Initialise();

	Surface* s = Surface::Allocate();
	app.myEd.drawSurface = s;
	PRectangle rcPaint(0, 0, 800, 600);

	app.InitialiseEditor();

	app.SendEditor(SCI_SETMARGINWIDTHN, 0, 44);//Calculate correct width

	char str[] = "void main()\n{\n\tgl_FragColor=vec4(0, 0, 0, 1);\n}\n";
	app.SendEditor(SCI_CANCEL);
	app.SendEditor(SCI_SETUNDOCOLLECTION, 0);
	app.SendEditor(SCI_ADDTEXT, sizeof(str)-1, reinterpret_cast<LPARAM>((char*)str));
	app.SendEditor(SCI_SETUNDOCOLLECTION, 1);
	app.SendEditor(EM_EMPTYUNDOBUFFER);
	app.SendEditor(SCI_SETSAVEPOINT);
	app.SendEditor(SCI_GOTOPOS, 0);
	app.myEd.SetFocusState(true);

#ifdef SCI_LEXER
	Scintilla_LinkLexers();
#endif

	Uint8	prevKState[SDLK_LAST] = {0};
	bool run = true;

	while (run)
	{
		SDL_Event	E;
		while (SDL_PollEvent(&E))
		{
			if (E.type==SDL_QUIT)
			{
					run=false;
			}
			else if (E.type == SDL_KEYDOWN)
			{
				int sciKey;
				switch(E.key.keysym.sym)
				{
					case SDLK_DOWN:				sciKey = SCK_DOWN;          break;
					case SDLK_UP:				sciKey = SCK_UP;            break;
					case SDLK_LEFT:				sciKey = SCK_LEFT;          break;
					case SDLK_RIGHT:			sciKey = SCK_RIGHT;         break;
					case SDLK_HOME:				sciKey = SCK_HOME;          break;
					case SDLK_END:				sciKey = SCK_END;           break;
					case SDLK_PAGEUP:			sciKey = SCK_PRIOR;         break;
					case SDLK_PAGEDOWN:			sciKey = SCK_NEXT;	        break;
					case SDLK_DELETE:			sciKey = SCK_DELETE;        break;
					case SDLK_INSERT:			sciKey = SCK_INSERT;        break;
					case SDLK_ESCAPE:			sciKey = SCK_ESCAPE;        break;
					case SDLK_BACKSPACE:		sciKey = SCK_BACK;	        break;
					case SDLK_TAB:				sciKey = SCK_TAB;	        break;
					case SDLK_RETURN:			sciKey = SCK_RETURN;        break;
					case SDLK_KP_PLUS:			sciKey = SCK_ADD;	        break;
					case SDLK_KP_MINUS:			sciKey = SCK_SUBTRACT;      break;
					case SDLK_KP_DIVIDE:		sciKey = SCK_DIVIDE;        break;
					case SDLK_LSUPER:			sciKey = SCK_WIN;	        break;
					case SDLK_RSUPER:			sciKey = SCK_RWIN;	        break;
					case SDLK_MENU:				sciKey = SCK_MENU;	        break;
					case SDLK_SLASH:			sciKey = '/';		        break;
					case SDLK_ASTERISK:			sciKey = '`';		        break;
					case SDLK_LEFTBRACKET:		sciKey = '[';		        break;
					case SDLK_BACKSLASH:		sciKey = '\\';		        break;
					case SDLK_RIGHTBRACKET:		sciKey = ']';		        break;
					case SDLK_LSHIFT:
					case SDLK_RSHIFT:
					case SDLK_LALT:
					case SDLK_RALT:
					case SDLK_LCTRL:
					case SDLK_RCTRL:
						sciKey = 0; break;
					default:					sciKey = E.key.keysym.sym;
				}

				if (sciKey)
				{
					bool consumed;
					bool ctrlPressed = !!(E.key.keysym.mod&KMOD_LCTRL | E.key.keysym.mod&KMOD_RCTRL);
					bool altPressed = !!(E.key.keysym.mod&KMOD_LALT | E.key.keysym.mod&KMOD_RALT);
					app.myEd.KeyDown((SDLK_a<=sciKey && sciKey<=SDLK_z)?sciKey-'a'+'A':sciKey,
						!!(E.key.keysym.mod&KMOD_LSHIFT | E.key.keysym.mod&KMOD_RSHIFT),
						ctrlPressed,
						altPressed,
						&consumed
					);
					if (!consumed && sciKey>=32 && sciKey<=128 && !ctrlPressed && !altPressed)
						app.myEd.AddCharUTF(E.key.keysym.unicode);
				}
			}
		}

		Uint8* curState = SDL_GetKeyState(0);
		
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glOrtho(0, 800, 0, 600, 0, 500);
		glTranslatef(0, 600, 0);
		glScalef(1, -1, 1);
		app.myEd.Paint(/*s,*/ rcPaint);

		SDL_GL_SwapBuffers();

		memcpy(prevKState, curState, SDLK_LALT);
	}

	s->Release();

	Platform_Finalise();

	SDL_Quit();

	return 0;
}
