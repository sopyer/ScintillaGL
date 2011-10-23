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

#include <SDL.h>
#include <gl/GLee.h>

#include "Platform.h"

#include "ILexer.h"
#include "Scintilla.h"
#include "SVector.h"
#include "SplitVector.h"
#include "Partitioning.h"
#include "RunStyles.h"
#include "ContractionState.h"
#include "CellBuffer.h"
#include "KeyMap.h"
#include "Indicator.h"
#include "XPM.h"
#include "LineMarker.h"
#include "Style.h"
#include "ViewStyle.h"
#include "Decoration.h"
#include "CharClassify.h"
#include "Document.h"
#include "Selection.h"
#include "PositionCache.h"
#include "Editor.h"
#include "UniConversion.h"

#include "SciLexer.h"
#include "LexerModule.h"
#include "SciLexer.h"
#include "LexerModule.h"
#include "Catalogue.h"

class LexState;

class ShaderEditOverlay
{
public:
	ShaderEditOverlay();
	~ShaderEditOverlay();

	void initialise(int w, int h);
	void reset();

	void handleKeyDown(SDL_KeyboardEvent& event);
	void renderFullscreen();

	bool requireReset() {bool prevValue=mRequireReset; mRequireReset = false; return prevValue;}

	void addPrograms(size_t count, GLuint* programs);

private:
	void initialiseShaderEditor();
	void initialiseDebugOutputView();
	void initialiseSelectionList();

	void saveShaderSource();
	void compileProgram();
	void fillListWithShaders();
	void fillListWithPrograms();
	void loadShaderSource();

private:
	enum ModesEnum
	{
		SELMODE_PROGRAM_LIST,
		SELMODE_SHADER_LIST
	};

private:
	static const size_t TICK_INTERVAL = 100;

	bool   mRequireReset;

	size_t mNextTick;

	ModesEnum mSelectionMode;
	GLuint    mSelectedProgram;
	GLuint    mSelectedShader;

	std::vector<GLuint> mPrograms;
	std::vector<GLuint> mAttachedShaders;

	LexState* mLexer;

	Editor  mShaderEditor;
	Editor  mDebugOutputView;
	Editor  mSelectionList;
	Editor* mActiveEditor;

	float mWidth;
	float mHeight;
};
