#include "gldebug.h"

// stretchy buffer // init: NULL // free: sb_free() // push_back: sb_push() // size: sb_count() //
#define sb_free(a)         ((a) ? free(stb__sbraw(a)),0 : 0)


#define sb_push(a,v)       (stb__sbmaybegrow(a,1), (a)[stb__sbn(a)++] = (v))
#define sb_count(a)        ((a) ? stb__sbn(a) : 0)
#define sb_add(a,n)        (stb__sbmaybegrow(a,n), stb__sbn(a)+=(n), &(a)[stb__sbn(a)-(n)])
#define sb_last(a)         ((a)[stb__sbn(a)-1])

#include <stdlib.h>
#include <assert.h>

#define stb__sbraw(a) ((int *) (a) - 2)
#define stb__sbm(a)   stb__sbraw(a)[0]
#define stb__sbn(a)   stb__sbraw(a)[1]

#define stb__sbneedgrow(a,n)  ((a)==0 || stb__sbn(a)+n >= stb__sbm(a))
#define stb__sbmaybegrow(a,n) (stb__sbneedgrow(a,(n)) ? stb__sbgrow(a,n) : 0)
#define stb__sbgrow(a,n)  stb__sbgrowf((void **) &(a), (n), sizeof(*(a)))

static void stb__sbgrowf(void **arr, int increment, int itemsize)
{
   int m = *arr ? 2*stb__sbm(*arr)+increment : increment+1;
   void *p = realloc(*arr ? stb__sbraw(*arr) : 0, itemsize * m + sizeof(int)*2);
   assert(p);
   if (p) {
      if (!*arr) ((int *) p)[1] = 0;
      *arr = (void *) ((int *) p + 2);
      stb__sbm(*arr) = m;
   }
}

GLuint* programsList=NULL;

GLuint    gldCreateTrackedProgram()
{
	GLuint prg = GLeeFuncPtr_glCreateProgram();
	sb_push(programsList, prg);
	return prg;
}

void      gldDeleteTrackedProgram(GLuint program)
{
	GLeeFuncPtr_glDeleteProgram(program);
	int count = sb_count(programsList);
	for (int i=0; i<count; ++i)
	{
		if (programsList[i]==program)
		{
			programsList[i]=sb_last(programsList);
			sb_add(programsList, -1);
		}
	}
}

GLsizei   gldGetProgramCount()
{
	return sb_count(programsList);
}

void      gldGetPrograms(GLuint* programs, GLsizei count)
{
	GLsizei programsCount = sb_count(programsList);
	GLsizei sizeToCopy = (programsCount>count)?count:programsCount;
	memcpy(programs, programsList, sizeToCopy*sizeof(GLuint));
}
