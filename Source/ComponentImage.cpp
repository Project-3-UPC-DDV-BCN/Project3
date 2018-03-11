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
	curr_animation_image = 0;
	loop = true;

	animation_play = true;
	animation_preview_play = false;

	c_rect_trans->SetSize(float2(100, 100));

	was_in_play_mode = true;
}

ComponentImage::~ComponentImage()
{
}

bool ComponentImage::Update()
{
	bool ret = true;

	if (App->IsPlaying() && !was_in_play_mode)
	{
		was_in_play_mode = true;
		animation_timer.Start();
		curr_animation_image = 0;
	}

	Texture* tex = nullptr;

	switch (mode)
	{
		case ImageMode::IM_SINGLE:
		{
			tex = texture;
		}
		break;

		case ImageMode::IM_ANIMATION:
		{
			if ((App->IsPlaying() && animation_play) || (!App->IsPlaying() && animation_preview_play))
			{
				if (animation_timer.Read() > (animation_speed * 1000))
				{
					animation_timer.Start();
					if (curr_animation_image < anim_textures.size() - 1)
					{
						curr_animation_image++;
					}
					else if(loop)
					{
						curr_animation_image = 0;
					}
				}
			}

			if (curr_animation_image < anim_textures.size())
			{
				tex = anim_textures[curr_animation_image];
			}
		}
		break;
	}

	ComponentCanvas* canvas = GetCanvas();

	if (canvas != nullptr)
	{
		CanvasDrawElement de(canvas, this);
		de.SetTransform(c_rect_trans->GetMatrix());
		de.SetOrtoTransform(c_rect_trans->GetOrtoMatrix());
		de.SetSize(c_rect_trans->GetScaledSize());
		de.SetColour(colour);
		de.SetFlip(false, flip);

		if(tex != nullptr)
		{
			de.SetTextureId(tex->GetID());
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
	if (set && !animation_play)
	{
		animation_timer.Start();
	}
	else if (!set && animation_play)
		curr_animation_image = 0;

	animation_play = set;
}

bool ComponentImage::GetAnimationPlay() const
{
	return animation_play;
}

void ComponentImage::SetAnimationPreviewPlay(bool set)
{
	if (set && !animation_preview_play)
	{
		animation_timer.Start();
	}
	else if(!set && animation_preview_play)
		curr_animation_image = 0;

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

void ComponentImage::SetLoop(bool _loop)
{
	loop = _loop;
}

bool ComponentImage::GetLoop() const
{
	return loop;
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
	data.AddInt("mode", mode);
	data.AddVector4("colour", colour);
	data.AddBool("flip", flip);
	data.AddBool("loop", loop);
	data.AddFloat("anim_speed", animation_speed);
	data.AddInt("anim_textures_count", anim_textures.size());
	if (texture != nullptr)
		data.AddString("texture", texture->GetName().c_str());
	for (int i = 0; i < anim_textures.size(); ++i)
	{
		if (anim_textures[i] != nullptr)
		{
			std::string text_name = "anim_texture_" + std::to_string(i);
			data.AddString(text_name.c_str(), anim_textures[i]->GetName().c_str());
		}
	}
	
}

void ComponentImage::Load(Data & data)
{
	SetActive(data.GetBool("Active"));
	SetUID(data.GetUInt("UUID"));
	SetMode(static_cast<ImageMode>(data.GetInt("mode")));
	SetColour(data.GetVector4("colour"));
	SetFlip(data.GetBool("flip"));
	SetLoop(data.GetBool("loop"));
	SetAnimSpeed(data.GetFloat("anim_speed"));
	std::string text_name = data.GetString("texture");
	Texture* t = App->resources->GetTexture(text_name);
	SetTexture(t);

	int textures_count = data.GetInt("anim_textures_count");
	SetNumAnimTextures(textures_count);
	for (int i = 0; i < textures_count; ++i)
	{
		std::string text_name = "anim_texture_" + std::to_string(i);
		Texture* anim_t = App->resources->GetTexture(data.GetString(text_name.c_str()));
		AddAnimTexture(anim_t, i);
	}
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
