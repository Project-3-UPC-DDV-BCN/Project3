#ifndef _H_FONT_IMPORTER_
#define _H_FONT_IMPORTER_

#include "Module.h"
#include <vector>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H

class Font;

class ModuleFontImporter : public Module
{
public:
	ModuleFontImporter(Application* app, bool start_enabled = true, bool is_game = false);
	~ModuleFontImporter();

	bool Init(Data* editor_config = nullptr);
	bool Start();
	bool CleanUp();

	Font* LoadFont(const char* filepath);

private:
	void InitFont(Font* font);

private:
	FT_Library  library;
	std::vector<Font*> fonts;
};

#endif // !_H_FONT_IMPORTER_