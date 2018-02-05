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

	void Save(Data& data) const;
	void Load(Data& data);

private:
	ComponentRectTransform * GetRectTrans();

private:
	CanvasDrawElement* image = nullptr;

};

#endif // !_H_COMPONENT_IMAGE__