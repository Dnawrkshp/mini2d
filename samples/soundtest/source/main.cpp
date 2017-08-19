#include <stdio.h>
#include <stdlib.h>
#include <io/pad.h>

#include <Mini2D/Mini2D.hpp>
#include <Mini2D/Font.hpp>
#include <Mini2D/Sound.hpp>

#include "comfortaa_regular_ttf.h"
#include "sbfsc_adhdj_mp3.h"
#include "snare_ogg.h"

int drawUpdate(float deltaTime, unsigned long frame);
void padUpdate(int changed, int port, padData pData);
void exit();

Mini2D * mini = NULL;

Font * font1 = NULL;
Sound * sound1 = NULL;
Sound * sound2 = NULL;

int doExit = 0;
int infiniteVoice = -1;
int status;

// Font sizes
float FONT_LARGE;
// Font locations
Vector2 PRINT_ITEM0;
Vector2 PRINT_ITEM1;

std::wstring TEXT_PAUSEAUDIO = 			L"Press Triangle to pause audio.";
std::wstring TEXT_RESUMEAUDIO = 		L"Press Triangle to resume audio.";
std::wstring TEXT_STARTAUDIO = 			L"Press Triangle to start infinite audio.";
std::wstring TEXT_SNARE =				L"Press Square for snare.";

int main(s32 argc, const char* argv[]) {

	// Load Mini2D
	mini = new Mini2D((Mini2D::PadCallback_f)&padUpdate, (Mini2D::DrawCallback_f)&drawUpdate, (Mini2D::ExitCallback_f)&exit);

	// Initialize location and size vectors
	FONT_LARGE = 0.02*mini->MAXW;

	PRINT_ITEM0 = Vector2(0.5*mini->MAXW, 0.5*mini->MAXH);
	PRINT_ITEM1 = Vector2(PRINT_ITEM0.X, 0.55*mini->MAXH);

	// Load comfortaa font
	font1 = new Font(mini);
	font1->TextAlign = Font::PRINT_ALIGN_CENTER;
	if (font1->Load((void*)comfortaa_regular_ttf, comfortaa_regular_ttf_size))
		printf("error loading font\n");

	// Initialize sounds
	sound1 = new Sound(mini);
	sound2 = new Sound(mini);

	// Load adhdj song, 101 seconds
	sound1->Load(sbfsc_adhdj_mp3, sbfsc_adhdj_mp3_size, 101);
	// Load snare sound, 0.4 seconds
	sound2->Load(snare_ogg, snare_ogg_size, 0.4f);

	mini->SetAnalogDeadzone(15);
	mini->SetClearColor(0x23B2D7FF);
	mini->SetAlphaState(1);
	mini->BeginDrawLoop();

	return 0;
}

int drawUpdate(float deltaTime, unsigned long frame) {
	if (infiniteVoice >= 0 && status == Sound::VOICE_RUNNING) {
		font1->ForeColor = 0x800000FF;
		font1->PrintLine(TEXT_PAUSEAUDIO, NULL, PRINT_ITEM0, FONT_LARGE);
	}
	else if (infiniteVoice < 0) {
		font1->ForeColor = 0x000080FF;
		font1->PrintLine(TEXT_STARTAUDIO, NULL, PRINT_ITEM0, FONT_LARGE);
	}
	else {
		font1->ForeColor = 0x000080FF;
		font1->PrintLine(TEXT_RESUMEAUDIO, NULL, PRINT_ITEM0, FONT_LARGE);
	}

	font1->ForeColor = 0x000000FF;
	font1->PrintLine(TEXT_SNARE, NULL, PRINT_ITEM1, FONT_LARGE);	

	return doExit;
}

void padUpdate(int changed, int port, padData pData) {
	status = sound1->Status(infiniteVoice);

	if (pData.BTN_START && changed & Mini2D::BTN_CHANGED_START)
		doExit = -1;

	if (pData.BTN_TRIANGLE && changed & Mini2D::BTN_CHANGED_TRIANGLE) {
		if (infiniteVoice < 0) {
			infiniteVoice = sound1->PlayInfinite(255, 255);
		}
		else if (status == Sound::VOICE_PAUSED) {
			sound1->Resume(infiniteVoice);
		}
		else {
			sound1->Pause(infiniteVoice);
		}
	}

	if (pData.BTN_SQUARE && changed & Mini2D::BTN_CHANGED_SQUARE) {
		sound2->PlayOnce(255, 255, 0, 0.4f);
	}
}

void exit() {
	printf("exiting\n");

	if (sound1) {
		delete sound1;
		sound1 = NULL;
	}

	if (sound2) {
		delete sound2;
		sound2 = NULL;
	}

	if (font1) {
		delete font1;
		font1 = NULL;
	}

	if (mini) {
		delete mini;
		mini = NULL;
	}
}
