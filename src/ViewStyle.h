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

enum IndentView {ivNone, ivReal, ivLookForward, ivLookBoth};

enum WhiteSpaceVisibility {wsInvisible=0, wsVisibleAlways=1, wsVisibleAfterIndent=2};

/**
 */
class ViewStyle {
public:
	FontNames fontNames;
	size_t stylesSize;
	Style *styles;
	LineMarker markers[MARKER_MAX + 1];
	Indicator indicators[INDIC_MAX + 1];
	float lineHeight;
	/*unsigned int*/float maxAscent;
	/*unsigned int*/float maxDescent;
	/*unsigned int*/float aveCharWidth;
	/*unsigned int*/float spaceWidth;
	bool selforeset;
	Colour/*Pair*/ selforeground;
	Colour/*Pair*/ selAdditionalForeground;
	bool selbackset;
	Colour/*Pair*/ selbackground;
	Colour/*Pair*/ selAdditionalBackground;
	Colour/*Pair*/ selbackground2;
	int selAlpha;
	int selAdditionalAlpha;
	bool selEOLFilled;
	bool whitespaceForegroundSet;
	Colour/*Pair*/ whitespaceForeground;
	bool whitespaceBackgroundSet;
	Colour/*Pair*/ whitespaceBackground;
	Colour/*Pair*/ selbar;
	Colour/*Pair*/ selbarlight;
	bool foldmarginColourSet;
	Colour/*Pair*/ foldmarginColour;
	bool foldmarginHighlightColourSet;
	Colour/*Pair*/ foldmarginHighlightColour;
	bool hotspotForegroundSet;
	Colour/*Pair*/ hotspotForeground;
	bool hotspotBackgroundSet;
	Colour/*Pair*/ hotspotBackground;
	bool hotspotUnderline;
	bool hotspotSingleLine;
	/// Margins are ordered: Line Numbers, Selection Margin, Spacing Margin
	enum { margins=5 };
	int leftMarginWidth;	///< Spacing margin on left of text
	int rightMarginWidth;	///< Spacing margin on left of text
	bool symbolMargin;
	int maskInLine;	///< Mask for markers to be put into text because there is nowhere for them to go in margin
	MarginStyle ms[margins];
	int fixedColumnWidth;
	int zoomLevel;
	WhiteSpaceVisibility viewWhitespace;
	int whitespaceSize;
	IndentView viewIndentationGuides;
	bool viewEOL;
	bool showMarkedLines;
	Colour/*Pair*/ caretcolour;
	Colour/*Pair*/ additionalCaretColour;
	bool showCaretLineBackground;
	Colour/*Pair*/ caretLineBackground;
	int caretLineAlpha;
	Colour/*Pair*/ edgecolour;
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

	ViewStyle();
	ViewStyle(const ViewStyle &source);
	~ViewStyle();
	void Init(size_t stylesSize_=64);
	//void RefreshColourPalette(Palette &pal, bool want);
	void Refresh(Surface &surface);
	void AllocStyles(size_t sizeNew);
	void EnsureStyle(size_t index);
	void ResetDefaultStyle();
	void ClearStyles();
	void SetStyleFontName(int styleIndex, const char *name);
	bool ProtectionActive() const;
	bool ValidStyle(size_t styleIndex) const;
};

#ifdef SCI_NAMESPACE
}
#endif

#endif
