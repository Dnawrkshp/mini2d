#include <stdio.h>
#include <stdlib.h>
#include <io/pad.h>

#include <Mini2D/Mini2D.hpp>
#include <Mini2D/Font.hpp>
#include <Mini2D/Sound.hpp>

#include "comfortaa_regular_ttf.h"
#include "sbfsc_adhdj_mp3.h"
#include "snare_ogg.h"

int drawUpdate(float deltaTime, unsigned int frame);
void padUpdate(int changed, int port, padData pData);
void exit();

Mini2D mini((Mini2D::PadCallback_f)&padUpdate, (Mini2D::DrawCallback_f)&drawUpdate, (Mini2D::ExitCallback_f)&exit);

int doExit = 0;
Font *font1;
Sound *sound1, *sound2;
int infiniteVoice = -1;
int status;

const Vector2 FONT_MEDIUM(0.04*mini.MAXW, 0.04*mini.MAXH);

const Vector2 PRINT_ITEM0(0.5*mini.MAXW, 0.5*mini.MAXH);
const Vector2 PRINT_ITEM1(PRINT_ITEM0.X, 0.55*mini.MAXH);


int main(s32 argc, const char* argv[]) {
	font1 = new Font(&mini);
	if (font1->Load((void*)comfortaa_regular_ttf, comfortaa_regular_ttf_size))
		printf("error loading font\n");

	sound1 = new Sound(&mini);
	sound2 = new Sound(&mini);

	// Load adhdj song, 101 seconds
	sound1->Load(sbfsc_adhdj_mp3, sbfsc_adhdj_mp3_size, 101);
	// Load snare sound, 0.4 seconds
	sound2->Load(snare_ogg, snare_ogg_size, 0.4f);

	mini.SetAnalogDeadzone(15);
	mini.SetClearColor(0x23B2D7FF);
	mini.SetAlphaState(1);
	mini.BeginDrawLoop();

	return 0;
}

int drawUpdate(float deltaTime, unsigned int frame) {
	if (infiniteVoice >= 0 && status == Sound::VOICE_RUNNING)
		font1->Print((char*)"Press Triangle to pause audio.", PRINT_ITEM0, FONT_MEDIUM, 0x800000FF, Font::PRINT_ALIGN_CENTER);
	else if (infiniteVoice < 0)
		font1->Print((char*)"Press Triangle to start infinite audio.", PRINT_ITEM0, FONT_MEDIUM, 0x000080FF, Font::PRINT_ALIGN_CENTER);
	else
		font1->Print((char*)"Press Triangle to resume audio.", PRINT_ITEM0, FONT_MEDIUM, 0x000080FF, Font::PRINT_ALIGN_CENTER);

	font1->Print((char*)"Press Square for snare.", PRINT_ITEM1, FONT_MEDIUM, 0x000000FF, Font::PRINT_ALIGN_CENTER);	

	return doExit;
}

void padUpdate(int changed, int port, padData pData) {
	status = sound1->Status(infiniteVoice);

	if (pData.BTN_START)
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
}
