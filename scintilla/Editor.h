// Scintilla source code edit control
/** @file Editor.h
 ** Defines the main editor class.
 **/
// Copyright 1998-2011 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef EDITOR_H
#define EDITOR_H

#ifdef SCI_NAMESPACE
namespace Scintilla {
#endif

/**
 */
class Caret {
public:
	bool active;
	bool on;
	int period;

	Caret();
};

/**
 */
class Timer {
public:
	bool ticking;
	int ticksToWait;
	enum {tickSize = 100};
	TickerID tickerID;

	Timer();
};

/**
 */
class Idler {
public:
	bool state;
	IdlerID idlerID;

	Idler();
};

/**
 * When platform has a way to generate an event before painting,
 * accumulate needed styling range in StyleNeeded to avoid unnecessary work.
 */
class StyleNeeded {
public:
	bool active;
	Position upTo;

	StyleNeeded() : active(false), upTo(0) {}
	void Reset() {
		active = false;
		upTo = 0;
	}
	void NeedUpTo(Position pos) {
		if (upTo < pos)
			upTo = pos;
	}
};

/**
 * Hold a piece of text selected for copying or dragging.
 * The text is expected to hold a terminating '\0' and this is counted in len.
 */
class SelectionText {
public:
	char *s;
	int len;
	bool rectangular;
	bool lineCopy;
	SelectionText() : s(0), len(0), rectangular(false), lineCopy(false)/*, codePage(0), characterSet(0)*/ {}
	~SelectionText() {
		Free();
	}
	void Free() {
		Set(0, 0, false, false);
	}
	void Set(char *s_, int len_, bool rectangular_, bool lineCopy_) {
		delete []s;
		s = s_;
		if (s)
			len = len_;
		else
			len = 0;
		rectangular = rectangular_;
		lineCopy = lineCopy_;
	}
	void Copy(const char *s_, int len_, bool rectangular_, bool lineCopy_) {
		delete []s;
		s = 0;
		s = new char[len_];
		len = len_;
		for (int i = 0; i < len_; i++) {
			s[i] = s_[i];
		}
		rectangular = rectangular_;
		lineCopy = lineCopy_;
	}
	void Copy(const SelectionText &other) {
		Copy(other.s, other.len, other.rectangular, other.lineCopy);
	}
};

/**
 */
class Editor : public DocWatcher {
	// Private so Editor objects can not be copied
	Editor(const Editor &);
	Editor &operator=(const Editor &);

protected:	// ScintillaBase subclass needs access to much of Editor
	/** Style resources may be expensive to allocate so are cached between uses.
	 * When a style attribute is changed, this cache is flushed. */
	bool stylesValid;
	ViewStyle vs;
	Point sizeRGBAImage;

	int printMagnification;
	int printColourMode;
	int printWrapState;
	int cursorMode;
	int controlCharSymbol;

	// Highlight current folding block
	HighlightDelimiter highlightDelimiter;

	bool hasFocus;
	bool hideSelection;
	bool inOverstrike;
	bool mouseDownCaptures;

	/** In bufferedDraw mode, graphics operations are drawn to a pixmap and then copied to
	 * the screen. This avoids flashing but is about 30% slower. */
	//bool bufferedDraw;
	/** In twoPhaseDraw mode, drawing is performed in two phases, first the background
	* and then the foreground. This avoids chopping off characters that overlap the next run. */
	bool twoPhaseDraw;

	int xOffset;		///< Horizontal scrolled amount in pixels
	int xCaretMargin;	///< Ensure this many pixels visible on both sides of caret
	bool horizontalScrollBarVisible;
	int scrollWidth;
	bool trackLineWidth;
	int lineWidthMaxSeen;
	bool verticalScrollBarVisible;
	bool endAtLastLine;
	int caretSticky;
	int marginOptions;
	bool multipleSelection;
	bool additionalSelectionTyping;
	int multiPasteMode;
	bool additionalCaretsBlink;
	bool additionalCaretsVisible;

	int virtualSpaceOptions;

	Surface*	drawSurface;

	Pixmap	pixmapSelPattern;
	Pixmap	pixmapIndentGuide;
	Pixmap	pixmapIndentGuideHighlight;

