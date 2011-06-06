#include <windows.h>
#include <gl/glee.h>

#include "ScintillaGL.h"

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
