// ScintillaGL source code edit control
// Surface.cxx - implementation of surface on OpenGL
// Copyright 2011 by Mykhailo Parfeniuk
// The License.txt file describes the conditions under which this software may be distributed.

//#include <string.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <stddef.h>
//#include <math.h>
#include <assert.h>

#include "Platform.h"

//#include "Scintilla.h"
//#include "UniConversion.h"
#include "XPM.h"

#include <gl/glee.h>

enum encodingType { singleByte, UTF8, dbcs};

// Required on OS X
#ifdef SCI_NAMESPACE
namespace Scintilla {
#endif
class SurfaceImpl : public Surface {
	//encodingType et;
	Colour penColour;
	int x;
	int y;
	bool inited;
	bool createdGC;
	int characterSet;
	void SetConverter(int characterSet_);
public:
	SurfaceImpl();
	virtual ~SurfaceImpl();

	void Init(WindowID wid);
	void Init(SurfaceID sid, WindowID wid);
	void InitPixMap(int width, int height, Surface *surface_, WindowID wid);

	void Release();
	bool Initialised();
	void PenColour(Colour/*Allocated*/ fore);
	int LogPixelsY();
	float DeviceHeightFont(int points);
	void MoveTo(float x_, float y_);
	void LineTo(float x_, float y_);
	void Polygon(Point *pts, int npts, Colour/*Allocated*/ fore, Colour/*Allocated*/ back);
	void RectangleDraw(PRectangle rc, Colour/*Allocated*/ fore, Colour/*Allocated*/ back);
	void FillRectangle(PRectangle rc, Colour/*Allocated*/ back);
	void FillRectangle(PRectangle rc, Surface &surfacePattern);
	void RoundedRectangle(PRectangle rc, Colour/*Allocated*/ fore, Colour/*Allocated*/ back);
	void AlphaRectangle(PRectangle rc, int cornerSize, Colour/*Allocated*/ fill, int alphaFill,
		Colour/*Allocated*/ outline, int alphaOutline, int flags);
	void Ellipse(PRectangle rc, Colour/*Allocated*/ fore, Colour/*Allocated*/ back);

	//TODO: Remove
	void Copy(PRectangle rc, Point from, Surface &surfaceSource);
	virtual void DrawPixmap(PRectangle rc, Pixmap pixmap);

	void DrawTextBase(PRectangle rc, Font &font_, float ybase, const char *s, int len, Colour/*Allocated*/ fore);
	void DrawTextNoClip(PRectangle rc, Font &font_, float ybase, const char *s, int len, Colour/*Allocated*/ fore, Colour/*Allocated*/ back);
	void DrawTextClipped(PRectangle rc, Font &font_, float ybase, const char *s, int len, Colour/*Allocated*/ fore, Colour/*Allocated*/ back);
	void DrawTextTransparent(PRectangle rc, Font &font_, float ybase, const char *s, int len, Colour/*Allocated*/ fore);
	void MeasureWidths(Font &font_, const char *s, int len, float *positions);
	float WidthText(Font &font_, const char *s, int len);
	float WidthChar(Font &font_, char ch);
	float Ascent(Font &font_);
	float Descent(Font &font_);
	float InternalLeading(Font &font_);
	float ExternalLeading(Font &font_);
	float Height(Font &font_);
	float AverageCharWidth(Font &font_);

	//int SetPalette(Palette *pal, bool inBackGround);
	void SetClip(PRectangle rc);
	void FlushCachedState();

	void SetUnicodeMode(bool unicodeMode_);
	void SetDBCSMode(int codePage);
};
#ifdef SCI_NAMESPACE
}
#endif


void SurfaceImpl::SetConverter(int /*characterSet_*/) {
	assert(0);
}

SurfaceImpl::SurfaceImpl() : //et(singleByte),
x(0), y(0), inited(false), createdGC(false)
, characterSet(-1) {
	//assert(0);
}

SurfaceImpl::~SurfaceImpl() {
	//assert(0);
}

void SurfaceImpl::Release() {
	//assert(0);
}

bool SurfaceImpl::Initialised() {
	//assert(0);
	return true;//return inited;
}

void SurfaceImpl::Init(WindowID /*wid*/) {
	assert(0);
	inited = true;
}

void SurfaceImpl::Init(SurfaceID /*sid*/, WindowID /*wid*/) {
	assert(0);
	inited = true;
}