	LineLayoutCache llc;
	PositionCache posCache;

	KeyMap kmap;

	Caret caret;
	Timer timer;
	Timer autoScrollTimer;
	enum { autoScrollDelay = 200 };

	Idler idler;

	Point lastClick;
	unsigned int lastClickTime;
	int dwellDelay;
	int ticksToDwell;
	bool dwelling;
	enum { selChar, selWord, selSubLine, selWholeLine } selectionType;
	Point ptMouseLast;
	enum { ddNone, ddInitial, ddDragging } inDragDrop;
	bool dropWentOutside;
	SelectionPosition posDrag;
	SelectionPosition posDrop;
	int hotSpotClickPos;
	int lastXChosen;
	int lineAnchorPos;
	int originalAnchorPos;
	int wordSelectAnchorStartPos;
	int wordSelectAnchorEndPos;
	int wordSelectInitialCaretPos;
	int targetStart;
	int targetEnd;
	int searchFlags;
	int topLine;
	int posTopLine;
	int lengthForEncode;

	int needUpdateUI;
	Position braces[2];
	int bracesMatchStyle;
	int highlightGuideColumn;

	int theEdge;

	enum { notPainting, painting, paintAbandoned } paintState;
	PRectangle rcPaint;
	bool paintingAllText;
	StyleNeeded styleNeeded;

	int modEventMask;

	SelectionText drag;
	Selection sel;
	bool primarySelection;

	int caretXPolicy;
	int caretXSlop;	///< Ensure this many pixels visible on both sides of caret

	int caretYPolicy;
	int caretYSlop;	///< Ensure this many lines visible on both sides of caret

	int visiblePolicy;
	int visibleSlop;

	int searchAnchor;

	bool recordingMacro;

	int foldFlags;
	ContractionState cs;

	// Hotspot support
	int hsStart;
	int hsEnd;

	// Wrapping support
	enum { eWrapNone, eWrapWord, eWrapChar } wrapState;
	enum { wrapLineLarge = 0x7ffffff };
	int wrapWidth;
	int wrapStart;
	int wrapEnd;
	int wrapVisualFlags;
	int wrapVisualFlagsLocation;
	int wrapVisualStartIndent;
	int wrapIndentMode; // SC_WRAPINDENT_FIXED, _SAME, _INDENT

	bool convertPastes;

	Document *pdoc;

	//TODO: check whether 2 following functions are required
	virtual void Initialise() {}
	virtual void Finalise();

	void InvalidateStyleData();
	void InvalidateStyleRedraw();
	void RefreshStyleData();

	PRectangle clientRect;

	virtual PRectangle GetClientRectangle();
	PRectangle GetTextRectangle();

	int LinesOnScreen();
	int LinesToScroll();
	int MaxScrollPos();
	SelectionPosition ClampPositionIntoDocument(SelectionPosition sp) const;
	Point LocationFromPosition(SelectionPosition pos);
	Point LocationFromPosition(int pos);
	int XFromPosition(int pos);
	int XFromPosition(SelectionPosition sp);
	SelectionPosition SPositionFromLocation(Point pt, bool canReturnInvalid=false, bool charPosition=false, bool virtualSpace=true);
	int PositionFromLocation(Point pt, bool canReturnInvalid=false, bool charPosition=false);
	SelectionPosition SPositionFromLineX(int lineDoc, int x);
	int PositionFromLineX(int line, int x);
	int LineFromLocation(Point pt);
	void SetTopLine(int topLineNew);

	PRectangle RectangleFromRange(int start, int end);

