#pragma once

#include <new>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <time.h>

#include <string>
#include <vector>

#include "Platform.h"

#include "ILexer.h"
#include "Scintilla.h"
#ifdef SCI_LEXER
#include "SciLexer.h"
#endif
#include "SVector.h"
#include "SplitVector.h"
#include "Partitioning.h"
#include "RunStyles.h"
#include "ContractionState.h"
#include "CellBuffer.h"
#include "CallTip.h"
#include "KeyMap.h"
#include "Indicator.h"
#include "XPM.h"
#include "LineMarker.h"
#include "Style.h"
#include "AutoComplete.h"
#include "ViewStyle.h"
#include "Decoration.h"
#include "CharClassify.h"
#include "Document.h"
#include "Selection.h"
#include "PositionCache.h"
#include "Editor.h"
#include "ScintillaBase.h"
#include "UniConversion.h"

#ifdef SCI_LEXER
#include "LexerModule.h"
#include "ExternalLexer.h"
#endif
#ifdef SCI_LEXER
#include "SciLexer.h"
#include "LexerModule.h"
#include "Catalogue.h"
#endif

class LexState2 : public LexInterface {
	const LexerModule *lexCurrent;
public:
	int lexLanguage;

	LexState2(Document *pdoc_) : LexInterface(pdoc_) {
		lexCurrent = 0;
		performingStyle = false;
		lexLanguage = SCLEX_CONTAINER;
	}

	~LexState2() {
		if (instance) {
			instance->Release();
			instance = 0;
		}
	}

	void SetLexerModule(const LexerModule *lex) {
		if (lex != lexCurrent) {
			if (instance) {
				instance->Release();
				instance = 0;
			}
			lexCurrent = lex;
			if (lexCurrent)
				instance = lexCurrent->Create();
			pdoc->LexerChanged();
		}
	}

	void SetLexer(uptr_t wParam) {
		lexLanguage = wParam;
		if (lexLanguage == SCLEX_CONTAINER) {
			SetLexerModule(0);
		} else {
			const LexerModule *lex = Catalogue::Find(lexLanguage);
			if (!lex)
				lex = Catalogue::Find(SCLEX_NULL);
			SetLexerModule(lex);
		}
	}

	void SetLexerLanguage(const char *languageName) {
		const LexerModule *lex = Catalogue::Find(languageName);
		if (!lex)
			lex = Catalogue::Find(SCLEX_NULL);
		if (lex)
			lexLanguage = lex->GetLanguage();
		SetLexerModule(lex);
	}

	void SetWordList(int n, const char *wl) {
		if (instance) {
			int firstModification = instance->WordListSet(n, wl);
			if (firstModification >= 0) {
				pdoc->ModifiedAt(firstModification);
			}
		}
	}

	void PropSet(const char *key, const char *val) {
		if (instance) {
			int firstModification = instance->PropertySet(key, val);
			if (firstModification >= 0) {
				pdoc->ModifiedAt(firstModification);
			}
		}
	}
	//const char *DescribeWordListSets();
	//void SetWordList(int n, const char *wl);
	//int GetStyleBitsNeeded() const;
	//const char *GetName() const;
	//void *PrivateCall(int operation, void *pointer);
	//const char *PropertyNames();
	//int PropertyType(const char *name);
	//const char *DescribeProperty(const char *name);
	//void PropSet(const char *key, const char *val);
	//const char *PropGet(const char *key) const;
	//int PropGetInt(const char *key, int defaultValue=0) const;
	//int PropGetExpanded(const char *key, char *result) const;
};

#ifdef SCI_NAMESPACE
}
#endif

class MyEditor: public Editor
{
	size_t nextTime;
	static const size_t tickInterval = 100;
	CLIPFORMAT cfColumnSelect;
	CLIPFORMAT cfLineSelect;
public:
	HWND hWnd;
	LexState2 ls;
	
	MyEditor();

	virtual void Initialise() {}

	virtual void SetVerticalScrollPos() {}
	virtual void SetHorizontalScrollPos() {}
	virtual bool ModifyScrollBars(int /*nMax*/, int /*nPage*/) {return true;}

	void InsertPasteText(const char *text, int len, SelectionPosition selStart, bool isRectangular, bool isLine);
	bool CanPaste();

	virtual void Copy();// {}
	virtual void Paste();// {}
	virtual void CopyToClipboard(const SelectionText &/*selectedText*/);// {}

	virtual void ClaimSelection() {}
	virtual void NotifyChange() {}
	virtual void NotifyParent(SCNotification /*scn*/) {}
	virtual void SetTicking(bool /*on*/) {}
	virtual void SetMouseCapture(bool /*on*/) {}
	virtual bool HaveMouseCapture() {return false;}
	virtual sptr_t DefWndProc(unsigned int /*iMessage*/, uptr_t /*wParam*/, sptr_t /*lParam*/) {return 0;}

	LRESULT Command(UINT Msg, WPARAM wParam=0, LPARAM lParam=0) {
		return WndProc(Msg, wParam, lParam);
	}

	void FindMatchingBracePos(int & braceAtCaret, int & braceOpposite);

	bool BraceMatch();

	void Paint();

	void AddCharUTF(char c) {Editor::AddCharUTF(&c, 1);}
};
