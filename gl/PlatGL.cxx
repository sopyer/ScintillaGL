// ScintillaGL source code edit control
// PlatGTK.cxx - implementation of platform facilities on GTK+/Linux
// Copyright 2011 by Mykhailo Parfeniuk
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

#include <gl/glee.h>

#ifdef _MSC_VER
// Ignore unreferenced local functions in GTK+ headers
#pragma warning(disable: 4505)
#endif

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

//enum encodingType { singleByte, UTF8, dbcs};

//Palette::Palette() {
//	//assert(0);
//}
//
//Palette::~Palette() {
//	//assert(0);
//}
//
//void Palette::Release() {
//	//assert(0);
//}
//
//void Palette::WantFind(Colour/*Pair*/ &cp, bool want) {
//	assert(0);
//}
//
//void Palette::Allocate(Window &w) {
//	assert(0);
//}

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
	PRectangle rc(0, 0, 800, 600);
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

//void Window::InvalidateAll() {
//	assert(0);
//}
//
//void Window::InvalidateRectangle(PRectangle rc) {
//	//assert(0);
//}

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

Colour/*Desired*/ Platform::Chrome() {
	return MakeRGBA/*Desired*/(0xe0, 0xe0, 0xe0);
}

Colour/*Desired*/ Platform::ChromeHighlight() {
	return MakeRGBA/*Desired*/(0xff, 0xff, 0xff);
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

namespace platform
{
	void InitializeFontSubsytem();
	void ShutdownFontSubsytem();
}

void Platform_Initialise()
{
	platform::InitializeFontSubsytem();
}

void Platform_Finalise()
{
	platform::ShutdownFontSubsytem();
}
