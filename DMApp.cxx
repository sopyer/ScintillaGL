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
#include "gl/stb_truetype.h"

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

const char htmlKeyWords[] = 
	"a abbr acronym address applet area b base basefont "
	"bdo big blockquote body br button caption center "
	"cite code col colgroup dd del dfn dir div dl dt em "
	"fieldset font form frame frameset h1 h2 h3 h4 h5 h6 "
	"head hr html i iframe img input ins isindex kbd label "
	"legend li link map menu meta noframes noscript "
	"object ol optgroup option p param pre q s samp "
	"script select small span strike strong style sub sup "
	"table tbody td textarea tfoot th thead title tr tt u ul "
	"var xmlns "
	"abbr accept-charset accept accesskey action align alink "
	"alt archive axis background bgcolor border "
	"cellpadding cellspacing char charoff charset checked cite "
	"class classid clear codebase codetype color cols colspan "
	"compact content coords "
	"data datafld dataformatas datapagesize datasrc datetime "
	"declare defer dir disabled enctype "
	"face for frame frameborder "
	"headers height href hreflang hspace http-equiv "
	"id ismap label lang language link longdesc "
	"marginwidth marginheight maxlength media method multiple "
	"name nohref noresize noshade nowrap "
	"object onblur onchange onclick ondblclick onfocus "
	"onkeydown onkeypress onkeyup onload onmousedown "
	"onmousemove onmouseover onmouseout onmouseup "
	"onreset onselect onsubmit onunload "
	"profile prompt readonly rel rev rows rowspan rules "
	"scheme scope shape size span src standby start style "
	"summary tabindex target text title type usemap "
	"valign value valuetype version vlink vspace width "
	"text password checkbox radio submit reset "
	"file hidden image "
	"public !doctype xml";

const char jsKeyWords[] = 
	"break case catch continue default "
	"do else for function if return throw try var while";

const char vbsKeyWords[] = 
	"and as byref byval case call const "
	"continue dim do each else elseif end error exit false for function global "
	"goto if in loop me new next not nothing on optional or private public "
	"redim rem resume select set sub then to true type while with "
	"boolean byte currency date double integer long object single string type "
	"variant";

