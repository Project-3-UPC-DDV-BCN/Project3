#include "ComponentButton.h"
#include "ComponentRectTransform.h"
#include "ComponentCanvas.h"
#include "GameObject.h"
#include "Texture.h"
#include "Application.h"
#include "ModuleResources.h"

ComponentButton::ComponentButton(GameObject * attached_gameobject) : Component()
{
	SetActive(true);
	SetName("Button");
	SetType(ComponentType::CompButton);
	SetGameObject(attached_gameobject);

	c_rect_trans = GetRectTrans();

	c_rect_trans->SetSize(float2(110, 40));

	button_mode = ButtonMode::BM_COLOUR;
	button_state = ButtonState::BS_IDLE;

	idle_texture = nullptr;
	idle_colour = float4(0.098f, 0.392f, 0.701f, 1.0f);

	over_texture = nullptr;
	over_colour = float4(0.078f, 0.129f, 0.180f, 1.0f);

	pressed_texture = nullptr;
	pressed_colour = float4(0.078f, 0.329f, 0.380f, 1.0f);
}

ComponentButton::~ComponentButton()
{
}

bool ComponentButton::Update()
{
	bool ret = true;

	UpdateState();

	ComponentCanvas* canvas = GetCanvas();

	if (canvas != nullptr)
	{
		float4 colour = idle_colour;
		Texture* texture = nullptr;

		switch (button_mode)
		{
		case BM_COLOUR:
		{
			switch (button_state)
			{
			case BS_IDLE:
				colour = idle_colour;
				break;
			case BS_OVER:
				colour = over_colour;
				break;
			case BS_PRESSED:
				colour = pressed_colour;
				break;
			default:
				break;
			}
		}
			break;
		case BM_IMAGE:
		{
			switch (button_state)
			{
			case BS_IDLE:
				texture = idle_texture;
				break;
			case BS_OVER:
				texture = over_texture;
				break;
			case BS_PRESSED:
				texture = pressed_texture;
				break;
			default:
				break;
			}
		}
			break;
		default:
			break;
		}

		CanvasDrawElement de(canvas, this);
		de.SetTransform(c_rect_trans->GetMatrix());
		de.SetOrtoTransform(c_rect_trans->GetOrtoMatrix());
		de.SetSize(c_rect_trans->GetScaledSize());

		de.SetColour(colour);
		de.SetFlip(false, false);

		if (texture != nullptr)
		{
			de.SetTextureId(texture->GetID());
		}

		canvas->AddDrawElement(de);
	}

	return ret;
}

bool ComponentButton::CleanUp()
{
	bool ret = true;

	return ret;
}

void ComponentButton::SetButtonMode(const ButtonMode & mode)
{
	button_mode = mode;
}

ButtonMode ComponentButton::GetButtonMode() const
{
	return button_mode;
}

void ComponentButton::SetIdleTexture(Texture * set)
{
	idle_texture = set;
}

void ComponentButton::SetOverTexture(Texture * set)
{
	over_texture = set;
}

void ComponentButton::SetPressedTexture(Texture * set)
{
	pressed_texture = set;
}

Texture * ComponentButton::GetIdleTexture() const
{
	return idle_texture;
}

Texture * ComponentButton::GetOverTexture() const
{
	return over_texture;
}

Texture * ComponentButton::GetPressedTexture() const
{
	return pressed_texture;
}

void ComponentButton::SetIdleColour(float4 set)
{
	idle_colour = set;
}

void ComponentButton::SetOverColour(float4 set)
{
	over_colour = set;
}

void ComponentButton::SetPressedColour(float4 set)
{
	pressed_colour = set;
}

float4 ComponentButton::GetIdleColour() const
{
	return idle_colour;
}

float4 ComponentButton::GetOverColour() const
{
	return over_colour;
}

float4 ComponentButton::GetPressedColour() const
{
	return pressed_colour;
}

void ComponentButton::Save(Data & data) const
{
	data.AddInt("Type", GetType());
	data.AddBool("Active", IsActive());
	data.AddUInt("UUID", GetUID());
	data.AddInt("mode", GetButtonMode());
	data.AddVector4("idle_colour", GetIdleColour());
	data.AddVector4("over_colour", GetOverColour());
	data.AddVector4("pressed_colour", GetPressedColour());
	if (idle_texture != nullptr)
		data.AddString("idle_texture", idle_texture->GetName().c_str());
	if (over_texture != nullptr)
		data.AddString("over_texture", over_texture->GetName().c_str());
	if (pressed_texture != nullptr)
		data.AddString("pressed_texture", pressed_texture->GetName().c_str());
}

void ComponentButton::Load(Data & data)
{
	SetActive(data.GetBool("Active"));
	SetUID(data.GetUInt("UUID"));
	SetButtonMode(static_cast<ButtonMode>(data.GetInt("mode")));
	SetIdleColour(data.GetVector4("idle_colour"));
	SetOverColour(data.GetVector4("over_colour"));
	SetPressedColour(data.GetVector4("pressed_colour"));
	idle_texture = App->resources->GetTexture(data.GetString("idle_texture"));
	over_texture = App->resources->GetTexture(data.GetString("over_texture"));
	pressed_texture = App->resources->GetTexture(data.GetString("pressed_texture"));
}

ComponentCanvas * ComponentButton::GetCanvas()
{
	ComponentCanvas* ret = nullptr;

	bool go_is_canvas;
	ret = GetRectTrans()->GetCanvas(go_is_canvas);

	return ret;
}

ComponentRectTransform * ComponentButton::GetRectTrans()
{
	ComponentRectTransform* ret = nullptr;

	ret = (ComponentRectTransform*)GetGameObject()->GetComponent(Component::CompRectTransform);

	return ret;
}

void ComponentButton::UpdateState()
{
	if (c_rect_trans->GetOnMouseOver())
	{
		button_state = ButtonState::BS_OVER;

		if (c_rect_trans->GetOnClick())
		{
			button_state = ButtonState::BS_PRESSED;
		}
	}
	else
		button_state = ButtonState::BS_IDLE;
}
