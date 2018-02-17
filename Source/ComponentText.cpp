#include "ComponentText.h"
#include "GameObject.h"
#include "ComponentCanvas.h"
#include "ComponentRectTransform.h"
#include "Texture.h"
#include "Application.h"
#include "UsefulFunctions.h"

ComponentText::ComponentText(GameObject * attached_gameobject)
{
	SetActive(true);
	SetName("Text");
	SetType(ComponentType::CompText);
	SetGameObject(attached_gameobject);

	texture = 0;
	colour = float4(1.0f, 1.0f, 1.0f, 1.0f);

	c_rect_trans = GetRectTrans();

	c_rect_trans->SetSize(float2(100, 40));

	font = App->font_importer->LoadFont("C:/Users/Guillem/Documents/GitHub/Project3/EngineResources/arial.ttf");
	SetText("This is a fucking test");
}

ComponentText::~ComponentText()
{
}

bool ComponentText::Update()
{
	bool ret = true;

	ComponentCanvas* canvas = GetCanvas();

	if (canvas != nullptr)
	{
		CanvasDrawElement de;
		de.SetTransform(c_rect_trans->GetMatrix());
		de.SetOrtoTransform(c_rect_trans->GetOrtoMatrix());
		de.SetSize(c_rect_trans->GetScaledSize());
		de.SetColour(colour);
		de.SetFlip(true);

		de.SetTextureId(texture);
		
		canvas->AddDrawElement(de);
	}

	return ret;
}

void ComponentText::Save(Data & data) const
{
}

void ComponentText::Load(Data & data)
{
}

void ComponentText::SetText(const char * _text)
{
	if (!TextCmp(_text, text.c_str()))
	{
		App->font_importer->UnloadText(texture);
		text = _text;
		UpdateText();
	}
}

std::string ComponentText::GetText()
{
	return text;
}

ComponentCanvas * ComponentText::GetCanvas()
{
	ComponentCanvas* ret = nullptr;

	bool go_is_canvas;
	ret = GetRectTrans()->GetCanvas(go_is_canvas);

	return ret;
}

ComponentRectTransform * ComponentText::GetRectTrans()
{
	ComponentRectTransform* ret = nullptr;

	ret = (ComponentRectTransform*)GetGameObject()->GetComponent(Component::CompRectTransform);

	return ret;
}

void ComponentText::UpdateText()
{
	texture = App->font_importer->LoadText(text.c_str(), font);
}
