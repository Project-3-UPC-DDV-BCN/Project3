#ifndef _H_COMPONENT_IMAGE__
#define _H_COMPONENT_IMAGE__

#include "Component.h"

class ComponentRectTransform;
class ComponentTransform;
class ComponentCanvas;
class CanvasDrawElement;
class Texture;

class ComponentImage : public Component
{
public:
	ComponentImage(GameObject* attached_gameobject);
	virtual ~ComponentImage();

	bool Update();

	void SetTexture(Texture* text);
	Texture* GetTexture() const;

	void SetColour(const float4& colour);
	float4 GetColour() const;

	void SetFlip(const bool& flip);
	bool GetFlip() const;

	void Save(Data& data) const;
	void Load(Data& data);

private:
	ComponentCanvas* GetCanvas();
	ComponentRectTransform * GetRectTrans();

private:
	ComponentRectTransform* c_rect_trans = nullptr;

	Texture* texture;
	float4 colour;
	bool flip;

};

#endif // !_H_COMPONENT_IMAGE__