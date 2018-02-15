#ifndef _H_RESOURCE_FONT__
#define _H_RESOURCE_FONT__

#include "Resource.h"
#include "ModuleFontImporter.h"

class FontFace
{
public:
	FontFace(FT_Face face);

	FT_Face GetFace();
	int LoadText(std::string text, int size);

private:
	FT_Face face;
	uint texture_id;
};

class Font : public Resource
{
public:
	Font(const char* filepath);

	void Save(Data& data) const;
	bool Load(Data& data);
	void CreateMeta() const;
	void LoadToMemory();
	void UnloadFromMemory();

	void AddFace(FontFace face);
	std::vector<FontFace> GetFaces();
	std::string GetFilePath();

private:
	std::vector<FontFace> faces;
	std::string filepathname;
};

#endif !_H_RESOURCE_FONT__