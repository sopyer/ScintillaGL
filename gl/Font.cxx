// ScintillaGL source code edit control
// Font.cxx - implementation of font on OpenGL
// Copyright 2011 by Mykhailo Parfeniuk
// The License.txt file describes the conditions under which this software may be distributed.

//#include <string.h>
#include <stdio.h>
//#include <stdlib.h>
//#include <stddef.h>
//#include <math.h>
#include <assert.h>

#include "Platform.h"

//#include "Scintilla.h"
//#include "UniConversion.h"
#include "XPM.h"

#include <gl/glee.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include "DefaultFontData.h"

struct stbtt_Font
{
	stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
	GLuint ftex;
};

Font::Font() : fid(0)
{
	//assert(0);
}

Font::~Font()
{
	//assert(0);
}

stbtt_Font defaultFont;

namespace platform
{
	void InitializeFontSubsytem()
	{
		unsigned char* bmp = (unsigned char*)malloc(512*512);

		stbtt_BakeFontBitmap(anonymousProRTTF, 0, 32.0, bmp, 512, 512, 32, 96, defaultFont.cdata); // no guarantee this fits!

		glGenTextures(1, &defaultFont.ftex);
		glBindTexture(GL_TEXTURE_2D, defaultFont.ftex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, bmp);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		//TODO: Crash if uncomment previous line - need further investigation
		//free(bmp);
	}

	void ShutdownFontSubsytem()
	{
		//glDeleteTextures(1, &defaultFont.ftex);
	}
}

void Font::Create(const char *faceName, int characterSet, int size,	bool bold, bool italic, int)
{
	stbtt_Font* newFont = new stbtt_Font;
	size_t len;

	FILE* f = fopen("c:/windows/fonts/times.ttf", "rb");

	fseek(f, 0, SEEK_END);
	len = ftell(f);
	fseek(f, 0, SEEK_SET);

	unsigned char* buf = new unsigned char[len];
	unsigned char* bmp = new unsigned char[512*512];
	fread(buf, 1, len, f);
	stbtt_BakeFontBitmap(buf, 0, 32.0, bmp, 512, 512, 32, 96, newFont->cdata); // no guarantee this fits!
	// can free ttf_buffer at this point
	glGenTextures(1, &newFont->ftex);
	glBindTexture(GL_TEXTURE_2D, newFont->ftex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, bmp);
	// can free temp_bitmap at this point
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	fclose(f);

	delete [] buf;
	delete [] bmp;

	fid = newFont;
}

void Font::Release()
{
	if (fid)
	{
		glDeleteTextures(1, &((stbtt_Font*)fid)->ftex);
		delete (stbtt_Font*)fid;
	}
}