void DMApp::InitialiseEditor() {
	SendEditor(SCI_SETLEXER, SCLEX_HTML);
	SendEditor(SCI_SETSTYLEBITS, 7);

	SendEditor(SCI_SETKEYWORDS, 0, 
		reinterpret_cast<LPARAM>(htmlKeyWords));
	SendEditor(SCI_SETKEYWORDS, 1, 
		reinterpret_cast<LPARAM>(jsKeyWords));
	SendEditor(SCI_SETKEYWORDS, 2, 
		reinterpret_cast<LPARAM>(vbsKeyWords));

	// Set up the global default style. These attributes are used wherever no explicit choices are made.
	SetAStyle(STYLE_DEFAULT, black, white, 11, "Verdana");
	SendEditor(SCI_STYLECLEARALL);	// Copies global style to all others

	const COLORREF red = RGB(0xFF, 0, 0);
	const COLORREF offWhite = RGB(0xFF, 0xFB, 0xF0);
	const COLORREF darkGreen = RGB(0, 0x80, 0);
	const COLORREF darkBlue = RGB(0, 0, 0x80);

	// Hypertext default is used for all the document's text
	SetAStyle(SCE_H_DEFAULT, black, white, 11, "Times New Roman");
	
	// Unknown tags and attributes are highlighed in red. 
	// If a tag is actually OK, it should be added in lower case to the htmlKeyWords string.
	SetAStyle(SCE_H_TAG, darkBlue);
	SetAStyle(SCE_H_TAGUNKNOWN, red);
	SetAStyle(SCE_H_ATTRIBUTE, darkBlue);
	SetAStyle(SCE_H_ATTRIBUTEUNKNOWN, red);
	SetAStyle(SCE_H_NUMBER, RGB(0x80,0,0x80));
	SetAStyle(SCE_H_DOUBLESTRING, RGB(0,0x80,0));
	SetAStyle(SCE_H_SINGLESTRING, RGB(0,0x80,0));
	SetAStyle(SCE_H_OTHER, RGB(0x80,0,0x80));
	SetAStyle(SCE_H_COMMENT, RGB(0x80,0x80,0));
	SetAStyle(SCE_H_ENTITY, RGB(0x80,0,0x80));

	SetAStyle(SCE_H_TAGEND, darkBlue);
	SetAStyle(SCE_H_XMLSTART, darkBlue);	// <?
	SetAStyle(SCE_H_XMLEND, darkBlue);		// ?>
	SetAStyle(SCE_H_SCRIPT, darkBlue);		// <script
	SetAStyle(SCE_H_ASP, RGB(0x4F, 0x4F, 0), RGB(0xFF, 0xFF, 0));	// <% ... %>
	SetAStyle(SCE_H_ASPAT, RGB(0x4F, 0x4F, 0), RGB(0xFF, 0xFF, 0));	// <%@ ... %>

	SetAStyle(SCE_HB_DEFAULT, black);
	SetAStyle(SCE_HB_COMMENTLINE, darkGreen);
	SetAStyle(SCE_HB_NUMBER, RGB(0,0x80,0x80));
	SetAStyle(SCE_HB_WORD, darkBlue);
	SendEditor(SCI_STYLESETBOLD, SCE_HB_WORD, 1);
	SetAStyle(SCE_HB_STRING, RGB(0x80,0,0x80));
	SetAStyle(SCE_HB_IDENTIFIER, black);
	
	// This light blue is found in the windows system palette so is safe to use even in 256 colour modes.
	const COLORREF lightBlue = RGB(0xA6, 0xCA, 0xF0);
	// Show the whole section of VBScript with light blue background
	for (int bstyle=SCE_HB_DEFAULT; bstyle<=SCE_HB_STRINGEOL; bstyle++) {
		SendEditor(SCI_STYLESETFONT, bstyle, 
			reinterpret_cast<LPARAM>("Georgia"));
		SendEditor(SCI_STYLESETBACK, bstyle, lightBlue);
		// This call extends the backround colour of the last style on the line to the edge of the window
		SendEditor(SCI_STYLESETEOLFILLED, bstyle, 1);
	}
	SendEditor(SCI_STYLESETBACK, SCE_HB_STRINGEOL, RGB(0x7F,0x7F,0xFF));
	SendEditor(SCI_STYLESETFONT, SCE_HB_COMMENTLINE, 
		reinterpret_cast<LPARAM>("Comic Sans MS"));

	SetAStyle(SCE_HBA_DEFAULT, black);
	SetAStyle(SCE_HBA_COMMENTLINE, darkGreen);
	SetAStyle(SCE_HBA_NUMBER, RGB(0,0x80,0x80));
	SetAStyle(SCE_HBA_WORD, darkBlue);
	SendEditor(SCI_STYLESETBOLD, SCE_HBA_WORD, 1);
	SetAStyle(SCE_HBA_STRING, RGB(0x80,0,0x80));
	SetAStyle(SCE_HBA_IDENTIFIER, black);
	
	// Show the whole section of ASP VBScript with bright yellow background
	for (int bastyle=SCE_HBA_DEFAULT; bastyle<=SCE_HBA_STRINGEOL; bastyle++) {
		SendEditor(SCI_STYLESETFONT, bastyle, 
			reinterpret_cast<LPARAM>("Georgia"));
		SendEditor(SCI_STYLESETBACK, bastyle, RGB(0xFF, 0xFF, 0));
		// This call extends the backround colour of the last style on the line to the edge of the window
		SendEditor(SCI_STYLESETEOLFILLED, bastyle, 1);
	}
	SendEditor(SCI_STYLESETBACK, SCE_HBA_STRINGEOL, RGB(0xCF,0xCF,0x7F));
	SendEditor(SCI_STYLESETFONT, SCE_HBA_COMMENTLINE, 
		reinterpret_cast<LPARAM>("Comic Sans MS"));
		
	// If there is no need to support embedded Javascript, the following code can be dropped.
	// Javascript will still be correctly processed but will be displayed in just the default style.
	
	SetAStyle(SCE_HJ_START, RGB(0x80,0x80,0));
	SetAStyle(SCE_HJ_DEFAULT, black);
	SetAStyle(SCE_HJ_COMMENT, darkGreen);
	SetAStyle(SCE_HJ_COMMENTLINE, darkGreen);
	SetAStyle(SCE_HJ_COMMENTDOC, darkGreen);
	SetAStyle(SCE_HJ_NUMBER, RGB(0,0x80,0x80));
	SetAStyle(SCE_HJ_WORD, black);
	SetAStyle(SCE_HJ_KEYWORD, darkBlue);
	SetAStyle(SCE_HJ_DOUBLESTRING, RGB(0x80,0,0x80));
	SetAStyle(SCE_HJ_SINGLESTRING, RGB(0x80,0,0x80));
	SetAStyle(SCE_HJ_SYMBOLS, black);

	SetAStyle(SCE_HJA_START, RGB(0x80,0x80,0));
	SetAStyle(SCE_HJA_DEFAULT, black);
	SetAStyle(SCE_HJA_COMMENT, darkGreen);
	SetAStyle(SCE_HJA_COMMENTLINE, darkGreen);
	SetAStyle(SCE_HJA_COMMENTDOC, darkGreen);
	SetAStyle(SCE_HJA_NUMBER, RGB(0,0x80,0x80));
	SetAStyle(SCE_HJA_WORD, black);
	SetAStyle(SCE_HJA_KEYWORD, darkBlue);
	SetAStyle(SCE_HJA_DOUBLESTRING, RGB(0x80,0,0x80));
	SetAStyle(SCE_HJA_SINGLESTRING, RGB(0x80,0,0x80));
	SetAStyle(SCE_HJA_SYMBOLS, black);

	// Show the whole section of Javascript with off white background
	for (int jstyle=SCE_HJ_DEFAULT; jstyle<=SCE_HJ_SYMBOLS; jstyle++) {
		SendEditor(SCI_STYLESETFONT, jstyle, 
			reinterpret_cast<LPARAM>("Lucida Sans Unicode"));
		SendEditor(SCI_STYLESETBACK, jstyle, offWhite);
		SendEditor(SCI_STYLESETEOLFILLED, jstyle, 1);
	}
	SendEditor(SCI_STYLESETBACK, SCE_HJ_STRINGEOL, RGB(0xDF, 0xDF, 0x7F));
	SendEditor(SCI_STYLESETEOLFILLED, SCE_HJ_STRINGEOL, 1);

	// Show the whole section of Javascript with brown background
	for (int jastyle=SCE_HJA_DEFAULT; jastyle<=SCE_HJA_SYMBOLS; jastyle++) {
		SendEditor(SCI_STYLESETFONT, jastyle, 
			reinterpret_cast<LPARAM>("Lucida Sans Unicode"));
		SendEditor(SCI_STYLESETBACK, jastyle, RGB(0xDF, 0xDF, 0x7F));
		SendEditor(SCI_STYLESETEOLFILLED, jastyle, 1);
	}
	SendEditor(SCI_STYLESETBACK, SCE_HJA_STRINGEOL, RGB(0x0,0xAF,0x5F));
	SendEditor(SCI_STYLESETEOLFILLED, SCE_HJA_STRINGEOL, 1);
}

