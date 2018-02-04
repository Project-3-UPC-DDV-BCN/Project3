#ifndef _H_COMPONENT_IMAGE__
#define _H_COMPONENT_IMAGE__

#include "Component.h"

class ComponentTransform;
class ComponentCanvas;

class ComponentImage : public Component
{
public:
	ComponentImage(GameObject* attached_gameobject);
	virtual ~ComponentImage();

	bool Update();

	void Save(Data& data) const;
	void Load(Data& data);
};

#endif // !_H_COMPONENT_IMAGE__