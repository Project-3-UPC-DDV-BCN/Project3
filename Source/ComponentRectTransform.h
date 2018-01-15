#ifndef _H_COMPONENT_RECT_TRANSFORM__
#define _H_COMPONENT_RECT_TRANSFORM__

#include "Component.h"

class ComponentRectTransfrom : public Component
{
public:
	ComponentRectTransfrom(GameObject* attached_gameobject);
	virtual ~ComponentRectTransfrom();

	bool Update();

private:

};

#endif // !_H_COMPONENT_RECT_TRANSFORM__