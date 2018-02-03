#pragma once

#include <string>
#include "Globals.h"

class Data;

class Resource
{
public:

	enum ResourceType {
		TextureResource, MeshResource, SceneResource, AnimationResource, PrefabResource, ScriptResource, 
		AudioResource, ParticleFXResource, FontResource, RenderTextureResource, GameObjectResource, MaterialResource, ShaderResource, ShaderProgramResource,
		SkeletonResource, Unknown
	};

	Resource();
	virtual ~Resource();

	UID GetUID() const;
	void SetUID(UID uid);
	std::string GetName() const;
	void SetName(std::string name);
	std::string GetAssetsPath() const;
	void SetAssetsPath(std::string path);
	std::string GetLibraryPath() const;
	void SetLibraryPath(std::string path);
	ResourceType GetType() const;
	void SetType(ResourceType type);
	void IncreaseUsedCount();
	void DecreaseUsedCount();
	int GetUsedCount() const;

	virtual void Save(Data& data) const = 0;
	virtual bool Load(Data& data) = 0;
	virtual void CreateMeta() const = 0;
	virtual void LoadToMemory() = 0;
	virtual void UnloadFromMemory() = 0;

private:
	std::string name;
	std::string assets_path;
	std::string library_path;
	UID uuid;
	ResourceType type;
	std::string creation_date;
	int used_count;
};

