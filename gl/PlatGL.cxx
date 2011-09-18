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

#include <windows.h>
#include <winuser.h>
#include <wtypes.h>

HWND hWnd; //Clipboard window
CLIPFORMAT cfColumnSelect;
CLIPFORMAT cfLineSelect;

void Platform_Initialise(HWND hWnd)
{
	// There does not seem to be a real standard for indicating that the clipboard
	// contains a rectangular selection, so copy Developer Studio.
	cfColumnSelect = static_cast<CLIPFORMAT>(
		::RegisterClipboardFormat(TEXT("MSDEVColumnSelect")));

	// Likewise for line-copy (copies a full line when no text is selected)
	cfLineSelect = static_cast<CLIPFORMAT>(
		::RegisterClipboardFormat(TEXT("MSDEVLineSelect")));
	platform::InitializeFontSubsytem();
}

void Platform_Finalise()
{
	platform::ShutdownFontSubsytem();
}

class GlobalMemory {
	HGLOBAL hand;
public:
	void *ptr;
	GlobalMemory() : hand(0), ptr(0) {
	}
	GlobalMemory(HGLOBAL hand_) : hand(hand_), ptr(0) {
		if (hand) {
			ptr = ::GlobalLock(hand);
		}
	}
	~GlobalMemory() {
		PLATFORM_ASSERT(!ptr);
	}
	void Allocate(size_t bytes) {
		hand = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, bytes);
		if (hand) {
			ptr = ::GlobalLock(hand);
		}
	}
	HGLOBAL Unlock() {
		PLATFORM_ASSERT(ptr);
		HGLOBAL handCopy = hand;
		::GlobalUnlock(hand);
		ptr = 0;
		hand = 0;
		return handCopy;
	}
	void SetClip(UINT uFormat) {
		::SetClipboardData(uFormat, Unlock());
	}
	operator bool() const {
		return ptr != 0;
	}
	SIZE_T Size() {
		return ::GlobalSize(hand);
	}
};

int IsClipboardTextAvailable(AdditionalTextFormat fmt)
{
	if (fmt == TEXT_FORMAT_UTF8_LINE) return ::IsClipboardFormatAvailable(cfLineSelect) != 0;
	if (fmt == TEXT_FORMAT_UTF8_RECT) return ::IsClipboardFormatAvailable(cfColumnSelect) != 0;
	if (::IsClipboardFormatAvailable(CF_TEXT))
		return true;
	return ::IsClipboardFormatAvailable(CF_UNICODETEXT) != 0;
}

void SetClipboardTextUTF8(const char* text, size_t len, int additionalFormat)
{
	if (!::OpenClipboard(hWnd))
		return;
	::EmptyClipboard();

	GlobalMemory uniText;

	int uchars = UTF16Length(text, len);
	uniText.Allocate(2 * uchars);
	if (uniText) {
		UTF16FromUTF8(text, len, static_cast<wchar_t *>(uniText.ptr), uchars);
	}

	if (uniText) {
		uniText.SetClip(CF_UNICODETEXT);
	}

	if (additionalFormat==TEXT_FORMAT_UTF8_RECT) {
		::SetClipboardData(cfColumnSelect, 0);
	} else if (additionalFormat==TEXT_FORMAT_UTF8_LINE) {
		::SetClipboardData(cfLineSelect, 0);
	}

	::CloseClipboard();
}

int GetClipboardTextUTF8(char* text, size_t len)
{
	if (!::OpenClipboard(hWnd))
		return 0;

	unsigned int clipLen = 0;

	GlobalMemory memUSelection(::GetClipboardData(CF_UNICODETEXT));
	if (memUSelection) {
		wchar_t *uptr = static_cast<wchar_t *>(memUSelection.ptr);
		unsigned int bytes = memUSelection.Size();
		clipLen = UTF8Length(uptr, bytes / 2);
		if (uptr && text) {
			UTF8FromUTF16(uptr, bytes / 2, text, len);
		}
		memUSelection.Unlock();
	} else {
		// CF_UNICODETEXT not available, paste ANSI text
		GlobalMemory memSelection(::GetClipboardData(CF_TEXT));
		if (memSelection) {
			char *ptr = static_cast<char *>(memSelection.ptr);
			if (ptr) {
				unsigned int bytes = memSelection.Size();
				unsigned int len = bytes;
				for (unsigned int i = 0; i < bytes; i++) {
					if ((len == bytes) && (0 == ptr[i]))
						len = i;
				}

				// convert clipboard text to UTF-8
				wchar_t *uptr = new wchar_t[len+1];

				unsigned int ulen = ::MultiByteToWideChar(CP_ACP, 0,
				                    ptr, len, uptr, len+1);

				clipLen = UTF8Length(uptr, ulen);
				if (text) {
					// CP_UTF8 not available on Windows 95, so use UTF8FromUTF16()
					UTF8FromUTF16(uptr, ulen, text, len);
				}

				delete []uptr;
			}
			memSelection.Unlock();
		}
	}
	::CloseClipboard();

	return clipLen;
}