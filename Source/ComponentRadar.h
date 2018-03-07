#ifndef _H_COMPONENT_RADAR__
#define _H_COMPONENT_RADAR__

#include "Component.h"
#include <vector>
#include "Timer.h"

class ComponentRectTransform;
class ComponentTransform;
class ComponentCanvas;
class CanvasDrawElement;
class Texture;
class Timer;

class ComponentRadar: public Component
{
public:
	ComponentRadar(GameObject* attached_gameobject);
	virtual ~ComponentRadar();

	bool Update();

	void Save(Data& data) const;
	void Load(Data& data);

private:
	ComponentCanvas * GetCanvas();
	ComponentRectTransform * GetRectTrans();

private:
	ComponentRectTransform * c_rect_trans = nullptr;
};

#endif // !_H_COMPONENT_RADAR__