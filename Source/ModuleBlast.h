#pragma once

#include "Module.h"
#include <NvBlastExtPxListener.h>
#include <NvBlastExtImpactDamageManager.h>
#include <map>
#include <vector>

class FixedBuffer
{
public:
	FixedBuffer(const uint32_t size)
	{
		m_buffer.resize(size);
		m_index = 0;
	}

	void* push(const void* data, uint32_t size)
	{
		if (m_index + size > m_buffer.size())
			return nullptr;

		void* dst = &m_buffer[m_index];
		memcpy(dst, data, size);
		m_index += size;
		return dst;
	}

	void clear()
	{
		m_index = 0;
	}

private:
	std::vector<char> m_buffer;
	uint32_t		  m_index;
};

namespace Nv
{
	namespace Blast
	{
		class TkFramework;
		class ExtPxManager;
		class ExtPxFamily;
		class TkGroup;
		class ExtGroupTaskManager;
	}
}

namespace physx
{
	class PxMaterial;
	class PxTaskManager;
}

class BlastModel;

class ModuleBlast :
	public Module, public Nv::Blast::ExtPxListener
{
public:
	ModuleBlast(Application* app, bool start_enabled = true, bool is_game = false);
	~ModuleBlast();

	bool Init(Data* editor_config = nullptr);
	update_status Update(float dt);
	bool CleanUp();

	Nv::Blast::TkFramework* GetFramework() const;
	Nv::Blast::ExtImpactDamageManager* GetImpactManager() const;

	void CreateFamily(BlastModel* model);
	void SpawnFamily(BlastModel* model);

	void onActorCreated(Nv::Blast::ExtPxFamily& family, Nv::Blast::ExtPxActor& actor);
	void onActorDestroyed(Nv::Blast::ExtPxFamily& family, Nv::Blast::ExtPxActor& actor);

	void ApplyDamage();

private:
	Nv::Blast::TkFramework* framework;
	Nv::Blast::ExtPxManager* px_manager;
	std::map<Nv::Blast::ExtPxFamily*, BlastModel*> families;
	physx::PxMaterial* default_material;
	FixedBuffer* damage_desc_buffer;
	FixedBuffer* damage_params_buffer;
	physx::PxTaskManager* phys_task_manager;
	Nv::Blast::TkGroup* group;
	Nv::Blast::ExtGroupTaskManager* task_manager;
	Nv::Blast::ExtImpactDamageManager* impact_damage_manager;
	Nv::Blast::ExtImpactSettings settings;
};