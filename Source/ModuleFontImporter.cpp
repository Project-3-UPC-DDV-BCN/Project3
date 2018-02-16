#include "ModuleFontImporter.h"
#include "Globals.h"
#include "Font.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "imgui\stb_truetype.h"

ModuleFontImporter::ModuleFontImporter(Application * app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
}

ModuleFontImporter::~ModuleFontImporter()
{
}

bool ModuleFontImporter::Init(Data * editor_config)
{
	bool ret = true;

	return ret;
}

bool ModuleFontImporter::Start()
{
	bool ret = true;

	LoadFont("C:/Users/guillemsc1/Documents/GitHub/Project3/EngineResources/arial.ttf");

	return ret;
}

bool ModuleFontImporter::CleanUp()
{
	bool ret = true;

	return ret;
}

Font* ModuleFontImporter::LoadFont(const char * filepath)
{
	long size = 0;
	unsigned char* fontBuffer = nullptr;
	
	FILE * fontFile = fopen(filepath, "rb");
	fseek(fontFile, 0, SEEK_END);
	size = ftell(fontFile); /* how long is the file ? */
	fseek(fontFile, 0, SEEK_SET); /* reset */
	
	fontBuffer = new unsigned char[size];
	
	fread(fontBuffer, size, 1, fontFile);
	fclose(fontFile);
	
	stbtt_fontinfo info;
	if (!stbtt_InitFont(&info, fontBuffer, 0))
	{
		CONSOLE_LOG("Loading font failed");
	}
	
	int b_w = 512; /* bitmap width */
	int b_h = 128; /* bitmap height */
	int l_h = 64; /* line height */
	
	
	Font* font = new Font(filepath, "name", fontBuffer, size, b_w, b_h, l_h, info);
	fonts.push_back(font);

	return font;
}

Font * ModuleFontImporter::GetFontAlreadyLoaded(const char * filepath)
{
	Font* ret = nullptr;

	for (std::vector<Font*>::iterator it = fonts.begin(); it != fonts.end(); ++it)
	{
		if((*it)->GetAssetsPath())
	}

	return ret;
}

