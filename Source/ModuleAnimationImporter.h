#ifndef __ANIMATION_IMPORTER_H__
#define __ANIMATION_IMPORTER_H__

#include "Module.h"
#include "Assimp\include\matrix4x4.h"

class aiNode;
class aiScene;

class ModuleAnimationImporter : public Module
{
public:
	ModuleAnimationImporter(Application* app, bool start_enabled = true, bool is_game = false);
	~ModuleAnimationImporter();

	bool Init(Data* editor_config = nullptr);
	bool CleanUp();

	void ImportAnimationComponents(std::string path);

	void ImportNodeAnimComponents(aiNode* node, aiMatrix4x4 parent_trans, const aiScene* scene, bool is_root = false);
};
	
	

#endif
