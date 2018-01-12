#pragma once
#include "Module.h"
#include <vector>

namespace Nv
{
	namespace Blast
	{
		class FractureTool;
		class TkFramework;
		class TkAsset;
		class Mesh;
		class VoronoiSitesGenerator;
		class RandomGeneratorBase;
		struct AuthoringResult;
	}
}

class Mesh;

class RandomGenerator : public Nv::Blast::RandomGeneratorBase
{
public:
	float getRandomValue();
	void seed(int32_t seed);
};

class ModuleBlast :
	public Module
{
public:
	ModuleBlast(Application* app, bool start_enabled = true, bool is_game = false);
	~ModuleBlast();

	bool Init(Data* editor_config = nullptr);
	bool CleanUp();

	Nv::Blast::TkAsset* CreateBlastAsset();
	void CreateBlastMesh(Mesh* mesh);
	bool ApplyVoronoiFracture(uint chunk_id, uint cell_count, bool replace);
	bool ApplyVoronoiClusteredFracture();
	bool ApplyVoronoiRadialFracture();
	bool ApplyVoronoiInSphereFracture();
	bool RemoveInSphereFracture();
	bool ApplySliceFracture();
	void UpdateMesh();

private:
	Nv::Blast::TkFramework* blast_framework;
	Nv::Blast::FractureTool* fract_tool;
	Nv::Blast::Mesh* current_mesh;
	Nv::Blast::VoronoiSitesGenerator* voronoi_sites_generator;
	Nv::Blast::AuthoringResult* authoring_result;
	RandomGenerator* blast_random_generator;
	std::vector<int32_t> fractured_chunks;
};