#include <SDL.h>
#include <gl/glee.h>

unsigned char ttf_buffer[1<<20];
unsigned char temp_bitmap[512*512];

stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
GLuint ftex;

void my_stbtt_initfont(void)
{
	FILE* f;
	fread(ttf_buffer, 1, 1<<20, f = fopen("c:/windows/fonts/times.ttf", "rb"));
	stbtt_BakeFontBitmap(ttf_buffer, 0, 32.0, temp_bitmap, 512, 512, 32, 96, cdata); // no guarantee this fits!
	// can free ttf_buffer at this point
	glGenTextures(1, &ftex);
	glBindTexture(GL_TEXTURE_2D, ftex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap);
	// can free temp_bitmap at this point
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	fclose(f);
}

void my_stbtt_print(float x, float y, char *text)
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	// assume orthographic projection with units = screen pixels, origin at top left
	glBindTexture(GL_TEXTURE_2D, ftex);
	glBegin(GL_QUADS);
	while (*text) {
		if (*text >= 32 && *text < 128) {
			stbtt_aligned_quad q;
			stbtt_GetBakedQuad(cdata, 512,512, *text-32, &x,&y,&q,1);//1=opengl,0=old d3d
			glTexCoord2f(q.s0,q.t0); glVertex2f(q.x0,q.y0);
			glTexCoord2f(q.s1,q.t0); glVertex2f(q.x1,q.y0);
			glTexCoord2f(q.s1,q.t1); glVertex2f(q.x1,q.y1);
			glTexCoord2f(q.s0,q.t1); glVertex2f(q.x0,q.y1);
		}
		++text;
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void Platform_Initialise();
void Platform_Finalise();

int main(int argc, char* argv[])
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

	if(!(mScreen = SDL_SetVideoMode(800, 600, 32, flags)))
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
	my_stbtt_initfont();

	app.InitialiseEditor();

	char str[] = "<HTML>\n\tHello Scintilla!\n</HTML>";
	app.SendEditor(SCI_CANCEL);
	app.SendEditor(SCI_SETUNDOCOLLECTION, 0);
	app.SendEditor(SCI_ADDTEXT, sizeof(str), reinterpret_cast<LPARAM>((char*)str));
	app.SendEditor(SCI_SETUNDOCOLLECTION, 1);
	app.SendEditor(EM_EMPTYUNDOBUFFER);
	app.SendEditor(SCI_SETSAVEPOINT);
	app.SendEditor(SCI_GOTOPOS, 0);

#ifdef SCI_LEXER
	//Scintilla_LinkLexers();
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
					default:					sciKey = E.key.keysym.sym;
				}

				if (SDLK_a<=sciKey && sciKey<=SDLK_z)
					sciKey = sciKey-'a'+'A';

				bool consumed;

				app.myEd.KeyDown(sciKey,
					E.key.keysym.mod&KMOD_LSHIFT | E.key.keysym.mod&KMOD_RSHIFT,
					E.key.keysym.mod&KMOD_LCTRL | E.key.keysym.mod&KMOD_RCTRL,
					E.key.keysym.mod&KMOD_LALT | E.key.keysym.mod&KMOD_RALT,
					&consumed
				);
				if (!consumed && sciKey>=32 && sciKey<=128)
					app.myEd.AddCharUTF(E.key.keysym.unicode);
			}
		}

		Uint8* curState = SDL_GetKeyState(0);
		
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
		GLenum err = glGetError();
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glBegin(GL_TRIANGLES);
		glColor3f(1, 0, 0);
		glVertex2f(0, 0.5f);
		glColor3f(0, 1, 0);
		glVertex2f(-0.5f, -0.5f);
		glColor3f(0, 0, 1);
		glVertex2f(0.5f, -0.5f);
		glEnd();

		glOrtho(0, 800, 0, 600, 0, 500);
		glTranslatef(0, 600, 0);
		glScalef(1, -1, 1);
		app.myEd.Paint(/*s,*/ rcPaint);
		glColor3f(1, 0, 0);
		my_stbtt_print(100, 100, "Hello world!!!");

		SDL_GL_SwapBuffers();

		memcpy(prevKState, curState, SDLK_LALT);
	}

	s->Release();

	Platform_Finalise();

	SDL_Quit();

	return 0;
}
