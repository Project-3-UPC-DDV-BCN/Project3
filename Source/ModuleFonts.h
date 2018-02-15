#ifndef _H_COMPONENT_CANVAS__
#define _H_COMPONENT_CANVAS__

#include "Module.h"
#include <vector>
#include <string>
//
//class FontFace
//{
//public:
//	FontFace();
//};
//
//class Font
//{
//public:
//	Font();
//
//private:
//	std::vector<FT_Face> faces;
//	std::string filepathname;
//};

class ModuleFonts : public Module
{
public:
	ModuleFonts(Application* app, bool start_enabled = true, bool is_game = false);
	~ModuleFonts();

	bool Init(Data* editor_config = nullptr);
	bool Start();
	bool CleanUp();

private:
	//FT_Library  library;

};

#endif // !_H_COMPONENT_CANVAS__