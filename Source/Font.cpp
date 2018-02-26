#include "Font.h"
#include "OpenGL.h"

Font::Font(const char * _filepath) : Resource()
{
	SetType(Resource::FontResource);
	SetAssetsPath(_filepath);
	SetFontSize(24);
}

void Font::Save(Data & data) const
{
}

bool Font::Load(Data & data)
{
	return false;
}

void Font::CreateMeta() const
{
}

void Font::LoadToMemory()
{
}

void Font::UnloadFromMemory()
{
}

void Font::CleanUp()
{
	TTF_CloseFont(font);
}

void Font::SetFontSize(uint _size)
{
	TTF_CloseFont(font);

	font = TTF_OpenFont(GetAssetsPath().c_str(), _size);

	valid = false;
	if (font != nullptr)
		valid = true;

	size = _size;
}

TTF_Font * Font::GetTTFFont()
{
	return font;
}

const char * Font::GetFamilyName()
{
	return TTF_FontFaceFamilyName(font);
}

const char * Font::GetStyleName()
{
	return TTF_FontFaceStyleName(font);
}

uint Font::GetFontSize()
{
	return size;
}

bool Font::GetValid() const
{
	return valid;
}
