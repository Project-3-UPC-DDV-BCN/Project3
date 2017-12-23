#pragma once
#include "Resource.h"

class Shader;

class ShaderProgram :
	public Resource
{
public:
	ShaderProgram();
	~ShaderProgram();

	void SetVertexShader(Shader* shader);
	void SetFragmentShader(Shader* shader);
	void SetShaders(Shader* vert, Shader* frag);

	Shader* GetVertexShader()const;
	Shader* GetFragmentShader()const;

	void UseProgram();
	void SaveToLibray() const;
	void LoadFromLibrary(const char* path);
	void LinkShaderProgram();
	uint GetProgramID() const;

	void Save(Data& data) const;
	bool Load(Data& data);
	void CreateMeta() const;
	void LoadToMemory();
	void UnloadFromMemory();

private:
	Shader* vertex_shader;
	Shader* fragment_shader;

	uint program_id;
};

