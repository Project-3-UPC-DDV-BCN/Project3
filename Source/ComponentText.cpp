#include "ComponentText.h"
#include "GameObject.h"
#include "ComponentCanvas.h"
#include "ComponentRectTransform.h"
#include "Texture.h"
#include "Application.h"
#include "UsefulFunctions.h"
#include "MathGeoLib\Math\Quat.h"

ComponentText::ComponentText(GameObject * attached_gameobject)
{
	SetActive(true);
	SetName("Text");
	SetType(ComponentType::CompText);
	SetGameObject(attached_gameobject);

	texture = 0;
	colour = float4(1, 1, 1, 1);
	bold = false;
	italic = false;
	underline = false;
	strikethrough = false;

	c_rect_trans = GetRectTrans();

	c_rect_trans->SetSize(float2(100, 40));

	font = App->font_importer->LoadFontInstance("C:/Users/guillemsc1/Documents/GitHub/Project3/EngineResources/arial.ttf");
	SetText("Text Component");

	de_scaler = 0.1f;
}

ComponentText::~ComponentText()
{
}

bool ComponentText::Update()
{
	bool ret = true;

	if (update_text)
	{
		UpdateText();
		update_text = false;
	}

	ComponentCanvas* canvas = GetCanvas();

	float ratio = text_size.x / text_size.y;
	c_rect_trans->SetSize(float2(c_rect_trans->GetSize().y * ratio, c_rect_trans->GetSize().y));

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

bool ComponentText::CleanUp()
{
	bool ret = true;

	App->font_importer->UnloadFont(font);

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
		text = _text;

		update_text = true;
	}
}

std::string ComponentText::GetText()
{
	return text;
}

void ComponentText::SetFontSize(uint size)
{
	if (font != nullptr)
	{
		if (size < 0)
			size = 0;

		if (size > 1000)
			size = 1000;

		font->SetFontSize(size);

		update_text = true;
	}
}

uint ComponentText::GetFontSize() const
{
	return font->GetFontSize();
}

const char * ComponentText::GetFontName()
{
	if (font != nullptr)
	{
		return font->GetFamilyName();
	}

	return "";
}

void ComponentText::SetColour(const float4 & _colour)
{
	if (colour.x != _colour.x || colour.y != _colour.y || colour.w != _colour.w || colour.z != _colour.z)
	{
		colour = _colour;

		if (colour.x > 1)
			colour.x = 1;

		if (colour.x < 0)
			colour.x = 0;

		if (colour.y > 1)
			colour.y = 1;

		if (colour.y < 0)
			colour.y = 0;

		if (colour.w > 1)
			colour.w = 1;

		if (colour.w < 0)
			colour.w = 1;

		if (colour.z > 1)
			colour.z = 0;

		if (colour.z < 0)
			colour.z =1;

		update_text = true;
	}
}

float4 ComponentText::GetColour() const
{
	return colour;
}

void ComponentText::SetStyleBold(const bool& set)
{
	if (set != bold)
	{
		bold = set;

		update_text = true;
	}
}

void ComponentText::SetStyleItalic(const bool& set)
{
	if (set != italic)
	{
		italic = set;

		update_text = true;
	}
}

void ComponentText::SetStyleUnderline(const bool& set)
{
	if (set != underline)
	{
		underline = set;

		update_text = true;
	}
}

void ComponentText::SetStyelStrikethrough(const bool& set)
{
	if (set != strikethrough)
	{
		strikethrough = set;

		update_text = true;
	}
}

bool ComponentText::GetStyleBold() const
{
	return bold;
}

bool ComponentText::GetStyleItalic() const
{
	return italic;
}

bool ComponentText::GetStyleUnderline() const
{
	return underline;
}

bool ComponentText::GetStyelStrikethrough() const
{
	return strikethrough;
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
	if(texture != 0)
		App->font_importer->UnloadText(texture);

	float4 colour255 = float4(colour.x * 255, colour.y * 255 , colour.z * 255 , colour.w * 255 );

	text_size = App->font_importer->CalcTextSize(text.c_str(), font, bold, italic, underline, strikethrough);

	texture = App->font_importer->LoadText(text.c_str(), font, colour255, bold, italic, underline, strikethrough);
}
