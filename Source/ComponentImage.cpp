#include "ComponentImage.h"
#include "GameObject.h"
#include "ComponentCanvas.h"
#include "ComponentRectTransform.h"

ComponentImage::ComponentImage(GameObject * attached_gameobject)
{
	SetActive(true);
	SetName("Image");
	SetType(ComponentType::CompImage);
	SetGameObject(attached_gameobject);
	
	texture_id = 0;
	colour = float4(1.0f, 1.0f, 1.0f, 1.0f);

	c_rect_trans = GetRectTrans();

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
		CanvasDrawElement de;
		de.SetTransform(c_rect_trans->GetMatrix());
		de.SetSize(c_rect_trans->GetSize());
		de.SetTextureId(texture_id);
		de.SetColour(colour);
		canvas->AddDrawElement(de);
	}

	return ret;
}

void ComponentImage::SetTextureId(const uint &_texture_id)
{
	texture_id = _texture_id;
}

void ComponentImage::SetColour(const float4 & _colour)
{
	colour = _colour;
}

float4 ComponentImage::GetColour() const
{
	return colour;
}

void ComponentImage::Save(Data & data) const
{
}

void ComponentImage::Load(Data & data)
{
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
