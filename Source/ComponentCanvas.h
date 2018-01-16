#ifndef _H_COMPONENT_CANVAS__
#define _H_COMPONENT_CANVAS__

#include "Component.h"

class ComponentCanvas : public Component
{
public:
	ComponentCanvas(GameObject* attached_gameobject);
	virtual ~ComponentCanvas();

	bool Update();

	// Returns all GameObjects that are child of the canvas that are UI.
	std::vector<GameObject*> GetUIGOChilds();

private:

};

#endif // !_H_COMPONENT_CANVAS__