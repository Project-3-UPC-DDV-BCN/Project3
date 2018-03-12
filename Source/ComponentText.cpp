#include "ComponentText.h"
#include "GameObject.h"
#include "ComponentCanvas.h"
#include "ComponentRectTransform.h"
#include "Texture.h"
#include "Application.h"
#include "UsefulFunctions.h"
#include "MathGeoLib\Quat.h"
#include "ModuleResources.h"

ComponentText::ComponentText(GameObject * attached_gameobject)
{
	SetActive(true);
	SetName("Text");
	SetType(ComponentType::CompText);
	SetGameObject(attached_gameobject);

	c_rect_trans = GetRectTrans();

	c_rect_trans->SetSize(float2(100, 40));

	font = nullptr;
	texture = 0;
	colour = float4(1, 1, 1, 1);
	bold = false;
	italic = false;
	underline = false;
	strikethrough = false;
	font_size = 24.0f;
	text_size = float2::zero;

	grow_dir = TextGrow::TG_CENTER;
	text_scale_diff = 0;

	c_rect_trans->SetFixedAspectRatio(true);

	SetFont(App->font_importer->GetDefaultFont());

	SetText("Text");
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

	if (canvas != nullptr)
	{
		CanvasDrawElement de(canvas, this);
		de.SetTransform(c_rect_trans->GetMatrix());
		de.SetOrtoTransform(c_rect_trans->GetOrtoMatrix());
		de.SetSize(c_rect_trans->GetScaledSize());
		de.SetColour(colour);
		de.SetFlip(true, false);

		de.SetTextureId(texture);
		
		canvas->AddDrawElement(de);
	}

	return ret;
}

bool ComponentText::CleanUp()
{
	bool ret = true;

	App->font_importer->UnloadFontInstance(font);

	return ret;
}

void ComponentText::Save(Data & data) const
{
	data.AddInt("Type", GetType());
	data.AddBool("Active", IsActive());
	data.AddUInt("UUID", GetUID());
	data.AddVector4("colour", colour);
	data.AddBool("bold", bold);
	data.AddBool("italic", italic);
	data.AddBool("underline", underline);
	data.AddBool("strikethrough", strikethrough);
	data.AddString("text", text);
	data.AddInt("font_size", font_size);
	data.AddInt("grow_dir", grow_dir);
	if (font != nullptr)
		data.AddString("font", font->GetName());
}

void ComponentText::Load(Data & data)
{
	SetActive(data.GetBool("Active"));
	SetUID(data.GetUInt("UUID"));
	SetColour(data.GetVector4("colour"));
	SetStyleBold(data.GetBool("bold"));
	SetStyleItalic(data.GetBool("italic"));
	SetStyleUnderline(data.GetBool("underline"));
	SetStyelStrikethrough(data.GetBool("strikethrough"));
	SetFontSize(data.GetInt("font_size"));
	SetGrowDirection(static_cast<TextGrow>(data.GetInt("grow_dir")));
	std::string font_name = data.GetString("font");
	SetFont(App->resources->GetFont(font_name));

	if (font != nullptr)
		SetText(data.GetString("text").c_str());
}

void ComponentText::SetFont(Font * _font)
{
	if (font != _font && _font != nullptr)
	{
		App->font_importer->UnloadFontInstance(font);
		font = App->font_importer->CreateFontInstance(_font);
		UpdateText(true);
	}
}

Font * ComponentText::GetFont()
{
	return font;
}

bool ComponentText::HasFont() const
{
	return font != nullptr;
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
	if (size < 0)
		size = 0;

	if (size > 1000)
		size = 1000;

	font_size = size;

	if (font != nullptr)
	{
		font->SetFontSize(size);

		UpdateText(true);
	}
}

uint ComponentText::GetFontSize() const
{
	return font_size;
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

void ComponentText::SetGrowDirection(TextGrow grow)
{
	grow_dir = grow;
}

TextGrow ComponentText::GetGrowDirection() const
{
	return grow_dir;
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

void ComponentText::UpdateText(bool font_update)
{
	if (font != nullptr)
	{
		std::string t = text;

		if (TextCmp("", text.c_str()))
			t = " ";

		float2 last_text_size = text_size;

		if (last_text_size.x > 0)
			text_scale_diff = c_rect_trans->GetSize().x / last_text_size.x;

		if (font_size != font->GetFontSize())
			font->SetFontSize(font_size);
		
		if (texture != 0)
			App->font_importer->UnloadText(texture);

		float4 colour255 = float4(colour.x * 255, colour.y * 255, colour.z * 255, colour.w * 255);

		text_size = App->font_importer->CalcTextSize(t.c_str(), font, bold, italic, underline, strikethrough);

		texture = App->font_importer->LoadText(t.c_str(), font, colour255, bold, italic, underline, strikethrough);

		c_rect_trans->SetAspectRatio(text_size.x / text_size.y);

		c_rect_trans->SetSize(float2(c_rect_trans->GetSize().x, c_rect_trans->GetSize().y));

		if (!font_update)
		{
			// Move text
			float2 movement = text_size - last_text_size;

			if (text_size.x > 0)
				text_scale_diff = c_rect_trans->GetScaledSize().x / text_size.x;

			movement.x *= text_scale_diff;
			movement.x /= c_rect_trans->GetScale();

			switch (grow_dir)
			{
			case TextGrow::TG_LEFT:
			{
				c_rect_trans->AddPos(float2((movement.x / 2), 0));
			}
			break;
			case TextGrow::TG_RIGHT:
			{
				c_rect_trans->AddPos(float2(-(movement.x / 2), 0));
			}
			break;
			case TextGrow::TG_CENTER:
			{

			}
			break;
			}
		}
	}
}