void SurfaceImpl::InitPixMap(int /*width*/, int /*height*/, Surface* /*surface_*/, WindowID /*wid*/) {
	assert(0);
	createdGC = true;
	inited = true;
}

void SurfaceImpl::PenColour(Colour/*Allocated*/ fore) {
	//assert(0);
	penColour = fore;
}

int SurfaceImpl::LogPixelsY() {
	return 72;
}

float SurfaceImpl::DeviceHeightFont(int points) {
	//assert(0);
	int logPix = LogPixelsY();
	return (points * logPix + logPix / 2) / 72;
}

void SurfaceImpl::MoveTo(float x_, float y_) {
	assert(0);
	x = x_;
	y = y_;
}

void SurfaceImpl::LineTo(float /*x_*/, float /*y_*/) {
	assert(0);
}

void SurfaceImpl::Polygon(Point* /*pts*/, int /*npts*/, Colour/*Allocated*/ /*fore*/,
                          Colour/*Allocated*/ /*back*/) {
	assert(0);
}

void SurfaceImpl::RectangleDraw(PRectangle /*rc*/, Colour/*Allocated*/ /*fore*/, Colour/*Allocated*/ /*back*/) {
	assert(0);
}

struct PixmapInternal
{
	GLuint tex;
	float scalex, scaley;
	bool initialised;
};

Pixmap	CreatePixmap()
{
	Pixmap pm = new PixmapInternal;
	pm->scalex = 0;
	pm->scaley = 0;
	pm->initialised = false;

	return pm;
}

bool	IsPixmapInitialised(Pixmap pixmap)
{
	return pixmap->initialised;
}

void	DestroyPixmap(Pixmap pixmap)
{
	glDeleteTextures(1, &pixmap->tex);
	delete pixmap;
}

void	UpdatePixmap(Pixmap pixmap, int w, int h, int* data)
{
	if (!pixmap->initialised)
	{
		glGenTextures(1, &pixmap->tex);
		glBindTexture(GL_TEXTURE_2D, pixmap->tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, pixmap->tex);
	}

	pixmap->initialised = true;
	pixmap->scalex = 1.0f/w;
	pixmap->scaley = 1.0f/h;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void SurfaceImpl::DrawPixmap(PRectangle rc, Pixmap pixmap)
{
	float w = (rc.right-rc.left)*pixmap->scalex, h=(rc.bottom-rc.top)*pixmap->scaley;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, pixmap->tex);
	glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(rc.left,  rc.top);
	glTexCoord2f(w, 0);
	glVertex2f(rc.right, rc.top);
	glTexCoord2f(w, h);
	glVertex2f(rc.right, rc.bottom);
	glTexCoord2f(0, h);
	glVertex2f(rc.left,  rc.bottom);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}

void SurfaceImpl::FillRectangle(PRectangle rc, Colour/*Allocated*/ back) {
	glColor4ubv((GLubyte*)&back);
	glBegin(GL_QUADS);
	glVertex2f(rc.left,  rc.top);
	glVertex2f(rc.right, rc.top);
	glVertex2f(rc.right, rc.bottom);
	glVertex2f(rc.left,  rc.bottom);
	glEnd();
	//assert(0);
}

void SurfaceImpl::FillRectangle(PRectangle /*rc*/, Surface &/*surfacePattern*/) {
	assert(0);
}

void SurfaceImpl::RoundedRectangle(PRectangle /*rc*/, Colour/*Allocated*/ /*fore*/, Colour/*Allocated*/ /*back*/) {
	assert(0);
}

//static unsigned int GetRValue(unsigned int co) {
//	return (co >> 16) & 0xff;
//}
//
//static unsigned int GetGValue(unsigned int co) {
//	return (co >> 8) & 0xff;
//}
//
//static unsigned int GetBValue(unsigned int co) {
//	return co & 0xff;
//}

void SurfaceImpl::AlphaRectangle(PRectangle /*rc*/, int /*cornerSize*/, Colour/*Allocated*/ /*fill*/, int /*alphaFill*/,
		Colour/*Allocated*/ /*outline*/, int /*alphaOutline*/, int /*flags*/) {
	assert(0);
}

void SurfaceImpl::Ellipse(PRectangle /*rc*/, Colour/*Allocated*/ /*fore*/, Colour/*Allocated*/ /*back*/) {
	assert(0);
}

void SurfaceImpl::Copy(PRectangle /*rc*/, Point /*from*/, Surface &/*surfaceSource*/) {
	assert(0);
}

