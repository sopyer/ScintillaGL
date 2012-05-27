// Scintilla source code edit control
/** @file ViewStyle.h
 ** Store information on how the document is to be viewed.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef VIEWSTYLE_H
#define VIEWSTYLE_H

#ifdef SCI_NAMESPACE
namespace Scintilla {
#endif

/**
 */
class MarginStyle {
public:
	int style;
	int width;
	int mask;
	bool sensitive;
	int cursor;
	MarginStyle();
};

/**
 */
class FontNames {
private:
	char **names;
	int size;
	int max;

public:
	FontNames();
	~FontNames();
	void Clear();
	const char *Save(const char *name);
};

class FontRealised : public FontSpecification, public FontMeasurements {
	// Private so FontRealised objects can not be copied
	FontRealised(const FontRealised &);
	FontRealised &operator=(const FontRealised &);
public:
	Font font;
	FontRealised *frNext;
	FontRealised(const FontSpecification &fs);
	virtual ~FontRealised();
	void Realise(Surface &surface, float zoomLevel);
	FontRealised *Find(const FontSpecification &fs);
	void FindMaxAscentDescent(float &maxAscent, float &maxDescent);
};

enum IndentView {ivNone, ivReal, ivLookForward, ivLookBoth};

enum WhiteSpaceVisibility {wsInvisible=0, wsVisibleAlways=1, wsVisibleAfterIndent=2};

/**
 */
class ViewStyle {
public:
	FontNames fontNames;
	FontRealised *frFirst;
	size_t stylesSize;
	Style *styles;
	LineMarker markers[MARKER_MAX + 1];
	int largestMarkerHeight;
	Indicator indicators[INDIC_MAX + 1];
	float lineHeight;
	float maxAscent;
	float maxDescent;
	float aveCharWidth;
	float spaceWidth;
	bool selforeset;
	Colour selforeground;
	Colour selAdditionalForeground;
	bool selbackset;
	Colour selbackground;
	Colour selAdditionalBackground;
	Colour selbackground2;
	int selAlpha;
	int selAdditionalAlpha;
	bool selEOLFilled;
	bool whitespaceForegroundSet;
	Colour whitespaceForeground;
	bool whitespaceBackgroundSet;
	Colour whitespaceBackground;
	Colour selbar;
	Colour selbarlight;
	bool foldmarginColourSet;
	Colour foldmarginColour;
	bool foldmarginHighlightColourSet;
	Colour foldmarginHighlightColour;
	bool hotspotForegroundSet;
	Colour hotspotForeground;
	bool hotspotBackgroundSet;
	Colour hotspotBackground;
	bool hotspotUnderline;
	bool hotspotSingleLine;
	/// Margins are ordered: Line Numbers, Selection Margin, Spacing Margin
	enum { margins=5 };
	int leftMarginWidth;	///< Spacing margin on left of text
	int rightMarginWidth;	///< Spacing margin on left of text
	int maskInLine;	///< Mask for markers to be put into text because there is nowhere for them to go in margin
	MarginStyle ms[margins];
	int fixedColumnWidth;
	int zoomLevel;
	WhiteSpaceVisibility viewWhitespace;
	int whitespaceSize;
	IndentView viewIndentationGuides;
	bool viewEOL;
	Colour caretcolour;
	Colour additionalCaretColour;
	bool showCaretLineBackground;
	Colour caretLineBackground;
	int caretLineAlpha;
	Colour edgecolour;
	int edgeState;
	int caretStyle;
	int caretWidth;
	bool someStylesProtected;
	bool someStylesForceCase;
	int extraFontFlag;
	int extraAscent;
	int extraDescent;
	int marginStyleOffset;
	int annotationVisible;
	int annotationStyleOffset;
	bool braceHighlightIndicatorSet;
	int braceHighlightIndicator;
	bool braceBadLightIndicatorSet;
	int braceBadLightIndicator;

	ViewStyle();
	ViewStyle(const ViewStyle &source);
	~ViewStyle();
	void Init(size_t stylesSize_=64);
	void CreateFont(const FontSpecification &fs);
	void Refresh(Surface &surface);
	void AllocStyles(size_t sizeNew);
	void EnsureStyle(size_t index);
	void ResetDefaultStyle();
	void ClearStyles();
	void SetStyleFontName(int styleIndex, const char *name);
	bool ProtectionActive() const;
	bool ValidStyle(size_t styleIndex) const;
	void CalcLargestMarkerHeight();
};

#ifdef SCI_NAMESPACE
}
#endif

#endif
