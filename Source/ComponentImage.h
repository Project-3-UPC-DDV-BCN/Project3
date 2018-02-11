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

	void SetTextureId(uint texture_id);

	void Save(Data& data) const;
	void Load(Data& data);

private:
	ComponentCanvas* GetCanvas();
	ComponentRectTransform * GetRectTrans();

private:
	ComponentRectTransform* c_rect_trans = nullptr;

	uint texture_id = 0;

};

#endif // !_H_COMPONENT_IMAGE__