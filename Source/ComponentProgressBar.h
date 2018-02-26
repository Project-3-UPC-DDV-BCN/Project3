#ifndef _H_COMPONENT_PROGRESS_BAR__
#define _H_COMPONENT_PROGRESS_BAR__

#include "Component.h"

class ComponentRectTransform;
class ComponentTransform;
class ComponentCanvas;
class CanvasDrawElement;
class Texture;

class ComponentProgressBar : public Component
{
public:
	ComponentProgressBar(GameObject* attached_gameobject);
	virtual ~ComponentProgressBar();

	bool Update();

	//void SetTexture(Texture* text);
	//Texture* GetTexture() const;

	void SetBaseColour(const float4& colour);
	float4 GetBaseColour() const;

	void SetProgressColour(const float4& colour);
	float4 GetProgressColour() const;


	void SetProgressPercentage(float progres);
	float GetProgressPercentage() const;

	void Save(Data& data) const;
	void Load(Data& data);

private:
	ComponentCanvas* GetCanvas();
	ComponentRectTransform * GetRectTrans();

	float GetProgesSize();

private:
	ComponentRectTransform* c_rect_trans = nullptr;
	
	float progress_percentage;

	float4 base_colour;
	float4 progress_colour;
};

#endif // !_H_COMPONENT_PROGRESS_BAR__