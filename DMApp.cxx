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
	MyEditor myEd, myEd2, shaderList;
	HINSTANCE hInstance;
	HWND currentDialog;
	HWND wMain;
	HWND wEditor;
	bool isDirty;
	char fullPath[MAX_PATH];

	//LRESULT SendEditor(UINT Msg, WPARAM wParam=0, LPARAM lParam=0) {
	//	return myEd.WndProc(Msg, wParam, lParam);
	//}

	void SetAStyle(MyEditor& ed, int style, COLORREF fore, COLORREF back=white, int size=-1, const char *face=0);
	void InitialiseEditor();
	void InitialiseEditor2();
	void InitialiseEditor3();
};

static DMApp app;

void DMApp::SetAStyle(MyEditor& ed, int style, COLORREF fore, COLORREF back, int size, const char *face) {
	ed.Command(SCI_STYLESETFORE, style, fore);
	ed.Command(SCI_STYLESETBACK, style, back);
	if (size >= 1)
		ed.Command(SCI_STYLESETSIZE, style, size);
	if (face) 
		ed.Command(SCI_STYLESETFONT, style, reinterpret_cast<LPARAM>(face));
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

const size_t NB_FOLDER_STATE = 7;
const size_t FOLDER_TYPE = 0;
const int markersArray[][NB_FOLDER_STATE] = {
  {SC_MARKNUM_FOLDEROPEN, SC_MARKNUM_FOLDER, SC_MARKNUM_FOLDERSUB, SC_MARKNUM_FOLDERTAIL, SC_MARKNUM_FOLDEREND,        SC_MARKNUM_FOLDEROPENMID,     SC_MARKNUM_FOLDERMIDTAIL},
  {SC_MARK_MINUS,         SC_MARK_PLUS,      SC_MARK_EMPTY,        SC_MARK_EMPTY,         SC_MARK_EMPTY,               SC_MARK_EMPTY,                SC_MARK_EMPTY},
  {SC_MARK_ARROWDOWN,     SC_MARK_ARROW,     SC_MARK_EMPTY,        SC_MARK_EMPTY,         SC_MARK_EMPTY,               SC_MARK_EMPTY,                SC_MARK_EMPTY},
  {SC_MARK_CIRCLEMINUS,   SC_MARK_CIRCLEPLUS,SC_MARK_VLINE,        SC_MARK_LCORNERCURVE,  SC_MARK_CIRCLEPLUSCONNECTED, SC_MARK_CIRCLEMINUSCONNECTED, SC_MARK_TCORNERCURVE},
  {SC_MARK_BOXMINUS,      SC_MARK_BOXPLUS,   SC_MARK_VLINE,        SC_MARK_LCORNER,       SC_MARK_BOXPLUSCONNECTED,    SC_MARK_BOXMINUSCONNECTED,    SC_MARK_TCORNER}
};

void DMApp::InitialiseEditor() {
	//SendEditor(SCI_SETLEXER, SCLEX_HTML);
	myEd.ls.SetLexer(SCLEX_CPP);

	myEd.Command(SCI_SETSTYLEBITS, 7);

	myEd.ls.SetWordList(0, glslKeyword);
	myEd.ls.SetWordList(1, glslType);
	myEd.ls.SetWordList(4, glslBuiltin);

	myEd.ls.PropSet("fold", "1");

	// Set up the global default style. These attributes are used wherever no explicit choices are made.
	SetAStyle(myEd, STYLE_DEFAULT,     0xFFFFFFFF, 0xD0000000, 16, "c:/windows/fonts/cour.ttf");
	myEd.Command(SCI_STYLECLEARALL);	// Copies global style to all others
	SetAStyle(myEd, STYLE_INDENTGUIDE, 0xFFC0C0C0, 0xD0000000, 16, "c:/windows/fonts/cour.ttf");
	SetAStyle(myEd, STYLE_BRACELIGHT,  0xFF00FF00, 0xD0000000, 16, "c:/windows/fonts/cour.ttf");
	SetAStyle(myEd, STYLE_BRACEBAD,    0xFF0000FF, 0xD0000000, 16, "c:/windows/fonts/cour.ttf");
	SetAStyle(myEd, STYLE_LINENUMBER,  0xFFC0C0C0, 0xD0333333, 16, "c:/windows/fonts/cour.ttf");
	myEd.Command(SCI_SETFOLDMARGINCOLOUR,   1, 0xD01A1A1A);
	myEd.Command(SCI_SETFOLDMARGINHICOLOUR, 1, 0xD01A1A1A);
	myEd.Command(SCI_SETSELBACK,            1, 0xD0CC9966);
	myEd.Command(SCI_SETCARETFORE,          0xFFFFFFFF, 0);
	myEd.Command(SCI_SETCARETLINEVISIBLE,   1);
	myEd.Command(SCI_SETCARETLINEBACK,      0xFFFFFFFF);
	myEd.Command(SCI_SETCARETLINEBACKALPHA, 0x20);
	
	myEd.Command(SCI_SETMARGINWIDTHN, 0, 44);//Calculate correct width
	myEd.Command(SCI_SETMARGINWIDTHN, 1, 20);//Calculate correct width
	myEd.Command(SCI_SETMARGINMASKN, 1, SC_MASK_FOLDERS);//Calculate correct width

	for (int i = 0 ; i < NB_FOLDER_STATE ; i++)
	{
		myEd.Command(SCI_MARKERDEFINE, markersArray[FOLDER_TYPE][i], markersArray[4][i]);
		myEd.Command(SCI_MARKERSETBACK, markersArray[FOLDER_TYPE][i], 0xFF6A6A6A);
		myEd.Command(SCI_MARKERSETFORE, markersArray[FOLDER_TYPE][i], 0xFF333333);
	}

	myEd.Command(SCI_SETUSETABS, 1);
	myEd.Command(SCI_SETTABWIDTH, 4);
	myEd.Command(SCI_SETINDENTATIONGUIDES, SC_IV_REAL);

	SetAStyle(myEd, SCE_C_DEFAULT,      0xFFFFFFFF, 0xD0000000, 16, "c:/windows/fonts/cour.ttf");
	SetAStyle(myEd, SCE_C_WORD,         0xFF0066FF, 0xD0000000);
	SetAStyle(myEd, SCE_C_WORD2,        0xFFFFFF00, 0xD0000000);
	//WTF??? SetAStyle(SCE_C_GLOBALCLASS, 0xFF0000FF, 0xFF000000);
	SetAStyle(myEd, SCE_C_PREPROCESSOR, 0xFFC0C0C0, 0xD0000000);
	SetAStyle(myEd, SCE_C_NUMBER,       0xFF0080FF, 0xD0000000);
	SetAStyle(myEd, SCE_C_OPERATOR,     0xFF00CCFF, 0xD0000000);
	SetAStyle(myEd, SCE_C_COMMENT,      0xFF00FF00, 0xD0000000);
	SetAStyle(myEd, SCE_C_COMMENTLINE,  0xFF00FF00, 0xD0000000);
}

void DMApp::InitialiseEditor2() {
	myEd2.Command(SCI_SETSTYLEBITS, 7);

	// Set up the global default style. These attributes are used wherever no explicit choices are made.
	SetAStyle(myEd2, STYLE_DEFAULT,     0xFFFFFFFF, 0xD0000000, 16, "c:/windows/fonts/cour.ttf");
	myEd2.Command(SCI_STYLECLEARALL);	// Copies global style to all others
	myEd2.Command(SCI_SETMARGINWIDTHN, 1, 0);//Calculate correct width

	myEd2.Command(SCI_SETUSETABS, 1);
	myEd2.Command(SCI_SETTABWIDTH, 4);
	
	myEd2.Command(SCI_SETSELBACK,            1, 0xD0CC9966);
	myEd2.Command(SCI_SETCARETFORE,          0xFFFFFFFF, 0);
	myEd2.Command(SCI_SETCARETLINEVISIBLE,   1);
	myEd2.Command(SCI_SETCARETLINEBACK,      0xFFFFFFFF);
	myEd2.Command(SCI_SETCARETLINEBACKALPHA, 0x20);
}

void DMApp::InitialiseEditor3() {
	shaderList.Command(SCI_SETSTYLEBITS, 7);

	// Set up the global default style. These attributes are used wherever no explicit choices are made.
	SetAStyle(shaderList, STYLE_DEFAULT,     0xFFFFFFFF, 0xD0000000, 16, "c:/windows/fonts/cour.ttf");
	shaderList.Command(SCI_STYLECLEARALL);	// Copies global style to all others
	shaderList.Command(SCI_SETMARGINWIDTHN, 1, 0);//Calculate correct width

	shaderList.Command(SCI_SETUSETABS, 1);
	shaderList.Command(SCI_SETTABWIDTH, 4);
	
	shaderList.Command(SCI_SETSELBACK,            1, 0xD0CC9966);
	shaderList.Command(SCI_SETCARETFORE,          0x00FFFFFF, 0);
	shaderList.Command(SCI_SETCARETLINEVISIBLE,   1);
	shaderList.Command(SCI_SETCARETLINEBACK,      0xFFFFFFFF);
	shaderList.Command(SCI_SETCARETLINEBACKALPHA, 0x20);
}

#include <SDL.h>
#include <SDL_syswm.h>
#include <gl/glee.h>
#include "gldebug.h"

void Platform_Initialise(HWND hWnd);
void Platform_Finalise();

char fragmentSource[65536] = "void main()\n{\n\tgl_FragColor=vec4(0, 1, 0, 1);\n}\n";

GLuint program;
char errbuf[65536];

GLuint CompileProgram(GLint srcLen, const char* src, GLint errbufLen, char* errbuf)
{
	GLuint prg = glCreateProgram();
	GLuint shd = glCreateShader(GL_FRAGMENT_SHADER);
	GLint size = 0, result = 0;

	glShaderSource(shd, 1, &src, &srcLen);
	glCompileShader(shd);
	glGetShaderInfoLog(shd, errbufLen, &size, errbuf);
	glGetShaderiv(shd, GL_COMPILE_STATUS, &result);
	if (!result) goto onError;

	glAttachShader(prg, shd);
	glLinkProgram(prg);
	glGetProgramInfoLog(prg, errbufLen-size, &size, errbuf+size);
	glGetProgramiv(prg, GL_LINK_STATUS, &result);
	if (result) goto onSuccess;

onError:
	glDeleteProgram(prg);
	prg = 0;

onSuccess:
	glDeleteShader(shd);

	return prg;
}

bool isModEnabled(int mod, int modState)
{
	int mask = ~(KMOD_NUM|KMOD_CAPS|KMOD_MODE|mod);
	return (modState&mask)==0 && ((modState&mod)!=0 || mod==0); 
}

int curMode=0;
GLuint curPrg=0, curSh=0;
GLuint prgs[24], shdrs[24];
GLint attachedCount;

void saveShaderSource()
{
	if (curPrg&&curSh)
	{
		GLint lengthDoc = app.myEd.Command(SCI_GETLENGTH);
		TextRange tr;
		tr.chrg.cpMin = 0;
		tr.chrg.cpMax = lengthDoc;
		tr.lpstrText = fragmentSource;
		app.myEd.Command(SCI_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&tr));
		const char* ptr = fragmentSource;
		glShaderSource(curSh, 1, (const char**)&ptr, &lengthDoc);
	}
}

