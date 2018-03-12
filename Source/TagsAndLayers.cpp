#include "TagsAndLayers.h"
#include "Data.h"

TagsAndLayers::TagsAndLayers()
{
	tags_list.push_back("Default");
	tags_list.push_back("Main Camera");
	layers_list.push_back("Default");
	layers_list.push_back("Ignore Raycast");
	layers_list.push_back("UI");
}

TagsAndLayers::~TagsAndLayers()
{
}

void TagsAndLayers::AddTag(int index, std::string tag)
{
	if(tags_list.size() < MAX_TAGS) tags_list[index] = tag;
}

void TagsAndLayers::RemoveTag(int index)
{
	tags_list.erase(tags_list.begin() + index);
}

int TagsAndLayers::GetTagIndex(std::string tag_name) const
{
	for (int i = 0; i < tags_list.size(); i++)
	{
		if (tags_list[i] == tag_name) return i;
	}
	return -1;
}

std::string TagsAndLayers::GetTagNameByIndex(int index) const
{
	return tags_list[index];
}

void TagsAndLayers::CreateEmptyTag()
{
	if(tags_list.size() < MAX_TAGS) tags_list.push_back("New Tag " + std::to_string(tags_list.size()));
}

void TagsAndLayers::AddLayer(int index, std::string layer)
{
	if(layers_list.size() < MAX_LAYERS && index > 2) layers_list[index] = layer;
}

void TagsAndLayers::RemoveLayer(int index)
{
	layers_list.erase(layers_list.begin() + index);
}

int TagsAndLayers::GetLayerIndex(std::string layer_name) const
{
	for (int i = 0; i < layers_list.size(); i++)
	{
		if (layers_list[i] == layer_name) return i;
	}
	return -1;
}

std::string TagsAndLayers::GetLayerNameByIndex(int index) const
{
	return layers_list[index];
}

void TagsAndLayers::CreateEmptyLayer()
{
	if (layers_list.size() < MAX_LAYERS) layers_list.push_back("New Layer " + std::to_string(layers_list.size()));
}

void TagsAndLayers::Load(Data * data)
{
	if (data->EnterSection("Tags"))
	{
		tags_list.clear();
		int tags_num = data->GetInt("tags_num");
		for (int i = 0; i < tags_num; i++)
		{
			tags_list.push_back(data->GetString("tag_" + std::to_string(i)));
		}
		data->LeaveSection();
	}
	
	if (data->EnterSection("Layers"))
	{
		layers_list.clear();
		int layers_num = data->GetInt("layers_num");
		for (int i = 0; i < layers_num; i++)
		{
			layers_list.push_back(data->GetString("layer_" + std::to_string(i)));
		}
		data->LeaveSection();
	}
}

void TagsAndLayers::Save(Data * data) const
{
	data->CreateSection("Tags");
	data->AddInt("tags_num", tags_list.size());
	for (int i = 0; i < tags_list.size(); i++)
	{
		data->AddString("tag_" + std::to_string(i), tags_list[i]);
	}
	data->CloseSection();

	data->CreateSection("Layers");
	data->AddInt("layers_num", layers_list.size());
	for (int i = 0; i < layers_list.size(); i++)
	{
		data->AddString("layer_" + std::to_string(i), layers_list[i]);
	}
	data->CloseSection();
}
