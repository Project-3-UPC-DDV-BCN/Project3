#ifndef __ANIMATIONCLIP_H__
#define __ANIMATIONCLIP_H__

#include "Resource.h"

class Data;

class AnimationClip : public Resource
{
	AnimationClip();

	void Save(Data& data) const = 0;
	bool Load(Data& data) = 0;
	void CreateMeta() const = 0;
	void LoadToMemory() = 0;
	void UnloadFromMemory() = 0;

private:

};

#endif
