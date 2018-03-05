#ifndef _H_COMPONENT_UI__
#define _H_COMPONENT_UI__

#include "Component.h"

class ComponentCanvas;
class ComponentRectTransform;

class ComponentUI : public Component
{
public:
	ComponentUI(GameObject* attached_gameobject);
	virtual ~ComponentUI();

private:
	ComponentCanvas * GetCanvas();
	ComponentRectTransform * GetRectTrans();
	void UpdateText();

private:
	ComponentRectTransform * c_rect_trans = nullptr;
	bool update_text = false;
};

#endif // !_H_COMPONENT_UI__