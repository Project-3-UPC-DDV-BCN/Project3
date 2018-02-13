#ifndef _H_COMPONENT_IMAGE__
#define _H_COMPONENT_IMAGE__

#include "Component.h"

class ComponentRectTransform;
class ComponentTransform;
class ComponentCanvas;
class CanvasDrawElement;

class ComponentImage : public Component
{
public:
	ComponentImage(GameObject* attached_gameobject);
	virtual ~ComponentImage();

	bool Update();

	void SetTextureId(const uint& texture_id);
	void SetColour(const float4& colour);
	float4 GetColour() const;

	void Save(Data& data) const;
	void Load(Data& data);

private:
	ComponentCanvas* GetCanvas();
	ComponentRectTransform * GetRectTrans();

private:
	ComponentRectTransform* c_rect_trans = nullptr;

	uint texture_id;
	float4 colour;

};

#endif // !_H_COMPONENT_IMAGE__