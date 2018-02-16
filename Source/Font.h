#ifndef _H_RESOURCE_FONT__
#define _H_RESOURCE_FONT__

#include <string>

#include "Resource.h"
#include "ModuleFontImporter.h"

#include "imgui/stb_truetype.h"

class Font : public Resource
{
public:
	Font(const char* _filepath, const char * _name, unsigned char * _bitmap, uint _bitmap_size, int _height, int _width, int _line_height, stbtt_fontinfo _info);

	void Save(Data& data) const;
	bool Load(Data& data);
	void CreateMeta() const;
	void LoadToMemory();
	void UnloadFromMemory();
	void CleanUp();

	const char* GetName();
	unsigned char* GetBitmap();
	uint GetBitmapSize();
	int GetWidth();
	int GetHeight();
	int GetLineHeight();
	stbtt_fontinfo GetInfo();

private:
	private:
	std::string    name;
	unsigned char* bitmap = nullptr;
	uint		   bitmap_size = 0;
	int			   width = 0;
	int			   height = 0;
	int			   line_height = 0;
	stbtt_fontinfo info;
};

#endif //!_H_RESOURCE_FONT__