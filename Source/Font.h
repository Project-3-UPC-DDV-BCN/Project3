#ifndef _H_RESOURCE_FONT__
#define _H_RESOURCE_FONT__

#include <string>

#include "Resource.h"
#include "ModuleFontImporter.h"
#include "SDL_ttf/include/SDL_ttf.h"

class Font : public Resource
{
public:
	Font(const char* _filepath);

	void Save(Data& data) const;
	bool Load(Data& data);
	void CreateMeta() const;
	void LoadToMemory();
	void UnloadFromMemory();
	void CleanUp();

	void SetFontSize(uint size);

	TTF_Font* GetTTFFont();
	const char* GetFamilyName();
	const char* GetStyleName();
	uint GetFontSize();
	bool GetValid() const;

private:
	TTF_Font* font = nullptr;
	uint size;
	bool valid = false;

};

#endif //!_H_RESOURCE_FONT__