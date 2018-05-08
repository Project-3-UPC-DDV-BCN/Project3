#ifndef __OPENGL_H__
#define __OPENGL_H__

#define WIN32_MEAN_AND_LEAN
#include <Windows.h>
#include "Glew/include/glew.h" // extension lib
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

enum BlendingMode {

	GlZero = 0,
	GlOne,
	GlSrcColor,
	GlOneMinusSrcColor,
	GlDstColor,
	GlOneMinusDstColor,
	GlSrcAlpha,
	GlOneMinusSrcAlpha,
	GlDstAlpha,
	GlOneMinusDstAlpha,
	GlSrcAlphaSaturate
};

#endif // __OPENGL_H__