	bool UserVirtualSpace() const {
		return ((virtualSpaceOptions & SCVS_USERACCESSIBLE) != 0);
	}
	int CurrentPosition();
	bool SelectionEmpty();
	SelectionPosition SelectionStart();
	SelectionPosition SelectionEnd();
	void SetRectangularRange();
	void ThinRectangularRange();
	void InvalidateSelection(SelectionRange newMain, bool invalidateWholeSelection=false);
	void SetSelection(SelectionPosition currentPos_, SelectionPosition anchor_);
	void SetSelection(int currentPos_, int anchor_);
	void SetSelection(SelectionPosition currentPos_);
	void SetSelection(int currentPos_);
	void SetEmptySelection(SelectionPosition currentPos_);
	void SetEmptySelection(int currentPos_);
	bool RangeContainsProtected(int start, int end) const;
	bool SelectionContainsProtected();
	int MovePositionOutsideChar(int pos, int moveDir, bool checkLineEnd=true) const;
	SelectionPosition MovePositionOutsideChar(SelectionPosition pos, int moveDir, bool checkLineEnd=true) const;
	int MovePositionTo(SelectionPosition newPos, Selection::selTypes sel=Selection::noSel, bool ensureVisible=true);
	int MovePositionTo(int newPos, Selection::selTypes sel=Selection::noSel, bool ensureVisible=true);
	SelectionPosition MovePositionSoVisible(SelectionPosition pos, int moveDir);
	SelectionPosition MovePositionSoVisible(int pos, int moveDir);
	Point PointMainCaret();
	void SetLastXChosen();

	void ScrollTo(int line, bool moveThumb=true);
	virtual void ScrollText(int linesToMove);
	void HorizontalScrollTo(int xPos);
	void VerticalCentreCaret();
	void MoveSelectedLines(int lineDelta);
	void MoveSelectedLinesUp();
	void MoveSelectedLinesDown();
	void MoveCaretInsideView(bool ensureVisible=true);
	int DisplayFromPosition(int pos);

	struct XYScrollPosition {
		int xOffset;
		int topLine;
		XYScrollPosition(int xOffset_, int topLine_) : xOffset(xOffset_), topLine(topLine_) {}
	};
	XYScrollPosition XYScrollToMakeVisible(const bool useMargin, const bool vert, const bool horiz);
	void SetXYScroll(XYScrollPosition newXY);
	void EnsureCaretVisible(bool useMargin=true, bool vert=true, bool horiz=true);
	void ShowCaretAtCurrentPosition();
	void DropCaret();
	void InvalidateCaret();
	virtual void UpdateSystemCaret();

	void NeedWrapping(int docLineStart = 0, int docLineEnd = wrapLineLarge);
	bool WrapOneLine(Surface *surface, int lineToWrap);
	bool WrapLines(bool fullWrap, int priorityWrapLineStart);
	void LinesJoin();
	void LinesSplit(int pixelWidth);

	int SubstituteMarkerIfEmpty(int markerCheck, int markerDefault);
	void PaintSelMargin(Surface *surface, PRectangle &rc);
	LineLayout *RetrieveLineLayout(int lineNumber);
	void LayoutLine(int line, Surface *surface, ViewStyle &vstyle, LineLayout *ll,
		int width=LineLayout::wrapWidthInfinite);
	Colour SelectionBackground(ViewStyle &vsDraw, bool main);
	Colour TextBackground(ViewStyle &vsDraw, bool overrideBackground, Colour background, int inSelection, bool inHotspot, int styleMain, int i, LineLayout *ll);
	void DrawIndentGuide(Surface *surface, int lineVisible, float lineHeight, int start, PRectangle rcSegment, bool highlight);
	void DrawWrapMarker(Surface *surface, PRectangle rcPlace, bool isEndMarker, Colour wrapColour);
	void DrawEOL(Surface *surface, ViewStyle &vsDraw, PRectangle rcLine, LineLayout *ll,
		int line, int lineEnd, int xStart, int subLine, double subLineStart,
		bool overrideBackground, Colour background,
		bool drawWrapMark, Colour wrapColour);
	void DrawIndicator(int indicNum, int startPos, int endPos, Surface *surface, ViewStyle &vsDraw,
		int xStart, PRectangle rcLine, LineLayout *ll, int subLine);
	void DrawIndicators(Surface *surface, ViewStyle &vsDraw, int line, int xStart,
		PRectangle rcLine, LineLayout *ll, int subLine, int lineEnd, bool under);
	void DrawAnnotation(Surface *surface, ViewStyle &vsDraw, int line, int xStart,
        PRectangle rcLine, LineLayout *ll, int subLine);
	void DrawLine(Surface *surface, ViewStyle &vsDraw, int line, int lineVisible, int xStart,
		PRectangle rcLine, LineLayout *ll, int subLine);
	void DrawBlockCaret(Surface *surface, ViewStyle &vsDraw, LineLayout *ll, int subLine,
		int xStart, int offset, int posCaret, PRectangle rcCaret, Colour caretColour);
	void DrawCarets(Surface *surface, ViewStyle &vsDraw, int line, int xStart,
		PRectangle rcLine, LineLayout *ll, int subLine);
	void RefreshPixMaps(Surface *surfaceWindow);
	long FormatRange(bool draw, Sci_RangeToFormat *pfr);
	int TextWidth(int style, const char *text);