char *UTF8FromLatin1(const char *s, int &len) {
	char *utfForm = new char[len*2+1];
	size_t lenU = 0;
	for (int i=0;i<len;i++) {
		unsigned int uch = static_cast<unsigned char>(s[i]);
		if (uch < 0x80) {
			utfForm[lenU++] = uch;
		} else {
			utfForm[lenU++] = static_cast<char>(0xC0 | (uch >> 6));
			utfForm[lenU++] = static_cast<char>(0x80 | (uch & 0x3f));
		}
	}
	utfForm[lenU] = '\0';
	len = lenU;
	return utfForm;
}

static char *UTF8FromDBCS(const char* /*s*/, int& /*len*/) {
	return 0;
}

static size_t UTF8CharLength(const char *s) {
	const unsigned char *us = reinterpret_cast<const unsigned char *>(s);
	unsigned char ch = *us;
	if (ch < 0x80) {
		return 1;
	} else if (ch < 0x80 + 0x40 + 0x20) {
		return 2;
	} else {
		return 3;
	}
}

// On GTK+, wchar_t is 4 bytes

const int maxLengthTextRun = 10000;

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
	stbtt_fontinfo	fontinfo;
	stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
	GLuint ftex;
	float scale;
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

		stbtt_BakeFontBitmap(anonymousProRTTF, 0, 12.0*2, bmp, 512, 512, 32, 96, defaultFont.cdata); // no guarantee this fits!

		glGenTextures(1, &defaultFont.ftex);
		glBindTexture(GL_TEXTURE_2D, defaultFont.ftex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, bmp);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		//TODO: Crash if uncomment previous line - need further investigation
		free(bmp);
	}

	void ShutdownFontSubsytem()
	{
		glDeleteTextures(1, &defaultFont.ftex);
	}
}

void Font::Create(const char *faceName, int /*characterSet*/, int size,	bool /*bold*/, bool /*italic*/, int)
{
	stbtt_Font* newFont = new stbtt_Font;
	size_t len;

	FILE* f = fopen(faceName, "rb");

	assert(f);

	fseek(f, 0, SEEK_END);
	len = ftell(f);
	fseek(f, 0, SEEK_SET);

	unsigned char* buf = (unsigned char*)malloc(len);
	unsigned char* bmp = new unsigned char[512*512];
	fread(buf, 1, len, f);
	stbtt_BakeFontBitmap(buf, 0, size, bmp, 512, 512, 32, 96, newFont->cdata); // no guarantee this fits!
	// can free ttf_buffer at this point
	glGenTextures(1, &newFont->ftex);
	glBindTexture(GL_TEXTURE_2D, newFont->ftex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, bmp);
	// can free temp_bitmap at this point
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	fclose(f);

	stbtt_InitFont(&newFont->fontinfo, buf, 0);

	newFont->scale = stbtt_ScaleForPixelHeight(&newFont->fontinfo, size);


	delete [] bmp;

	fid = newFont;
}

void Font::Release()
{
	if (fid)
	{
		free(((stbtt_Font*)fid)->fontinfo.data);
		glDeleteTextures(1, &((stbtt_Font*)fid)->ftex);
		delete (stbtt_Font*)fid;
	}
}

