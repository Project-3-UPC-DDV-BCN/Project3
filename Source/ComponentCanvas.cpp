#include "ComponentCanvas.h"
#include "GameObject.h"

ComponentCanvas::ComponentCanvas(GameObject * attached_gameobject)
{
	SetActive(true);
	SetName("Canvas");
	SetType(ComponentType::CompCanvas);
	SetGameObject(attached_gameobject);
}

ComponentCanvas::~ComponentCanvas()
{
}

bool ComponentCanvas::Update()
{
	bool ret = true;

	return ret;
}

std::vector<GameObject*> ComponentCanvas::GetUIGOChilds()
{
	std::vector<GameObject*> ret;

	std::vector<GameObject*> to_check;
	to_check.push_back(GetGameObject());

	// Iterate through all childs and get UI childs
	while (!to_check.empty())
	{
		std::vector<GameObject*>::iterator check = to_check.begin();

		for (std::list<GameObject*>::iterator it = (*check)->childs.begin(); it != (*check)->childs.end(); ++it)
		{
			if ((*it)->GetIsUI())
			{
				ret.push_back(*it);
			}

			to_check.push_back(*it);
		}

		to_check.erase(check);
	}

	return ret;
}
