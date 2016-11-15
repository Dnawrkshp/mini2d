#include <stdio.h>
#include <io/pad.h>
#include <math.h>

#include <Mini2D/Mini2D.hpp>
#include <Mini2D/Image.hpp>
#include <Mini2D/Font.hpp>
#include <Mini2D/Emitter.hpp>

#include "comfortaa_regular_ttf.h"

int drawUpdate(float deltaTime, unsigned int frame);
void padUpdate(int changed, int port, padData pData);
void exit();

Mini2D mini((Mini2D::PadCallback_f)&padUpdate, (Mini2D::DrawCallback_f)&drawUpdate, (Mini2D::ExitCallback_f)&exit);

int doExit = 0;
unsigned int emitterBackgroundPixel = 0xFF000000;

Emitter * emitter1;
Font * comfortaa;
Image * emitterBackground;

Vector2 CENTER(mini.MAXW*0.5, mini.MAXH*0.5);

const Vector2 FONT_SMALL(0.03*mini.MAXW,0.03*mini.MAXH);
const Vector2 PRINT_ITEM(CENTER.X,0.07*mini.MAXH);
Vector2 PRINT_INC(0,0.04*mini.MAXH);

int main(s32 argc, const char* argv[]) {
	emitter1 = new Emitter(&mini, 1000);										// Initialize new emitter with 1000 max particles
	emitter1->RangeDimensionW = 	Vector2(1,4);								// Range for width (default is 10-50)
	emitter1->RangeDimensionH = 	Vector2(1,4);								// Range for height (default is 10-50)
	emitter1->RangeVelocity = 		Vector2(-1,1) * 100;						// Range for X velocity per second (default is 0 to 10)
	emitter1->RangeVelocity = 		Vector2(-1,1) * 100;						// Range for Y velocity per second (default is 0 to 10)
	emitter1->RangeVelocityTheta =	Vector2(0,360);								// Range for particle velocity direction (default is 0-360)
	emitter1->RangeRotation =		Vector2(-60,60);							// Range for particle angle rotation per second
	emitter1->RangeTTL =			Vector2(5,10);								// Range for time to live (seconds)
	emitter1->RangeColorRed =		Vector2(0xFF, 0xFF);						// Range for color channel red (default is 0-255)
	emitter1->RangeColorGreen =		Vector2(0xFF, 0xFF);						// Range for color channel green (default is 0-255)
	emitter1->RangeColorBlue =		Vector2(0xFF, 0xFF);						// Range for color channel blue (deafult is 0-255)
	emitter1->MinParticles =		750;										// Minimum number of particles to spawn (will spawn between this and max)
	emitter1->Revive =				1;											// Revive particles that die part way through emission session
	emitter1->SkipExplosion =		1;											// Skip sequence from start point to outer reaches (smooths loop)
	emitter1->Clip =				RectangleF(CENTER, CENTER);					// Particles outside this box will not be drawn. Default is (x:0, y:0, w:MAXW, h:MAXH)

	emitterBackground = new Image(&mini);
	emitterBackground->Load((void*)&emitterBackgroundPixel, 1, 1);
	emitterBackground->DrawRegion = emitter1->Clip;

	comfortaa = new Font(&mini);
	comfortaa->Load((void*)comfortaa_regular_ttf, comfortaa_regular_ttf_size);

	mini.SetAnalogDeadzone(15);
	mini.SetClearColor(0xFFFFFFFF);
	mini.SetAlphaState(1);
	mini.BeginDrawLoop();

	return 0;
}

int drawUpdate(float deltaTime, unsigned int frame) {
	comfortaa->Print((char*)"Press CROSS to start the emission", PRINT_ITEM, FONT_SMALL, 0x000000FF, Font::PRINT_ALIGN_CENTER);
	comfortaa->Print((char*)"Press TRIANGLE to pause/resume the emission", PRINT_ITEM + PRINT_INC, FONT_SMALL, 0x000000FF, Font::PRINT_ALIGN_CENTER);
	comfortaa->Print((char*)"Press CIRCLE to stop the emission", PRINT_ITEM + (PRINT_INC*2.f), FONT_SMALL, 0x000000FF, Font::PRINT_ALIGN_CENTER);
	comfortaa->Print((char*)"Use the analog stick to direct the emission", PRINT_ITEM + (PRINT_INC * 3.f), FONT_SMALL, 0x000000FF, Font::PRINT_ALIGN_CENTER);

	emitterBackground->Draw(0xFFFFFFFF);
	if (emitter1->Status())
		emitter1->Draw(deltaTime);

	return doExit;
}

void padUpdate(int changed, int port, padData pData) {
	short x,y,dead=15;
	float theta, d=5;

	if (pData.BTN_START)
		doExit = -1;

	if (pData.BTN_CROSS && changed & Mini2D::BTN_CHANGED_CROSS) {
		emitter1->Start(CENTER, -1);
	}
	if (pData.BTN_CIRCLE && changed & Mini2D::BTN_CHANGED_CIRCLE) {
		emitter1->Stop();
	}
	if (pData.BTN_TRIANGLE && changed & Mini2D::BTN_CHANGED_TRIANGLE) {
		if (emitter1->Status() == Emitter::EMITTER_PAUSED)
			emitter1->Resume();
		else
			emitter1->Pause();
	}

	// Direct particles with analog stick
	x = 0x80 - pData.ANA_L_H;
	y = 0x80 - pData.ANA_L_V;
	if (x <= dead && x >= -dead)
		x = 0;
	if (y <= dead && y >= -dead)
		y = 0;

	if (x || y) {
		theta = atan2(pData.ANA_L_V-0x80,pData.ANA_L_H-0x80) * 180.f / 3.14159;
		emitter1->RangeVelocityTheta.X = theta-d;
		emitter1->RangeVelocityTheta.Y = theta+d;
	}
	else {
		emitter1->RangeVelocityTheta.X = 0;
		emitter1->RangeVelocityTheta.Y = 360;
	}

}

void exit() {
	printf("exiting\n");
	delete emitter1;
	delete emitterBackground;
}
