// Scintilla source code edit control
/** @file Platform.h
 ** Interface to platform facilities. Also includes some basic utilities.
 ** Implemented in PlatGTK.cxx for GTK+/Linux, PlatWin.cxx for Windows, and PlatWX.cxx for wxWindows.
 **/
// Copyright 1998-2009 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef SCI_NAMESPACE
namespace Scintilla {
#endif

// Underlying the implementation of the platform classes are platform specific types.
// Sometimes these need to be passed around by client code so they are defined here

typedef void *FontID;
typedef void *SurfaceID;
typedef void *WindowID;
typedef void *MenuID;
typedef void *TickerID;
typedef void *Function;
typedef void *IdlerID;

/**
 * A geometric point class.
 * Point is exactly the same as the Win32 POINT and GTK+ GdkPoint so can be used interchangeably.
 */
class Point {
public:
	float x;
	float y;

	explicit Point(float x_=0, float y_=0) : x(x_), y(y_) {
	}
};

/**
 * A geometric rectangle class.
 * PRectangle is exactly the same as the Win32 RECT so can be used interchangeably.
 * PRectangles contain their top and left sides, but not their right and bottom sides.
 */
class PRectangle {
public:
	float left;
	float top;
	float right;
	float bottom;

	PRectangle(float left_=0, float top_=0, float right_=0, float bottom_ = 0) :
		left(left_), top(top_), right(right_), bottom(bottom_) {
	}

	// Other automatically defined methods (assignment, copy constructor, destructor) are fine

	bool operator==(PRectangle &rc) {
		return (rc.left == left) && (rc.right == right) &&
			(rc.top == top) && (rc.bottom == bottom);
	}
	bool Contains(Point pt) {
		return (pt.x >= left) && (pt.x <= right) &&
			(pt.y >= top) && (pt.y <= bottom);
	}
	bool Contains(PRectangle rc) {
		return (rc.left >= left) && (rc.right <= right) &&
			(rc.top >= top) && (rc.bottom <= bottom);
	}
	bool Intersects(PRectangle other) {
		return (right > other.left) && (left < other.right) &&
			(bottom > other.top) && (top < other.bottom);
	}
	void Move(float xDelta, float yDelta) {
		left += xDelta;
		top += yDelta;
		right += xDelta;
		bottom += yDelta;
	}
	float Width() { return right - left; }
	float Height() { return bottom - top; }
	bool Empty() {
		return (Height() <= 0) || (Width() <= 0);
	}
};

typedef unsigned int Colour;

inline Colour MakeRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a=0xFF)
{
	return a<<24|b<<16|g<<8|r;
}

inline int GetRed(Colour c)   { return c&0xFF;       }
inline int GetGreen(Colour c) {	return (c>>8)&0xFF;  }
inline int GetBlue(Colour c)  { return (c>>16)&0xFF; }
inline int GetAlpha(Colour c) {	return (c>>24)&0xFF; }

inline unsigned char ValueOfHex(const char ch) {
	if (ch >= '0' && ch <= '9')
		return ch - '0';
	else if (ch >= 'A' && ch <= 'F')
		return ch - 'A' + 10;
	else if (ch >= 'a' && ch <= 'f')
		return ch - 'a' + 10;
	else
		return 0;
}

inline Colour ColourFromText(const char *val) {
	if (*val == '#') {
		val++;
	}
	unsigned char r = ValueOfHex(val[0]) * 16 + ValueOfHex(val[1]);
	unsigned char g = ValueOfHex(val[2]) * 16 + ValueOfHex(val[3]);
	unsigned char b = ValueOfHex(val[4]) * 16 + ValueOfHex(val[5]);
	return MakeRGBA(r, g, b);
}

/**
 * Font management.
 */


struct FontParameters {
	const char *faceName;
	float size;
	int weight;
	bool italic;
	int extraFontFlag;
	int technology;
	int characterSet;

	FontParameters(
		const char *faceName_,
		float size_=10,
		int weight_=400,
		bool italic_=false,
		int extraFontFlag_=0,
		int technology_=0,
		int characterSet_=0) :

		faceName(faceName_),
		size(size_),
		weight(weight_),
		italic(italic_),
		extraFontFlag(extraFontFlag_),
		technology(technology_),
		characterSet(characterSet_)
	{
	}

};

class Font {
protected:
	FontID fid;
	// Private so Font objects can not be copied
	Font(const Font &);
	Font &operator=(const Font &);
public:
	Font();
	virtual ~Font();

	virtual void Create(const FontParameters &fp);
	virtual void Release();

	FontID GetID() { return fid; }
	// Alias another font - caller guarantees not to Release
	void SetID(FontID fid_) { fid = fid_; }
	friend class Surface;
        friend class SurfaceImpl;
};

struct pixmap_t;
typedef pixmap_t* Pixmap;

