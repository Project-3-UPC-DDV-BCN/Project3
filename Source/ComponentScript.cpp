#include "ComponentScript.h"
#include "CSScript.h"
#include "ModuleResources.h"
#include "Application.h"
#include "ModuleScriptImporter.h"
#include "ModulePhysics.h"

ComponentScript::ComponentScript(GameObject* attached_gameobject)
{
	SetName("Script");
	SetType(ComponentType::CompScript);
	SetGameObject(attached_gameobject);
	script = nullptr;
	SetActive(true);
}

ComponentScript::~ComponentScript()
{
}

void ComponentScript::InitScript()
{
	if (script)
	{
		CSScript* last_script = App->script_importer->ns_importer->current_script;
		App->script_importer->ns_importer->current_script = (CSScript*)script;
		script->SetAttachedGameObject(GetGameObject());
		script->InitScript();
		App->script_importer->ns_importer->current_script = last_script;
	}
}

void ComponentScript::StartScript()
{
	if (script)
	{
		CSScript* last_script = App->script_importer->ns_importer->current_script;
		App->script_importer->ns_importer->current_script = (CSScript*)script;
		script->StartScript();
		App->script_importer->ns_importer->current_script = last_script;
	}
}

void ComponentScript::UpdateScript()
{
	if (script)
	{
		CSScript* last_script = App->script_importer->ns_importer->current_script;
		App->script_importer->ns_importer->current_script = (CSScript*)script;
		script->UpdateScript();
		App->script_importer->ns_importer->current_script = last_script;
	}
}

void ComponentScript::OnCollisionEnter(CollisionData& col_data)
{
	if (script)
	{
		CSScript* last_script = App->script_importer->ns_importer->current_script;
		App->script_importer->ns_importer->current_script = (CSScript*)script;
		script->OnCollisionEnter(col_data);
		App->script_importer->ns_importer->current_script = last_script;
	}
}

void ComponentScript::OnCollisionStay(CollisionData& col_data)
{
	if (script)
	{
		CSScript* last_script = App->script_importer->ns_importer->current_script;
		App->script_importer->ns_importer->current_script = (CSScript*)script;
		script->OnCollisionStay(col_data);
		App->script_importer->ns_importer->current_script = last_script;
	}
}

void ComponentScript::OnCollisionExit(CollisionData& col_data)
{
	if (script)
	{
		CSScript* last_script = App->script_importer->ns_importer->current_script;
		App->script_importer->ns_importer->current_script = (CSScript*)script;
		script->OnCollisionExit(col_data);
		App->script_importer->ns_importer->current_script = last_script;
	}
}

void ComponentScript::OnTriggerEnter(CollisionData& col_data)
{
	if (script)
	{
		CSScript* last_script = App->script_importer->ns_importer->current_script;
		App->script_importer->ns_importer->current_script = (CSScript*)script;
		script->OnTriggerEnter(col_data);
		App->script_importer->ns_importer->current_script = last_script;
	}
}

void ComponentScript::OnTriggerStay(CollisionData& col_data)
{
	if (script)
	{
		CSScript* last_script = App->script_importer->ns_importer->current_script;
		App->script_importer->ns_importer->current_script = (CSScript*)script;
		script->OnTriggerStay(col_data);
		App->script_importer->ns_importer->current_script = last_script;
	}
}

void ComponentScript::OnTriggerExit(CollisionData& col_data)
{
	if (script)
	{
		CSScript* last_script = App->script_importer->ns_importer->current_script;
		App->script_importer->ns_importer->current_script = (CSScript*)script;
		script->OnTriggerExit(col_data);
		App->script_importer->ns_importer->current_script = last_script;
	}
}

void ComponentScript::OnEnable()
{
	if (script && App->IsPlaying())
	{
		CSScript* last_script = App->script_importer->ns_importer->current_script;
		App->script_importer->ns_importer->current_script = (CSScript*)script;
		script->OnEnable();
		App->script_importer->ns_importer->current_script = last_script;
	}
}

void ComponentScript::OnDisable()
{
	if (script && App->IsPlaying())
	{
		CSScript* last_script = App->script_importer->ns_importer->current_script;
		App->script_importer->ns_importer->current_script = (CSScript*)script;
		script->OnDisable();
		App->script_importer->ns_importer->current_script = last_script;
	}
}

void ComponentScript::SetScript(Script * script)
{
	this->script = script;
}

std::string ComponentScript::GetScriptName() const
{
	std::string name;
	if (script)
	{
		name = script->GetName();
	}
	else
	{
		name = "No Script";
	}
	return name;
}

Script * ComponentScript::GetScript() const
{
	return script;
}

std::vector<ScriptField*> ComponentScript::GetScriptFields() const
{
	std::vector<ScriptField*> fields;
	if (script)
	{
		fields = script->GetScriptFields();
	}

	return fields;
}

void ComponentScript::UpdateScriptFields()
{
	if (script)
	{
		script_fields = script->GetScriptFields();
	}
}

void ComponentScript::Save(Data & data) const
{
	data.AddInt("Type", GetType());
	data.AddBool("Active", IsActive());
	data.AddUInt("UUID", GetUID());
	data.CreateSection("Script");
	if (script)
	{
		GetScriptFields();
		script->FillSavingData();
		script->Save(data);
	}
	data.CloseSection();
}

void ComponentScript::Load(Data & data)
{
	SetType((Component::ComponentType)data.GetInt("Type"));
	SetActive(data.GetBool("Active"));
	SetUID(data.GetUInt("UUID"));
	if (data.EnterSection("Script"))
	{
		std::string script_name = data.GetString("script_name");
		if (!script)
		{
			script = new CSScript();
		}
		if (!script->Load(data))
		{
			CONSOLE_ERROR("Cannot find %s. Script not loaded", script_name.c_str());
			RELEASE(script);
		}
		/*else
		{
			script->SetAttachedGameObject(GetGameObject());
		}*/
		data.LeaveSection();
	}
}