void compileProgram()
{
	saveShaderSource();
	GLint type;
	glGetShaderiv(curSh, GL_SHADER_TYPE, &type);
	GLuint sh = glCreateShader(type);
	const char* ptr=fragmentSource;
	glShaderSource(sh, 1, &ptr, 0);
	glCompileShader(sh);
	GLint size;
	glGetShaderInfoLog(sh, 65536, &size, errbuf);
	GLint res;
	glGetShaderiv(sh, GL_COMPILE_STATUS, &res);

	if (!res) {glDeleteShader(sh); return;}
	else
	{
		glCompileShader(curSh);
		glGetShaderiv(curSh, GL_COMPILE_STATUS, &res);
		assert(res);
	}

	GLuint prg = glCreateProgram();


	for (int i=0; i<attachedCount; ++i)
	{
		glAttachShader(prg, shdrs[i]==curSh?sh:shdrs[i]);
	}
	glDeleteShader(sh);
	glLinkProgram(prg);
	glGetProgramInfoLog(prg, 65536-size, &size, errbuf+size);
	glGetProgramiv(prg, GL_LINK_STATUS, &res);
	if (res)
	{
		glLinkProgram(curPrg);
		glGetProgramiv(curPrg, GL_LINK_STATUS, &res);
		assert(res);
	}
	glDeleteProgram(prg);
}

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

	mScreen = SDL_SetVideoMode(800, 600, 32, flags);
	if (!mScreen)
	{
		SDL_Quit();
		return 0;															// And Exit
	}
	
	SDL_EnableUNICODE(TRUE);
	SDL_EnableKeyRepeat(500, 100);

	SDL_SysWMinfo info = {{0, 0}, 0, 0};
	SDL_GetWMInfo(&info);

	Platform_Initialise(info.window);

	Surface* s = Surface::Allocate();
	app.myEd.drawSurface = s;
	app.myEd2.drawSurface = s;
	app.shaderList.drawSurface = s;

	float w=500, h=410;

	app.InitialiseEditor();
	//Set these values first otherwise there will be incorrect state
	app.myEd.SetSize(w, h);

	app.InitialiseEditor2();
	app.myEd2.SetSize(w, 100);

	app.InitialiseEditor3();
	app.shaderList.SetSize(w-300, h+100+25);

	app.myEd.Command(SCI_CANCEL);
	app.myEd.Command(SCI_SETUNDOCOLLECTION, 0);
	app.myEd.Command(SCI_SETUNDOCOLLECTION, 1);
	app.myEd.Command(EM_EMPTYUNDOBUFFER);
	app.myEd.Command(SCI_SETSAVEPOINT);
	app.myEd.Command(SCI_GOTOPOS, 0);
	app.myEd.Command(SCI_SETFOCUS, false);

	program = CompileProgram(strlen(fragmentSource), fragmentSource, sizeof(errbuf), errbuf);

	app.myEd2.Command(SCI_CANCEL);
	app.myEd2.Command(SCI_SETUNDOCOLLECTION, 0);
	app.myEd2.Command(SCI_SETUNDOCOLLECTION, 1);
	app.myEd2.Command(EM_EMPTYUNDOBUFFER);
	app.myEd2.Command(SCI_SETSAVEPOINT);
	app.myEd2.Command(SCI_GOTOPOS, 0);
	app.myEd2.Command(SCI_SETFOCUS, false);

	app.shaderList.Command(SCI_GOTOPOS, 0);
	app.shaderList.Command(SCI_SETFOCUS, true);

