/*	FIT2096 - Example Code
*	AudioSystem.h
*	Created by Mike Yeates - 2017 - Monash University
*	A wrapper for the FMOD low-level audio API. Responsible for loading and
*	requesting sounds. Also supports basic 3D positioning.
*	http://www.fmod.org/documentation/#content/generated/lowlevel_api.html
*/

#ifndef AUDIO_SYSTEM_H
#define AUDIO_SYSTEM_H

#include <d3d11.h>
#include <map>

#include "DirectXTK/SimpleMath.h"
#include "fmod.hpp"
#include "fmod_errors.h"
#include "AudioClip.h"

using namespace DirectX::SimpleMath;

class AudioSystem
{
private:
	// This object is how we talk to FMOD
	FMOD::System* m_audioEngine;

	// Map actual sounds to a string just like we do in the mesh and texture managers
	typedef std::map<std::string, FMOD::Sound*> AudioMap;
	AudioMap m_audioMap;

	// Helpers
	FMOD::Sound* FetchSound(const char* filepath); // Fetch the sound we want to play out of the map
	bool DidSucceed(FMOD_RESULT result); // Converts an FMOD_RESULT into a boolean and outputs errors
	FMOD_VECTOR ConvertVector(const Vector3& pos); // Convert a SimpleMath vector into an FMOD vector
	void ReleaseAllSounds(); // Shutdown uses this to cleanup

public:
	AudioSystem();
	~AudioSystem() {};

	bool Initialise();
	void Shutdown();
	void Update();
	
	bool Load(const char* filepath);
	bool LoadStream(const char* filepath);
	bool ReleaseSound(const char* filepath);

	AudioClip* Play(const char* filepath, bool startPaused);

	// We need to tell FMOD where our listener is, its orientation, and also its velocity for doppler
	bool SetListener3DAttributes(const Vector3& pos, const Vector3& forward, const Vector3& up, const Vector3& velocity);

};

#endif