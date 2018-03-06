#ifndef _H_COMPONENT_BUTON__
#define _H_COMPONENT_BUTON__

#include "Component.h"

class ComponentRectTransform;
class ComponentCanvas;
class CanvasDrawElement;
class Texture;

enum ButtonMode
{
	BM_COLOUR,
	BM_IMAGE,
};

enum ButtonState
{
	BS_IDLE,
	BS_OVER,
	BS_PRESSED,
};

class ComponentButton : public Component
{
public:
	ComponentButton(GameObject* attached_gameobject);
	virtual ~ComponentButton();

	bool Update();
	bool CleanUp();

	void SetButtonMode(const ButtonMode& mode);
	ButtonMode GetButtonMode() const;

	void SetIdleTexture(Texture* set);
	void SetOverTexture(Texture* set);
	void SetPressedTexture(Texture* set);

	Texture* GetIdleTexture() const;
	Texture* GetOverTexture() const;
	Texture* GetPressedTexture() const;

	void SetIdleColour(float4 set);
	void SetOverColour(float4 set);
	void SetPressedColour(float4 set);

	float4 GetIdleColour() const;
	float4 GetOverColour() const;
	float4 GetPressedColour() const;

	void Save(Data& data) const;
	void Load(Data& data);

private:
	ComponentCanvas * GetCanvas();
	ComponentRectTransform * GetRectTrans();
	void UpdateState();

private:
	ComponentRectTransform * c_rect_trans = nullptr;

	ButtonMode  button_mode;
	ButtonState button_state;

	Texture*    idle_texture;
	float4      idle_colour;
			    
	Texture*    over_texture;
	float4      over_colour;
			    
	Texture*    pressed_texture;
	float4      pressed_colour;
};

#endif // !_H_COMPONENT_BUTON__