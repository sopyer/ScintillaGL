// Scintilla source code edit control
/** @file ViewStyle.cxx
 ** Store information on how the document is to be viewed.
 **/
// Copyright 1998-2003 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <string.h>

#include "Platform.h"

#include "Scintilla.h"
#include "SplitVector.h"
#include "Partitioning.h"
#include "RunStyles.h"
#include "Indicator.h"
#include "XPM.h"
#include "LineMarker.h"
#include "Style.h"
#include "ViewStyle.h"

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

MarginStyle::MarginStyle() :
	style(SC_MARGIN_SYMBOL), width(0), mask(0), sensitive(false), cursor(SC_CURSORREVERSEARROW) {
}

// A list of the fontnames - avoids wasting space in each style
FontNames::FontNames() {
	size = 8;
	names = new char *[size];
	max = 0;
}

FontNames::~FontNames() {
	Clear();
	delete []names;
	names = 0;
}

void FontNames::Clear() {
	for (int i=0; i<max; i++) {
		delete []names[i];
	}
	max = 0;
}

const char *FontNames::Save(const char *name) {
	if (!name)
		return 0;
	for (int i=0; i<max; i++) {
		if (strcmp(names[i], name) == 0) {
			return names[i];
		}
	}
	if (max >= size) {
		// Grow array
		int sizeNew = size * 2;
		char **namesNew = new char *[sizeNew];
		for (int j=0; j<max; j++) {
			namesNew[j] = names[j];
		}
		delete []names;
		names = namesNew;
		size = sizeNew;
	}
	names[max] = new char[strlen(name) + 1];
	strcpy(names[max], name);
	max++;
	return names[max-1];
}

ViewStyle::ViewStyle() {
	Init();
}

ViewStyle::ViewStyle(const ViewStyle &source) {
	Init(source.stylesSize);
	for (unsigned int sty=0; sty<source.stylesSize; sty++) {
		styles[sty] = source.styles[sty];
		// Can't just copy fontname as its lifetime is relative to its owning ViewStyle
		styles[sty].fontName = fontNames.Save(source.styles[sty].fontName);
	}
	for (int mrk=0; mrk<=MARKER_MAX; mrk++) {
		markers[mrk] = source.markers[mrk];
	}
	for (int ind=0; ind<=INDIC_MAX; ind++) {
		indicators[ind] = source.indicators[ind];
	}

	selforeset = source.selforeset;
	selforeground = source.selforeground;
	selAdditionalForeground = source.selAdditionalForeground;
	selbackset = source.selbackset;
	selbackground = source.selbackground;
	selAdditionalBackground = source.selAdditionalBackground;
	selbackground2 = source.selbackground2;
	selAlpha = source.selAlpha;
	selAdditionalAlpha = source.selAdditionalAlpha;
	selEOLFilled = source.selEOLFilled;

	foldmarginColourSet = source.foldmarginColourSet;
	foldmarginColour = source.foldmarginColour;
	foldmarginHighlightColourSet = source.foldmarginHighlightColourSet;
	foldmarginHighlightColour = source.foldmarginHighlightColour;

	hotspotForegroundSet = source.hotspotForegroundSet;
	hotspotForeground = source.hotspotForeground;
	hotspotBackgroundSet = source.hotspotBackgroundSet;
	hotspotBackground = source.hotspotBackground;
	hotspotUnderline = source.hotspotUnderline;
	hotspotSingleLine = source.hotspotSingleLine;

	whitespaceForegroundSet = source.whitespaceForegroundSet;
	whitespaceForeground = source.whitespaceForeground;
	whitespaceBackgroundSet = source.whitespaceBackgroundSet;
	whitespaceBackground = source.whitespaceBackground;
	selbar = source.selbar;
	selbarlight = source.selbarlight;
	caretcolour = source.caretcolour;
	additionalCaretColour = source.additionalCaretColour;
	showCaretLineBackground = source.showCaretLineBackground;
	caretLineBackground = source.caretLineBackground;
	caretLineAlpha = source.caretLineAlpha;
	edgecolour = source.edgecolour;
	edgeState = source.edgeState;
	caretStyle = source.caretStyle;
	caretWidth = source.caretWidth;
	someStylesProtected = false;
	someStylesForceCase = false;
	leftMarginWidth = source.leftMarginWidth;
	rightMarginWidth = source.rightMarginWidth;
	for (int i=0; i < margins; i++) {
		ms[i] = source.ms[i];
	}
	symbolMargin = source.symbolMargin;
	maskInLine = source.maskInLine;
	fixedColumnWidth = source.fixedColumnWidth;
	zoomLevel = source.zoomLevel;
	viewWhitespace = source.viewWhitespace;
	whitespaceSize = source.whitespaceSize;
	viewIndentationGuides = source.viewIndentationGuides;
	viewEOL = source.viewEOL;
	showMarkedLines = source.showMarkedLines;
	extraFontFlag = source.extraFontFlag;
	extraAscent = source.extraAscent;
	extraDescent = source.extraDescent;
	marginStyleOffset = source.marginStyleOffset;
	annotationVisible = source.annotationVisible;
	annotationStyleOffset = source.annotationStyleOffset;
}

