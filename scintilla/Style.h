// Scintilla source code edit control
/** @file Style.h
 ** Defines the font and colour style for a class of text.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef STYLE_H
#define STYLE_H

#ifdef SCI_NAMESPACE
namespace Scintilla {
#endif

struct FontSpecification {
	const char *fontName;
	bool bold;
	bool italic;
	int size;
	int characterSet;
	int extraFontFlag;
	FontSpecification() :
		fontName(0),
		bold(false),
		italic(false),
		size(10),
		characterSet(0),
		extraFontFlag(0) {
	}
	bool EqualTo(const FontSpecification &other) const;
};

// Just like Font but only has a copy of the FontID so should not delete it
class FontAlias : public Font {
	// Private so FontAlias objects can not be copied
	FontAlias(const FontAlias &);
	FontAlias &operator=(const FontAlias &);
public:
	FontAlias();
	virtual ~FontAlias();
	void MakeAlias(Font &fontOrigin);
	void ClearFont();
};

struct FontMeasurements {
	float lineHeight;
	float ascent;
	float descent;
	float externalLeading;
	float aveCharWidth;
	float spaceWidth;
	float sizeZoomed;
	FontMeasurements();
	void Clear();
};

/**
 */
class Style : public FontSpecification, public FontMeasurements {
public:
	Colour/*Pair*/ fore;
	Colour/*Pair*/ back;
	bool eolFilled;
	bool underline;
	enum ecaseForced {caseMixed, caseUpper, caseLower};
	ecaseForced caseForce;
	bool visible;
	bool changeable;
	bool hotspot;

	FontAlias font;

	Style();
	Style(const Style &source);
	~Style();
	Style &operator=(const Style &source);
	void Clear(Colour fore_, Colour back_,
	           int size_,
	           const char *fontName_, int characterSet_,
	           bool bold_, bool italic_, bool eolFilled_,
	           bool underline_, ecaseForced caseForce_,
		   bool visible_, bool changeable_, bool hotspot_);
	void ClearTo(const Style &source);
	void Copy(Font &font_, const FontMeasurements &fm_);
	bool IsProtected() const { return !(changeable && visible);}
};

#ifdef SCI_NAMESPACE
}
#endif

#endif