#ifdef SCI_LEXER
	Scintilla_LinkLexers();
#endif

	Uint8	prevKState[SDLK_LAST] = {0};
	bool run = true;
	bool visible = false;
	MyEditor* curEd = &app.shaderList;
	while (run)
	{
		SDL_Event	E;
		while (SDL_PollEvent(&E))
		{
			if (E.type==SDL_QUIT) run=false;
			else if (E.type == SDL_KEYDOWN)
			{
				if (E.key.keysym.sym==SDLK_ESCAPE) run=false;

				if (E.key.keysym.sym==SDLK_F5 && isModEnabled(0, E.key.keysym.mod))
				{
					visible = !visible;
					if (visible)
					{
						GLsizei cnt = gldGetProgramCount();
						gldGetPrograms(prgs, 24);

						app.shaderList.Command(SCI_CANCEL);
						app.shaderList.Command(SCI_SETUNDOCOLLECTION, 0);
						app.shaderList.Command(SCI_CLEARALL);
						for (int i=0; i<cnt; ++i)
						{
							char str[128];
							_snprintf(str, 128, "%sProgram #%d", (i==0)?"":"\n", prgs[i]);
							app.shaderList.Command(SCI_ADDTEXT, strlen(str), reinterpret_cast<LPARAM>(str));
						}
						app.shaderList.Command(SCI_SETUNDOCOLLECTION, 1);
						app.shaderList.Command(EM_EMPTYUNDOBUFFER);
						app.shaderList.Command(SCI_SETSAVEPOINT);
						app.shaderList.Command(SCI_GOTOPOS, 0);
						curMode=0;
						curSh=0;
						curPrg=0;
						app.myEd.Command(SCI_CANCEL);
						app.myEd.Command(SCI_CLEARALL);
						app.myEd.Command(SCI_SETUNDOCOLLECTION, 0);
						app.myEd.Command(SCI_SETUNDOCOLLECTION, 1);
						app.myEd.Command(EM_EMPTYUNDOBUFFER);
						app.myEd.Command(SCI_SETSAVEPOINT);
						app.myEd.Command(SCI_GOTOPOS, 0);
					}
				}
				
				if (!visible) continue;

				//Handle keys only when visible
				if (E.key.keysym.sym=='s' && isModEnabled(KMOD_CTRL, E.key.keysym.mod))
				{
					saveShaderSource();
				}
				if ('1'<=E.key.keysym.sym && E.key.keysym.sym<='3' && isModEnabled(KMOD_ALT, E.key.keysym.mod))
				{
					curEd->Command(SCI_SETFOCUS, false);
					switch(E.key.keysym.sym)
					{
						case '1': curEd=&app.shaderList; break;
						case '2': curEd=&app.myEd; break;
						case '3': curEd=&app.myEd2; break;
					}
					curEd->Command(SCI_SETFOCUS, true);
				}
				if (E.key.keysym.sym==SDLK_F7 && isModEnabled(0, E.key.keysym.mod)&&curSh&&curPrg)
				{
					compileProgram();
					//update result window
					app.myEd2.Command(SCI_CANCEL);
					app.myEd2.Command(SCI_SETUNDOCOLLECTION, 0);
					app.myEd2.Command(SCI_CLEARALL);
					app.myEd2.Command(SCI_ADDTEXT, strlen(errbuf), reinterpret_cast<LPARAM>((char*)errbuf));
					app.myEd2.Command(SCI_SETUNDOCOLLECTION, 1);
					app.myEd2.Command(EM_EMPTYUNDOBUFFER);
					app.myEd2.Command(SCI_SETSAVEPOINT);
					app.myEd2.Command(SCI_GOTOPOS, 0);
				}
				else if (curEd==&app.shaderList)
				{
					switch (E.key.keysym.sym)
					{
						case SDL_KEYUP:
						case SDL_KEYDOWN:
						{
							app.shaderList.OnKeyDown(E.key);
							break;
						}
						case SCK_RETURN:
							if (curMode==0)
							{
								int pos = (int)app.shaderList.Command(SCI_GETCURRENTPOS);
								int line = app.shaderList.Command(SCI_LINEFROMPOSITION, pos);
								curPrg = prgs[line];
								glGetAttachedShaders(curPrg, 24, &attachedCount, shdrs);
								app.shaderList.Command(SCI_CANCEL);
								app.shaderList.Command(SCI_SETUNDOCOLLECTION, 0);
								app.shaderList.Command(SCI_CLEARALL);
								for (int i=0; i<attachedCount; ++i)
								{
									char str[128];
									GLint type;
									glGetShaderiv(shdrs[i], GL_SHADER_TYPE, &type);
									_snprintf(str, 128, "%s%s #%d",
										(i==0)?"":"\n",
										type==GL_VERTEX_SHADER?"GL_VERTEX_SHADER":"GL_FRAGMENT_SHADER",
										shdrs[i]);
									app.shaderList.Command(SCI_ADDTEXT, strlen(str), reinterpret_cast<LPARAM>(str));
								}
								app.shaderList.Command(SCI_SETUNDOCOLLECTION, 1);
								app.shaderList.Command(EM_EMPTYUNDOBUFFER);
								app.shaderList.Command(SCI_SETSAVEPOINT);
								app.shaderList.Command(SCI_GOTOPOS, 0);
								curMode=1;
							}
							else
							{
								int pos = (int)app.shaderList.Command(SCI_GETCURRENTPOS);
								int line = app.shaderList.Command(SCI_LINEFROMPOSITION, pos);
								
								GLint len;
								char buf[65536];
								curSh = shdrs[line];
								glGetShaderSource(curSh, 65536, &len, buf);

								app.myEd.Command(SCI_CANCEL);
								app.myEd.Command(SCI_CLEARALL);
								app.myEd.Command(SCI_SETUNDOCOLLECTION, 0);
								app.myEd.Command(SCI_ADDTEXT, len, reinterpret_cast<LPARAM>(buf));
								app.myEd.Command(SCI_SETUNDOCOLLECTION, 1);
								app.myEd.Command(EM_EMPTYUNDOBUFFER);
								app.myEd.Command(SCI_SETSAVEPOINT);
								app.myEd.Command(SCI_GOTOPOS, 0);
							}
							break;
						case SCK_BACK:
							if (curMode==1)
							{
								GLsizei cnt = gldGetProgramCount();
								gldGetPrograms(prgs, 24);

								app.shaderList.Command(SCI_CANCEL);
								app.shaderList.Command(SCI_SETUNDOCOLLECTION, 0);
								app.shaderList.Command(SCI_CLEARALL);
								for (int i=0; i<cnt; ++i)
								{
									char str[128];
									_snprintf(str, 128, "%sProgram #%d", (i==0)?"":"\n", prgs[i]);
									app.shaderList.Command(SCI_ADDTEXT, strlen(str), reinterpret_cast<LPARAM>(str));
								}
								app.shaderList.Command(SCI_SETUNDOCOLLECTION, 1);
								app.shaderList.Command(EM_EMPTYUNDOBUFFER);
								app.shaderList.Command(SCI_SETSAVEPOINT);
								app.shaderList.Command(SCI_GOTOPOS, 0);
								curMode=0;
							}
							break;
					}

				}
				else
				{
					if (!(curEd==&app.myEd2 && E.key.keysym.unicode>=32))
						curEd->OnKeyDown(E.key);
				}
			}
		}

		Uint8* curState = SDL_GetKeyState(0);
		
		glClearColor(0.08f, 0.18f, 0.18f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glUseProgram(program);
		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(-0.80f, -0.80f);
		glTexCoord2f(1.0f, 0.0f); glVertex2f( 0.80f, -0.80f);
		glTexCoord2f(1.0f, 1.0f); glVertex2f( 0.80f,  0.80f);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(-0.80f,  0.80f);
		glEnd();
		glUseProgram(0);

		if (visible)
		{
			glUseProgram(0);
			glLoadIdentity();

			glOrtho(0, 800, 0, 600, 0, 500);
			glTranslatef(0, 600, 0);
			glScalef(1, -1, 1);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glTranslatef(30, 30, 0);

			app.shaderList.Paint();

			glTranslatef(w-300+30, 0, 0);

			app.myEd.Paint();

			glTranslatef(0, 430, 0);

			app.myEd2.Paint();

		}

		SDL_GL_SwapBuffers();

		memcpy(prevKState, curState, SDLK_LALT);
	}

	if (program) glDeleteProgram(program);

	s->Release();

	Platform_Finalise();

	SDL_Quit();

	return 0;
}
