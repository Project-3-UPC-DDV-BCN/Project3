#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow(Application* app, bool start_enabled = true, bool is_game = false);

	// Destructor
	virtual ~ModuleWindow();

	bool Init(Data* editor_config = nullptr);
	bool CleanUp();

	void SetTitle(const char* title);
	uint GetWidth();
	uint GetHeight();
	void SetWidth(uint width);
	void SetHeight(uint height);
	void SetSize(uint width, uint height);
	float GetBrightness() const;
	void SetBrightness(float brightness);
	bool IsFullScreen() const;
	void SetFullScreen(bool fullscreen);
	bool IsFullScreenDesktop() const;
	void SetFullScreenDesktop(bool full_desktop);
	bool IsBorderless() const;
	void SetBorderless(bool borderless);

	void SetIcon(const char* path);

	void SaveData(Data* data);

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;

	SDL_Texture* engine_icon;

private:
	int screen_width;
	int screen_height;
	bool is_fullscreen;
	bool is_resizable;
	bool is_borderless;
	bool is_full_desktop;
};

#endif // __ModuleWindow_H__