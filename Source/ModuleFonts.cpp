#include "ModuleFonts.h"
#include "Globals.h"

#pragma comment (lib, "FreeType/lib/freetype.lib")

#include <ft2build.h>
#include FT_FREETYPE_H


ModuleFonts::ModuleFonts(Application * app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
}

ModuleFonts::~ModuleFonts()
{
}

bool ModuleFonts::Init(Data * editor_config)
{
	bool ret = true;

	FT_Library  library;

	FT_Error error = FT_Init_FreeType(&library);
	if (error)
	{
		CONSOLE_LOG("An error occurred during FreeType library initialization");
	}

	return ret;
}

bool ModuleFonts::Start()
{
	bool ret = true;

	return ret;
}

bool ModuleFonts::CleanUp()
{
	bool ret = true;

	

	return ret;
}
