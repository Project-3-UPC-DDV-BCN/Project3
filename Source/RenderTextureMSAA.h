#pragma once

#include "OpenGL.h"
#include "Resource.h"

class RenderTextureMSAA : public Resource
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

	uint GetTextureID() const;
	int GetMaxMSAALevel() const;
	int GetCurrentMSAALevel() const;

	void Save(Data& data) const;
	bool Load(Data& data);
	void CreateMeta() const;

	void LoadToMemory();
	void UnloadFromMemory();

	uint GetWidth() const;
	uint GetHeight() const;

	uint GetFrameBufferID() const;

private:
	uint fbo_id;
	uint fbo_msaa_id;
	uint texture_id;
	uint rbo_id;
	uint texture_msaa_id;

	uint width;
	uint height;

	int max_msaa_samples;
	int current_msaa_samples;
};