	virtual void SetVerticalScrollPos() {}
	virtual void SetHorizontalScrollPos() {}
	virtual bool ModifyScrollBars(int /*nMax*/, int /*nPage*/) {return true;}
	virtual void ReconfigureScrollBars();
	void SetScrollBars();
	void ChangeSize();

	void FilterSelections();
	int InsertSpace(int position, unsigned int spaces);
	void InsertPaste(SelectionPosition selStart, const char *text, int len);
	void ClearSelection(bool retainMultipleSelections=false);
	void ClearAll();
	void ClearDocumentStyle();
	void Cut();
	void PasteRectangular(SelectionPosition pos, const char *ptr, int len);
	virtual void Copy();
	virtual void CopyAllowLine();
	bool CanPaste();
	void Paste();
	void Clear();
	void SelectAll();
	void Undo();
	void Redo();
	void DelChar();
	void DelCharBack(bool allowLineStartDeletion);

	enum Cursor { cursorInvalid, cursorText, cursorArrow, cursorUp, cursorWait, cursorHoriz, cursorVert, cursorReverseArrow, cursorHand };

	virtual void NotifyChange() {}
	virtual void NotifyFocus(bool focus);
	virtual void NotifyParent(SCNotification /*scn*/) {};
	virtual void NotifySetCursor(Cursor /*newCursor*/) {}
	virtual void NotifyStyleToNeeded(int endStyleNeeded);
	void NotifyChar(int ch);
	void NotifySavePoint(bool isSavePoint);
	void NotifyModifyAttempt();
	virtual void NotifyDoubleClick(Point pt, bool shift, bool ctrl, bool alt);
	void NotifyHotSpotClicked(int position, bool shift, bool ctrl, bool alt);
	void NotifyHotSpotDoubleClicked(int position, bool shift, bool ctrl, bool alt);
	void NotifyHotSpotReleaseClick(int position, bool shift, bool ctrl, bool alt);
	void NotifyUpdateUI();
	void NotifyIndicatorClick(bool click, int position, bool shift, bool ctrl, bool alt);
	bool NotifyMarginClick(Point pt, bool shift, bool ctrl, bool alt);
	void NotifyNeedShown(int pos, int len);
	void NotifyDwelling(Point pt, bool state);
	void NotifyZoom();

	void NotifyModifyAttempt(Document *document, void *userData);
	void NotifySavePoint(Document *document, void *userData, bool atSavePoint);
	void CheckModificationForWrap(DocModification mh);
	void NotifyModified(Document *document, DocModification mh, void *userData);
	void NotifyDeleted(Document *document, void *userData);
	void NotifyStyleNeeded(Document *doc, void *userData, int endPos);
	void NotifyLexerChanged(Document *doc, void *userData);
	void NotifyErrorOccurred(Document *doc, void *userData, int status);
	void NotifyMacroRecord(unsigned int iMessage, uptr_t wParam, sptr_t lParam);

	void ContainerNeedsUpdate(int flags);
	void PageMove(int direction, Selection::selTypes sel=Selection::noSel, bool stuttered = false);
	enum { cmSame, cmUpper, cmLower } caseMap;
	virtual std::string CaseMapString(const std::string &s, int caseMapping);
	void ChangeCaseOfSelection(int caseMapping);
	void LineTranspose();
	void Duplicate(bool forLine);
	virtual void CancelModes();
	void NewLine();
	void CursorUpOrDown(int direction, Selection::selTypes sel=Selection::noSel);
	void ParaUpOrDown(int direction, Selection::selTypes sel=Selection::noSel);
	int StartEndDisplayLine(int pos, bool start);
	virtual int KeyCommand(unsigned int iMessage);
	virtual int KeyDefault(int /* key */, int /*modifiers*/);

