#include "Data.h"
#include <algorithm>

Data::Data()
{
}


Data::~Data()
{
}

bool Data::CanLoadAsJSON(std::string path, std::string extension)
{
	bool ret = false;

	if (path.find(extension) != std::string::npos)
		ret = true;

	return ret;
}

bool Data::CanLoadAsBinary(std::string path, std::string binary_extension)
{
	bool ret = false;

	if (path.find(binary_extension.c_str()) != std::string::npos)
		ret = true;

	return ret;
}

void Data::SaveAsXML(std::string path)
{
	if (path.find(".xml") == std::string::npos) { //xml extension is not especified
		path += ".xml";
	}
	std::ofstream file(path);
	cereal::XMLOutputArchive archive(file);
	for (int i = 0; i < data_names.size(); i++) {
		if (data_names[i] == "New_Section") {
			std::replace(data_names[i + 1].begin(), data_names[i + 1].end(), ' ', '_');
			archive.setNextName(data_names[i + 1].c_str());
			archive.startNode();
			i++;
		}
		else if (data_names[i] == "Section_Close") {
			archive.finishNode();
		}
		else {
			archive(cereal::make_nvp(data_names[i], data_values[i]));
		}
	}
}

bool Data::LoadXML(std::string path)
{
	bool ret = false;
	bool outsideSection = true;
	int sectionsOpen = 0;
	data_names.clear();
	data_values.clear();
	std::ifstream file(path);
	if (file.is_open()) {
		cereal::XMLInputArchive archive(file);
		while (true) {
			std::string nodeName;
			std::string nodeValue;
			if (archive.getNodeName() != NULL) {
				nodeName = archive.getNodeName();
			}
			else {
				if (sectionsOpen == 1) {
					outsideSection = true;
				}
				if (archive.isLastNode()) {
					break;
				}
				else {
					data_names.push_back("Section_Close");
					data_values.push_back("");
					archive.finishNode();
					sectionsOpen--;
					continue;
				}
			}
			archive.startNode();
			archive.loadValue(nodeValue);
			data_names.push_back(nodeName);
			if (nodeValue != "") {
				data_values.push_back(nodeValue);
				if (outsideSection) {
					out_section_values.push_back(nodeValue);
					out_section_names.push_back(nodeName);
				}
			}
			else {
				data_values.push_back("Section");
				outsideSection = false;
				sectionsOpen++;
				continue;
			}
			archive.finishNode();
		}
		ret = true;
	}
	return ret;
}

void Data::SaveAsJSON(std::string path, std::string extension)
{
	extension = '.' + extension;

	if (path.find(extension) == std::string::npos) { //json extension is not especified
		path += extension;
	}
	std::ofstream file(path);
	cereal::JSONOutputArchive archive(file);
	for (int i = 0; i < data_names.size(); i++) {
		if (data_names[i] == "New_Section") {
			std::replace(data_names[i + 1].begin(), data_names[i + 1].end(), ' ', '_');
			archive.setNextName(data_names[i + 1].c_str());
			archive.startNode();
			i++;
		}
		else if (data_names[i] == "Section_Close") {
			archive.finishNode();
		}
		else {
			archive(cereal::make_nvp(data_names[i], data_values[i]));
		}
	}
}

