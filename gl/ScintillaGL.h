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

#include "windows.h"

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
public:
	LexState2 ls;
	MyEditor():ls(pdoc)
	{
		nextTime = timeGetTime()+tickInterval;
		pdoc->pli = &ls;
	}

	virtual void Initialise() {}
	virtual void SetVerticalScrollPos() {}
	virtual void SetHorizontalScrollPos() {}
	virtual bool ModifyScrollBars(int /*nMax*/, int /*nPage*/) {return true;}
	virtual void Copy() {}
	virtual void Paste() {}
	virtual void ClaimSelection() {}
	virtual void NotifyChange() {}
	virtual void NotifyParent(SCNotification /*scn*/) {}
	virtual void CopyToClipboard(const SelectionText &/*selectedText*/) {}
	virtual void SetTicking(bool /*on*/) {}
	virtual void SetMouseCapture(bool /*on*/) {}
	virtual bool HaveMouseCapture() {return false;}
	virtual sptr_t DefWndProc(unsigned int /*iMessage*/, uptr_t /*wParam*/, sptr_t /*lParam*/) {return 0;}

	LRESULT Command(UINT Msg, WPARAM wParam=0, LPARAM lParam=0) {
		return WndProc(Msg, wParam, lParam);
	}

	void findMatchingBracePos(int & braceAtCaret, int & braceOpposite)
	{
		int caretPos = int(Command(SCI_GETCURRENTPOS));
		braceAtCaret = -1;
		braceOpposite = -1;
		char charBefore = '\0';

		int lengthDoc = int(Command(SCI_GETLENGTH));

		if ((lengthDoc > 0) && (caretPos > 0)) 
		{
			charBefore = TCHAR(Command(SCI_GETCHARAT, caretPos - 1, 0));
		}
		// Priority goes to character before caret
		if (charBefore && strchr("[](){}", charBefore))
		{
			braceAtCaret = caretPos - 1;
		}

		if (lengthDoc > 0  && (braceAtCaret < 0)) 
		{
			// No brace found so check other side
			TCHAR charAfter = TCHAR(Command(SCI_GETCHARAT, caretPos, 0));
			if (charAfter && strchr("[](){}", charAfter))
			{
				braceAtCaret = caretPos;
			}
		}
		if (braceAtCaret >= 0) 
			braceOpposite = int(Command(SCI_BRACEMATCH, braceAtCaret, 0));
	}

	// return true if 1 or 2 (matched) brace(s) is found
	bool braceMatch() 
	{
		int braceAtCaret = -1;
		int braceOpposite = -1;
		findMatchingBracePos(braceAtCaret, braceOpposite);

		if ((braceAtCaret != -1) && (braceOpposite == -1))
		{
			Command(SCI_BRACEBADLIGHT, braceAtCaret);
			Command(SCI_SETHIGHLIGHTGUIDE, 0);
		} 
		else 
		{
			Command(SCI_BRACEHIGHLIGHT, braceAtCaret, braceOpposite);

			//if (_pEditView->isShownIndentGuide())
			{
				int columnAtCaret = int(Command(SCI_GETCOLUMN, braceAtCaret));
				int columnOpposite = int(Command(SCI_GETCOLUMN, braceOpposite));
				Command(SCI_SETHIGHLIGHTGUIDE, (columnAtCaret < columnOpposite)?columnAtCaret:columnOpposite);
			}
		}

		return (braceAtCaret != -1);
	}


	void Paint(/*Surface *surfaceWindow,*/ PRectangle rcArea)
	{
		if (timeGetTime()>nextTime)
		{
			Tick();
			nextTime = timeGetTime()+tickInterval;
		}

		braceMatch();

		Editor::Paint(/*surfaceWindow, */rcArea);
	}
	void AddCharUTF(char c) {Editor::AddCharUTF(&c, 1);}
};
