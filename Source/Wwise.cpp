#include "Wwise.h"
#include <assert.h>
#include <string>
#include "Globals.h"
#include "include_wwise.h"

#include "Wwise/IO/Win32/AkFilePackageLowLevelIOBlocking.h"
#include <AK/Plugin/AkRoomVerbFXFactory.h>
#include <AK\SoundEngine\Common\AkSoundEngine.h>

CAkFilePackageLowLevelIOBlocking g_lowLevelIO;

//Initialize all Wwise modules. Receives the base path for soundbanks and the current language
bool Wwise::InitWwise(const char* language)
{
	bool ret = true;
	//Init default Wwise memory manager
	
	ret = Wwise::InitMemSettings();

	//Initialize stream manager
	ret = Wwise::InitStreamSettings();

	//Initialize default IO device
	ret = Wwise::InitDeviceSettings();

	// Create the Sound Engine using default initialization parameters
	ret = Wwise::InitSoundEngine();


	// Initialize the music engine using default initialization parameters
	ret = InitMusicEngine();


#ifndef AK_OPTIMIZED
	// Initialize communications for debug purposes
	AkCommSettings commSettings;
	AK::Comm::GetDefaultInitSettings(commSettings);
	if (AK::Comm::Init(commSettings) != AK_Success)
	{
		assert(!"Could not initialize communication.");
		ret =  false;
	}
#endif 

	//Set base path for sound banks
	/*AKRESULT base_path_res = g_lowLevelIO.SetBasePath(base_path);
	if (base_path_res != AK_Success)
	{
		assert(!"Invalid base path!");
		return false;
	}*/

	//Set language
	SetLanguage(language);

	//Loads the Init Sound Bank
	std::string init_path = ASSETS_SOUNDBANK_FOLDER;
	init_path += "Init.bnk";
	LoadBank(init_path.c_str());


	return ret;
}

bool Wwise::InitMemSettings()
{
	AkMemSettings memSettings;
	memSettings.uMaxNumPools = 20;
	if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
	{
		assert(!"Could not create the memory manager.");
		return false;
	}
	else {
		return true;
	}

}

bool Wwise::InitStreamSettings()
{
	AkStreamMgrSettings stmSettings;
	AK::StreamMgr::GetDefaultSettings(stmSettings);
	if (!AK::StreamMgr::Create(stmSettings))
	{
		assert(!"Could not create the Streaming Manager");
		return false;
	}
	else {
		return true;
	}
}

bool Wwise::InitDeviceSettings()
{
	AkDeviceSettings deviceSettings;
	AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);
	if (g_lowLevelIO.Init(deviceSettings) != AK_Success)
	{
		assert(!"Could not create the streaming device and Low-Level I/O system");
		return false;
	}
	else {
		return true;
	}
}

bool Wwise::InitSoundEngine()
{
	/*AkInitSettings initSettings;
	AkPlatformInitSettings platformInitSettings;
	AK::SoundEngine::GetDefaultInitSettings(initSettings);
	AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);
	if (AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success)
	{
		assert(!"Could not initialize the Sound Engine.");
		return false;
	}
	else {
		return true;
	}*/

	if (AK::SoundEngine::Init(NULL, NULL) != AK_Success) // Default values
	{
		assert(!"Could not initialize the Sound Engine.");
		return false;
	}
	else 
	{
		return true;
	}
}

bool Wwise::InitMusicEngine()
{
	AkMusicSettings musicInit;
	AK::MusicEngine::GetDefaultInitSettings(musicInit);
	if (AK::MusicEngine::Init(&musicInit) != AK_Success)
	{
		assert(!"Could not initialize the Music Engine.");
		return false;
	}
	else {
		return true;
	}
}

void Wwise::ProcessAudio()
{
	AK::SoundEngine::RenderAudio();
}

bool Wwise::CloseWwise()
{
	//Terminate comunication module (IMPORTANT: this must be the first module in being terminated)
#ifndef AK_OPTIMIZED
	AK::Comm::Term();
#endif 

	//Terminate the music engine
	AK::MusicEngine::Term();

	//Terminate the sound engine
	AK::SoundEngine::Term();

	//Terminate IO device
	g_lowLevelIO.Term();
	if (AK::IAkStreamMgr::Get())
		AK::IAkStreamMgr::Get()->Destroy();

	//Terminate the Memory Manager
	AK::MemoryMgr::Term();


	return true;
}

void Wwise::SetDefaultListeners(unsigned long* id)
{
	AK::SoundEngine::SetDefaultListeners((AkGameObjectID*)id, 0);

}

void Wwise::SetLanguage(const char * language)
{
	AKRESULT res = AK::StreamMgr::SetCurrentLanguage((AkOSChar*)language);
	if (res == AK_Fail)
	{
		assert(!"Invalid language!");
	}
}

//Gets the bank path and returns it's ID. IMPORTANT: never call this before InitWwished()
unsigned long Wwise :: LoadBank(const char * path)
{
	unsigned long bank_id;
	AKRESULT res = AK::SoundEngine::LoadBank(path, AK_DEFAULT_POOL_ID, bank_id);

	if (res != AK_Success)
	{
		//The .bnk file is missing
		CONSOLE_DEBUG("Could not initialize soundbank!");
	}

	return bank_id;
}

