#pragma once

#include <string>
#include "MathGeoLib\Math\float3.h"

class pTexturedCube;
class Texture;

class CubeMap
{
public:
	CubeMap(float size);
	~CubeMap();

	void CreateCubeMap();
	void RenderCubeMap(float3 position);
	void DeleteCubeMap();

	void SetCubeMapTopTexture(std::string texture_path);
	void SetCubeMapLeftTexture(std::string texture_path);
	void SetCubeMapFrontTexture(std::string texture_path);
	void SetCubeMapRightTexture(std::string texture_path);
	void SetCubeMapBackTexture(std::string texture_path);
	void SetCubeMapBottomTexture(std::string texture_path);

private:
	Texture* textures[6];
	std::string textures_path[6];
	unsigned int cube_map_id;
	pTexturedCube* cube;
};

