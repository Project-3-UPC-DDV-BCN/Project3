#pragma once
#include "Component.h"

#define SCRIPT_INIT_TIME				0
#define SCRIPT_START_TIME				1
#define SCRIPT_UPDATE_TIME				2
#define SCRIPT_ONCOLLISIONENTER_TIME	3
#define SCRIPT_ONCOLLISIONSTAY_TIME		4
#define SCRIPT_ONCOLLISIONEXIT_TIME		5
#define SCRIPT_ONTRIGGERENTER_TIME		6
#define SCRIPT_ONTRIGGERSTAY_TIME		7
#define SCRIPT_TRIGGEREXIT_TIME			8
#define SCRIPT_ONENABLE_TIME			9
#define SCRIPT_ONDISABLE_TIME			10

class Script;
struct ScriptField;
class ComponentRectTransform;
struct CollisionData;

class ComponentScript :
	public Component
{
public:
	ComponentScript(GameObject* attached_gameobject);
	~ComponentScript();

	void InitScript();
	void StartScript();
	void UpdateScript();
	void OnCollisionEnter(CollisionData& col_data);
	void OnCollisionStay(CollisionData& col_data);
	void OnCollisionExit(CollisionData& col_data);
	void OnTriggerEnter(CollisionData& col_data);
	void OnTriggerStay(CollisionData& col_data);
	void OnTriggerExit(CollisionData& col_data);
	void OnEnable();
	void OnDisable();

	void SetScript(Script* script);
	std::string GetScriptName() const;
	Script* GetScript() const;
	std::vector<ScriptField*> GetScriptFields() const;
	void UpdateScriptFields();

	std::vector<double> GetScriptTimes();

	void Save(Data& data) const;
	void Load(Data& data);

private:
	Script* script;
	std::vector<ScriptField*> script_fields;
	std::vector<double> script_times;
};

