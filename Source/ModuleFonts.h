#ifndef _H_COMPONENT_CANVAS__
#define _H_COMPONENT_CANVAS__

#include "Module.h"

class ModuleFonts : public Module
{
public:
	ModuleFonts(Application* app, bool start_enabled = true, bool is_game = false);
	~ModuleFonts();

	bool Init(Data* editor_config = nullptr);
	bool Start();
	bool CleanUp();

private:


};

#endif // !_H_COMPONENT_CANVAS__