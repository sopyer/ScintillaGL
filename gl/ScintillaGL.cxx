#include <windows.h>
#include <gl/glee.h>

#include "ScintillaGL.h"

MyEditor::MyEditor():ls(pdoc)
{
	nextTime = 0;
	pdoc->pli = &ls;
}

void MyEditor::OnKeyDown(SDL_KeyboardEvent& event)
{
	int sciKey;
	switch(event.keysym.sym)
	{
		case SDLK_DOWN:				sciKey = SCK_DOWN;          break;
		case SDLK_UP:				sciKey = SCK_UP;            break;
		case SDLK_LEFT:				sciKey = SCK_LEFT;          break;
		case SDLK_RIGHT:			sciKey = SCK_RIGHT;         break;
		case SDLK_HOME:				sciKey = SCK_HOME;          break;
		case SDLK_END:				sciKey = SCK_END;           break;
		case SDLK_PAGEUP:			sciKey = SCK_PRIOR;         break;
		case SDLK_PAGEDOWN:			sciKey = SCK_NEXT;	        break;
		case SDLK_DELETE:			sciKey = SCK_DELETE;        break;
		case SDLK_INSERT:			sciKey = SCK_INSERT;        break;
		case SDLK_ESCAPE:			sciKey = SCK_ESCAPE;        break;
		case SDLK_BACKSPACE:		sciKey = SCK_BACK;	        break;
		case SDLK_TAB:				sciKey = SCK_TAB;	        break;
		case SDLK_RETURN:			sciKey = SCK_RETURN;        break;
		case SDLK_KP_PLUS:			sciKey = SCK_ADD;	        break;
		case SDLK_KP_MINUS:			sciKey = SCK_SUBTRACT;      break;
		case SDLK_KP_DIVIDE:		sciKey = SCK_DIVIDE;        break;
		case SDLK_LSUPER:			sciKey = SCK_WIN;	        break;
		case SDLK_RSUPER:			sciKey = SCK_RWIN;	        break;
		case SDLK_MENU:				sciKey = SCK_MENU;	        break;
		case SDLK_SLASH:			sciKey = '/';		        break;
		case SDLK_ASTERISK:			sciKey = '`';		        break;
		case SDLK_LEFTBRACKET:		sciKey = '[';		        break;
		case SDLK_BACKSLASH:		sciKey = '\\';		        break;
		case SDLK_RIGHTBRACKET:		sciKey = ']';		        break;
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
		case SDLK_LALT:
		case SDLK_RALT:
		case SDLK_LCTRL:
		case SDLK_RCTRL:
			sciKey = 0;
			break;
		default:
			sciKey = event.keysym.sym;
	}

	if (sciKey)
	{
		bool consumed;
		bool ctrlPressed  = event.keysym.mod&KMOD_LCTRL  || event.keysym.mod&KMOD_RCTRL;
		bool altPressed   = event.keysym.mod&KMOD_LALT   || event.keysym.mod&KMOD_RALT;
		bool shiftPressed = event.keysym.mod&KMOD_LSHIFT || event.keysym.mod&KMOD_RSHIFT;
		KeyDown((SDLK_a<=sciKey && sciKey<=SDLK_z)?sciKey-'a'+'A':sciKey,
			shiftPressed, ctrlPressed, altPressed,
			&consumed
		);
		if (!consumed && event.keysym.unicode>=32 && !ctrlPressed && !altPressed)
		{
			char    utf8[5];
			wchar_t utf16[2] = {event.keysym.unicode, 0};
			UTF8FromUTF16(utf16, 1, utf8, sizeof(utf8));
			AddCharUTF(utf8, strlen(utf8));
		}
	}
}

bool MyEditor::BraceMatch() 
{
	int braceAtCaret = -1;
	int braceOpposite = -1;
	int caretPos = int(Command(SCI_GETCURRENTPOS));
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
