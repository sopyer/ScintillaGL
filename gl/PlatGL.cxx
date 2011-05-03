// Scintilla source code edit control
// PlatGTK.cxx - implementation of platform facilities on GTK+/Linux
// Copyright 1998-2004 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <assert.h>

#include "Platform.h"

#include "Scintilla.h"
#include "UniConversion.h"
#include "XPM.h"

#ifdef _MSC_VER
// Ignore unreferenced local functions in GTK+ headers
#pragma warning(disable: 4505)
#endif

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

enum encodingType { singleByte, UTF8, dbcs};

Palette::Palette() {
	//assert(0);
}

Palette::~Palette() {
	//assert(0);
}

void Palette::Release() {
	//assert(0);
}

void Palette::WantFind(ColourPair &cp, bool want) {
	assert(0);
}

void Palette::Allocate(Window &w) {
	assert(0);
}

Font::Font() : fid(0){
	//assert(0);
}

Font::~Font() {
	//assert(0);
}

void Font::Create(const char *faceName, int characterSet, int size,
	bool bold, bool italic, int) {
	assert(0);
}

void Font::Release() {
	//assert(0);
}

// Required on OS X
#ifdef SCI_NAMESPACE
namespace Scintilla {
#endif
class SurfaceImpl : public Surface {
	encodingType et;
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
	void PenColour(ColourAllocated fore);
	int LogPixelsY();
	int DeviceHeightFont(int points);
	void MoveTo(int x_, int y_);
	void LineTo(int x_, int y_);
	void Polygon(Point *pts, int npts, ColourAllocated fore, ColourAllocated back);
	void RectangleDraw(PRectangle rc, ColourAllocated fore, ColourAllocated back);
	void FillRectangle(PRectangle rc, ColourAllocated back);
	void FillRectangle(PRectangle rc, Surface &surfacePattern);
	void RoundedRectangle(PRectangle rc, ColourAllocated fore, ColourAllocated back);
	void AlphaRectangle(PRectangle rc, int cornerSize, ColourAllocated fill, int alphaFill,
		ColourAllocated outline, int alphaOutline, int flags);
	void Ellipse(PRectangle rc, ColourAllocated fore, ColourAllocated back);
	void Copy(PRectangle rc, Point from, Surface &surfaceSource);

	void DrawTextBase(PRectangle rc, Font &font_, int ybase, const char *s, int len, ColourAllocated fore);
	void DrawTextNoClip(PRectangle rc, Font &font_, int ybase, const char *s, int len, ColourAllocated fore, ColourAllocated back);
	void DrawTextClipped(PRectangle rc, Font &font_, int ybase, const char *s, int len, ColourAllocated fore, ColourAllocated back);
	void DrawTextTransparent(PRectangle rc, Font &font_, int ybase, const char *s, int len, ColourAllocated fore);
	void MeasureWidths(Font &font_, const char *s, int len, int *positions);
	int WidthText(Font &font_, const char *s, int len);
	int WidthChar(Font &font_, char ch);
	int Ascent(Font &font_);
	int Descent(Font &font_);
	int InternalLeading(Font &font_);
	int ExternalLeading(Font &font_);
	int Height(Font &font_);
	int AverageCharWidth(Font &font_);

