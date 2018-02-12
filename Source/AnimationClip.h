#ifndef __ANIMATIONCLIP_H__
#define __ANIMATIONCLIP_H__

#include "Resource.h"
#include "Skeleton.h"
#include <map>
#include <vector>

class Data;

class AnimationSample
{
public:
	AnimationSample();
	~AnimationSample();

private:
	JointPose joint_info;
	double frame = -1;

};

class AnimationClip : public Resource
{
public:
	AnimationClip();
	~AnimationClip();

	void Save(Data& data) const;
	bool Load(Data& data);
	void CreateMeta() const;
	void LoadToMemory();
	void UnloadFromMemory();

private:
	Skeleton * skeleton = nullptr;
	double duration = 0.0;
	double ticks_per_sec = 0.0;
	std::map<int, std::vector<AnimationSample*>> anim_frames;
	bool loop = false;
};

#endif
