#include "GOAPAction.h"
#include "Application.h"
#include "ModuleResources.h"
#include "Globals.h"
#include "Data.h"
#include <fstream>
#include <sstream>
#include "CSScript.h"

GOAPAction::GOAPAction(const char* name, uint cost, bool create_script) : cost(cost)
{
	if (create_script)
	{
		//Create script
		std::ifstream in_file;
		std::string new_file_name = name;
		std::string script_name = name;

		in_file.open(GOAP_TEMPLATE_FILE_PATH);
		new_file_name += ".cs";

		if (in_file.is_open()) {
			std::stringstream str_stream;
			str_stream << in_file.rdbuf();//read the file
			std::string str = str_stream.str();//str holds the content of the file

			if (str.empty())
				return;
			size_t start_pos = 0;
			std::string class_name_template = "#CLASS_NAME#";
			while ((start_pos = str.find(class_name_template, start_pos)) != std::string::npos) {
				str.replace(start_pos, class_name_template.length(), script_name);
				start_pos += script_name.length();
			}

			in_file.close();

			std::ofstream output_file(ASSETS_GOAP_FOLDER + new_file_name);
			output_file << str;
			output_file.close();

			App->resources->CreateResource(ASSETS_GOAP_FOLDER + new_file_name);
			script = (CSScript*)App->resources->GetScript(script_name);
		}
	}
	
	SetName(name);
	SetType(Resource::GOAPActionResource);
}

GOAPAction::~GOAPAction()
{
}

void GOAPAction::Save(Data & data) const
{
	data.AddString("library_path", GetLibraryPath());
	data.AddString("assets_path", GetAssetsPath());
	data.AddString("name", GetName());
	data.AddUInt("UUID", GetUID());
	if(script != nullptr)
		data.AddString("script", script->GetName());
	data.AddInt("preconditions_num", preconditions.size());
	for (int i = 0; i < preconditions.size(); ++i)
	{
		data.CreateSection("precondition_" + std::to_string(i));
		data.AddInt("type", preconditions[i]->GetType());
		data.AddInt("comparison", preconditions[i]->GetComparisonMethod());
		data.AddString("name", preconditions[i]->GetName());
		switch (preconditions[i]->GetType())
		{
		case GOAPVariable::T_BOOL:
			data.AddBool("value", preconditions[i]->GetValueB());
			break;
		case GOAPVariable::T_FLOAT:
			data.AddFloat("value", preconditions[i]->GetValueF());
			break;
		default:
			break;
		}
		data.CloseSection();
	}
	data.AddInt("effects_num", effects.size());
	for (int i = 0; i < effects.size(); ++i)
	{
		data.CreateSection("effect_" + std::to_string(i));
		data.AddInt("type", effects[i]->GetType());
		data.AddInt("effect", effects[i]->GetEffect());
		data.AddString("name", effects[i]->GetName());
		switch (effects[i]->GetType())
		{
		case GOAPVariable::T_BOOL:
			data.AddBool("value", effects[i]->GetValueB());
			break;
		case GOAPVariable::T_FLOAT:
			data.AddFloat("value", effects[i]->GetValueF());
			break;
		default:
			break;
		}
		data.CloseSection();
	}

}

bool GOAPAction::Load(Data & data)
{
	SetUID(data.GetUInt("UUID"));
	SetAssetsPath(data.GetString("assets_path"));
	SetLibraryPath(data.GetString("library_path"));
	SetName(data.GetString("name"));
	script = (CSScript*)App->resources->GetScript(data.GetString("script"));
	int preconditions_num = data.GetInt("preconditions_num");
	for (int i = 0; i < preconditions_num; ++i)
	{
		if (data.EnterSection("precondition_" + std::to_string(i)))
		{
			GOAPVariable::VariableType type = (GOAPVariable::VariableType)data.GetInt("type");
			GOAPField::ComparisonMethod cm = (GOAPField::ComparisonMethod)data.GetInt("comparison");
			std::string name = data.GetString("name");
			switch (type)
			{
			case GOAPVariable::T_NULL:
				break;
			case GOAPVariable::T_BOOL:
				AddPreCondition(name, cm, data.GetBool("value"));
				break;
			case GOAPVariable::T_FLOAT:
				AddPreCondition(name, cm, data.GetFloat("value"));
				break;
			default:
				break;
			}
			data.LeaveSection();
		}
	}
	int effects_num = data.GetInt("effects_num");
	for (int i = 0; i < effects_num; ++i)
	{
		if (data.EnterSection("effect_" + std::to_string(i)))
		{
			std::string name = data.GetString("name");
			GOAPVariable::VariableType type = (GOAPVariable::VariableType)data.GetInt("type");
			switch (type)
			{
			case GOAPVariable::T_NULL:
				break;
			case GOAPVariable::T_BOOL:
				AddEffect(name, data.GetBool("value"));
				break;
			case GOAPVariable::T_FLOAT:
				AddEffect(name, (GOAPEffect::EffectType)data.GetInt("effect"), data.GetFloat("value"));
				break;
			default:
				break;
			}
			data.LeaveSection();
		}
	}

	return true;
}

void GOAPAction::CreateMeta() const
{
}

void GOAPAction::LoadToMemory()
{
}

void GOAPAction::UnloadFromMemory()
{
}

void GOAPAction::AddPreCondition(std::string & name, GOAPField::ComparisonMethod comparison_method, bool value)
{
	preconditions.push_back(new GOAPField(name.c_str(), comparison_method, value));
}

void GOAPAction::AddPreCondition(std::string & name, GOAPField::ComparisonMethod comparison_method, float value)
{
	preconditions.push_back(new GOAPField(name.c_str(), comparison_method, value));
}

void GOAPAction::AddEffect(std::string & name, bool value)
{
	effects.push_back(new GOAPEffect(name.c_str(), value));
}

void GOAPAction::AddEffect(std::string & name, GOAPEffect::EffectType effect, float value)
{
	effects.push_back(new GOAPEffect(name.c_str(), effect, value));
}

int GOAPAction::GetNumEffects() const
{
	return effects.size();
}

GOAPEffect * GOAPAction::GetEffect(int index) const
{
	return (index >= 0 && index < effects.size()) ? effects[index] : nullptr;
}

int GOAPAction::GetNumPreconditions() const
{
	return preconditions.size();
}

GOAPField * GOAPAction::GetPrecondition(int index) const
{
	return (index >= 0 && index < preconditions.size()) ? preconditions[index] : nullptr;
}

uint GOAPAction::GetCost() const
{
	return cost;
}

void GOAPAction::Update()
{
	if (script != nullptr)
		script->UpdateScript();
	else
	{
		CONSOLE_ERROR("GOAP Action: %s doesn't have an asosiated script!!!", GetName());
	}
}

void GOAPAction::Start()
{
	if (script != nullptr)
		script->StartScript();
	else
	{
		CONSOLE_ERROR("GOAP Action: %s doesn't have an asosiated script!!!", GetName());
	}
}

void GOAPAction::OnComplete()
{
	if (script != nullptr)
		script->OnComplete();
	else
	{
		CONSOLE_ERROR("GOAP Action: %s doesn't have an asosiated script!!!", GetName());
	}
}

void GOAPAction::OnFail()
{
	if (script != nullptr)
		script->OnFail();
	else
	{
		CONSOLE_ERROR("GOAP Action: %s doesn't have an asosiated script!!!", GetName());
	}
}

CSScript * GOAPAction::GetScript() const
{
	return script;
}