	int SetPalette(Palette *pal, bool inBackGround);
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

SurfaceImpl::SurfaceImpl() : et(singleByte),
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
	assert(0);
	return inited;
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

void SurfaceImpl::PenColour(ColourAllocated fore) {
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

void SurfaceImpl::Polygon(Point *pts, int npts, ColourAllocated fore,
                          ColourAllocated back) {
	assert(0);
}

void SurfaceImpl::RectangleDraw(PRectangle rc, ColourAllocated fore, ColourAllocated back) {
	assert(0);
}

void SurfaceImpl::FillRectangle(PRectangle rc, ColourAllocated back) {
	assert(0);
}

void SurfaceImpl::FillRectangle(PRectangle rc, Surface &surfacePattern) {
	assert(0);
}

void SurfaceImpl::RoundedRectangle(PRectangle rc, ColourAllocated fore, ColourAllocated back) {
	assert(0);
}

static unsigned int GetRValue(unsigned int co) {
	return (co >> 16) & 0xff;
}

static unsigned int GetGValue(unsigned int co) {
	return (co >> 8) & 0xff;
}

static unsigned int GetBValue(unsigned int co) {
	return co & 0xff;
}

void SurfaceImpl::AlphaRectangle(PRectangle rc, int cornerSize, ColourAllocated fill, int alphaFill,
		ColourAllocated outline, int alphaOutline, int flags) {
	assert(0);
}

void SurfaceImpl::Ellipse(PRectangle rc, ColourAllocated fore, ColourAllocated back) {
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
                                 ColourAllocated fore) {
	assert(0);
}

void SurfaceImpl::DrawTextNoClip(PRectangle rc, Font &font_, int ybase, const char *s, int len,
                                 ColourAllocated fore, ColourAllocated back) {
	assert(0);
}

// On GTK+, exactly same as DrawTextNoClip
void SurfaceImpl::DrawTextClipped(PRectangle rc, Font &font_, int ybase, const char *s, int len,
                                  ColourAllocated fore, ColourAllocated back) {
	assert(0);
}

void SurfaceImpl::DrawTextTransparent(PRectangle rc, Font &font_, int ybase, const char *s, int len,
                                  ColourAllocated fore) {
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

int SurfaceImpl::SetPalette(Palette *, bool) {
	// Handled in palette allocation for GTK so this does nothing
	return 0;
}

void SurfaceImpl::SetClip(PRectangle rc) {
	assert(0);
}

void SurfaceImpl::FlushCachedState() {}

void SurfaceImpl::SetUnicodeMode(bool unicodeMode_) {
	if (unicodeMode_)
		et = UTF8;
}

void SurfaceImpl::SetDBCSMode(int codePage) {
	if (codePage && (codePage != SC_CP_UTF8))
		et = dbcs;
}

Surface *Surface::Allocate() {
	return new SurfaceImpl;
}

Window::~Window() {}

void Window::Destroy() {
	assert(0);
}

bool Window::HasFocus() {
	assert(0);
	return true;
}

PRectangle Window::GetPosition() {
	// Before any size allocated pretend its 1000 wide so not scrolled
	PRectangle rc(0, 0, 1000, 1000);
	//assert(0);
	return rc;
}

void Window::SetPosition(PRectangle rc) {
	assert(0);
}

void Window::SetPositionRelative(PRectangle rc, Window relativeTo) {
	assert(0);
}

PRectangle Window::GetClientPosition() {
	// On GTK+, the client position is the window position
	return GetPosition();
}

void Window::Show(bool show) {
	assert(0);
}

void Window::InvalidateAll() {
	assert(0);
}

void Window::InvalidateRectangle(PRectangle rc) {
	//assert(0);
}

void Window::SetFont(Font &) {
	assert(0);
}

void Window::SetCursor(Cursor curs) {
	assert(0);
}

void Window::SetTitle(const char *s) {
	assert(0);
}

/* Returns rectangle of monitor pt is on, both rect and pt are in Window's
   gdk window coordinates */
PRectangle Window::GetMonitorRect(Point pt) {
	assert(0);
	return PRectangle(0,0,1000,1000);
}

ListBox::ListBox() {
}

ListBox::~ListBox() {
}

class ListBoxX : public ListBox {
public:
	ListBoxX() {
	assert(0);
	}
	virtual ~ListBoxX() {
	assert(0);
	}
	virtual void SetFont(Font &font);
	virtual void Create(Window &parent, int ctrlID, Point location_, int lineHeight_, bool unicodeMode_);
	virtual void SetAverageCharWidth(int width);
	virtual void SetVisibleRows(int rows);
	virtual int GetVisibleRows() const;
	virtual PRectangle GetDesiredRect();
	virtual int CaretFromEdge();
	virtual void Clear();
	virtual void Append(char *s, int type = -1);
	virtual int Length();
	virtual void Select(int n);
	virtual int GetSelection();
	virtual int Find(const char *prefix);
	virtual void GetValue(int n, char *value, int len);
	virtual void RegisterImage(int type, const char *xpm_data);
	virtual void ClearRegisteredImages();
	virtual void SetDoubleClickAction(CallBackAction action, void *data) {
	assert(0);
	}
	virtual void SetList(const char *listText, char separator, char typesep);
};

ListBox *ListBox::Allocate() {
	ListBoxX *lb = new ListBoxX();
	return lb;
}

void ListBoxX::Create(Window &, int, Point, int, bool) {
	assert(0);
}

void ListBoxX::SetFont(Font &scint_font) {
	assert(0);
}

void ListBoxX::SetAverageCharWidth(int width) {
	assert(0);
}

void ListBoxX::SetVisibleRows(int rows) {
	assert(0);
}

int ListBoxX::GetVisibleRows() const {
	assert(0);
	return 0;
}

PRectangle ListBoxX::GetDesiredRect() {
	// Before any size allocated pretend its 100 wide so not scrolled
	PRectangle rc(0, 0, 100, 100);
	assert(0);
	return rc;
}

int ListBoxX::CaretFromEdge() {
assert(0);	return 0;
}

void ListBoxX::Clear() {
assert(0);	}

void ListBoxX::Append(char *s, int type) {
assert(0);	}

int ListBoxX::Length() {
	assert(0);
	return 0;
}

void ListBoxX::Select(int n) {
assert(0);	}

int ListBoxX::GetSelection() {
assert(0);		return -1;
}

int ListBoxX::Find(const char *prefix) {
assert(0);		return -1;
}

void ListBoxX::GetValue(int n, char *value, int len) {
assert(0);	}

// g_return_if_fail causes unnecessary compiler warning in release compile.
#ifdef _MSC_VER
#pragma warning(disable: 4127)
#endif

void ListBoxX::RegisterImage(int type, const char *xpm_data) {
assert(0);	}

void ListBoxX::ClearRegisteredImages() {
assert(0);	}

void ListBoxX::SetList(const char *listText, char separator, char typesep) {
assert(0);	}

Menu::Menu() : mid(0) {}

void Menu::CreatePopUp() {
assert(0);	}

void Menu::Destroy() {
assert(0);	}

void Menu::Show(Point pt, Window &) {
assert(0);	}

ElapsedTime::ElapsedTime() {
assert(0);	}

class DynamicLibraryImpl : public DynamicLibrary {
public:
	DynamicLibraryImpl(const char *modulePath) {
		assert(0);
	}

	virtual ~DynamicLibraryImpl() {
		assert(0);
	}

	// Use g_module_symbol to get a pointer to the relevant function.
	virtual Function FindFunction(const char *name) {
		assert(0);
		return 0;
	}

	virtual bool IsValid() {
		assert(0);
		return true;
	}
};

DynamicLibrary *DynamicLibrary::Load(const char *modulePath) {
	return static_cast<DynamicLibrary *>( new DynamicLibraryImpl(modulePath) );
}

double ElapsedTime::Duration(bool reset) {
assert(0);		return 0;
}

ColourDesired Platform::Chrome() {
	return ColourDesired(0xe0, 0xe0, 0xe0);
}

ColourDesired Platform::ChromeHighlight() {
	return ColourDesired(0xff, 0xff, 0xff);
}

const char *Platform::DefaultFont() {
	return "Lucida Console";
}

int Platform::DefaultFontSize() {
	return 10;
}

unsigned int Platform::DoubleClickTime() {
	return 500; 	// Half a second
}

bool Platform::MouseButtonBounce() {
	return true;
}

void Platform::DebugDisplay(const char *s) {
	fprintf(stderr, "%s", s);
}

bool Platform::IsKeyDown(int) {
	// TODO: discover state of keys in GTK+/X
	return false;
}

long Platform::SendScintilla(
    WindowID w, unsigned int msg, unsigned long wParam, long lParam) {
assert(0);		return 0;
}

long Platform::SendScintillaPointer(
    WindowID w, unsigned int msg, unsigned long wParam, void *lParam) {
assert(0);		return 0;
}

bool Platform::IsDBCSLeadByte(int codePage, char ch) {
	// Byte ranges found in Wikipedia articles with relevant search strings in each case
	unsigned char uch = static_cast<unsigned char>(ch);
	switch (codePage) {
		case 932:
			// Shift_jis
			return ((uch >= 0x81) && (uch <= 0x9F)) ||
				((uch >= 0xE0) && (uch <= 0xEF));
		case 936:
			// GBK
			return (uch >= 0x81) && (uch <= 0xFE);
		case 950:
			// Big5
			return (uch >= 0x81) && (uch <= 0xFE);
		// Korean EUC-KR may be code page 949.
	}
	return false;
}

int Platform::DBCSCharLength(int codePage, const char *s) {
	if (codePage == 932 || codePage == 936 || codePage == 950) {
		return IsDBCSLeadByte(codePage, s[0]) ? 2 : 1;
	} else {
		int bytes = mblen(s, MB_CUR_MAX);
		if (bytes >= 1)
			return bytes;
		else
			return 1;
	}
}

int Platform::DBCSCharMaxLength() {
	return MB_CUR_MAX;
	//return 2;
}

// These are utility functions not really tied to a platform

int Platform::Minimum(int a, int b) {
	if (a < b)
		return a;
	else
		return b;
}

int Platform::Maximum(int a, int b) {
	if (a > b)
		return a;
	else
		return b;
}

//#define TRACE

#ifdef TRACE
void Platform::DebugPrintf(const char *format, ...) {
	char buffer[2000];
	va_list pArguments;
	va_start(pArguments, format);
	vsprintf(buffer, format, pArguments);
	va_end(pArguments);
	Platform::DebugDisplay(buffer);
}
#else
void Platform::DebugPrintf(const char *, ...) {}

#endif

// Not supported for GTK+
static bool assertionPopUps = true;

bool Platform::ShowAssertionPopUps(bool assertionPopUps_) {
	bool ret = assertionPopUps;
	assertionPopUps = assertionPopUps_;
	return ret;
}

void Platform::Assert(const char *c, const char *file, int line) {
	char buffer[2000];
	sprintf(buffer, "Assertion [%s] failed at %s %d", c, file, line);
	strcat(buffer, "\r\n");
	Platform::DebugDisplay(buffer);
	abort();
}

int Platform::Clamp(int val, int minVal, int maxVal) {
	if (val > maxVal)
		val = maxVal;
	if (val < minVal)
		val = minVal;
	return val;
}

void Platform_Initialise() {
assert(0);
}

void Platform_Finalise() {
assert(0);
}
