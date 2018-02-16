#ifndef _H_COMPONENT_TEXT__
#define _H_COMPONENT_TEXT__

#include "Component.h"
#include "ModuleFontImporter.h"
#include "Font.h"

class ComponentRectTransform;
class ComponentTransform;
class ComponentCanvas;
class CanvasDrawElement;
class Texture;

class ComponentText : public Component
{
public:
	ComponentText(GameObject* attached_gameobject);
	virtual ~ComponentText();

	bool Update();

	void Save(Data& data) const;
	void Load(Data& data);

private:
	ComponentCanvas * GetCanvas();
	ComponentRectTransform * GetRectTrans();

private:
	ComponentRectTransform * c_rect_trans = nullptr;

	Font* font;
	float4 colour;
	bool flip;
	uint texture;

};

#endif // !_H_COMPONENT_TEXT__