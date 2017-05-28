#include "AudioClip.h"
#include <iostream>

AudioClip::AudioClip(FMOD::Channel* channel)
{
	m_channel = channel;
	
	// Default settings
	m_channel->setMode(FMOD_LOOP_OFF);
}

void AudioClip::Stop()
{
	// This will invalidate our FMOD channel and make it available for re-use
	// by FMODs priority system. We shouldn't do anything with the AudioClip
	// after calling Stop (also why no Play function is exposed in here)
	m_channel->stop();

	// Good practice would be to delete the AudioClip after stopping it,
	// or delete it straight away for single-shot sounds where we don't need
	// to keep track of them. FMOD's channel will keep playing regardless of whether
	// its AudioClip is alive.
}

void AudioClip::SetVolume(float volume)
{
	// TODO not handling the FMOD_RESULT these mutators return
	m_channel->setVolume(volume);
}

void AudioClip::SetMute(bool mute)
{
	m_channel->setMute(mute);
}

void AudioClip::SetPaused(bool paused)
{
	m_channel->setPaused(paused);
}

void AudioClip::SetLoopCount(int loopCount)
{
	// This is just an inline IF statement using the ternary operator
	// On the left of the quesiton mark is the condition, then true and false seperated by a colon
	m_channel->setMode(loopCount == 0 ? FMOD_LOOP_OFF : FMOD_LOOP_NORMAL);
	m_channel->setLoopCount(loopCount);
}

void AudioClip::SetPan(float pan)
{
	// We can pan the sound between speakers (-1.0 for left speaker, and 1.0 for right)
	// This only works when using 2D sounds
	m_channel->setPan(pan);
}

void AudioClip::SetIs3D(bool is3D)
{
	// Using the ternary operator again for an inline IF
	// Also using the Bitwise OR operator (the pipe) to combine together two settings
	m_channel->setMode(is3D ? FMOD_3D | FMOD_3D_LINEARROLLOFF : FMOD_2D);
}

bool AudioClip::Set3DAttributes(const Vector3& pos, const Vector3& velocity)
{
	return DidSucceed(m_channel->set3DAttributes(&ConvertVector(pos), &ConvertVector(velocity)));
}

void AudioClip::SetMinMaxDistance(float min, float max)
{
	// This controls how far the sound can travel.
	// It will be full volume between 0 and min distance.
	// It will fall off linearly between min and max distance.
	// It will be silent after max distance.
	m_channel->set3DMinMaxDistance(min, max);
}

bool AudioClip::DidSucceed(FMOD_RESULT result)
{
	// Basically converts an FMOD_RESULT into a boolean

	if (result != FMOD_OK)
	{
		// Peek definition of FMOD_RESULT to see all possible errors
		std::cout << "FMOD error in AudioClip: " << result << std::endl;
		return false;
	}

	// Good times
	return true;
}

FMOD_VECTOR AudioClip::ConvertVector(const Vector3& pos)
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