#include "ModuleFonts.h"
#include "FreeType\ft2build.h"
#include "FreeType\freetype\freetype.h"

ModuleFonts::ModuleFonts(Application * app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
}

ModuleFonts::~ModuleFonts()
{
}

bool ModuleFonts::Init(Data * editor_config)
{
	bool ret = true;


	FT_Library library;

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
