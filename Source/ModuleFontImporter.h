#ifndef _H_FONT_IMPORTER_
#define _H_FONT_IMPORTER_

#include "Module.h"
#include <vector>
#include <string>
#include "SDL/include/SDL_pixels.h"
#include "MathGeoLib\Math\float4.h"

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

	Font* LoadFontInstance(const char* filepath);
	void  UnloadFont(Font* font);
	void ClearFonts();

	uint LoadText(const char * text, Font* font, float4 colour = float4(255, 255, 255, 255), bool bold = false, bool italic = false, bool underline = false, bool strikethrough = false);
	void UnloadText(uint id);

	float2 CalcTextSize(const char * text, Font* font, bool bold = false, bool italic = false, bool underline = false, bool strikethrough = false);

private:
	std::vector<Font*> fonts;
};

#endif // !_H_FONT_IMPORTER_