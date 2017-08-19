/*
 * Sound.cpp
 *
 *  Created on: November 8th, 2016
 *  Author: Daniel Gerendasy
 */

#include <malloc.h>							// memalign()
#include <math.h>							// floor()

#include <Mini2D/Sound.hpp>					// Class definition

namespace Mini2D {

	typedef struct _voiceSoundInfo_t {
		Sound * sound;
		bool loop;
		int freq;
		int stereo;
		int sampleSize;
		float length;
		int delay;
		int lv;
		int rv;
		int voice;
		short * sample;
	} VoiceSoundInfo;

	static VoiceSoundInfo * voiceSounds[MAX_SND_VOICES];

	static void setVoice(VoiceSoundInfo * soundInfo);
	static void setVoiceCallback(int voice);

	//---------------------------------------------------------------------------
	// Init Functions
	//---------------------------------------------------------------------------
	Sound::Sound(Mini * mini) : _mini(mini) {
		_sample = NULL;
		_sampleSize = 0;

		for(int i=0;i<MAX_SND_VOICES;i++) {
			if (!voiceSounds[i]) {
				voiceSounds[i] = new VoiceSoundInfo();
				voiceSounds[i]->voice = -1;
			}
		}
	}

	Sound::~Sound() {
		if (_sample) {
			free(_sample);
		}
	}

	//---------------------------------------------------------------------------
	// Load Functions
	//---------------------------------------------------------------------------
	Sound::SoundLoadStatus Sound::Load(const void * buffer, unsigned int size, float seconds) {
		if (!_mini)
			return SOUND_INVALID_MINI2D;
		if (!buffer || !size || seconds <= 0)
			return SOUND_INVALID_ARG;
		if (_sample)
			free(_sample);

		// Get frequency and stereo/mono
		_sampleSize = 0;
		if (DecodeAudio((void*)buffer, (int)size, NULL, &_sampleSize, &_freq, &_stereo) < 0)
			return SOUND_DECODE_FAILED;

		_sampleSize = SPU_SIZE((int)((float)_freq * (_stereo ? 4.f : 2.f) * seconds));
		_sample = (short *)memalign(128, _sampleSize);
		memset(_sample, 0, _sampleSize);
		_seconds = seconds;

		// Decode audio into buffer
		if (DecodeAudio((void*)buffer, (int)size, _sample, &_sampleSize, &_freq, &_stereo) < 0)
			return SOUND_DECODE_FAILED;

		return SOUND_SUCCESS;
	}

	Sound::SoundLoadStatus Sound::Load(const char * filepath, float seconds) {
		char * buffer = NULL;
		int bufferSize = 0;
		Sound::SoundLoadStatus result;

		FILE *fp = fopen(filepath, "r");
		if (fp != NULL) {
		    if (fseek(fp, 0L, SEEK_END) == 0) {
		        bufferSize = ftell(fp);
		        if (bufferSize <= 0) {
		        	return SOUND_INVALID_FILE;
		        }

		        buffer = new char[bufferSize+1];

		        if (fseek(fp, 0L, SEEK_SET) != 0) {
		        	delete [] buffer;
		        	return SOUND_INVALID_FILE;
		        }

		        fread(buffer, sizeof(char), bufferSize, fp);
		        if (ferror(fp) != 0) {
		        	delete [] buffer;
		        	return SOUND_INVALID_FILE;
		        }
		    }
		    fclose(fp);
		}

		if (!buffer || bufferSize < 0)
			return SOUND_INVALID_FILE;

		result = Load(buffer, bufferSize, seconds);

		delete [] buffer;

		return result;
	}

	//---------------------------------------------------------------------------
	// Playback Functions
	//---------------------------------------------------------------------------
	int Sound::PlayOnce(int leftVolume, int rightVolume, float delay, float length) {
		return Play(leftVolume, rightVolume, delay, length, 0);
	}

	int Sound::PlayInfinite(int leftVolume, int rightVolume, float delay, float length) {
		return Play(leftVolume, rightVolume, delay, length, 1);
	}

	int Sound::Play(int leftVolume, int rightVolume, float delay, float length, int loop) {
		int i = SND_GetFirstUnusedVoice();

		if (i == SND_INVALID)
			return -1;

		if (length <= 0 || length > _seconds)
			length = _seconds;

		if (!voiceSounds[i])
			voiceSounds[i] = new VoiceSoundInfo();
		voiceSounds[i]->sound = this;
		voiceSounds[i]->loop = loop;
		voiceSounds[i]->freq = _freq;
		voiceSounds[i]->sampleSize = _sampleSize;
		voiceSounds[i]->length = length;
		voiceSounds[i]->stereo = _stereo;
		voiceSounds[i]->sample = _sample;
		voiceSounds[i]->delay = (int)(delay * 1000);
		voiceSounds[i]->lv = leftVolume;
		voiceSounds[i]->rv = rightVolume;
		voiceSounds[i]->voice = i;

		setVoice(voiceSounds[i]);

		return i;
	}

	//---------------------------------------------------------------------------
	// Voice Control Functions
	//---------------------------------------------------------------------------
	Sound::SoundVoiceStatus Sound::Status(int voice) {
		if (voice < 0 || voice >= MAX_SND_VOICES)
			return VOICE_INVALID;
		if (voiceSounds[voice]->voice < 0)
			return VOICE_UNUSED;

		if (SND_StatusVoice(voice) == SND_WORKING)
			return VOICE_RUNNING;

		return VOICE_PAUSED;
	}

	void Sound::Stop(int voice) {
		if (voiceSounds[voice]) {
			voiceSounds[voice]->voice = -1;
			voiceSounds[voice]->sound = NULL;
		}
		SND_StopVoice(voice);
	}

	void Sound::Pause(int voice) {
		if (!voiceSounds[voice] || voiceSounds[voice]->voice < 0)
			return;
		SND_PauseVoice(voice, 1);
	}

	void Sound::Resume(int voice) {
		if (!voiceSounds[voice] || voiceSounds[voice]->voice < 0)
			return;
		SND_PauseVoice(voice, 0);
	}

	//---------------------------------------------------------------------------
	// Private Functions
	//---------------------------------------------------------------------------
	static void setVoice(VoiceSoundInfo * soundInfo) {
		SND_SetVoice(soundInfo->voice,
			soundInfo->stereo ? VOICE_STEREO_16BIT : VOICE_MONO_16BIT,
			soundInfo->freq,
			soundInfo->delay,
			soundInfo->sample,
			(int)((float)soundInfo->freq * (soundInfo->stereo ? 4.f : 2.f) * soundInfo->length),
			soundInfo->lv,
			soundInfo->rv,
			setVoiceCallback);	
	}

	static void setVoiceCallback(int voice) {
		if (voiceSounds[voice])
		{
			if (!voiceSounds[voice]->loop) {
				voiceSounds[voice]->sound->Stop(voice);
			}
			else {
				SND_StopVoice(voice);
				setVoice(voiceSounds[voice]);
			}
		}
	}

}