bool Data::LoadJSON(std::string path)
{
	bool ret = false;
	bool outsideSection = true;
	int sectionsOpen = 0;
	data_names.clear();
	data_values.clear();
	std::ifstream file(path);
	if (file.is_open()) {
		cereal::JSONInputArchive archive(file);
		while (true) {
			std::string nodeName;
			std::string nodeValue;
			if (archive.isObject()) {
				if (archive.isEmptyObject()) {
					archive.startNode();
					archive.finishNode();
					continue;
				}
				else {
					if (archive.getNodeName() != NULL) {
						nodeName = archive.getNodeName();
						data_names.push_back(nodeName);
						data_values.push_back("Section");
						archive.startNode();
						outsideSection = false;
						sectionsOpen++;
					}
					else {
						data_names.push_back("Section_Close");
						data_values.push_back("");
						sectionsOpen--;
						if (sectionsOpen == 0) {
							outsideSection = true;
						}
						if (archive.finishNode())
						{
							break;
						}
					}
				}
			}
			else {
				if (archive.getNodeName() != NULL) {
					nodeName = archive.getNodeName();
					archive.loadValue(nodeValue);
					data_names.push_back(nodeName);
					data_values.push_back(nodeValue);
					if (outsideSection) {
						out_section_values.push_back(nodeValue);
						out_section_names.push_back(nodeName);
					}
				}
				else {
					data_names.push_back("Section_Close");
					sectionsOpen--;
					data_values.push_back("");
					if (sectionsOpen == 0) {
						outsideSection = true;
					}
					if (archive.finishNode())
					{
						break;
					}
				}
			}
		}
		ret = true;
	}
	return ret;
}

void Data::SaveAsBinary(std::string path)
{
	std::ofstream file(path);
	cereal::BinaryOutputArchive archive(file);
	archive(*this);
	file.close();
}

bool Data::LoadBinary(std::string path)
{
	bool ret = false;
	bool outsideSection = true;
	int sectionsOpen = 0;
	data_names.clear();
	data_values.clear();
	std::ifstream file(path);
	if (file.is_open()) 
	{
		cereal::BinaryInputArchive archive(file);
		archive(*this);
		for (int i = 0; i < data_names.size(); i++) 
		{
			if (data_names[i] == "New_Section") {
				data_names.erase(data_names.begin() + i);
				data_values.erase(data_values.begin() + i);
				outsideSection = false;
				sectionsOpen++;
			}
			else if (data_names[i] == "Section_Close") 
			{
				sectionsOpen--;
				if (sectionsOpen == 0) {
					outsideSection = true;
				}
			}
			else {
				if (outsideSection) {
					out_section_names.push_back(data_names[i]);
					out_section_values.push_back(data_values[i]);
				}
			}
		}
		ret = true;

		file.close();
	}
	return ret;
}

void Data::SaveAsMeta(std::string path)
{
	if (path.find(".meta") == std::string::npos) { //meta extension is not especified
		path += ".meta";
	}
	std::ofstream file(path);
	cereal::JSONOutputArchive archive(file);
	for (int i = 0; i < data_names.size(); i++) {
		if (data_names[i] == "New_Section") {
			std::replace(data_names[i + 1].begin(), data_names[i + 1].end(), ' ', '_');
			archive.setNextName(data_names[i + 1].c_str());
			archive.startNode();
			i++;
		}
		else if (data_names[i] == "Section_Close") {
			archive.finishNode();
		}
		else {
			archive(cereal::make_nvp(data_names[i], data_values[i]));
		}
	}
}

bool Data::LoadMeta(std::string path)
{
	return LoadJSON(path);
}

void Data::ResetData()
{
	while (1)
	{
		if (!LeaveSection())
			break;
	}

	current_index = 0;
}

void Data::ClearData()
{
	data_names.clear();
	data_values.clear();

	in_section_names.clear();
	in_section_names.clear();
	sections_open = 0;
	current_index = 0;
	last_index.clear();
	last_index_name.clear();

	out_section_names.clear();
	out_section_values.clear();

	getting_from_section = false;
}

void Data::CreateSection(std::string sectionName)
{
	data_names.push_back("New_Section");
	data_values.push_back("");
	data_names.push_back(sectionName);
	data_values.push_back("Section");
}

