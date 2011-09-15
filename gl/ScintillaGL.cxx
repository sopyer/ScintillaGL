#include <windows.h>
#include <gl/glee.h>

#include "ScintillaGL.h"

MyEditor::MyEditor():ls(pdoc)
{
	// There does not seem to be a real standard for indicating that the clipboard
	// contains a rectangular selection, so copy Developer Studio.
	cfColumnSelect = static_cast<CLIPFORMAT>(
		::RegisterClipboardFormat(TEXT("MSDEVColumnSelect")));

	// Likewise for line-copy (copies a full line when no text is selected)
	cfLineSelect = static_cast<CLIPFORMAT>(
		::RegisterClipboardFormat(TEXT("MSDEVLineSelect")));
	nextTime = 0;
	pdoc->pli = &ls;
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

void MyEditor::CopyToClipboard(const SelectionText& selectedText) {
	if (!::OpenClipboard(hWnd))
		return;
	::EmptyClipboard();

	GlobalMemory uniText;

	int uchars = UTF16Length(selectedText.s, selectedText.len);
	uniText.Allocate(2 * uchars);
	if (uniText) {
		UTF16FromUTF8(selectedText.s, selectedText.len, static_cast<wchar_t *>(uniText.ptr), uchars);
	}

	if (uniText) {
		uniText.SetClip(CF_UNICODETEXT);
	}

	if (selectedText.rectangular) {
		::SetClipboardData(cfColumnSelect, 0);
	}

	if (selectedText.lineCopy) {
		::SetClipboardData(cfLineSelect, 0);
	}

	::CloseClipboard();
}

void MyEditor::Copy() {
	SelectionText selectedText;
	CopySelectionRange(&selectedText, true);
	CopyToClipboard(selectedText);
}

bool MyEditor::CanPaste() {
	if (!Editor::CanPaste())
		return false;
	if (::IsClipboardFormatAvailable(CF_TEXT))
		return true;
	return ::IsClipboardFormatAvailable(CF_UNICODETEXT) != 0;
}

void MyEditor::InsertPasteText(const char *text, int len, SelectionPosition selStart, bool isRectangular, bool isLine) {
	if (isRectangular) {
		PasteRectangular(selStart, text, len);
	} else {
		char *convertedText = 0;
		if (convertPastes) {
			// Convert line endings of the paste into our local line-endings mode
			convertedText = Document::TransformLineEnds(&len, text, len, pdoc->eolMode);
			text = convertedText;
		}
		if (isLine) {
			int insertPos = pdoc->LineStart(pdoc->LineFromPosition(sel.MainCaret()));
			pdoc->InsertString(insertPos, text, len);
			// add the newline if necessary
			if ((len > 0) && (text[len-1] != '\n' && text[len-1] != '\r')) {
				const char *endline = StringFromEOLMode(pdoc->eolMode);
				pdoc->InsertString(insertPos + len, endline, strlen(endline));
				len += strlen(endline);
			}
			if (sel.MainCaret() == insertPos) {
				SetEmptySelection(sel.MainCaret() + len);
			}
		} else {
			InsertPaste(selStart, text, len);
		}
		delete []convertedText;
	}
}

void MyEditor::Paste() {
	if (!::OpenClipboard(hWnd))
		return;
	UndoGroup ug(pdoc);
	bool isLine = SelectionEmpty() && (::IsClipboardFormatAvailable(cfLineSelect) != 0);
	ClearSelection(multiPasteMode == SC_MULTIPASTE_EACH);
	SelectionPosition selStart = sel.IsRectangular() ?
		sel.Rectangular().Start() :
		sel.Range(sel.Main()).Start();
	bool isRectangular = ::IsClipboardFormatAvailable(cfColumnSelect) != 0;

	// Always use CF_UNICODETEXT if available
	GlobalMemory memUSelection(::GetClipboardData(CF_UNICODETEXT));
	if (memUSelection) {
		wchar_t *uptr = static_cast<wchar_t *>(memUSelection.ptr);
		if (uptr) {
			unsigned int len;
			char *putf;

			unsigned int bytes = memUSelection.Size();
			len = UTF8Length(uptr, bytes / 2);
			putf = new char[len + 1];
			UTF8FromUTF16(uptr, bytes / 2, putf, len);

			InsertPasteText(putf, len, selStart, isRectangular, isLine);
			delete []putf;
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

				unsigned int mlen = UTF8Length(uptr, ulen);
				char *putf = new char[mlen + 1];
				if (putf) {
					// CP_UTF8 not available on Windows 95, so use UTF8FromUTF16()
					UTF8FromUTF16(uptr, ulen, putf, mlen);
				}

				delete []uptr;

				if (putf) {
					InsertPasteText(putf, mlen, selStart, isRectangular, isLine);
					delete []putf;
				}
			}
			memSelection.Unlock();
		}
	}
	::CloseClipboard();
}

void MyEditor::FindMatchingBracePos(int & braceAtCaret, int & braceOpposite)
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
bool MyEditor::BraceMatch() 
{
	int braceAtCaret = -1;
	int braceOpposite = -1;
	FindMatchingBracePos(braceAtCaret, braceOpposite);

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


void MyEditor::Paint()
{
	//Update
	if (timeGetTime()>nextTime)
	{
		Tick();
		nextTime = timeGetTime()+tickInterval;
	}

	BraceMatch();
	
	//Render
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CLIP_PLANE0);
	glEnable(GL_CLIP_PLANE1);
	glEnable(GL_CLIP_PLANE2);
	glEnable(GL_CLIP_PLANE3);

	double plane0[] = { 1,  0, 0, -clientRect.left };
	double plane1[] = {-1,  0, 0,  clientRect.right };
	double plane2[] = { 0,  1, 0, -clientRect.top };
	double plane3[] = { 0, -1, 0,  clientRect.bottom };

	glClipPlane(GL_CLIP_PLANE0, plane0);
	glClipPlane(GL_CLIP_PLANE1, plane1);
	glClipPlane(GL_CLIP_PLANE2, plane2);
	glClipPlane(GL_CLIP_PLANE3, plane3);

	Editor::Paint();

	glDisable(GL_CLIP_PLANE0);
	glDisable(GL_CLIP_PLANE1);
	glDisable(GL_CLIP_PLANE2);
	glDisable(GL_CLIP_PLANE3);

	glDisable(GL_BLEND);
}
