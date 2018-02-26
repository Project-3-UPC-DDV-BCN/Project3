#include "ComponentDistorsionZone.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "ModuleRenderer3D.h"

#include "GameObject.h"
#include "ComponentTransform.h"

#include "OpenGL.h"

ComponentDistorsionZone::ComponentDistorsionZone(GameObject * attached_gameobject)
{
	SetActive(true);
	SetName("Distorsion Zone");
	SetType(Component::ComponentType::CompAudioDistZone);
	SetGameObject(attached_gameobject);
	zone.SetNegativeInfinity();
	App->audio->AddEnvironment(this);
	
}

ComponentDistorsionZone::~ComponentDistorsionZone()
{
}

bool ComponentDistorsionZone::Update()
{
	ComponentTransform* trans = (ComponentTransform*)GetGameObject()->GetComponent(Component::ComponentType::CompTransform);
	
	if (trans)
	{
		float3 pos = trans->GetGlobalPosition();
		Quat rot = Quat::FromEulerXYZ(trans->GetGlobalRotation().x * DEGTORAD, trans->GetGlobalRotation().y * DEGTORAD, trans->GetGlobalRotation().z * DEGTORAD);
		float3 scale = trans->GetLocalScale();

		zone.pos = pos;
		zone.axis[0] = rot.Transform(float3(1, 0, 0));
		zone.axis[1] = rot.Transform(float3(0, 1, 0));
		zone.axis[2] = rot.Transform(float3(0, 0, 1));
		zone.r = scale;

		DebugDraw();
	}

	return true;
}

void ComponentDistorsionZone::DebugDraw()
{
	float3 corners[8];
	OBB box = zone;
	box.GetCornerPoints(corners);
	const int s = 24;

	float3* lines = new float3[s];
	float3* colors = new float3[s];

	lines[0] = float3(corners[0].x, corners[0].y, corners[0].z);
	lines[1] = float3(corners[2].x, corners[2].y, corners[2].z);

	lines[2] = float3(corners[2].x, corners[2].y, corners[2].z);
	lines[3] = float3(corners[6].x, corners[6].y, corners[6].z);

	lines[4] = float3(corners[4].x, corners[4].y, corners[4].z);
	lines[5] = float3(corners[6].x, corners[6].y, corners[6].z);

	lines[6] = float3(corners[4].x, corners[4].y, corners[4].z);
	lines[7] = float3(corners[0].x, corners[0].y, corners[0].z);

	//

	lines[8] = float3(corners[1].x, corners[1].y, corners[1].z);
	lines[9] = float3(corners[3].x, corners[3].y, corners[3].z);

	lines[10] = float3(corners[3].x, corners[3].y, corners[3].z);
	lines[11] = float3(corners[7].x, corners[7].y, corners[7].z);

	lines[12] = float3(corners[5].x, corners[5].y, corners[5].z);
	lines[13] = float3(corners[7].x, corners[7].y, corners[7].z);

	lines[14] = float3(corners[5].x, corners[5].y, corners[5].z);
	lines[15] = float3(corners[1].x, corners[1].y, corners[1].z);

	//

	lines[16] = float3(corners[0].x, corners[0].y, corners[0].z);
	lines[17] = float3(corners[1].x, corners[1].y, corners[1].z);

	lines[18] = float3(corners[2].x, corners[2].y, corners[2].z);
	lines[19] = float3(corners[3].x, corners[3].y, corners[3].z);

	lines[20] = float3(corners[4].x, corners[4].y, corners[4].z);
	lines[21] = float3(corners[5].x, corners[5].y, corners[5].z);

	lines[22] = float3(corners[6].x, corners[6].y, corners[6].z);
	lines[23] = float3(corners[7].x, corners[7].y, corners[7].z);

	for (int i = 0; i < s; i++)
	{
		colors[i] = float3(60, 60, 1);
	}

	//	DrawLinesList(lines, s, 5, colors);

	glLineWidth((float)5);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, (float*)lines->ptr());

	if (colors != nullptr)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(3, GL_FLOAT, 0, (float*)colors->ptr());
	}

	glDrawArrays(GL_LINES, 0, s);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glLineWidth(1);

	delete[] lines;
	delete[] colors;
}

bool ComponentDistorsionZone::CheckCollision(AABB target)
{
	return zone.Intersects(target);
}

void ComponentDistorsionZone::Save(Data & data) const
{
	data.AddInt("Type", GetType());
	data.AddBool("Active", IsActive());
	data.AddUInt("UUID", GetUID());
	data.AddString("Bus", bus);	
	data.AddFloat("Distorsion Value", distorsion_value);
	data.AddFloat("Radius", radius);
}

void ComponentDistorsionZone::Load(Data & data)
{
	SetType((Component::ComponentType)data.GetInt("Type"));
	SetActive(data.GetBool("Active"));
	SetUID(data.GetUInt("UUID"));
	bus = data.GetString("Bus");
	distorsion_value = data.GetFloat("Distorsion Value");
	radius = data.GetFloat("Radius");
}
