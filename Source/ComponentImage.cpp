#include "ComponentImage.h"
#include "GameObject.h"
#include "ComponentCanvas.h"
#include "ComponentRectTransform.h"
#include "Texture.h"
#include "Application.h"
#include "ModuleResources.h"

ComponentImage::ComponentImage(GameObject * attached_gameobject)
{
	SetActive(true);
	SetName("Image");
	SetType(ComponentType::CompImage);
	SetGameObject(attached_gameobject);
	
	c_rect_trans = GetRectTrans();

	mode = ImageMode::IM_SINGLE;

	texture = nullptr;
	colour = float4(1.0f, 1.0f, 1.0f, 1.0f);
	flip = false;

	animation_speed = 1.0f;
	SetNumAnimTextures(1);

	animation_play = true;
	animation_preview_play = false;

	c_rect_trans->SetSize(float2(100, 100));
}

ComponentImage::~ComponentImage()
{
}

bool ComponentImage::Update()
{
	bool ret = true;

	ComponentCanvas* canvas = GetCanvas();

	if (canvas != nullptr)
	{
		CanvasDrawElement de(canvas, this);
		de.SetTransform(c_rect_trans->GetMatrix());
		de.SetOrtoTransform(c_rect_trans->GetOrtoMatrix());
		de.SetSize(c_rect_trans->GetScaledSize());
		de.SetColour(colour);
		de.SetFlip(false, flip);

		if(texture != nullptr)
		{
			de.SetTextureId(texture->GetID());
		}

		canvas->AddDrawElement(de);
	}

	return ret;
}

void ComponentImage::SetMode(ImageMode _mode)
{
	mode = _mode;
}

ImageMode ComponentImage::GetMode() const
{
	return mode;
}

bool ComponentImage::HasImage()
{
	return texture != nullptr;
}

void ComponentImage::SetTexture(Texture* _texture)
{
	texture = _texture;
}

Texture* ComponentImage::GetTexture() const
{
	return texture;
}

void ComponentImage::SetColour(const float4 & _colour)
{
	colour = _colour;
}

float4 ComponentImage::GetColour() const
{
	return colour;
}

void ComponentImage::SetNativeSize()
{
	if (HasImage())
	{
		uint size_w, size_h = 0;
		texture->GetSize(size_w, size_h);
		c_rect_trans->SetSize(float2(size_w, size_h));
	}
}

void ComponentImage::SetFlip(const bool & _flip)
{
	flip = _flip;
}

bool ComponentImage::GetFlip() const
{
	return flip;
}

void ComponentImage::SetAnimSpeed(float _speed)
{
	animation_speed = _speed;

	if (animation_speed < 0)
		animation_speed = 0.0f;
}

float ComponentImage::GetAnimSpeed() const
{
	return animation_speed;
}

void ComponentImage::SetAnimationPlay(bool set)
{
	animation_play = set;
}

bool ComponentImage::GetAnimationPlay() const
{
	return animation_play;
}

void ComponentImage::SetAnimationPreviewPlay(bool set)
{
	animation_preview_play = set;
}

bool ComponentImage::GetAnimationPreviewPlay() const
{
	return animation_preview_play;
}

void ComponentImage::AddAnimTexture(Texture * texture, int index)
{
	for (int i = 0; i < anim_textures.size(); ++i)
	{
		if (i == index)
		{
			anim_textures[i] = texture;
		}
	}
}

void ComponentImage::ClearAnimTextures()
{
	anim_textures.clear();
}

void ComponentImage::SetNumAnimTextures(uint set)
{
	if (set != num_anim_textures)
	{
		num_anim_textures = set;

		if (num_anim_textures < 0)
			num_anim_textures = 0;

		anim_textures.clear();

		for (int i = 0; i < set && i < 100; ++i)
		{
			anim_textures.push_back(nullptr);
		}
	}
}

uint ComponentImage::GetNumAnimTextures() const
{
	return num_anim_textures;
}

std::vector<Texture*> ComponentImage::GetAnimTextures()
{
	return anim_textures;
}

void ComponentImage::Save(Data & data) const
{
	data.AddInt("Type", GetType());
	data.AddBool("Active", IsActive());
	data.AddUInt("UUID", GetUID());
	data.AddVector4("colour", colour);
	data.AddBool("flip", flip);
	if (texture != nullptr)
		data.AddString("texture_name", texture->GetName().c_str());
	
}

void ComponentImage::Load(Data & data)
{
	SetActive(data.GetBool("Active"));
	SetUID(data.GetUInt("UUID"));
	SetColour(data.GetVector4("colour"));
	SetFlip(data.GetBool("flip"));
	texture = App->resources->GetTexture(data.GetString("texture_name"));
}

ComponentCanvas * ComponentImage::GetCanvas()
{
	ComponentCanvas* ret = nullptr;

	bool go_is_canvas;
	ret = GetRectTrans()->GetCanvas(go_is_canvas);

	return ret;
}

ComponentRectTransform * ComponentImage::GetRectTrans()
{
	ComponentRectTransform* ret = nullptr;

	ret = (ComponentRectTransform*)GetGameObject()->GetComponent(Component::CompRectTransform);

	return ret;
}
