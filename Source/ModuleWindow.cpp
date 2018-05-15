#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "Data.h"

ModuleWindow::ModuleWindow(Application* app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
	window = nullptr;
	screen_surface = nullptr;
	screen_width = 1280;
	screen_height = 950;
	is_fullscreen = false;
	is_resizable = true;
	is_borderless = false;
	is_full_desktop = false;
	engine_icon = nullptr;

	name = "Window";
}

// Destructor
ModuleWindow::~ModuleWindow()
{

}

// Called before render is available
bool ModuleWindow::Init(Data* editor_config)
{
	is_game = false;
	CONSOLE_DEBUG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		CONSOLE_DEBUG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Get Config Data
		if (editor_config->EnterSection("Window_Config"))
		{
			int width = editor_config->GetInt("screen_width");
			int height = editor_config->GetInt("screen_height");
			SetSize(width, height);
			is_fullscreen = editor_config->GetBool("Fullscreen");
			is_resizable = editor_config->GetBool("Window_Resizable");
			is_borderless = editor_config->GetBool("Window_Borderless");
			is_full_desktop = editor_config->GetBool("Window_Full_Desktop");
			editor_config->LeaveSection();
		}

		//Create window
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Set OpenGL Attributes
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

		//Use OpenGL 4.5
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

		if(is_fullscreen == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(is_resizable == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(is_borderless == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(is_full_desktop == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		const char* title = DEFAULT_SCENE_TITLE;
		if (App->IsGame())
		{
			title = "Slave One";
		}

		window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, flags);

		if(window == NULL)
		{
			CONSOLE_DEBUG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
		if (App->IsGame())
		{
			SetIcon("./Slave-One-Logo.bmp");
		}
		else
		{
			SetIcon("./The-Creator-Logo.bmp");
		}
	}

	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	CONSOLE_DEBUG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetTitle(const char* title)
{
	if(App->IsGame())
		title = "Slave One";
	SDL_SetWindowTitle(window, title);
}

uint ModuleWindow::GetWidth()
{
	SDL_GetWindowSize(window, &screen_width, &screen_height);

	return screen_width;
}

uint ModuleWindow::GetHeight()
{
	SDL_GetWindowSize(window, &screen_width, &screen_height);

	return screen_height;
}

void ModuleWindow::SetWidth(uint width)
{
	if (width > 0)
	{
		screen_width = width;

		SDL_SetWindowSize(window, screen_width, screen_height);
	}
}

void ModuleWindow::SetHeight(uint height)
{
	if (height > 0)
	{
		screen_height = height;

		SDL_SetWindowSize(window, screen_width, screen_height);
	}
}

void ModuleWindow::SetSize(uint width, uint height)
{
	if (width > 0)
		screen_width = width;

	if (height > 0)
		screen_height = height;

	SDL_SetWindowSize(window, screen_width, screen_height);
}

float ModuleWindow::GetBrightness() const
{
	return SDL_GetWindowBrightness(window);
}

void ModuleWindow::SetBrightness(float brightness)
{
	SDL_SetWindowBrightness(window, brightness);
}

bool ModuleWindow::IsFullScreen() const
{
	return is_fullscreen;
}

void ModuleWindow::SetFullScreen(bool fullscreen)
{
	if (fullscreen)
	{
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	}
	else
	{
		SDL_SetWindowFullscreen(window, 0);
	}
	is_fullscreen = fullscreen;
}

bool ModuleWindow::IsFullScreenDesktop() const
{
	return is_full_desktop;
}

void ModuleWindow::SetFullScreenDesktop(bool full_desktop)
{
	if (full_desktop)
	{
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
	else
	{
		SDL_SetWindowFullscreen(window, 0);
	}
	is_full_desktop = full_desktop;
}

bool ModuleWindow::IsBorderless() const
{
	return is_borderless;
}

void ModuleWindow::SetBorderless(bool borderless)
{
	if (borderless && !is_fullscreen && !is_full_desktop)
	{
		SDL_SetWindowBordered(window, (SDL_bool)borderless);
	}
	is_borderless = borderless;
}

void ModuleWindow::SetIcon(const char * path)
{
	if (path != nullptr)
	{
		SDL_Surface *surface = SDL_LoadBMP(path);
		SDL_SetWindowIcon(window, surface);
		SDL_FreeSurface(surface);
	}
}

void ModuleWindow::SaveData(Data * data)
{
	data->CreateSection("Window_Config");
	data->AddInt("screen_width", GetWidth());
	data->AddInt("screen_height", GetHeight());
	data->AddBool("Fullscreen", is_fullscreen);
	data->AddBool("Window_Resizable", is_resizable);
	data->AddBool("Window_Borderless", is_borderless);
	data->AddBool("Window_Full_Desktop", is_full_desktop);
	data->CloseSection();
}
