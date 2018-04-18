#include "ComponentProgressBar.h"
#include "GameObject.h"
#include "ComponentCanvas.h"
#include "ComponentRectTransform.h"
#include "Texture.h"
#include "Application.h"
#include "ModuleResources.h"

ComponentProgressBar::ComponentProgressBar(GameObject * attached_gameobject)
{
	SetActive(true);
	SetName("ProgressBar");
	SetType(ComponentType::CompProgressBar);
	SetGameObject(attached_gameobject);

	c_rect_trans = GetRectTrans();

	c_rect_trans->SetSize(float2(100, 30));

	progress_percentage = 50.0f;

	base_colour = float4(1.0f, 1.0f, 1.0f, 1.0f);
	progress_colour = float4(0.5f, 0.5f, 0.5f, 1.0f);
}

ComponentProgressBar::~ComponentProgressBar()
{
}

bool ComponentProgressBar::Update()
{
	BROFILER_CATEGORY("Component - ProgressBar - Update", Profiler::Color::Beige);

	bool ret = true;

	ComponentCanvas* canvas = GetCanvas();

	if (canvas != nullptr)
	{
		CanvasDrawElement base(canvas, this);
		base.SetTransform(c_rect_trans->GetMatrix());
		base.SetOrtoTransform(c_rect_trans->GetOrtoMatrix());
		base.SetSize(c_rect_trans->GetScaledSize());
		base.SetColour(base_colour);

		canvas->AddDrawElement(base);


		CanvasDrawElement progres(canvas, this);
		
		float2 pos = float2::zero;
		pos.x = -(c_rect_trans->GetScaledSize().x / 2);
		pos.x += (GetProgesSize() / 2);

		progres.SetTransform(c_rect_trans->GetMatrix());
		progres.SetOrtoTransform(c_rect_trans->GetOrtoMatrix());
		progres.SetPosition(pos);
		progres.SetSize(float2(GetProgesSize(), c_rect_trans->GetScaledSize().y));
		progres.SetColour(progress_colour);

		if(progress_percentage != 0.0f)
			canvas->AddDrawElement(progres);
	}

	return ret;
}

void ComponentProgressBar::SetBaseColour(const float4 & colour)
{
	base_colour = colour;
}

float4 ComponentProgressBar::GetBaseColour() const
{
	return base_colour;
}

void ComponentProgressBar::SetProgressColour(const float4 & colour)
{
	progress_colour = colour;
}

float4 ComponentProgressBar::GetProgressColour() const
{
	return progress_colour;
}

void ComponentProgressBar::SetProgressPercentage(float progres)
{
	progress_percentage = progres;

	if (progress_percentage < 0)
		progress_percentage = 0.0f;

	if (progress_percentage > 100)
		progress_percentage = 100.0f;
}

float ComponentProgressBar::GetProgressPercentage() const
{
	return progress_percentage;
}

void ComponentProgressBar::Save(Data & data) const
{
	data.AddInt("Type", GetType());
	data.AddBool("Active", IsActive());
	data.AddUInt("UUID", GetUID());
	data.AddFloat("progress_percentage", progress_percentage);
	data.AddVector4("base_colour", base_colour);
	data.AddVector4("progress_colour", progress_colour);
}

void ComponentProgressBar::Load(Data & data)
{
	SetActive(data.GetBool("Active"));
	SetUID(data.GetUInt("UUID"));
	SetProgressPercentage(data.GetFloat("progress_percentage"));
	SetBaseColour(data.GetVector4("base_colour"));
	SetProgressColour(data.GetVector4("progress_colour"));
}

ComponentCanvas * ComponentProgressBar::GetCanvas()
{
	ComponentCanvas* ret = nullptr;

	bool go_is_canvas;
	ret = GetRectTrans()->GetCanvas(go_is_canvas);

	return ret;
}

ComponentRectTransform * ComponentProgressBar::GetRectTrans()
{
	ComponentRectTransform* ret = nullptr;

	ret = (ComponentRectTransform*)GetGameObject()->GetComponent(Component::CompRectTransform);

	return ret;
}

float ComponentProgressBar::GetProgesSize()
{
	float ret = 0.0f;

	float2 scaled_size = c_rect_trans->GetScaledSize();

	ret = (scaled_size.x / 100) * progress_percentage;

	return ret;
}
