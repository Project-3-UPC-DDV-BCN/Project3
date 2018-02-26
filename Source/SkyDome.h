#pragma once

#include "MathGeoLib/float3.h"

class pTexturedSphere;
class Texture;

class SkyDome
{
public:
	SkyDome();
	~SkyDome();

	void Create(float radius);
	void SetTexture(std::string texture_path);
	void SetTexture(Texture* texture_path);
	void SetPosition(float3 pos);
	void Render();

private:
	pTexturedSphere* sphere;
	Texture* texture;
};

