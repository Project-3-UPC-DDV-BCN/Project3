#include "ModuleBlast.h"
#include "Nvidia/Blast/Include/toolkit/NvBlastTk.h"
#include "Nvidia/Blast/Include/extensions/authoring/NvBlastExtAuthoring.h"
#include "Nvidia/Blast/Include/extensions/authoring/NvBlastExtAuthoringFractureTool.h"
#include "Nvidia/Blast/Include/extensions/authoring/NvBlastExtAuthoringMesh.h"
#include "Nvidia/Blast/Include/extensions/authoring/NvBlastExtAuthoringCollisionBuilder.h"
#include "Nvidia/Blast/Include/extensions/authoring/NvBlastExtAuthoringBondGenerator.h"
#include "Mesh.h"
#include "Application.h"
#include "ModulePhysics.h"


ModuleBlast::ModuleBlast(Application* app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
	blast_framework = nullptr;
	fract_tool = nullptr;
	current_mesh = nullptr;
	voronoi_sites_generator = nullptr;
	blast_random_generator = nullptr;
	authoring_result = nullptr;
}

ModuleBlast::~ModuleBlast()
{
}

bool ModuleBlast::Init(Data * editor_config)
{
	blast_framework = NvBlastTkFrameworkCreate();
	fract_tool = NvBlastExtAuthoringCreateFractureTool();
	fract_tool->setInteriorMaterialId(MATERIAL_INTERIOR);
	blast_random_generator = new RandomGenerator();
	return true;
}

bool ModuleBlast::CleanUp()
{
	fract_tool->release();
	current_mesh->release();
	blast_framework->release();
	if (voronoi_sites_generator)
	{
		voronoi_sites_generator->release();
	}
	RELEASE(blast_random_generator);
	return true;
}

Nv::Blast::TkAsset * ModuleBlast::CreateBlastAsset()
{
	Nv::Blast::TkAssetDesc desc;
	//desc.
	

	return blast_framework->createAsset(desc);
}

void ModuleBlast::CreateBlastMesh(Mesh * mesh)
{
	current_mesh = NvBlastExtAuthoringCreateMesh((physx::PxVec3*)mesh->vertices, (physx::PxVec3*)mesh->normals, (physx::PxVec2*)mesh->texture_coords,
		mesh->num_vertices, mesh->indices, mesh->num_indices);
	fract_tool->setSourceMesh(current_mesh);
	if (voronoi_sites_generator) voronoi_sites_generator->release();
	voronoi_sites_generator = NvBlastExtAuthoringCreateVoronoiSitesGenerator(current_mesh, blast_random_generator);
}

bool ModuleBlast::ApplyVoronoiFracture(uint chunk_id, uint cell_count, bool replace)
{
	blast_random_generator->seed(0);
	voronoi_sites_generator->uniformlyGenerateSitesInMesh(cell_count);
	const physx::PxVec3* sites = nullptr;
	uint sites_count = voronoi_sites_generator->getVoronoiSites(sites);
	int fracture_chunk_id = authoring_result->assetToFractureChunkIdMap[chunk_id];
	if (fract_tool->voronoiFracturing(fracture_chunk_id, sites_count, sites, replace) == 0)
	{
		fractured_chunks.push_back(fracture_chunk_id);
		UpdateMesh();
		fract_tool->createChunkMesh(0)->getVertices
		return true;
	}
	return false;
}

bool ModuleBlast::ApplyVoronoiClusteredFracture()
{
	return false;
}

bool ModuleBlast::ApplyVoronoiRadialFracture()
{
	return false;
}

bool ModuleBlast::ApplyVoronoiInSphereFracture()
{
	return false;
}

bool ModuleBlast::RemoveInSphereFracture()
{
	return false;
}

bool ModuleBlast::ApplySliceFracture()
{
	return false;
}

void ModuleBlast::UpdateMesh()
{
	Nv::Blast::BlastBondGenerator* bond_generator = NvBlastExtAuthoringCreateBondGenerator(App->physics->GetPhysXCooking(), &App->physics->GetPhysXPhysics()->getPhysicsInsertionCallback());
	Nv::Blast::ConvexMeshBuilder* collision_builder = NvBlastExtAuthoringCreateConvexMeshBuilder(App->physics->GetPhysXCooking(), &App->physics->GetPhysXPhysics()->getPhysicsInsertionCallback());
	Nv::Blast::CollisionParams param;
	param.maximumNumberOfHulls = 1;
	param.voxelGridResolution = 0;
	authoring_result = NvBlastExtAuthoringProcessFracture(*fract_tool, *bond_generator, *collision_builder, param);
	bond_generator->release();
	collision_builder->release();
}


//Blast Random Generator
float RandomGenerator::getRandomValue()
{
	return App->RandomNumber().Float();
}

void RandomGenerator::seed(int32_t seed)
{
	App->RandomNumber().Seed(seed);
}
