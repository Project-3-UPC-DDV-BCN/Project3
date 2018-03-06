#pragma once

#include <string>
#include "MathGeoLib/float3.h"

class pTexturedCube;
class Texture;
class ComponentCamera;

class CubeMap
{
public:
	CubeMap(float size);
	~CubeMap();

	void CreateCubeMap();
	void RenderCubeMap(float3 position, ComponentCamera* active_camera);
	void DeleteCubeMap();

	void SetCubeMapTopTexture(std::string texture_path);
	void SetCubeMapLeftTexture(std::string texture_path);
	void SetCubeMapFrontTexture(std::string texture_path);
	void SetCubeMapRightTexture(std::string texture_path);
	void SetCubeMapBackTexture(std::string texture_path);
	void SetCubeMapBottomTexture(std::string texture_path);

	void SetSize(float s) { size = s; }

private:
	Texture* textures[6];
	std::string textures_path[6];
	unsigned int cube_map_id;
	float size = 1.0f;
};

