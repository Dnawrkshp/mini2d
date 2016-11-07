#include <stdio.h>
#include <io/pad.h>

#include <Mini2D/Mini2D.hpp>
#include <Mini2D/Font.hpp>

#include "comfortaa_regular_ttf.h"

int drawUpdate(float deltaTime, unsigned int frame);
void padUpdate(int changed, int port, padData pData);
void exit();

int doExit = 0;
Font *font1;

int main(s32 argc, const char* argv[]) {
	Mini2D mini((Mini2D::PadCallback_f)&padUpdate, (Mini2D::DrawCallback_f)&drawUpdate, (Mini2D::ExitCallback_f)&exit);
	
	font1 = new Font(&mini);
	if (font1->Load((void*)comfortaa_regular_ttf, comfortaa_regular_ttf_size))
		printf("error loading font\n");

	mini.SetAnalogDeadzone(15);
	mini.SetClearColor(0xFFFFFFFF);
	mini.SetAlphaState(1);
	mini.BeginDrawLoop();

	return 0;
}

int drawUpdate(float deltaTime, unsigned int frame) {
	font1->Print((char*)"Left align.", 0, 0, 0.04, 0.04, 0x000000FF, Font::PRINT_ALIGN_LEFT);	
	font1->Print((char*)"Right align.", 1, 0.96, 0.04, 0.04, 0x000000FF, Font::PRINT_ALIGN_RIGHT);	
	font1->Print((char*)"Center.", 0.5, 0.5, 0.04, 0.04, 0x000000FF, Font::PRINT_ALIGN_CENTER);

	return doExit;
}

void padUpdate(int changed, int port, padData pData) {
	if (pData.BTN_START)
		doExit = -1;
}

void exit() {
	printf("exiting\n");
}