	void Indent(bool forwards);

	virtual CaseFolder *CaseFolderForEncoding();
	long FindText(uptr_t wParam, sptr_t lParam);
	void SearchAnchor();
	long SearchText(unsigned int iMessage, uptr_t wParam, sptr_t lParam);
	long SearchInTarget(const char *text, int length);
	void GoToLine(int lineNo);

	void CopyToClipboard(const SelectionText &selectedText);
	char *CopyRange(int start, int end);
	void CopySelectionRange(SelectionText *ss, bool allowLineCopy=false);
	void CopyRangeToClipboard(int start, int end);
	void CopyText(int length, const char *text);
	void SetDragPosition(SelectionPosition newPos);
	virtual bool DragThreshold(Point ptStart, Point ptNow);
	virtual void StartDrag();
	void DropAt(SelectionPosition position, const char *value, bool moving, bool rectangular);
	/** PositionInSelection returns true if position in selection. */
	bool PositionInSelection(int pos);
	bool PointInSelection(Point pt);
	bool PointInSelMargin(Point pt);
	Cursor GetMarginCursor(Point pt);
	void LineSelection(int lineCurrentPos_, int lineAnchorPos_, bool wholeLine);
	void WordSelection(int pos);
	void DwellEnd(bool mouseMoved);
	void MouseLeave();
	virtual void ButtonDown(Point pt, unsigned int curTime, bool shift, bool ctrl, bool alt);
	void ButtonMove(Point pt);
	void ButtonUp(Point pt, unsigned int curTime, bool ctrl);

	bool Idle();
	virtual bool SetIdle(bool) { return false; }
	virtual void SetMouseCapture(bool /*on*/) {}
	virtual bool HaveMouseCapture() { return false; }

	int PositionAfterArea(PRectangle rcArea);
	void StyleToPositionInView(Position pos);
	void IdleStyling();
	virtual void QueueStyling(int upTo);

	void SetBraceHighlight(Position pos0, Position pos1, int matchStyle);

	void SetAnnotationHeights(int start, int end);
	void SetDocPointer(Document *document);

	void SetAnnotationVisible(int visible);

	void Expand(int &line, bool doExpand);
	void ToggleContraction(int line);
	int ContractedFoldNext(int lineStart);
	void EnsureLineVisible(int lineDoc, bool enforcePolicy);
	int GetTag(char *tagValue, int tagNumber);
	int ReplaceTarget(bool replacePatterns, const char *text, int length=-1);

	bool PositionIsHotspot(int position);
	bool PointIsHotspot(Point pt);
	void SetHotSpotRange(Point *pt);
	void GetHotSpotRange(int &hsStart, int &hsEnd);

	void SetFocusState(bool focusState);

	int WrapCount(int line);
	void AddStyledText(char *buffer, int appendLength);

	void StyleSetMessage(unsigned int iMessage, uptr_t wParam, sptr_t lParam);
	sptr_t StyleGetMessage(unsigned int iMessage, uptr_t wParam, sptr_t lParam);

	static const char *StringFromEOLMode(int eolMode);

	static sptr_t StringResult(sptr_t lParam, const char *val);

public:
	Editor();
	virtual ~Editor();

	void SetSize(float width, float height)	{clientRect = PRectangle(0, 0, width, height);}
	
	//temporary to work with lexers
	Document* GetDocument() {return pdoc;}
	void SetLexer(LexInterface* ls) {pdoc->pli = ls;}

	void Tick();
	void Paint();

	int  KeyDown(int key, bool shift, bool ctrl, bool alt, bool *consumed=0);
	int  KeyDownWithModifiers(int key, int modifiers, bool *consumed);
	void AddChar(char ch);
	void AddCharUTF(char *s, unsigned int len);

	sptr_t Command(unsigned int iMessage, uptr_t wParam=0, sptr_t lParam=0);

	int errorStatus;
};

#ifdef SCI_NAMESPACE
}
#endif

#endif
