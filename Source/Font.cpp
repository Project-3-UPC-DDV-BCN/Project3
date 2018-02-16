#include "Font.h"
#include "freetype/ftglyph.h" 
#include "OpenGL.h"

Font::Font(const char * _filepath, const char * _name, unsigned char * _bitmap, uint _bitmap_size, int _height, int _width, int _line_height, stbtt_fontinfo _info)
{
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
	RELEASE_ARRAY(bitmap);
}

const char* Font::GetName()
{
	return name.c_str();
}

unsigned char* Font::GetBitmap()
{
	return bitmap;
}

uint Font::GetBitmapSize()
{
	return bitmap_size;
}

int Font::GetWidth()
{
	return width;
}

int Font::GetHeight()
{
	return height;
}

int Font::GetLineHeight()
{
	return line_height;
}

stbtt_fontinfo Font::GetInfo()
{
	return info;
}

const char* Font::GetFilePath()
{
	return filepath.c_str();
}
