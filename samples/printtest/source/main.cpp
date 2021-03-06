#include <stdio.h>
#include <io/pad.h>

#include <Mini2D/Mini.hpp>
#include <Mini2D/Font.hpp>

#include "comfortaa_regular_ttf.h"

// 
using namespace Mini2D;

// callbacks
int drawUpdate(float deltaTime, unsigned long frame);
void padUpdate(int changed, int port, padData pData);
void exit();

Mini * mini = NULL;

Font * font1 = NULL;

int doExit = 0;

// Font sizes
float FONT_LARGE;
// Font locations
Vector2 PRINT_TOPLEFT;
Vector2 PRINT_CENTER;
Vector2 PRINT_BOTTOMRIGHT;

std::wstring TEXT_LEFT = 	L"Left align.";
std::wstring TEXT_RIGHT = 	L"Right align.";
std::wstring TEXT_CENTER = 	L"Center.";

int main(s32 argc, const char* argv[]) {

	// Load Mini
	mini = new Mini((Mini::PadCallback_f)&padUpdate, (Mini::DrawCallback_f)&drawUpdate, (Mini::ExitCallback_f)&exit);

	// Initialize location and size vectors
	FONT_LARGE = 0.02*mini->MAXW;

	PRINT_TOPLEFT = Vector2(0,0);
	PRINT_CENTER = Vector2(0.5*mini->MAXW, 0.5*mini->MAXH);
	PRINT_BOTTOMRIGHT = Vector2(mini->MAXW, mini->MAXH);

	// Load comfortaa font
	font1 = new Font(mini);
	font1->SpacingOffset = 1;
	if (font1->Load((void*)comfortaa_regular_ttf, comfortaa_regular_ttf_size, 64))
		printf("error loading font\n");

	mini->SetAnalogDeadzone(15);
	mini->SetClearColor(0xFFFFFFFF);
	mini->SetAlphaState(1);
	mini->BeginDrawLoop();

	return 0;
}

int drawUpdate(float deltaTime, unsigned long frame) {
	font1->TextAlign = Font::PRINT_ALIGN_TOPLEFT;
	font1->PrintLine(TEXT_LEFT, NULL, PRINT_TOPLEFT, FONT_LARGE);

	font1->TextAlign = Font::PRINT_ALIGN_BOTTOMRIGHT;
	font1->PrintLine(TEXT_RIGHT, NULL, PRINT_BOTTOMRIGHT, FONT_LARGE);

	font1->TextAlign = Font::PRINT_ALIGN_CENTER;
	font1->PrintLine(TEXT_CENTER, NULL, PRINT_CENTER, FONT_LARGE);
	
	return doExit;
}

void padUpdate(int changed, int port, padData pData) {
	if (pData.BTN_START && changed & Mini::BTN_CHANGED_START)
		doExit = -1;
}

void exit() {
	printf("exiting\n");

	if (font1) {
		delete font1;
		font1 = NULL;
	}

	if (mini) {
		delete mini;
		mini = NULL;
	}
}
