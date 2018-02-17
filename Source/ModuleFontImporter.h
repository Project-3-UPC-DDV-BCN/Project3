#ifndef _H_FONT_IMPORTER_
#define _H_FONT_IMPORTER_

#include "Module.h"
#include <vector>
#include <string>
#include "SDL/include/SDL_pixels.h"

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
	void  UnloadFont(Font* font);
	Font* GetFont(const char* filepath) const;
	void ClearFonts();
	uint LoadText(const char * text, Font* font, bool bold = false, bool italic = false, bool underline = false, bool strikethrough = false);
	void UnloadText(uint id);

private:
	std::vector<Font*> fonts;
};

#endif // !_H_FONT_IMPORTER_