#pragma once

#include "Resource.h"

class JSONTool;
class SoundBank;

class SoundBankResource : 
	public Resource
{
public:
	SoundBankResource();
	~SoundBankResource();

	void Save(Data& data) const;
	bool Load(Data& data);
	void CreateMeta() const;
	void LoadToMemory();
	void UnloadFromMemory();
	unsigned int GetBankInfo(std::string path, SoundBank *& bank);

private:
	JSONTool * json = nullptr;
	SoundBank* soundbank = nullptr;
};