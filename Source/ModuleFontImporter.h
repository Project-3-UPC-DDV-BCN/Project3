#ifndef _H_FONT_IMPORTER_
#define _H_FONT_IMPORTER_

#include "Module.h"
#include <vector>
#include <string>

#include "STB\stb_truetype.h"

class Font;
class Texture;

class ModuleFontImporter : public Module
{
public:
	ModuleFontImporter(Application* app, bool start_enabled = true, bool is_game = false);
	~ModuleFontImporter();

	bool Init(Data* editor_config = nullptr);
	bool Start();
	bool CleanUp();

	Font* LoadFont(const char* filepath);
	//void UnloadFont(Font* font);
	//Texture LoadText(const char* text, Font* font, uint size);

private:
	std::vector<Font*> fonts;
};

#endif // !_H_FONT_IMPORTER_