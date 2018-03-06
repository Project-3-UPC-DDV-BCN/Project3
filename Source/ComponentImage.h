#ifndef _H_COMPONENT_IMAGE__
#define _H_COMPONENT_IMAGE__

#include "Component.h"
#include <vector>

class ComponentRectTransform;
class ComponentTransform;
class ComponentCanvas;
class CanvasDrawElement;
class Texture;

enum ImageMode
{
	IM_SINGLE,
	IM_ANIMATION,
};

class ComponentImage : public Component
{
public:
	ComponentImage(GameObject* attached_gameobject);
	virtual ~ComponentImage();

	bool Update();

	void SetMode(ImageMode mode);
	ImageMode GetMode() const;

	bool HasImage();

	void SetTexture(Texture* text);
	Texture* GetTexture() const;

	void SetColour(const float4& colour);
	float4 GetColour() const;

	void SetNativeSize();

	void SetFlip(const bool& flip);
	bool GetFlip() const;

	void SetAnimSpeed(float speed);
	float GetAnimSpeed() const;

	void AddAnimTexture(Texture* texture);
	void ClearAnimTextures();

	void Save(Data& data) const;
	void Load(Data& data);

private:
	ComponentCanvas* GetCanvas();
	ComponentRectTransform * GetRectTrans();

private:
	ComponentRectTransform* c_rect_trans = nullptr;

	ImageMode mode;

	Texture* texture;
	float4 colour;
	bool flip;

	std::vector<Texture*> anim_textures;
	float speed;

};

#endif // !_H_COMPONENT_IMAGE__