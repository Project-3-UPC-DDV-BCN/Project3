#include "ModuleFontImporter.h"
#include "Globals.h"
#include "Font.h"

#pragma comment (lib, "FreeType/lib/freetype.lib")

ModuleFontImporter::ModuleFontImporter(Application * app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
}

ModuleFontImporter::~ModuleFontImporter()
{
}

bool ModuleFontImporter::Init(Data * editor_config)
{
	bool ret = true;

	FT_Error error = FT_Init_FreeType(&library);
	if (error)
	{
		CONSOLE_LOG("An error occurred during FreeType library initialization");
	}

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

	FT_Done_FreeType(library);

	return ret;
}

Font * ModuleFontImporter::LoadFont(const char * filepath)
{
	Font* ret = new Font(filepath);

	bool next_face = true;
	int face_index = 0;

	bool found = false;
	for (std::vector<Font*>::iterator it = fonts.begin(); it != fonts.end(); ++it)
	{
		if ((*it)->GetFilePath() == filepath)
		{
			found = true;
			ret = *it;
		}
	}

	if (!found)
	{
		bool next_face = true;
		int face_index = 0;

		while (next_face)
		{
			FT_Face face;

			FT_Error error = FT_New_Face(library, filepath, face_index, &face);
			if (error)
			{
				next_face = false;
			}
			else
			{
				ret->AddFace(face);
			}

			++face_index;
		}
	}

	return ret;
}

void ModuleFontImporter::InitFont(Font * font)
{
	std::vector<FontFace> faces = font->GetFaces();

	for (std::vector<FontFace>::iterator it = faces.begin(); it != faces.end(); ++it)
	{
		FT_Face curr_face = (*it).GetFace();

		for (int i = 0; i < 256; ++i)
		{
			//Load and render glyph
			FT_Error error = FT_Load_Char(curr_face, i, FT_LOAD_RENDER);

			if (!error)
			{
				curr_face->name
			}
		}
	}
}

