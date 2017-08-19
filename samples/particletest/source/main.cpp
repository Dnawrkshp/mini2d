#include <stdio.h>
#include <io/pad.h>
#include <math.h>

#include <Mini2D/Mini2D.hpp>
#include <Mini2D/Image.hpp>
#include <Mini2D/Font.hpp>
#include <Mini2D/Emitter.hpp>

#include "comfortaa_regular_ttf.h"

int drawUpdate(float deltaTime, unsigned long frame);
void padUpdate(int changed, int port, padData pData);
void exit();

Mini2D * mini = NULL;

Emitter * emitter1 = NULL;
Font * comfortaa = NULL;
Image * emitterBackground = NULL;

int doExit = 0;
unsigned int emitterBackgroundPixel = 0xFF000000;

// Center location
Vector2 CENTER;

// Font sizes
float FONT_LARGE;
// Font locations
Vector2 PRINT_ITEM;
Vector2 PRINT_INC;

std::wstring TEXT_START = 		L"Press CROSS to start the emission";
std::wstring TEXT_PAUSERESUME = L"Press TRIANGLE to pause/resume the emission";
std::wstring TEXT_STOP =		L"Press CIRCLE to stop the emission";
std::wstring TEXT_DIRECT =		L"Use the analog stick to direct the emission";

int main(s32 argc, const char* argv[]) {
	// Load Mini2D
	mini = new Mini2D((Mini2D::PadCallback_f)&padUpdate, (Mini2D::DrawCallback_f)&drawUpdate, (Mini2D::ExitCallback_f)&exit);

	// Initialize location and size vectors
	CENTER = Vector2(mini->MAXW*0.5, mini->MAXH*0.5);

	FONT_LARGE = 0.02*mini->MAXW;

	PRINT_ITEM = Vector2(CENTER.X,0.07*mini->MAXH);
	PRINT_INC = Vector2(0,0.04*mini->MAXH);

	// Load emitter
	emitter1 = new Emitter(mini, 1000);											// Initialize new emitter with 1000 max particles
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

	// Load background image
	emitterBackground = new Image(mini);
	emitterBackground->Load((void*)&emitterBackgroundPixel, 1, 1);
	emitterBackground->DrawRegion = emitter1->Clip;

	// Load comfortaa font
	comfortaa = new Font(mini);
	comfortaa->Load((void*)comfortaa_regular_ttf, comfortaa_regular_ttf_size, 64);
	comfortaa->TextAlign = Font::PRINT_ALIGN_CENTER;

	mini->SetAnalogDeadzone(15);
	mini->SetClearColor(0xFFFFFFFF);
	mini->SetAlphaState(1);
	mini->BeginDrawLoop();

	return 0;
}

int drawUpdate(float deltaTime, unsigned long frame) {
	comfortaa->PrintLine(NULL, &TEXT_START, NULL, PRINT_ITEM, FONT_LARGE);
	comfortaa->PrintLine(NULL, &TEXT_PAUSERESUME, NULL, PRINT_ITEM + PRINT_INC, FONT_LARGE);
	comfortaa->PrintLine(NULL, &TEXT_STOP, NULL, PRINT_ITEM + (PRINT_INC*2.f), FONT_LARGE);
	comfortaa->PrintLine(NULL, &TEXT_DIRECT, NULL, PRINT_ITEM + (PRINT_INC * 3.f), FONT_LARGE);

	emitterBackground->Draw(0xFFFFFFFF);
	if (emitter1->Status())
		emitter1->Draw(deltaTime);

	return doExit;
}

void padUpdate(int changed, int port, padData pData) {
	short x,y,dead=15;
	float theta, d=5;

	if (pData.BTN_START && changed & Mini2D::BTN_CHANGED_START)
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
	
	if (emitter1) {
		delete emitter1;
		emitter1 = NULL;
	}

	if (emitterBackground) {
		delete emitterBackground;
		emitterBackground = NULL;
	}

	if (comfortaa) {
		delete comfortaa;
		comfortaa = NULL;
	}

	if (mini) {
		delete mini;
		mini = NULL;
	}
}
