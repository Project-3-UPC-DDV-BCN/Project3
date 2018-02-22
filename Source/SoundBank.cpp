#include "SoundBank.h"

SoundBank::~SoundBank()
{
	for (int i = 0; i < events.size(); i++) {
		delete events[i];
	}
}
