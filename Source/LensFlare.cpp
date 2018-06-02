#include "LensFlare.h"
#include "ComponentCanvas.h"
#include "ComponentCamera.h"
#include "Texture.h"

LensFlare::LensFlare()
{

}

LensFlare::~LensFlare()
{

}

bool LensFlare::SetActiveCamera(ComponentCamera * active_camera)
{
	if (active_camera == nullptr) return false;

	//active_camera->camera_frustum.NearPlane;

	return true;
}
