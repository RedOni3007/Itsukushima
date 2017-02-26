/**
* helper function for print the opengl error message
*
* @author: Kai Yang
**/

#ifndef OPENGL_ERRORCHECK_H
#define OPENGL_ERRORCHECK_H

#include <Core/CoreHeaders.h>

#include <Core/GLEW_GLFW.h>

#define CHECK_GL_ERROR 1

#if X_DEBUG && CHECK_GL_ERROR
#define printOpenGLError() printOglError(__FILE__, __LINE__)
#else
#define printOpenGLError() 
#endif


	static int printOglError(char *file, int line)
	{
		GLenum glErr;
		int    retCode = 0;
	
		glErr = glGetError();
		if (glErr != GL_NO_ERROR)
		{
			switch(glErr)
			{
				case GL_INVALID_ENUM:
					Debug::Log("glError in file %s @ line %d: GL_INVALID_ENUM\n", file, line);
					break;
				case GL_INVALID_VALUE:
					Debug::Log("glError in file %s @ line %d: GL_INVALID_VALUE\n", file, line);
					break;
				case GL_INVALID_OPERATION:
					Debug::Log("glError in file %s @ line %d: GL_INVALID_OPERATION\n", file, line);
					break;
				case GL_INVALID_FRAMEBUFFER_OPERATION:
					Debug::Log("glError in file %s @ line %d: GL_INVALID_FRAMEBUFFER_OPERATION\n", file, line);
					break;
				case GL_OUT_OF_MEMORY:
					Debug::Log("glError in file %s @ line %d: GL_OUT_OF_MEMORY\n", file, line);
					break;
				default:
					Debug::Log("glError in file %s @ line %d: ENUM = %d\n", file, line,glErr);
					break;
			}
			retCode = 1;
		}
		return retCode;
	};


#endif