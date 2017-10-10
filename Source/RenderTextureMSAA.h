#pragma once

#include "OpenGL.h"

typedef unsigned int uint;

class RenderTextureMSAA
{
public:
public:
	RenderTextureMSAA();
	~RenderTextureMSAA();

	bool Create(uint width, uint height, int MSAA_level);
	void Bind();
	void Unbind();
	void Render();
	void ChangeMSAALevel(int MSAA_level);
	void Destroy();

	uint GetTexture() const;
	int GetMaxMSAALevel() const;
	int GetCurrentMSAALevel() const;

private:
	uint fboId, fboMsaaId;                // IDs of FBOs
	uint textureId;                       // ID of texture
	uint rboId, rboColorId, rboDepthId;   // IDs of Renderbuffer objects

	uint width;
	uint height;

	int max_msaa_samples;
	int current_msaa_samples;
};