bool Data::EnterSection(std::string sectionName)
{
	bool ret = false;
	getting_from_section = true;
	std::vector<std::string>::iterator it = find(data_names.begin() + current_index, data_names.end(), sectionName);
	if (it != data_names.end()) {
		int index = it - data_names.begin();
		if (data_values[index] == "Section") {
			last_index.push_back(index);
			last_index_name.push_back(sectionName);
			current_index = index;
			in_section_values.clear();
			in_section_names.clear();
			sections_open++;
			int internalSectionsOpen = 1; //Avoid getting data from other sections inside this one
			bool is_vector = false;
			for (int i = index + 1; internalSectionsOpen > 0; i++) {
				if (data_values[i] == "Section") {
					if (data_names[i + 1] != "value_X" && data_names[i + 2] != "value_Y") { //std::vectors are not new sections
						internalSectionsOpen++;
					}
					else {
						is_vector = true;
					}
				}
				if (data_names[i] == "Section_Close") {
					if (!is_vector) {
						internalSectionsOpen--;
					}
					else {
						is_vector = false;
					}
				}
				else if (internalSectionsOpen < 2) {
					in_section_values.push_back(data_values[i]);
					in_section_names.push_back(data_names[i]);
				}
			}
			ret = true;
		}
	}
	return ret;
}

bool Data::LeaveSection()
{
	bool ret = false;

	in_section_values.clear();
	in_section_names.clear();

	sections_open--;

	if (sections_open <= 0)
	{
		getting_from_section = false;
		sections_open = 0;
	}
	else
	{
		if (!last_index.empty() && !last_index_name.empty())
		{
			last_index.pop_back();
			last_index_name.pop_back();
			current_index = last_index.back();
			EnterSection(last_index_name.back());

			//remove again because we are adding those during "EnterSection"
			last_index.pop_back();
			last_index_name.pop_back();
			sections_open--;

			ret = true;
		}
	}

	return ret;
}

void Data::CloseSection()
{
	data_names.push_back("Section_Close");
	data_values.push_back("");
}

bool Data::GetBool(std::string valueName)
{
	std::replace(valueName.begin(), valueName.end(), ' ', '_');
	std::vector<std::string> vec_names;
	std::vector<std::string> vec_values;

	if (getting_from_section) {
		vec_names = in_section_names;
		vec_values = in_section_values;
	}
	else {
		vec_names = out_section_names;
		vec_values = out_section_values;
	}

	std::vector<std::string>::iterator it = find(vec_names.begin(), vec_names.end(), valueName);
	if (it != vec_names.end()) {
		int index = it - vec_names.begin();
		if (vec_values[index] == "1") return true;
	}

	return false;
}

int Data::GetInt(std::string valueName)
{
	std::replace(valueName.begin(), valueName.end(), ' ', '_');
	std::vector<std::string> vec_names;
	std::vector<std::string> vec_values;

	if (getting_from_section) {
		vec_names = in_section_names;
		vec_values = in_section_values;
	}
	else {
		vec_names = out_section_names;
		vec_values = out_section_values;
	}

	std::vector<std::string>::iterator it = find(vec_names.begin(), vec_names.end(), valueName);
	if (it != vec_names.end()) {
		int index = it - vec_names.begin();
		return stoi(vec_values[index]);
	}
	return -1;
}

uint Data::GetUInt(std::string valueName)
{
	std::replace(valueName.begin(), valueName.end(), ' ', '_');
	std::vector<std::string> vec_names;
	std::vector<std::string> vec_values;

	if (getting_from_section) {
		vec_names = in_section_names;
		vec_values = in_section_values;
	}
	else {
		vec_names = out_section_names;
		vec_values = out_section_values;
	}

	std::vector<std::string>::iterator it = find(vec_names.begin(), vec_names.end(), valueName);
	if (it != vec_names.end()) {
		int index = it - vec_names.begin();
		return stoul(vec_values[index]);
	}
	return 0;
}

double Data::GetDouble(std::string valueName)
{
	std::replace(valueName.begin(), valueName.end(), ' ', '_');
	std::vector<std::string> vec_names;
	std::vector<std::string> vec_values;

	if (getting_from_section) {
		vec_names = in_section_names;
		vec_values = in_section_values;
	}
	else {
		vec_names = out_section_names;
		vec_values = out_section_values;
	}

	std::vector<std::string>::iterator it = find(vec_names.begin(), vec_names.end(), valueName);
	if (it != vec_names.end()) {
		int index = it - vec_names.begin();
		return stod(vec_values[index]);
	}
	return -1.0;
}

