#ifndef _H_COMPONENT_IMAGE__
#define _H_COMPONENT_IMAGE__

#include "Component.h"
#include <vector>

class ComponentRectTransform;
class ComponentTransform;
class ComponentCanvas;
class CanvasDrawElement;
class Texture;
class Timer;

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

	// Mode single
	void SetTexture(Texture* text);
	Texture* GetTexture() const;

	void SetColour(const float4& colour);
	float4 GetColour() const;

	void SetNativeSize();

	// Mode animation
	void SetAnimSpeed(float speed);
	float GetAnimSpeed() const;

	void SetAnimationPlay(bool set);
	bool GetAnimationPlay() const;

	void SetAnimationPreviewPlay(bool set);
	bool GetAnimationPreviewPlay() const;

	void AddAnimTexture(Texture* texture, int index);
	void ClearAnimTextures();

	void SetNumAnimTextures(uint set);
	uint GetNumAnimTextures() const;

	std::vector<Texture*> GetAnimTextures();

	void SetFlip(const bool& flip);
	bool GetFlip() const;

	void Save(Data& data) const;
	void Load(Data& data);

private:
	ComponentCanvas* GetCanvas();
	ComponentRectTransform * GetRectTrans();

private:
	ComponentRectTransform* c_rect_trans = nullptr;

	ImageMode mode;

	// Mode single
	Texture* texture;
	float4 colour;
	bool flip;

	// Mode animation
	uint num_anim_textures;
	std::vector<Texture*> anim_textures;
	float animation_speed;
	bool animation_play;
	bool animation_preview_play;
	//Timer animation_timer;
};

#endif // !_H_COMPONENT_IMAGE__