ViewStyle::~ViewStyle() {
	delete []styles;
	styles = NULL;
}

void ViewStyle::Init(size_t stylesSize_) {
	stylesSize = 0;
	styles = NULL;
	AllocStyles(stylesSize_);
	fontNames.Clear();
	ResetDefaultStyle();

	indicators[0].style = INDIC_SQUIGGLE;
	indicators[0].under = false;
	indicators[0].fore = MakeRGBA(0, 0x7f, 0);
	indicators[1].style = INDIC_TT;
	indicators[1].under = false;
	indicators[1].fore = MakeRGBA(0, 0, 0xff);
	indicators[2].style = INDIC_PLAIN;
	indicators[2].under = false;
	indicators[2].fore = MakeRGBA(0xff, 0, 0);

	lineHeight = 1;
	maxAscent = 1;
	maxDescent = 1;
	aveCharWidth = 8;
	spaceWidth = 8;

	selforeset = false;
	selforeground = MakeRGBA(0xff, 0, 0);
	selAdditionalForeground = MakeRGBA(0xff, 0, 0);
	selbackset = true;
	selbackground = MakeRGBA(0xc0, 0xc0, 0xc0);
	selAdditionalBackground = MakeRGBA(0xd7, 0xd7, 0xd7);
	selbackground2 = MakeRGBA(0xb0, 0xb0, 0xb0);
	selAlpha = SC_ALPHA_NOALPHA;
	selAdditionalAlpha = SC_ALPHA_NOALPHA;
	selEOLFilled = false;

	foldmarginColourSet = false;
	foldmarginColour = MakeRGBA(0xff, 0, 0);
	foldmarginHighlightColourSet = false;
	foldmarginHighlightColour = MakeRGBA(0xc0, 0xc0, 0xc0);

	whitespaceForegroundSet = false;
	whitespaceForeground = MakeRGBA(0, 0, 0);
	whitespaceBackgroundSet = false;
	whitespaceBackground = MakeRGBA(0xff, 0xff, 0xff);
	selbar = Platform::Chrome();
	selbarlight = Platform::ChromeHighlight();
	styles[STYLE_LINENUMBER].fore = MakeRGBA(0, 0, 0);
	styles[STYLE_LINENUMBER].back = Platform::Chrome();
	caretcolour = MakeRGBA(0, 0, 0);
	additionalCaretColour = MakeRGBA(0x7f, 0x7f, 0x7f);
	showCaretLineBackground = false;
	caretLineBackground = MakeRGBA(0xff, 0xff, 0);
	caretLineAlpha = SC_ALPHA_NOALPHA;
	edgecolour = MakeRGBA(0xc0, 0xc0, 0xc0);
	edgeState = EDGE_NONE;
	caretStyle = CARETSTYLE_LINE;
	caretWidth = 1;
	someStylesProtected = false;
	someStylesForceCase = false;

	hotspotForegroundSet = false;
	hotspotForeground = MakeRGBA(0, 0, 0xff);
	hotspotBackgroundSet = false;
	hotspotBackground = MakeRGBA(0xff, 0xff, 0xff);
	hotspotUnderline = true;
	hotspotSingleLine = true;

	leftMarginWidth = 1;
	rightMarginWidth = 1;
	ms[0].style = SC_MARGIN_NUMBER;
	ms[0].width = 0;
	ms[0].mask = 0;
	ms[1].style = SC_MARGIN_SYMBOL;
	ms[1].width = 16;
	ms[1].mask = ~SC_MASK_FOLDERS;
	ms[2].style = SC_MARGIN_SYMBOL;
	ms[2].width = 0;
	ms[2].mask = 0;
	fixedColumnWidth = leftMarginWidth;
	symbolMargin = false;
	maskInLine = 0xffffffff;
	for (int margin=0; margin < margins; margin++) {
		fixedColumnWidth += ms[margin].width;
		symbolMargin = symbolMargin || (ms[margin].style != SC_MARGIN_NUMBER);
		if (ms[margin].width > 0)
			maskInLine &= ~ms[margin].mask;
	}
	zoomLevel = 0;
	viewWhitespace = wsInvisible;
	whitespaceSize = 1;
	viewIndentationGuides = ivNone;
	viewEOL = false;
	showMarkedLines = true;
	extraFontFlag = 0;
	extraAscent = 0;
	extraDescent = 0;
	marginStyleOffset = 0;
	annotationVisible = ANNOTATION_HIDDEN;
	annotationStyleOffset = 0;
}

