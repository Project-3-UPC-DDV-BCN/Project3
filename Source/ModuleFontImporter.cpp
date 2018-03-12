#include "ModuleFontImporter.h"
#include "Globals.h"
#include "Font.h"
#include "UsefulFunctions.h"
#include "Texture.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "OpenGL.h"
#include "SDL_FontCache.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"

#include "SDL/include/SDL.h"
#include "SDL_ttf/include/SDL_ttf.h"
#pragma comment( lib, "SDL_ttf/libx86/SDL2_ttf.lib" )

ModuleFontImporter::ModuleFontImporter(Application * app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
	if (TTF_Init() == -1)
	{
		CONSOLE_LOG("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
	}
}

ModuleFontImporter::~ModuleFontImporter()
{
}

bool ModuleFontImporter::Init(Data * editor_config)
{
	bool ret = true;

	std::string default_font_name = EDITOR_FONTS_FOLDER "arial.ttf";
	default_font_name.erase(default_font_name.begin(), default_font_name.begin() + 2);
	default_font = LoadFontInstance(App->file_system->GetFullPath(default_font_name).c_str());

	return ret;
}

bool ModuleFontImporter::Start()
{
	bool ret = true;

	return ret;
}

bool ModuleFontImporter::CleanUp()
{
	bool ret = true;

	std::map<uint, Font*> fonts_list = App->resources->GetFontsList();

	for (std::map<uint, Font*>::iterator it = fonts_list.begin(); it != fonts_list.end(); ++it)
	{
		it->second->CleanUp();
	}

	UnloadFontInstance(default_font);
	TTF_Quit();

	return ret;
}

std::string ModuleFontImporter::ImportFont(std::string path)
{
	std::string file_name = App->file_system->GetFileName(path);
	std::string library_path = LIBRARY_FONTS_FOLDER + file_name;

	if (!App->file_system->FileExist(path))
	{
		App->file_system->Copy(path, library_path);
	}

	return library_path;
}

Font* ModuleFontImporter::LoadFontFromLibrary(std::string path)
{
	path.erase(path.begin(), path.begin() + 2);
	return LoadFontInstance(App->file_system->GetFullPath(path).c_str());;
}

Font * ModuleFontImporter::CreateFontInstance(Font * font)
{
	Font* ret = nullptr;

	if (font != nullptr)
	{
		ret = new Font(font->GetAssetsPath().c_str());
		ret->SetName(font->GetName());
		ret->SetLibraryPath(font->GetLibraryPath());

		if (!ret->GetValid())
		{
			RELEASE(ret);
		}
	}

	return ret;
}

Font* ModuleFontImporter::LoadFontInstance(const char * filepath)
{
	Font* font = new Font(filepath);

	std::string file_name = App->file_system->GetFileNameWithoutExtension(filepath);
	font->SetName(file_name);

	if (font->GetValid())
	{
	/*	fonts.push_back(font);*/
	}
	else
	{
		CONSOLE_ERROR("Error loading font with path (Wrong path?): %s", filepath);

		font->CleanUp();
		RELEASE(font);
	}
	

	return font;
}

void ModuleFontImporter::UnloadFontInstance(Font*& font)
{
	if (font != nullptr)
	{
		font->CleanUp();
		RELEASE(font);
		font = nullptr;
	}
}

Font * ModuleFontImporter::GetDefaultFont()
{
	return default_font;
}

uint ModuleFontImporter::LoadText(const char * text, Font * font, float4 colour, bool bold, bool italic, bool underline, bool strikethrough)
{
	int id = 0;
	
	if (font != nullptr)
	{
		if (TextCmp(text, ""))
		{
			text = " ";
		}

		SDL_Texture* texture = nullptr;

		SDL_Color color = { colour.x, colour.y, colour.w, colour.z };
		TTF_Font* ttf_font = font->GetTTFFont();
		int style = 0;

		if(bold)
		{
			style |= TTF_STYLE_BOLD;
		}

		if (italic)
		{
			style |= TTF_STYLE_ITALIC;
		}

		if (underline)
		{
			style |= TTF_STYLE_UNDERLINE;
		}

		if (strikethrough)
		{
			style |= TTF_STYLE_STRIKETHROUGH;
		}

		TTF_SetFontStyle(ttf_font, style);

		SDL_Surface* surface = TTF_RenderText_Blended(ttf_font, text, color);

		if (surface == NULL)
		{
			CONSOLE_ERROR("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
		}
		else
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glGenTextures(1, (GLuint*)&id);
			glBindTexture(GL_TEXTURE_2D, id);

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);

			// Finish
			SDL_FreeSurface(surface);
			TTF_SetFontStyle(ttf_font, TTF_STYLE_NORMAL);
		}
	}

	return id;
}

void ModuleFontImporter::UnloadText(uint id)
{
	if(id > 0)
		glDeleteTextures(1, &id);
}

float2 ModuleFontImporter::CalcTextSize(const char * text, Font * font, bool bold, bool italic, bool underline, bool strikethrough)
{
	float2 size;

	if (font != nullptr)
	{
		TTF_Font* ttf_font = font->GetTTFFont();
		int style = 0;

		if (bold)
		{
			style |= TTF_STYLE_BOLD;
		}

		if (italic)
		{
			style |= TTF_STYLE_ITALIC;
		}

		if (underline)
		{
			style |= TTF_STYLE_UNDERLINE;
		}

		if (strikethrough)
		{
			style |= TTF_STYLE_STRIKETHROUGH;
		}

		int size_w, size_h = 0;
		if (TTF_SizeText(ttf_font, text, &size_w, &size_h) == 0)
		{
			size.x = size_w;
			size.y = size_h;
		}
	}

	return size;
}


