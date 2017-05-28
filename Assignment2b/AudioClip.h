/*	FIT2096 - Example Code
*	AudioClip.h
*	Created by Mike Yeates - 2017 - Monash University
*	A playing audio clip is just an FMOD channel. This class
*	abstracts away FMOD specifics from the rest of the engine.
*	Use this class to alter the settings of a sound currently playing.
*/

#ifndef AUDIO_CLIP_H
#define AUDIO_CLIP_H

#include <d3d11.h>
#include "DirectXTK/SimpleMath.h"

#include "fmod.hpp"
#include "fmod_errors.h"

using namespace DirectX::SimpleMath;

class AudioClip
{
// We're friends with the AudioSystem class, meaning we're granting it the 
// ability to access our private data and functions. Only it can access our constructor
friend class AudioSystem;

private:
	FMOD::Channel* m_channel;

	// Our constructor is private as you should never create one of these yourself
	// Instead we're enforcing that only the AudioSystem can instantiate us
	// (which it does inside AudioSystem::Play)
	AudioClip(FMOD::Channel* channel);

	// Helpers
	bool DidSucceed(FMOD_RESULT result); // Converts an FMOD_RESULT into a boolean and outputs errors
	FMOD_VECTOR ConvertVector(const Vector3& pos); // Convert a SimpleMath vector into an FMOD vector

public:
	void SetVolume(float volume);
	void SetMute(bool mute);
	void SetPaused(bool paused);
	void SetLoopCount(int loopCount);
	void SetPan(float pan);
	void SetIs3D(bool is3D);
	bool Set3DAttributes(const Vector3& pos, const Vector3& velocity);
	void SetMinMaxDistance(float min, float max);
	void Stop();
};

#endif