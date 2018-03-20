#pragma once

#include "Globals.h"
#include <fstream>
#include <iostream>
#include "cereal-1.2.2\include\cereal\archives\xml.hpp"
#include "cereal-1.2.2\include\cereal\archives\json.hpp"
#include "cereal-1.2.2\include\cereal\archives\binary.hpp"
#include "cereal-1.2.2\include\cereal\types\string.hpp"
#include "cereal-1.2.2\include\cereal\types\vector.hpp"
#include "MathGeoLib/float2.h"
#include "MathGeoLib/float3.h"
#include "MathGeoLib/float4.h"
#include "imgui\imgui.h"

class Data
{
public:
	Data();
	~Data();

	bool CanLoadAsJSON(std::string path);
	bool CanLoadAsBinary(std::string path, std::string binary_extension);

	void SaveAsXML(std::string path);
	bool LoadXML(std::string path);
	void SaveAsJSON(std::string path);
	bool LoadJSON(std::string path);
	void SaveAsBinary(std::string path);
	bool LoadBinary(std::string path);
	void SaveAsMeta(std::string path);
	bool LoadMeta(std::string path);

	void ClearData();

	void CreateSection(std::string sectionName);
	void CloseSection();
	bool EnterSection(std::string sectionName);
	void LeaveSection();

	bool GetBool(std::string valueName);
	int GetInt(std::string valueName);
	uint GetUInt(std::string valueName);
	double GetDouble(std::string valueName);
	float GetFloat(std::string valueName);
	std::string GetString(std::string valueName);
	float2 GetVector2(std::string valueName);
	float3 GetVector3(std::string valueName);
	float4 GetVector4(std::string valueName);
	ImVec2 GetImVector2(std::string valueName);
	ImVec3 GetImVector3(std::string valueName);
	ImVec4 GetImVector4(std::string valueName);

	void AddBool(std::string valueName, bool value);
	void AddInt(std::string valueName, int value);
	void AddUInt(std::string valueName, uint value);
	void AddDouble(std::string valueName, double value);
	void AddFloat(std::string valueName, float value);
	void AddString(std::string valueName, std::string value);
	void AddVector2(std::string valueName, float2 value);
	void AddVector3(std::string valueName, float3 value);
	void AddVector4(std::string valueName, float4 value);
	void AddImVector2(std::string valueName, ImVec2 value);
	void AddImVector3(std::string valueName, ImVec3 value);
	void AddImVector4(std::string valueName, ImVec4 value);

	void DeleteValue(std::string valueName);
	
private:

	friend class cereal::access;

	template<class Archive>
	void serialize(Archive & archive)
	{
		archive(data_names, data_values);
	}

private:
	std::vector<std::string> data_names;
	std::vector<std::string> data_values;

	std::vector<std::string> in_section_names;
	std::vector<std::string> in_section_values;
	int sections_open = 0;
	int current_index = 0;
	std::list<int> last_index;
	std::list<std::string> last_index_name;

	std::vector<std::string> out_section_names;
	std::vector<std::string> out_section_values;

	bool getting_from_section = false;
};

