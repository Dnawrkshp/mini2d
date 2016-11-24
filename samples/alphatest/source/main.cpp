#include <stdio.h>
#include <io/pad.h>

#include <Mini2D/Mini2D.hpp>
#include <Mini2D/Image.hpp>

#include "alphatest_png.h"

int drawUpdate(float deltaTime, unsigned long frame);
void padUpdate(int changed, int port, padData pData);
void exit();

Mini2D * mini = NULL;
Image * img1 = NULL;

int doExit = 0;
int doSaturate = 0;

Vector2 ALPHATEST_LOC;
Vector2 ALPHATEST_SIZE;

int main(s32 argc, const char* argv[]) {

	// Initialize Mini2D
	mini = new Mini2D((Mini2D::PadCallback_f)&padUpdate, (Mini2D::DrawCallback_f)&drawUpdate, (Mini2D::ExitCallback_f)&exit);

	// Initialize location and size vectors
	ALPHATEST_LOC = Vector2(0.5*mini->MAXW,0.5*mini->MAXH);
	ALPHATEST_SIZE = Vector2(0.5*mini->MAXW,0.5*mini->MAXH);

	// Load image
	img1 = new Image(mini);
	img1->Load((void*)alphatest_png, alphatest_png_size, Image::IMAGE_TYPE_PNG);
	img1->DrawRegion.Location = ALPHATEST_LOC;
	img1->DrawRegion.Dimension = ALPHATEST_SIZE;

	mini->SetAnalogDeadzone(15);
	mini->SetClearColor(0x23B2D7FF);
	mini->SetAlphaState(1);
	mini->BeginDrawLoop();

	return 0;
}

int drawUpdate(float deltaTime, unsigned long frame) {
	img1->Draw(doSaturate ? 0xFFFFFFFF : 0xFFFFFFC0);
	return doExit;
}

void padUpdate(int changed, int port, padData pData) {
	if (pData.BTN_START && changed & Mini2D::BTN_CHANGED_START)
		doExit = -1;

	if (pData.BTN_LEFT && changed & Mini2D::BTN_CHANGED_LEFT)
		doSaturate = !doSaturate;
	if (pData.BTN_RIGHT && changed & Mini2D::BTN_CHANGED_RIGHT)
		doSaturate = !doSaturate;
}

void exit() {
	printf("exiting\n");

	if (img1) {
		delete img1;
		img1 = NULL;
	}

	if (mini) {
		delete mini;
		mini = NULL;
	}
}