void ViewStyle::Refresh(Surface &surface) {
	selbar = Platform::Chrome();
	selbarlight = Platform::ChromeHighlight();
	styles[STYLE_DEFAULT].Realise(surface, zoomLevel, NULL, extraFontFlag);
	maxAscent = styles[STYLE_DEFAULT].ascent;
	maxDescent = styles[STYLE_DEFAULT].descent;
	someStylesProtected = false;
	someStylesForceCase = false;
	for (unsigned int i=0; i<stylesSize; i++) {
		if (i != STYLE_DEFAULT) {
			styles[i].Realise(surface, zoomLevel, &styles[STYLE_DEFAULT], extraFontFlag);
			if (maxAscent < styles[i].ascent)
				maxAscent = styles[i].ascent;
			if (maxDescent < styles[i].descent)
				maxDescent = styles[i].descent;
		}
		if (styles[i].IsProtected()) {
			someStylesProtected = true;
		}
		if (styles[i].caseForce != Style::caseMixed) {
			someStylesForceCase = true;
		}
	}
	maxAscent += extraAscent;
	maxDescent += extraDescent;

	lineHeight = maxAscent + maxDescent;
	aveCharWidth = styles[STYLE_DEFAULT].aveCharWidth;
	spaceWidth = styles[STYLE_DEFAULT].spaceWidth;

	fixedColumnWidth = leftMarginWidth;
	symbolMargin = false;
	maskInLine = 0xffffffff;
	for (int margin=0; margin < margins; margin++) {
		fixedColumnWidth += ms[margin].width;
		symbolMargin = symbolMargin || (ms[margin].style != SC_MARGIN_NUMBER);
		if (ms[margin].width > 0)
			maskInLine &= ~ms[margin].mask;
	}
}

void ViewStyle::AllocStyles(size_t sizeNew) {
	Style *stylesNew = new Style[sizeNew];
	size_t i=0;
	for (; i<stylesSize; i++) {
		stylesNew[i] = styles[i];
		stylesNew[i].fontName = styles[i].fontName;
	}
	if (stylesSize > STYLE_DEFAULT) {
		for (; i<sizeNew; i++) {
			if (i != STYLE_DEFAULT) {
				stylesNew[i].ClearTo(styles[STYLE_DEFAULT]);
			}
		}
	}
	delete []styles;
	styles = stylesNew;
	stylesSize = sizeNew;
}

void ViewStyle::EnsureStyle(size_t index) {
	if (index >= stylesSize) {
		size_t sizeNew = stylesSize * 2;
		while (sizeNew <= index)
			sizeNew *= 2;
		AllocStyles(sizeNew);
	}
}

void ViewStyle::ResetDefaultStyle() {
	styles[STYLE_DEFAULT].Clear(MakeRGBA(0,0,0),
		MakeRGBA(0xff,0xff,0xff),
		Platform::DefaultFontSize(), fontNames.Save(Platform::DefaultFont()),
		SC_CHARSET_DEFAULT,
		false, false, false, false, Style::caseMixed, true, true, false);
}

void ViewStyle::ClearStyles() {
	// Reset all styles to be like the default style
	for (unsigned int i=0; i<stylesSize; i++) {
		if (i != STYLE_DEFAULT) {
			styles[i].ClearTo(styles[STYLE_DEFAULT]);
		}
	}
	styles[STYLE_LINENUMBER].back = Platform::Chrome();

	// Set call tip fore/back to match the values previously set for call tips
	styles[STYLE_CALLTIP].back = MakeRGBA(0xff, 0xff, 0xff);
	styles[STYLE_CALLTIP].fore = MakeRGBA(0x80, 0x80, 0x80);
}

void ViewStyle::SetStyleFontName(int styleIndex, const char *name) {
	styles[styleIndex].fontName = fontNames.Save(name);
}

bool ViewStyle::ProtectionActive() const {
	return someStylesProtected;
}

bool ViewStyle::ValidStyle(size_t styleIndex) const {
	return styleIndex < stylesSize;
}