Pixmap	CreatePixmap();
void	DestroyPixmap(Pixmap pixmap);
void	UpdatePixmap(Pixmap pixmap, int w, int h, int* data);
bool	IsPixmapInitialised(Pixmap pixmap);

/**
 * A surface abstracts a place to draw.
 */
class Surface {
private:
	// Private so Surface objects can not be copied
	Surface(const Surface &) {}
	Surface &operator=(const Surface &) { return *this; }
public:
	Surface() {}
	virtual ~Surface() {}

	static Surface *Allocate();
	virtual void Release()=0;

	virtual void PenColour(Colour fore)=0;
	virtual int LogPixelsY()=0;
	virtual float DeviceHeightFont(float points)=0;
	virtual void MoveTo(float x_, float y_)=0;
	virtual void LineTo(float x_, float y_)=0;
	virtual void Polygon(Point *pts, int npts, Colour fore, Colour back)=0;
	virtual void RectangleDraw(PRectangle rc, Colour fore, Colour back)=0;
	virtual void FillRectangle(PRectangle rc, Colour back)=0;
	virtual void FillRectangle(PRectangle rc, Surface &surfacePattern)=0;
	virtual void RoundedRectangle(PRectangle rc, Colour fore, Colour back)=0;
	virtual void AlphaRectangle(PRectangle rc, int cornerSize, Colour fill, int alphaFill,
		Colour outline, int alphaOutline, int flags)=0;
	virtual void Ellipse(PRectangle rc, Colour fore, Colour back)=0;
	
	virtual void DrawPixmap(PRectangle rc, Point from, Pixmap pixmap) = 0;
	virtual void DrawRGBAImage(PRectangle rc, int width, int height, const unsigned char *pixelsImage) = 0;

	virtual void DrawTextNoClip(PRectangle rc, Font &font_, float ybase, const char *s, int len, Colour fore, Colour back)=0;
	virtual void DrawTextClipped(PRectangle rc, Font &font_, float ybase, const char *s, int len, Colour fore, Colour back)=0;
	virtual void DrawTextTransparent(PRectangle rc, Font &font_, float ybase, const char *s, int len, Colour fore)=0;
	virtual void MeasureWidths(Font &font_, const char *s, int len, float *positions)=0;
	virtual float WidthText(Font &font_, const char *s, int len)=0;
	virtual float WidthChar(Font &font_, char ch)=0;
	virtual float Ascent(Font &font_)=0;
	virtual float Descent(Font &font_)=0;
	virtual float InternalLeading(Font &font_)=0;
	virtual float ExternalLeading(Font &font_)=0;
	virtual float Height(Font &font_)=0;
	virtual float AverageCharWidth(Font &font_)=0;

	virtual void SetClip(PRectangle rc)=0;
	virtual void FlushCachedState()=0;
};

class ElapsedTime {
	long bigBit;
	long littleBit;
public:
	ElapsedTime();
	double Duration(bool reset=false);
};

/**
 * Platform class used to retrieve system wide parameters such as double click speed
 * and chrome colour. Not a creatable object, more of a module with several functions.
 */
namespace Platform {
	Colour       Chrome();
	Colour       ChromeHighlight();
	const char*  DefaultFont();
	int          DefaultFontSize();
	unsigned int DoubleClickTime();
	bool         MouseButtonBounce();

	// These are utility functions not really tied to a platform
	inline int Minimum(int a, int b) { return a<b ? a : b; }
	inline int Maximum(int a, int b) { return a>b ? a : b; }

	inline short HighShortFromLong(long x) { return static_cast<short>(x >> 16);    }
	inline short LowShortFromLong(long x)  { return static_cast<short>(x & 0xffff);	}

	inline int Clamp(int val, int minVal, int maxVal) { return Minimum( maxVal, Maximum( val, minVal ) ); }

	void DebugPrintf(const char *format, ...);
	void Assert(const char *c, const char *file, int line);
};

enum AdditionalTextFormat
{
	TEXT_FORMAT_UTF8,
	TEXT_FORMAT_UTF8_LINE,
	TEXT_FORMAT_UTF8_RECT
};

int  IsClipboardTextAvailable(AdditionalTextFormat fmt=TEXT_FORMAT_UTF8);
int  GetClipboardTextUTF8(char* text, size_t len);
void SetClipboardTextUTF8(const char* text, size_t  len, int additionalFormat);

#ifdef SCI_NAMESPACE
}
#endif

#ifdef  NDEBUG
#	define PLATFORM_ASSERT(c) ((void)0)
#else
#	ifdef SCI_NAMESPACE
#		define PLATFORM_ASSERT(c) ((c) ? (void)(0) : Scintilla::Platform::Assert(#c, __FILE__, __LINE__))
#	else
#		define PLATFORM_ASSERT(c) ((c) ? (void)(0) : Platform::Assert(#c, __FILE__, __LINE__))
#	endif
#endif

#endif
