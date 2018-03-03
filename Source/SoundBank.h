#pragma once
#ifndef _SOUNDBANK_H_
#define _SOUNDBANK_H_

#include <string>
#include <vector>
#include "AudioEvent.h"

class AudioEvent;

class SoundBank 
{

public: 

	SoundBank() {};
	~SoundBank();
	std::vector<AudioEvent*>events;
	std::string name;
	std::string path;

	unsigned int id = 0;
};

#endif
