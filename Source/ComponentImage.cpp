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

	GetRectTrans()->SetSize(float2(100, 100));
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
		de.SetTransform(GetRectTrans()->GetMatrix());
		de.SetTextureId(texture_id);
		canvas->AddDrawElement(de);
	}

	return ret;
}

void ComponentImage::SetTextureId(uint _texture_id)
{
	texture_id = _texture_id;
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
