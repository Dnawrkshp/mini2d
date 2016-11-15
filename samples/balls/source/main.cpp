#include <stdio.h>
#include <io/pad.h>
#include <math.h>

#include <Mini2D/Mini2D.hpp>
#include <Mini2D/Image.hpp>

#include "paper_jpg.h"
#include "ball_png.h"
#include "cannon_png.h"

int drawUpdate(float deltaTime, unsigned int frame);
void padUpdate(int changed, int port, padData pData);
void exit();

int doExit = 0;
Image *paper;
Image *ball;
Image *cannon;

// The cannon image isn't rendered upright so we have to offset the analog stick's angle by the cannon's starting angle
// This is an estimation that looks good enough rendered
const float CANNON_ANGLE_OFFSET = 35.f;

Mini2D mini((Mini2D::PadCallback_f)&padUpdate, (Mini2D::DrawCallback_f)&drawUpdate, (Mini2D::ExitCallback_f)&exit);

Vector2 CENTER(0.5*mini.MAXW,0.5*mini.MAXH);
Vector2 SIZE(0.1*mini.MAXW,0.1*mini.MAXW);
Vector2 SIZE_FULL(mini.MAXW,mini.MAXW);

int main(s32 argc, const char* argv[]) {
	paper = new Image(&mini);
	paper->Load((void*)paper_jpg, paper_jpg_size, Image::IMAGE_TYPE_JPG);
	paper->DrawRegion.Location = CENTER;
	paper->DrawRegion.Dimension = SIZE_FULL;

	ball = new Image(&mini);
	ball->Load((void*)ball_png, ball_png_size, Image::IMAGE_TYPE_PNG);
	ball->DrawRegion.Location = CENTER + Vector2(64, 64);
	ball->DrawRegion.Dimension = Vector2(32, 32);

	cannon = new Image(&mini);
	cannon->Load((void*)cannon_png, cannon_png_size, Image::IMAGE_TYPE_PNG);
	cannon->DrawRegion.Location = CENTER;
	cannon->DrawRegion.Dimension = Vector2(128, 128);
	cannon->DrawRegion.Anchor = CENTER + Vector2(-32, 32);
	cannon->DrawRegion.UseAnchor = 1;
	cannon->DrawRegion.RectangleAngle = 315;
	cannon->DrawRegion.AnchorAngle = 315;

	mini.SetAnalogDeadzone(15);
	mini.SetClearColor(0x23B2D7FF);
	mini.SetAlphaState(1);
	mini.BeginDrawLoop();

	return 0;
}

int drawUpdate(float deltaTime, unsigned int frame) {
	paper->Draw(0xFFFFFFFF);
	cannon->Draw(0xFFFFFFFF);
	ball->Draw(0xFFFFFFFF);
	return doExit;
}

void padUpdate(int changed, int port, padData pData) {
	float theta;
	short x,y,dead=10;

	if (pData.BTN_START && changed & Mini2D::BTN_CHANGED_START)
		doExit = -1;

	// Direct cannon
	x = 0x80 - pData.ANA_L_H;
	y = 0x80 - pData.ANA_L_V;
	if (x <= dead && x >= -dead)
		x = 0;
	if (y <= dead && y >= -dead)
		y = 0;

	if (x || y) {
		theta = atan2(pData.ANA_L_V-0x80,pData.ANA_L_H-0x80) * 180.f / 3.14159;
		cannon->DrawRegion.RectangleAngle = theta + CANNON_ANGLE_OFFSET;
		cannon->DrawRegion.AnchorAngle = theta + CANNON_ANGLE_OFFSET;
	}
}

void exit() {
	printf("exiting\n");
}
