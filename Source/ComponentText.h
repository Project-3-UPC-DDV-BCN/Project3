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

	void SetColour(const float4& colour);
	float4 GetColour() const;

	void SetStyleBold(const bool& set);
	void SetStyleItalic(const bool& set);
	void SetStyleUnderline(const bool& set);
	void SetStyelStrikethrough(const bool& set);

	bool GetStyleBold() const;
	bool GetStyleItalic() const;
	bool GetStyleUnderline() const;
	bool GetStyelStrikethrough() const;

private:
	ComponentCanvas * GetCanvas();
	ComponentRectTransform * GetRectTrans();
	void UpdateText();

private:
	ComponentRectTransform * c_rect_trans = nullptr;

	Font* font;
	uint texture;

	std::string text;
	float4 colour;
	bool bold;
	bool italic;
	bool underline;
	bool strikethrough;

};

#endif // !_H_COMPONENT_TEXT__