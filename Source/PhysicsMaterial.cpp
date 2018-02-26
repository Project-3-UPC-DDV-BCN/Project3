#include "PhysicsMaterial.h"
#include "Data.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"
#include "ModulePhysMatImporter.h"
#include <ctime>

PhysicsMaterial::PhysicsMaterial()
{
	static_friction = 0.6f;
	dynamic_friction = 0.6f;
	restitution = 0;
	friction_combine_mode = 0;
	restitution_combine_mode = 0;
	SetType(Resource::PhysicsMatResource);
}

PhysicsMaterial::~PhysicsMaterial()
{
}

void PhysicsMaterial::SetStaticFriction(float friction)
{
	static_friction = friction;
}

float PhysicsMaterial::GetStaticFriction() const
{
	return static_friction;
}

void PhysicsMaterial::SetDynamicFriction(float friction)
{
	dynamic_friction = friction;
}

float PhysicsMaterial::GetDynamicFriction() const
{
	return dynamic_friction;
}

void PhysicsMaterial::SetRestitution(float restitution)
{
	this->restitution = restitution;
}

float PhysicsMaterial::GetRestitution() const
{
	return restitution;
}

void PhysicsMaterial::SetFrictionMode(uint mode)
{
	friction_combine_mode = mode;
}

uint PhysicsMaterial::GetFrictionMode() const
{
	return friction_combine_mode;
}

void PhysicsMaterial::SetRestitutionMode(uint mode)
{
	restitution_combine_mode = mode;
}

uint PhysicsMaterial::GetRestitutionMode() const
{
	return restitution_combine_mode;
}

void PhysicsMaterial::Save(Data & data) const
{
	data.AddString("library_path", GetLibraryPath());
	data.AddString("assets_path", GetAssetsPath());
	data.AddUInt("UUID", GetUID());
	data.AddFloat("static_friction", static_friction);
	data.AddFloat("dynamic_friction", dynamic_friction);
	data.AddFloat("restitution", restitution);
	data.AddString("name", GetName());
	data.AddUInt("friction_mode", GetFrictionMode());
	data.AddUInt("restitution_mode", GetRestitutionMode());
}

bool PhysicsMaterial::Load(Data & data)
{
	bool ret = true;
	std::string library_path = data.GetString("library_path");

	PhysicsMaterial* phys_mat = App->phys_mats_importer->LoadPhysicsMaterialFromLibrary(library_path);
	if (!phys_mat)
	{
		std::string assets_path = data.GetString("assets_path");
		if (App->file_system->FileExist(assets_path))
		{
			library_path = App->resources->CreateLibraryFile(Resource::PhysicsMatResource, assets_path);
			if (!library_path.empty())
			{
				Load(data);
			}
		}
		else
		{
			ret = false;
		}
	}
	else
	{
		SetAssetsPath(data.GetString("assets_path"));
		SetLibraryPath(data.GetString("library_path"));
		SetUID(data.GetUInt("UUID"));
		static_friction = data.GetFloat("static_friction");
		dynamic_friction = data.GetFloat("dynamic_friction");
		restitution = data.GetFloat("restitution");
		SetName(data.GetString("name"));
		SetFrictionMode(data.GetUInt("friction_mode"));
		SetRestitutionMode(data.GetUInt("restitution_mode"));
	}

	return ret;
}

void PhysicsMaterial::CreateMeta() const
{
	time_t now = time(0);
	char* dt = ctime(&now);

	Data data;
	data.AddInt("Type", GetType());
	data.AddUInt("UUID", GetUID());
	data.AddString("Time_Created", dt);
	data.AddString("Library_path", GetLibraryPath());
	data.AddFloat("static_friction", static_friction);
	data.AddFloat("dynamic_friction", dynamic_friction);
	data.AddFloat("restitution", restitution);

	data.SaveAsMeta(GetAssetsPath());
}

void PhysicsMaterial::LoadToMemory()
{
}

void PhysicsMaterial::UnloadFromMemory()
{
}
