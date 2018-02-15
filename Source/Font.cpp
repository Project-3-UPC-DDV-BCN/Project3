#include "Font.h"
#include "freetype/ftglyph.h"

FontFace::FontFace(FT_Face _face)
{
	face = _face;
}

FT_Face FontFace::GetFace()
{
	return face;
}

int FontFace::LoadText(std::string text, int size)
{
	FT_Set_Pixel_Sizes(face, 0, size);

	for (int i = 0; i < text.length(); ++i)
	{
		FT_Load_Char(face, text[i], FT_LOAD_RENDER);

		FT_GlyphSlot glyph = face->glyph;
	}
}

void Font::AddFace(FontFace face)
{
	faces.push_back(face);
}

std::vector<FontFace> Font::GetFaces()
{
	return faces;
}

std::string Font::GetFilePath()
{
	return filepathname;
}

Font::Font(const char * filepath)
{
	filepathname = filepath;
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
