#include "ComponentScript.h"
#include "CSScript.h"
#include "ModuleResources.h"
#include "Application.h"
#include "ModuleScriptImporter.h"
#include "ModulePhysics.h"
#include "PerfTimer.h"

ComponentScript::ComponentScript(GameObject* attached_gameobject)
{
	SetName("Script");
	SetType(ComponentType::CompScript);
	SetGameObject(attached_gameobject);
	script = nullptr;
	SetActive(true);
	for (int i = 0; i < 11; ++i)
	{
		script_times.push_back(0.0f);
	}
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
		PerfTimer ms_timer; ms_timer.Start();
		script->InitScript();
		script_times[SCRIPT_INIT_TIME] = (double)ms_timer.ReadMs();
		App->script_importer->ns_importer->current_script = last_script;
	}
}

void ComponentScript::StartScript()
{
	if (script)
	{
		CSScript* last_script = App->script_importer->ns_importer->current_script;
		App->script_importer->ns_importer->current_script = (CSScript*)script;
		PerfTimer ms_timer; ms_timer.Start();
		script->StartScript();
		script_times[SCRIPT_START_TIME] = (double)ms_timer.ReadMs();
		App->script_importer->ns_importer->current_script = last_script;
	}
}

void ComponentScript::UpdateScript()
{
	if (script)
	{
		CSScript* last_script = App->script_importer->ns_importer->current_script;
		App->script_importer->ns_importer->current_script = (CSScript*)script;
		PerfTimer ms_timer; ms_timer.Start();
		script->UpdateScript();
		script_times[SCRIPT_UPDATE_TIME] = (double)ms_timer.ReadMs();
		App->script_importer->ns_importer->current_script = last_script;
	}
}

void ComponentScript::OnCollisionEnter(CollisionData& col_data)
{
	if (script)
	{
		CSScript* last_script = App->script_importer->ns_importer->current_script;
		App->script_importer->ns_importer->current_script = (CSScript*)script;
		PerfTimer ms_timer; ms_timer.Start();
		script->OnCollisionEnter(col_data);
		script_times[SCRIPT_ONCOLLISIONENTER_TIME] = (double)ms_timer.ReadMs();
		App->script_importer->ns_importer->current_script = last_script;
	}
}

void ComponentScript::OnCollisionStay(CollisionData& col_data)
{
	if (script)
	{
		CSScript* last_script = App->script_importer->ns_importer->current_script;
		App->script_importer->ns_importer->current_script = (CSScript*)script;
		PerfTimer ms_timer; ms_timer.Start();
		script->OnCollisionStay(col_data);
		script_times[SCRIPT_ONCOLLISIONSTAY_TIME] = (double)ms_timer.ReadMs();
		App->script_importer->ns_importer->current_script = last_script;
	}
}

void ComponentScript::OnCollisionExit(CollisionData& col_data)
{
	if (script)
	{
		CSScript* last_script = App->script_importer->ns_importer->current_script;
		App->script_importer->ns_importer->current_script = (CSScript*)script;
		PerfTimer ms_timer; ms_timer.Start();
		script->OnCollisionExit(col_data);
		script_times[SCRIPT_ONCOLLISIONEXIT_TIME] = (double)ms_timer.ReadMs();
		App->script_importer->ns_importer->current_script = last_script;
	}
}

void ComponentScript::OnTriggerEnter(CollisionData& col_data)
{
	if (script)
	{
		CSScript* last_script = App->script_importer->ns_importer->current_script;
		App->script_importer->ns_importer->current_script = (CSScript*)script;
		PerfTimer ms_timer; ms_timer.Start();
		script->OnTriggerEnter(col_data);
		script_times[SCRIPT_ONTRIGGERENTER_TIME] = (double)ms_timer.ReadMs();
		App->script_importer->ns_importer->current_script = last_script;
	}
}

void ComponentScript::OnTriggerStay(CollisionData& col_data)
{
	if (script)
	{
		CSScript* last_script = App->script_importer->ns_importer->current_script;
		App->script_importer->ns_importer->current_script = (CSScript*)script;
		PerfTimer ms_timer; ms_timer.Start();
		script->OnTriggerStay(col_data);
		script_times[SCRIPT_ONTRIGGERSTAY_TIME] = (double)ms_timer.ReadMs();
		App->script_importer->ns_importer->current_script = last_script;
	}
}

void ComponentScript::OnTriggerExit(CollisionData& col_data)
{
	if (script)
	{
		CSScript* last_script = App->script_importer->ns_importer->current_script;
		App->script_importer->ns_importer->current_script = (CSScript*)script;
		PerfTimer ms_timer; ms_timer.Start();
		script->OnTriggerExit(col_data);
		script_times[SCRIPT_TRIGGEREXIT_TIME] = (double)ms_timer.ReadMs();
		App->script_importer->ns_importer->current_script = last_script;
	}
}

void ComponentScript::OnEnable()
{
	if (script && App->IsPlaying())
	{
		CSScript* last_script = App->script_importer->ns_importer->current_script;
		App->script_importer->ns_importer->current_script = (CSScript*)script;
		PerfTimer ms_timer; ms_timer.Start();
		script->OnEnable();
		script_times[SCRIPT_ONENABLE_TIME] = (double)ms_timer.ReadMs();
		App->script_importer->ns_importer->current_script = last_script;
	}
}

void ComponentScript::OnDisable()
{
	if (script && App->IsPlaying())
	{
		CSScript* last_script = App->script_importer->ns_importer->current_script;
		App->script_importer->ns_importer->current_script = (CSScript*)script;
		PerfTimer ms_timer; ms_timer.Start();
		script->OnDisable();
		script_times[SCRIPT_ONDISABLE_TIME] = (double)ms_timer.ReadMs();
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

std::vector<double> ComponentScript::GetScriptTimes()
{
	return script_times;
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
			script->SetAttachedGameObject(GetGameObject());
		}
		if (!script->Load(data))
		{
			CONSOLE_ERROR("Cannot find %s. Script not loaded", script_name.c_str());
			RELEASE(script);
		}
		data.LeaveSection();
	}
}
