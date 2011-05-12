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
	int DeviceHeightFont(int points);
	void MoveTo(int x_, int y_);
	void LineTo(int x_, int y_);
	void Polygon(Point *pts, int npts, Colour/*Allocated*/ fore, Colour/*Allocated*/ back);
	void RectangleDraw(PRectangle rc, Colour/*Allocated*/ fore, Colour/*Allocated*/ back);
	void FillRectangle(PRectangle rc, Colour/*Allocated*/ back);
	void FillRectangle(PRectangle rc, Surface &surfacePattern);
	void RoundedRectangle(PRectangle rc, Colour/*Allocated*/ fore, Colour/*Allocated*/ back);
	void AlphaRectangle(PRectangle rc, int cornerSize, Colour/*Allocated*/ fill, int alphaFill,
		Colour/*Allocated*/ outline, int alphaOutline, int flags);
	void Ellipse(PRectangle rc, Colour/*Allocated*/ fore, Colour/*Allocated*/ back);
	void Copy(PRectangle rc, Point from, Surface &surfaceSource);

	void DrawTextBase(PRectangle rc, Font &font_, int ybase, const char *s, int len, Colour/*Allocated*/ fore);
	void DrawTextNoClip(PRectangle rc, Font &font_, int ybase, const char *s, int len, Colour/*Allocated*/ fore, Colour/*Allocated*/ back);
	void DrawTextClipped(PRectangle rc, Font &font_, int ybase, const char *s, int len, Colour/*Allocated*/ fore, Colour/*Allocated*/ back);
	void DrawTextTransparent(PRectangle rc, Font &font_, int ybase, const char *s, int len, Colour/*Allocated*/ fore);
	void MeasureWidths(Font &font_, const char *s, int len, int *positions);
	int WidthText(Font &font_, const char *s, int len);
	int WidthChar(Font &font_, char ch);
	int Ascent(Font &font_);
	int Descent(Font &font_);
	int InternalLeading(Font &font_);
	int ExternalLeading(Font &font_);
	int Height(Font &font_);
	int AverageCharWidth(Font &font_);

	//int SetPalette(Palette *pal, bool inBackGround);
	void SetClip(PRectangle rc);
	void FlushCachedState();

	void SetUnicodeMode(bool unicodeMode_);
	void SetDBCSMode(int codePage);
};
#ifdef SCI_NAMESPACE
}
#endif


void SurfaceImpl::SetConverter(int characterSet_) {
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

void SurfaceImpl::Init(WindowID wid) {
	assert(0);
	inited = true;
}

void SurfaceImpl::Init(SurfaceID sid, WindowID wid) {
	assert(0);
	inited = true;
}

void SurfaceImpl::InitPixMap(int width, int height, Surface *surface_, WindowID wid) {
	assert(0);
	createdGC = true;
	inited = true;
}

void SurfaceImpl::PenColour(Colour/*Allocated*/ fore) {
	assert(0);
}

int SurfaceImpl::LogPixelsY() {
	return 72;
}

int SurfaceImpl::DeviceHeightFont(int points) {
	assert(0);
	int logPix = LogPixelsY();
	return (points * logPix + logPix / 2) / 72;
}

void SurfaceImpl::MoveTo(int x_, int y_) {
	assert(0);
	x = x_;
	y = y_;
}

void SurfaceImpl::LineTo(int x_, int y_) {
	assert(0);
}

void SurfaceImpl::Polygon(Point *pts, int npts, Colour/*Allocated*/ fore,
                          Colour/*Allocated*/ back) {
	assert(0);
}

void SurfaceImpl::RectangleDraw(PRectangle rc, Colour/*Allocated*/ fore, Colour/*Allocated*/ back) {
	assert(0);
}

void SurfaceImpl::FillRectangle(PRectangle rc, Colour/*Allocated*/ back) {
	glColor4ubv((GLubyte*)&back);
	glBegin(GL_QUADS);
	glVertex2i(rc.left,  rc.top);
	glVertex2i(rc.right, rc.top);
	glVertex2i(rc.right, rc.bottom);
	glVertex2i(rc.left,  rc.bottom);
	glEnd();
	//assert(0);
}

void SurfaceImpl::FillRectangle(PRectangle rc, Surface &surfacePattern) {
	assert(0);
}

void SurfaceImpl::RoundedRectangle(PRectangle rc, Colour/*Allocated*/ fore, Colour/*Allocated*/ back) {
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

void SurfaceImpl::AlphaRectangle(PRectangle rc, int cornerSize, Colour/*Allocated*/ fill, int alphaFill,
		Colour/*Allocated*/ outline, int alphaOutline, int flags) {
	assert(0);
}

void SurfaceImpl::Ellipse(PRectangle rc, Colour/*Allocated*/ fore, Colour/*Allocated*/ back) {
	assert(0);
}

void SurfaceImpl::Copy(PRectangle rc, Point from, Surface &surfaceSource) {
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

static char *UTF8FromDBCS(const char *s, int &len) {
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

void SurfaceImpl::DrawTextBase(PRectangle rc, Font &font_, int ybase, const char *s, int len,
                                 Colour/*Allocated*/ fore) {
	assert(0);
}

void SurfaceImpl::DrawTextNoClip(PRectangle rc, Font &font_, int ybase, const char *s, int len,
                                 Colour/*Allocated*/ fore, Colour/*Allocated*/ back) {
	assert(0);
}

// On GTK+, exactly same as DrawTextNoClip
void SurfaceImpl::DrawTextClipped(PRectangle rc, Font &font_, int ybase, const char *s, int len,
                                  Colour/*Allocated*/ fore, Colour/*Allocated*/ back) {
	assert(0);
}

void SurfaceImpl::DrawTextTransparent(PRectangle rc, Font &font_, int ybase, const char *s, int len,
                                  Colour/*Allocated*/ fore) {
	assert(0);
}

void SurfaceImpl::MeasureWidths(Font &font_, const char *s, int len, int *positions) {
	assert(0);
}

int SurfaceImpl::WidthText(Font &font_, const char *s, int len) {
	assert(0);
	return 0;
}

int SurfaceImpl::WidthChar(Font &font_, char ch) {
	assert(0);
	return 1;
}

int SurfaceImpl::Ascent(Font &font_) {
	assert(0);
	return 0;
}

int SurfaceImpl::Descent(Font &font_) {
	assert(0);
	return 0;
}

int SurfaceImpl::InternalLeading(Font &) {
	return 0;
}

int SurfaceImpl::ExternalLeading(Font &) {
	return 0;
}

int SurfaceImpl::Height(Font &font_) {
	return Ascent(font_) + Descent(font_);
}

int SurfaceImpl::AverageCharWidth(Font &font_) {
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

void SurfaceImpl::SetUnicodeMode(bool unicodeMode_) {
	//if (unicodeMode_)
	//	et = UTF8;
}

void SurfaceImpl::SetDBCSMode(int codePage) {
	//if (codePage && (codePage != SC_CP_UTF8))
	//	et = dbcs;
}

Surface *Surface::Allocate() {
	return new SurfaceImpl;
}