float Data::GetFloat(std::string valueName)
{
	std::replace(valueName.begin(), valueName.end(), ' ', '_');
	std::vector<std::string> vec_names;
	std::vector<std::string> vec_values;

	if (getting_from_section) {
		vec_names = in_section_names;
		vec_values = in_section_values;
	}
	else {
		vec_names = out_section_names;
		vec_values = out_section_values;
	}

	std::vector<std::string>::iterator it = find(vec_names.begin(), vec_names.end(), valueName);
	if (it != vec_names.end()) {
		int index = it - vec_names.begin();
		return stof(vec_values[index]);
	}
	return -1.0f;
}

std::string Data::GetString(std::string valueName)
{
	std::replace(valueName.begin(), valueName.end(), ' ', '_');
	std::vector<std::string> vec_names;
	std::vector<std::string> vec_values;

	if (getting_from_section) {
		vec_names = in_section_names;
		vec_values = in_section_values;
	}
	else {
		vec_names = out_section_names;
		vec_values = out_section_values;
	}

	std::vector<std::string>::iterator it = find(vec_names.begin(), vec_names.end(), valueName);
	if (it != vec_names.end()) {
		int index = it - vec_names.begin();
		return vec_values[index];
	}
	return "value not found";
}

float2 Data::GetVector2(std::string valueName)
{
	float2 ret;
	std::replace(valueName.begin(), valueName.end(), ' ', '_');

	std::vector<std::string> vec_names;
	std::vector<std::string> vec_values;

	if (EnterSection(valueName))
	{
		if (getting_from_section) {
			vec_names = in_section_names;
			vec_values = in_section_values;
		}

		if (vec_names.size() == 2)
		{
			ret.x = stof(vec_values[0]);
			ret.y = stof(vec_values[1]);
		}
		LeaveSection();
	}
	else {
		ret.x = -1.0f;
		ret.y = -1.0f;
	}

	return ret;
}

float3 Data::GetVector3(std::string valueName)
{
	float3 ret;
	std::replace(valueName.begin(), valueName.end(), ' ', '_');

	std::vector<std::string> vec_names;
	std::vector<std::string> vec_values;

	if (EnterSection(valueName))
	{
		if (getting_from_section) {
			vec_names = in_section_names;
			vec_values = in_section_values;
		}

		if (vec_names.size() == 3)
		{
			ret.x = stof(vec_values[0]);
			ret.y = stof(vec_values[1]);
			ret.z = stof(vec_values[2]);
		}
		LeaveSection();
	}
	else {
		ret.x = -1.0f;
		ret.y = -1.0f;
		ret.z = -1.0f;
	}

	return ret;
}

float4 Data::GetVector4(std::string valueName)
{
	float4 ret;
	std::replace(valueName.begin(), valueName.end(), ' ', '_');
	std::vector<std::string> vec_names;
	std::vector<std::string> vec_values;

	if (EnterSection(valueName))
	{
		if (getting_from_section) {
			vec_names = in_section_names;
			vec_values = in_section_values;
		}

		if (vec_names.size() == 4)
		{
			ret.x = stof(vec_values[0]);
			ret.y = stof(vec_values[1]);
			ret.z = stof(vec_values[2]);
			ret.w = stof(vec_values[3]);
		}
		LeaveSection();
	}
	else {
		ret.x = -1.0f;
		ret.y = -1.0f;
		ret.z = -1.0f;
		ret.w = -1.0f;
	}

	return ret;
}

ImVec2 Data::GetImVector2(std::string valueName)
{
	ImVec2 vec2;
	float2 f2 = GetVector2(valueName);
	vec2.x = f2.x;
	vec2.y = f2.y;
	return vec2;
}

ImVec3 Data::GetImVector3(std::string valueName)
{
	ImVec3 vec3;
	float3 f3 = GetVector3(valueName);
	vec3.x = f3.x;
	vec3.y = f3.y;
	vec3.z = f3.z;
	return vec3;
}

ImVec4 Data::GetImVector4(std::string valueName)
{
	ImVec4 vec4;
	float4 f4 = GetVector4(valueName);
	vec4.x = f4.x;
	vec4.y = f4.y;
	vec4.z = f4.z;
	vec4.w = f4.w;
	return vec4;
}

