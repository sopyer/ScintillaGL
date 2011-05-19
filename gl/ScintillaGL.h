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

class MyEditor: public Editor
{
	size_t nextTime;
	static const size_t tickInterval = 100; 
public:
	MyEditor()
	{
		nextTime = timeGetTime()+tickInterval;
	}
	virtual void Initialise() {}
	virtual void SetVerticalScrollPos() {}
	virtual void SetHorizontalScrollPos() {}
	virtual bool ModifyScrollBars(int nMax, int nPage) {return true;}
	virtual void Copy() {}
	virtual void Paste() {}
	virtual void ClaimSelection() {}
	virtual void NotifyChange() {}
	virtual void NotifyParent(SCNotification scn) {}
	virtual void CopyToClipboard(const SelectionText &selectedText) {}
	virtual void SetTicking(bool on) {}
	virtual void SetMouseCapture(bool on) {}
	virtual bool HaveMouseCapture() {return false;}
	virtual sptr_t DefWndProc(unsigned int iMessage, uptr_t wParam, sptr_t lParam) {return 0;}
	void Paint(/*Surface *surfaceWindow,*/ PRectangle rcArea)
	{
		if (timeGetTime()>nextTime)
		{
			Tick();
			nextTime = timeGetTime()+tickInterval;
		}
		Editor::Paint(/*surfaceWindow, */rcArea);
	}
	void AddCharUTF(char c) {Editor::AddCharUTF(&c, 1);}
};
