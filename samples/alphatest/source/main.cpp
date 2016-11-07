#include <stdio.h>
#include <io/pad.h>

#include <Mini2D/Mini2D.hpp>
#include <Mini2D/Image.hpp>

#include "alphatest_png.h"

int drawUpdate(float deltaTime, unsigned int frame);
void padUpdate(int changed, int port, padData pData);
void exit();

int doExit = 0;
int doSaturate = 0;
Image *img1;

int main(s32 argc, const char* argv[]) {
	Mini2D mini((Mini2D::PadCallback_f)&padUpdate, (Mini2D::DrawCallback_f)&drawUpdate, (Mini2D::ExitCallback_f)&exit);
	
	img1 = new Image(&mini);
	img1->Load((void*)alphatest_png, alphatest_png_size, Image::IMAGE_TYPE_PNG);

	mini.SetAnalogDeadzone(15);
	mini.SetClearColor(0x23B2D7FF);
	mini.SetAlphaState(1);
	mini.BeginDrawLoop();

	return 0;
}

int drawUpdate(float deltaTime, unsigned int frame) {
	img1->Draw(0, 0, 1, 1, doSaturate ? 0xFFFFFFFF : 0xFFFFFFC0,Image::DRAW_CENTER);
	return doExit;
}

void padUpdate(int changed, int port, padData pData) {
	if (pData.BTN_START)
		doExit = -1;

	if (pData.BTN_LEFT && changed & Mini2D::BTN_CHANGED_LEFT)
		doSaturate = !doSaturate;
	if (pData.BTN_RIGHT && changed & Mini2D::BTN_CHANGED_RIGHT)
		doSaturate = !doSaturate;
}

void exit() {
	printf("exiting\n");
}
