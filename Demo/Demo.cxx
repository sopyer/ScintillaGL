// A simple demonstration application using Scintilla
#include <stdio.h>
#include "ShaderEditOverlay.h"
#include <SDL.h>
#include <SDL_syswm.h>
#include <gl/glee.h>

char fragmentSource[65536] = "void main()\n{\n\tgl_FragColor=vec4(0, 1, 0, 1);\n}\n";

static ShaderEditOverlay app;

void Platform_Initialise(HWND hWnd);
void Platform_Finalise();

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

	app.initialise(800, 600);

	program = CompileProgram(strlen(fragmentSource), fragmentSource, sizeof(errbuf), errbuf);

	app.addPrograms(1, &program);
	Scintilla_LinkLexers();

	bool run = true;
	bool visible = false;

	while (run)
	{
		SDL_Event	E;
		while (SDL_PollEvent(&E))
		{
			if (E.type==SDL_QUIT) run=false;
			else if (E.type == SDL_KEYDOWN)
			{
				if (E.key.keysym.sym==SDLK_ESCAPE) run=false;

				if (E.key.keysym.sym==SDLK_F5)
				{
					visible = !visible;
					if (visible)
					{
						app.reset();
					}
				}
				
				if (!visible) continue;
				app.handleKeyDown(E.key);
			}
		}

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
			app.renderFullscreen();
		}

		SDL_GL_SwapBuffers();

	}

	if (program) glDeleteProgram(program);

	Platform_Finalise();

	SDL_Quit();

	return 0;
}
