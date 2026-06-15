#pragma once
#include <GLEW\glew.h>
#include <FreeGLUT\freeglut.h>

struct FramebufferInfo
{
	unsigned int Flags;
	bool MSAA;

	FramebufferInfo()
	{
		Flags = GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH;
		MSAA = false;
	}

	FramebufferInfo(bool _Color, bool _Depth, bool _Stencil, bool _MASS)
	{
		Flags = GLUT_DOUBLE;

		if (_Color)
		{
			Flags |= GLUT_RGBA | GLUT_ALPHA;
		}

		if (_Depth)
		{
			Flags |= GLUT_DEPTH;
		}

		if (_Stencil)
		{
			Flags |= GLUT_STENCIL;
		}

		if (_MASS)
		{
			Flags |= GLUT_MULTISAMPLE;
		}

		MSAA = _MASS;
	}
};