void SurfaceImpl::DrawTextBase(PRectangle rc, Font &font_, float ybase, const char *s, int len,
                                 Colour/*Allocated*/ fore) {
	//assert(0);
	stbtt_Font* realFont = (stbtt_Font*)font_.GetID();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// assume orthographic projection with units = screen pixels, origin at top left
	glBindTexture(GL_TEXTURE_2D, realFont->ftex);
	glBegin(GL_QUADS);
	glColor3ubv((GLubyte*)&fore);
	float x = rc.left, y=ybase;
	while (len--) {
		if (*s >= 32 && *s < 128) {
			stbtt_aligned_quad q;
			stbtt_GetBakedQuad(realFont->cdata, 512,512, *s-32, &x,&y,&q,1);//1=opengl,0=old d3d
			//x = floor(x);
			glTexCoord2f(q.s0,q.t0); glVertex2f(q.x0,q.y0);
			glTexCoord2f(q.s1,q.t0); glVertex2f(q.x1,q.y0);
			glTexCoord2f(q.s1,q.t1); glVertex2f(q.x1,q.y1);
			glTexCoord2f(q.s0,q.t1); glVertex2f(q.x0,q.y1);
		}
		++s;
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void SurfaceImpl::DrawTextNoClip(PRectangle rc, Font &font_, float ybase, const char *s, int len,
                                 Colour/*Allocated*/ fore, Colour/*Allocated*/ /*back*/) {
	//assert(0);
	DrawTextBase(rc, font_, ybase, s, len, fore);
}

// On GTK+, exactly same as DrawTextNoClip
void SurfaceImpl::DrawTextClipped(PRectangle rc, Font &font_, float ybase, const char *s, int len,
                                  Colour/*Allocated*/ fore, Colour/*Allocated*/ /*back*/) {
	//assert(0);
	DrawTextBase(rc, font_, ybase, s, len, fore);
}

void SurfaceImpl::DrawTextTransparent(PRectangle rc, Font &font_, float ybase, const char *s, int len,
                                  Colour/*Allocated*/ fore) {
	//assert(0);
	DrawTextBase(rc, font_, ybase, s, len, fore);
}

void SurfaceImpl::MeasureWidths(Font &font_, const char *s, int len, float *positions) {
	//assert(0);
	stbtt_Font* realFont = (stbtt_Font*)font_.GetID();
	//TODO: implement proper UTF-8 handling
	float position = 0;
	while (len--) {
		int advance, leftBearing;
		
		stbtt_GetCodepointHMetrics(&realFont->fontinfo, *s++, &advance, &leftBearing);
		
		position     += advance;//TODO: +Kerning
		*positions++  = position*realFont->scale;
	}
}

float SurfaceImpl::WidthText(Font &font_, const char *s, int len) {
	//assert(0);
	stbtt_Font* realFont = (stbtt_Font*)font_.GetID();
	//TODO: implement proper UTF-8 handling
	int position = 0;
	while (len--) {
		int advance, leftBearing;
		stbtt_GetCodepointHMetrics(&realFont->fontinfo, *s++, &advance, &leftBearing);
		position += advance*realFont->scale;//TODO: +Kerning
	}
	return position;
}

float SurfaceImpl::WidthChar(Font &font_, char ch) {
	stbtt_Font* realFont = (stbtt_Font*)font_.GetID();
	int advance, leftBearing;
	stbtt_GetCodepointHMetrics(&realFont->fontinfo, ch, &advance, &leftBearing);
	return advance*realFont->scale;
}

float SurfaceImpl::Ascent(Font &font_) {
	//assert(0);
	stbtt_Font* realFont = (stbtt_Font*)font_.GetID();
	int ascent, descent, lineGap;
	stbtt_GetFontVMetrics(&realFont->fontinfo, &ascent, &descent, &lineGap);
	return ascent*realFont->scale;
}

float SurfaceImpl::Descent(Font &font_) {
	//assert(0);
	stbtt_Font* realFont = (stbtt_Font*)font_.GetID();
	int ascent, descent, lineGap;
	stbtt_GetFontVMetrics(&realFont->fontinfo, &ascent, &descent, &lineGap);
	return -descent*realFont->scale;
}

float SurfaceImpl::InternalLeading(Font &) {
	//WTF is this?????
	return 0;
}

float SurfaceImpl::ExternalLeading(Font& font_) {
	//WTF is this?????
	stbtt_Font* realFont = (stbtt_Font*)font_.GetID();
	int ascent, descent, lineGap;
	stbtt_GetFontVMetrics(&realFont->fontinfo, &ascent, &descent, &lineGap);
	return lineGap*realFont->scale;
}

float SurfaceImpl::Height(Font &font_) {
	return Ascent(font_) + Descent(font_);
}

float SurfaceImpl::AverageCharWidth(Font &font_) {
	return WidthChar(font_, 'n');
}

//int SurfaceImpl::SetPalette(Palette *, bool) {
//	// Handled in palette allocation for GTK so this does nothing
//	return 0;
//}

void SurfaceImpl::SetClip(PRectangle rc) {
	glScissor(rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top);
	//assert(0);
}

void SurfaceImpl::FlushCachedState() {}

void SurfaceImpl::SetUnicodeMode(bool /*unicodeMode_*/) {
	//if (unicodeMode_)
	//	et = UTF8;
}

void SurfaceImpl::SetDBCSMode(int /*codePage*/) {
	//if (codePage && (codePage != SC_CP_UTF8))
	//	et = dbcs;
}

Surface *Surface::Allocate() {
	return new SurfaceImpl;
}
