/*
 * Sound.hpp
 *
 *  Created on: November 8th, 2016
 *  Author: Daniel Gerendasy
 */

#ifndef SOUND_HPP_
#define SOUND_HPP_

#include <Mini2D/Mini.hpp>					// Class definition

namespace Mini2D {

	class Sound {
	public:
		typedef enum _soundLoadStatus_t {
			SOUND_SUCCESS = 0,
			SOUND_INVALID_ARG,
			SOUND_DECODE_FAILED,
			SOUND_INVALID_MINI2D,
			SOUND_INVALID_FILE
		} SoundLoadStatus;

		typedef enum _soundVoiceStatus_t {
			VOICE_INVALID = -1,
			VOICE_UNUSED,
			VOICE_PAUSED,
			VOICE_RUNNING
		} SoundVoiceStatus;

		// Constructors
		Sound(Mini * mini);
		virtual ~Sound();

		/*
		 * Load:
		 * 		Load MP3/OGG
		 * 
		 * buffer:
		 * 		Array containing MP3/OGG
		 * size:
		 * 		Size of buffer
		 * seconds:
		 * 		How many seconds of the audio to load
		 * 
		 * Return:
		 * 		Result of the load operation
		 */
		SoundLoadStatus Load(const void * buffer, unsigned int size, float seconds);

		/*
		 * Load:
		 * 		Load MP3/OGG
		 * 
		 * filepath:
		 * 		Path to MP3/OGG
		 * seconds:
		 * 		How many seconds of the audio to load
		 * 
		 * Return:
		 * 		Result of the load operation
		 */
		SoundLoadStatus Load(const char * filepath, float seconds);

		/*
		 * PlayOnce:
		 * 		Play a sound once
		 * 
		 * leftVolume:
		 * 		Volume from left speaker
		 * rightVolume:
		 * 		Volume from right speaker
		 * delay:
		 * 		How many seconds to wait before playing the sound
		 * length:
		 *		How many seconds of the audio to play
		 * 
		 * Return:
		 * 		The index of the used voice
		 */
		int PlayOnce(int leftVolume, int rightVolume, float delay = 0, float length = 0);

		/*
		 * PlayInfinite:
		 * 		Play a sound on a loop
		 * 
		 * leftVolume:
		 * 		Volume from left speaker
		 * rightVolume:
		 * 		Volume from right speaker
		 * delay:
		 * 		How many seconds to wait before playing the sound
		 * length:
		 *		How many seconds of the audio to play
		 * 
		 * Return:
		 * 		The index of the used voice
		 */
		int PlayInfinite(int leftVolume, int rightVolume, float delay = 0, float length = 0);

		/*
		 * Status:
		 * 		Get the status of a voice
		 * 
		 * voice:
		 * 		Voice
		 * 
		 * Return:
		 * 		Status of voice
		 */
		SoundVoiceStatus Status(int voice);

		/*
		 * Stop:
		 * 		Stop a voice from playing
		 * 
		 * voice:
		 * 		Voice
		 */
		void Stop(int voice);

		/*
		 * Pause:
		 * 		Pause a voice
		 * 
		 * voice:
		 * 		Voice
		 */
		void Pause(int voice);

		/*
		 * Resume:
		 * 		Resume a voice
		 * 
		 * voice:
		 * 		Voice
		 */
		void Resume(int voice);

	private:
		Mini * _mini;

		int _freq;								// Frequency of sample audio
		int _stereo;							// Whether sample is stereo or mono
		int _sampleSize;						// Size of sample
		float _seconds;							// Length of sample in seconds
		short * _sample;						// Sample

		// Play a sound
		int Play(int leftVolume, int rightVolume, float delay, float length, int loop);
	};

}

#endif /* SOUND_HPP_ */
