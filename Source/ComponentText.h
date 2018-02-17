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

	void SetText(const char* text);
	std::string GetText();

private:
	ComponentCanvas * GetCanvas();
	ComponentRectTransform * GetRectTrans();
	void UpdateText();

private:
	ComponentRectTransform * c_rect_trans = nullptr;

	std::string text;

	Font* font;
	float4 colour;
	bool bold;
	uint texture;

};

#endif // !_H_COMPONENT_TEXT__