#include <gl/glee.h>

#ifdef glCreateProgram
#	undef glCreateProgram
#endif

#ifdef glDeleteProgram
#	undef glDeleteProgram
#endif

typedef GLenum GLDresult;

// !!! Code is not thread safe !!!

#define glCreateProgram gldCreateTrackedProgram
#define glDeleteProgram gldDeleteTrackedProgram

GLuint    gldCreateTrackedProgram();
void      gldDeleteTrackedProgram(GLuint program);
GLsizei   gldGetProgramCount();
void      gldGetPrograms(GLuint* programs, GLsizei count);
