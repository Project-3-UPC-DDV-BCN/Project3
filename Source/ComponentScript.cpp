#include "ComponentScript.h"
#include "CSScript.h"
#include "ModuleResources.h"
#include "Application.h"
#include "ModuleScriptImporter.h"

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
		App->script_importer->SetCurrentScript((CSScript*)script);
		script->InitScript();
	}
}

void ComponentScript::StartScript()
{
	if (script)
	{
		App->script_importer->SetCurrentScript((CSScript*)script);
		script->StartScript();
	}
}

void ComponentScript::UpdateScript()
{
	if (script)
	{
		App->script_importer->SetCurrentScript((CSScript*)script);
		script->UpdateScript();
	}
}

void ComponentScript::OnCollisionEnter(GameObject* other_collider)
{
	if (script)
	{
		App->script_importer->SetCurrentScript((CSScript*)script);
		script->OnCollisionEnter(other_collider);
	}
}

void ComponentScript::OnCollisionStay(GameObject* other_collider)
{
	if (script)
	{
		App->script_importer->SetCurrentScript((CSScript*)script);
		script->OnCollisionStay(other_collider);
	}
}

void ComponentScript::OnCollisionExit(GameObject* other_collider)
{
	if (script)
	{
		App->script_importer->SetCurrentScript((CSScript*)script);
		script->OnCollisionExit(other_collider);
	}
}

void ComponentScript::OnTriggerEnter(GameObject * other_collider)
{
	if (script)
	{
		App->script_importer->SetCurrentScript((CSScript*)script);
		script->OnTriggerEnter(other_collider);
	}
}

void ComponentScript::OnTriggerStay(GameObject * other_collider)
{
	if (script)
	{
		App->script_importer->SetCurrentScript((CSScript*)script);
		script->OnTriggerStay(other_collider);
	}
}

void ComponentScript::OnTriggerExit(GameObject * other_collider)
{
	if (script)
	{
		App->script_importer->SetCurrentScript((CSScript*)script);
		script->OnTriggerExit(other_collider);
	}
}

void ComponentScript::OnEnable()
{
	if (script && App->IsPlaying())
	{
		App->script_importer->SetCurrentScript((CSScript*)script);
		script->OnEnable();
	}
}

void ComponentScript::OnDisable()
{
	if (script && App->IsPlaying())
	{
		App->script_importer->SetCurrentScript((CSScript*)script);
		script->OnDisable();
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
		script = new CSScript();
		if (!script->Load(data))
		{
			CONSOLE_ERROR("Cannot find %s. Script not loaded", script_name.c_str());
			RELEASE(script);
		}
		else
		{
			script->SetAttachedGameObject(GetGameObject());
		}
		data.LeaveSection();
	}
}