void Data::AddBool(std::string valueName, bool value)
{
	std::replace(valueName.begin(), valueName.end(), ' ', '_');
	data_values.push_back(std::to_string(value));
	data_names.push_back(valueName);
}

void Data::AddInt(std::string valueName, int value)
{
	std::replace(valueName.begin(), valueName.end(), ' ', '_');
	data_values.push_back(std::to_string(value));
	data_names.push_back(valueName);
}

void Data::AddUInt(std::string valueName, uint value)
{
	std::replace(valueName.begin(), valueName.end(), ' ', '_');
	data_values.push_back(std::to_string(value));
	data_names.push_back(valueName);
}

void Data::AddDouble(std::string valueName, double value)
{
	std::replace(valueName.begin(), valueName.end(), ' ', '_');
	data_values.push_back(std::to_string(value));
	data_names.push_back(valueName);
}

void Data::AddFloat(std::string valueName, float value)
{
	std::replace(valueName.begin(), valueName.end(), ' ', '_');
	data_values.push_back(std::to_string(value));
	data_names.push_back(valueName);
}

void Data::AddString(std::string valueName, std::string value)
{
	std::replace(valueName.begin(), valueName.end(), ' ', '_');
	data_values.push_back(value);
	data_names.push_back(valueName);
}

void Data::AddVector2(std::string valueName, float2 value)
{
	std::replace(valueName.begin(), valueName.end(), ' ', '_');
	CreateSection(valueName);
	data_values.push_back(std::to_string(value.x));
	data_names.push_back("value_X");
	data_values.push_back(std::to_string(value.y));
	data_names.push_back("value_Y");
	CloseSection();
}

void Data::AddVector3(std::string valueName, float3 value)
{
	std::replace(valueName.begin(), valueName.end(), ' ', '_');
	CreateSection(valueName);
	data_values.push_back(std::to_string(value.x));
	data_names.push_back("value_X");
	data_values.push_back(std::to_string(value.y));
	data_names.push_back("value_Y");
	data_values.push_back(std::to_string(value.z));
	data_names.push_back("value_Z");
	CloseSection();
}

void Data::AddVector4(std::string valueName, float4 value)
{
	std::replace(valueName.begin(), valueName.end(), ' ', '_');
	CreateSection(valueName);
	data_values.push_back(std::to_string(value.x));
	data_names.push_back("value_X");
	data_values.push_back(std::to_string(value.y));
	data_names.push_back("value_Y");
	data_values.push_back(std::to_string(value.z));
	data_names.push_back("value_Z");
	data_values.push_back(std::to_string(value.w));
	data_names.push_back("value_W");
	CloseSection();
}

void Data::AddImVector2(std::string valueName, ImVec2 value)
{
	float2 f2;
	f2.x = value.x;
	f2.y = value.y;
	AddVector2(valueName, f2);
}

void Data::AddImVector3(std::string valueName, ImVec3 value)
{
	float3 f3;
	f3.x = value.x;
	f3.y = value.y;
	f3.z = value.z;
	AddVector3(valueName, f3);
}

void Data::AddImVector4(std::string valueName, ImVec4 value)
{
	float4 f4;
	f4.x = value.x;
	f4.y = value.y;
	f4.z = value.z;
	f4.w = value.w;
	AddVector4(valueName, f4);
}


void Data::DeleteValue(std::string valueName)
{
	int counter = -1;
	bool found = false;
	for (std::vector<std::string>::iterator it = data_names.begin(); it != data_names.end(); ++it)
	{
		++counter;
		if (valueName == (*it))
		{
			data_names.erase(it);
			found = true;
			break;
		}
	}

	if (found)
	{
		int counter2 = -1;
		for (std::vector<std::string>::iterator it = data_values.begin(); it != data_values.end(); ++it)
		{
			++counter2;

			if (counter == counter2)
			{
				data_values.erase(it);
				break;
			}
		}
	}
}
