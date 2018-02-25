#pragma once

#include "Window.h"

class AudioWindow :
	public Window
{
public:
	AudioWindow();
	virtual ~AudioWindow();
	
	void DrawWindow();
};