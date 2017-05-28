#include "AudioSystem.h"
#include <iostream>

AudioSystem::AudioSystem()
{
	m_audioEngine = NULL;
}

bool AudioSystem::Initialise()
{
	if(!DidSucceed(FMOD::System_Create(&m_audioEngine)))
		return false;
	
	// The only paramater of note here is the first one, which is the maximum
	// amount of channels FMOD will create for us. We shouldn't need more than 32.
	if(!DidSucceed(m_audioEngine->init(32, FMOD_INIT_NORMAL, (void*)0)))	
		return false;

	// We only have one set of ears in this world
	m_audioEngine->set3DNumListeners(1);

	float dopplerScale = 0.3f;		// The intensity of the doppler effect. Set to 0.0f to disable it
	float distanceFactor = 1.0f;	// A modifier for any setting involving world space position units
	float rolloffScale = 1.0f;		// We can exaggerate the falloff for all our 3D sounds (linear rolloff ignores this)

	// http://www.fmod.org/docs/content/generated/FMOD_System_Set3DSettings.html
	m_audioEngine->set3DSettings(dopplerScale, distanceFactor, rolloffScale);

	return true;
}

bool AudioSystem::Load(const char* filepath)
{
	FMOD::Sound* sound;
	
	if(!DidSucceed(m_audioEngine->createSound(filepath, FMOD_DEFAULT, NULL, &sound)))
		return false;

	// Add the sound to our audio map so we can easily retrieve it when we are told to play it
	m_audioMap[filepath] = sound;

	return true;
}

bool AudioSystem::LoadStream(const char* filepath)
{
	// The only change in here is the call to createStream instead of createSound.
	// Remember to consider streams for longer sounds played less frequently like 
	// background music and general ambience.

	FMOD::Sound* sound;

	if(!DidSucceed(m_audioEngine->createStream(filepath, FMOD_DEFAULT, NULL, &sound)))
		return false;

	// Add sound to our audio map so we can easily retrieve it when we are told to play it
	m_audioMap[filepath] = sound;

	return true;
}

AudioClip* AudioSystem::Play(const char* filepath, bool startPaused)
{
	// Pull the sound we want to play out of the map
	FMOD::Sound* soundToPlay = FetchSound(filepath);

	if (soundToPlay)
	{
		// We'll store the channel FMOD allocates this sound
		FMOD::Channel* channel = NULL;

		if (DidSucceed(m_audioEngine->playSound(soundToPlay, NULL, startPaused, &channel)))
		{
			// Be careful about memory leaks here. The object requesting the AudioClip is
			// responsibile for deteting it. TODO refactor this if you want.
			return new AudioClip(channel);
		}
	}

	return NULL;
}


void AudioSystem::Update()
{
	// FMOD likes to be updated each tick of our gameloop so it can do some housekeeping
	// Remember to call this function out in Game
	m_audioEngine->update();
}

bool AudioSystem::SetListener3DAttributes(const Vector3& pos, const Vector3& forward, const Vector3& up, const Vector3& velocity)
{
	// Have a read through the docs for this function
	// https://www.fmod.org/docs/content/generated/FMOD_System_Set3DListenerAttributes.html

	return DidSucceed(m_audioEngine->set3DListenerAttributes(0, &ConvertVector(pos), &ConvertVector(velocity), &ConvertVector(forward), &ConvertVector(up)));
}

FMOD::Sound* AudioSystem::FetchSound(const char* filepath)
{
	// Fetch the desired sound out of our map

	AudioMap::const_iterator searchResult = m_audioMap.find(filepath);

	if (searchResult != m_audioMap.end())
	{
		return searchResult->second;
	}

	return NULL;
}

bool AudioSystem::DidSucceed(FMOD_RESULT result)
{
	// Basically converts an FMOD_RESULT into a boolean

	if (result != FMOD_OK)
	{
		// Peek definition of FMOD_RESULT to see all possible errors
		std::cout << "FMOD error in AudioSystem: " << result << std::endl;
		return false;
	}

	// Good times
	return true;
}

FMOD_VECTOR AudioSystem::ConvertVector(const Vector3& pos)
{
	// Convert a SimpleMath vector into an FMOD vector

	// Peek definition and you'll see this is just a basic struct
	FMOD_VECTOR fVec;

	// Transfer the floats across
	fVec.x = pos.x;
	fVec.y = pos.y;
	fVec.z = pos.z;

	return fVec;
}

void AudioSystem::Shutdown()
{
	ReleaseAllSounds();

	m_audioEngine->release();
	m_audioEngine = NULL;
}

void AudioSystem::ReleaseAllSounds()
{
	// Loop through all loaded sounds and free their resources
	AudioMap::iterator iterator;

	for (iterator = m_audioMap.begin(); iterator != m_audioMap.end(); iterator++)
	{
		if (iterator->second)
		{
			iterator->second->release();
		}
	}

	m_audioMap.clear();
}

bool AudioSystem::ReleaseSound(const char* filepath)
{
	// Release a particular sound

	AudioMap::const_iterator searchResult = m_audioMap.find(filepath);

	if (searchResult != m_audioMap.end())
	{
		if(!DidSucceed(searchResult->second->release()))
			return false;

		// Remove that entry from our audio map
		m_audioMap.erase(searchResult);
		delete searchResult->second;

		return true;
	}

	// The identifier didn't match a loaded sound
	return false;
}