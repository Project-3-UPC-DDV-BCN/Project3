#include "Application.h"
#include "ModuleAnimationImporter.h"
#include "ModuleMeshImporter.h"
#include "Skeleton.h"
#include "Mesh.h"
#include "ModuleResources.h"

#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include "Assimp/include/cimport.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")

ModuleAnimationImporter::ModuleAnimationImporter(Application* app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
}

ModuleAnimationImporter::~ModuleAnimationImporter()
{
}

bool ModuleAnimationImporter::Init(Data * editor_config)
{
	name = "Animation_Importer";

	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	stream.callback = &Callback;
	aiAttachLogStream(&stream);

	return true;
}

bool ModuleAnimationImporter::CleanUp()
{
	aiDetachAllLogStreams();
	return true;
}

void ModuleAnimationImporter::ImportAnimationComponents(std::string path)
{
	const aiScene* scene = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		aiNode* root_node = scene->mRootNode;
		aiMatrix4x4 root_transform = root_node->mTransformation;
		ImportNodeAnimComponents(root_node, root_transform, scene, true);
	}
}

void ModuleAnimationImporter::ImportNodeAnimComponents(aiNode * node, aiMatrix4x4 parent_trans, const aiScene* scene, bool is_root)
{
	//check this node transform
	aiMatrix4x4 node_transform;
	if (!is_root)
		node_transform = parent_trans * node->mTransformation;
	else
		node_transform = parent_trans;

	//check if the node has meshes
	if (node->mNumMeshes > 0)
	{
		for (int i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			Mesh* loaded_mesh = App->resources->GetMesh(mesh->mName.C_Str());

			// ----- Skeleton -----
			if (mesh->HasBones()) //check if the mesh has an skeleton
			{
				uint num_bones = mesh->mNumBones;
				if (num_bones != 0) //check if there are bones to load
				{
					Skeleton* skeleton = new Skeleton;
					Joint* skeleton_joints = new Joint[num_bones];

					for (int i = 0; i < num_bones; ++i)
					{
						(skeleton_joints + i)->SetName(mesh->mBones[i]->mName.C_Str());
					}
				}
			}
			// --------------------
		}
	}
}