Wwise::SoundObject * Wwise::CreateSoundObj(unsigned long id, const char * name, float x, float y, float z, bool is_default_listener)
{
	SoundObject* emitter = nullptr;

	emitter = new SoundObject(id, name);

	if (is_default_listener)
	{
		AkGameObjectID listener_id = emitter->GetID();
		AK::SoundEngine::SetDefaultListeners(&listener_id, 1);
	}

	emitter->SetPosition(x, y, z);

	return emitter;
}

void Wwise::ChangeState(const char* group, const char* new_state)
{
	AK::SoundEngine::SetState(group, new_state);
}



// SoundEmitter class methods -------------------------------------------------
Wwise::SoundObject::SoundObject(unsigned long id, const char * n)
{
	SoundID = id;
	name = n;
	AKRESULT res = AK::SoundEngine::RegisterGameObj((AkGameObjectID)SoundID, name);
	if (res != AK_Success)
	{
		CONSOLE_DEBUG("Could not register GameObj! See res variable to more info");
	}

	position.X = 0;
	position.Y = 0;
	position.Z = 0;

	top.X = 0;
	top.Y = 0;
	top.Z = 0;

	front.X = 0;
	front.Y = 0;
	front.Z = 0;

}

Wwise::SoundObject::~SoundObject()
{
	AKRESULT res = AK::SoundEngine::UnregisterGameObj(SoundID);
	if (res != AK_Success)
	{
		CONSOLE_DEBUG("Could not unregister GameObj! See res variable to more info");
	}
}

Wwise::SoundObject * Wwise::SoundObject::operator=(const SoundObject & so)
{
	SoundID = so.SoundID;
	name = so.name;
	position = so.position;
	top = so.top;
	front = so.front;

	return this;
}

unsigned long Wwise::SoundObject::GetID()
{
	return SoundID;
}

AkGameObjectID Wwise::SoundObject::GetSoundID() const
{
	return SoundID;
}

const char * Wwise::SoundObject::GetName()
{
	return name;
}



//The two vectors must be normalized and orthogonal!
void Wwise::SoundObject::SetPosition(float x, float y, float z, float x_front, float y_front, float z_front, float x_top, float y_top, float z_top)
{
	position.X = -x;
	position.Y = y;
	position.Z = -z;

	front.X = x_front;
	front.Y = y_front;
	front.Z = z_front;
	top.X = x_top;
	top.Y = y_top;
	top.Z = z_top;

	float length_front = sqrt(pow(front.X, 2) + pow(front.Y, 2) + pow(front.Z, 2));
	float length_top = sqrt(pow(top.X, 2) + pow(top.Y, 2) + pow(top.Z, 2));

	//Normalize vectors
	front.X =front.X / length_front;
	front.Y =front.Y / length_front;
	front.Z =front.Z / length_front;
	top.X = top.X / length_top;
	top.Y = top.Y / length_top;
	top.Z = top.Z / length_top;

	//Check if the are orthogonals
	float dot_prod = top.X*front.X + top.Y*front.Y + top.Z*front.Z;

	if (dot_prod >= 0.0001)
		CONSOLE_DEBUG("Vectors are not orthogonal!");

	AkSoundPosition sound_pos;
	sound_pos.Set(position, front, top);
	AKRESULT res = AK::SoundEngine::SetPosition((AkGameObjectID)SoundID, sound_pos);
	if (res != AK_Success)
		CONSOLE_DEBUG("Couldn't update position");
}

AkVector Wwise::SoundObject::GetPosition() const
{
	return position;
}

AkVector Wwise::SoundObject::GetTop() const
{
	return top;
}

AkVector Wwise::SoundObject::GetFront() const
{
	return front;
}

void Wwise::SoundObject::SetListener(unsigned long * id)
{
	AKRESULT res = AK::SoundEngine::SetListeners(SoundID, (AkGameObjectID*)id, 1);
}


void Wwise::SoundObject::PlayEvent(unsigned long id)
{
	AK::SoundEngine::PostEvent(id, SoundID);
}

void Wwise::SoundObject::PlayEvent(const char* name)
{
	AK::SoundEngine::PostEvent(name, SoundID);
	CONSOLE_DEBUG("Played: %s", name);
}

void Wwise::SoundObject::PlayMusic(unsigned long music_id)
{
	AK::SoundEngine::PostEvent(music_id, SoundID, AK_EnableGetMusicPlayPosition);
}

void Wwise::SoundObject::PlayMusic(const char * music_name)
{
	AK::SoundEngine::PostEvent(music_name, SoundID, AK_EnableGetMusicPlayPosition);

}

void Wwise::SoundObject::SetAuxiliarySends(AkReal32 value, const char * target_bus, AkGameObjectID listener_id)
{
	AkAuxSendValue reverb;
	reverb.listenerID = listener_id;
	reverb.auxBusID = AK::SoundEngine::GetIDFromString(target_bus);
	reverb.fControlValue = value;

	AKRESULT res = AK::SoundEngine::SetGameObjectAuxSendValues(SoundID, &reverb, 1);
}
