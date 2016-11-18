#include <stdio.h>
#include <io/pad.h>

#include <Mini2D/Mini2D.hpp>
#include <Mini2D/Font.hpp>

#include "comfortaa_regular_ttf.h"

int drawUpdate(float deltaTime, unsigned int frame);
void padUpdate(int changed, int port, padData pData);
void exit();

Mini2D * mini = NULL;

Font * font1 = NULL;

int doExit = 0;

// Font sizes
Vector2 FONT_SMALL;
// Font locations
Vector2 PRINT_TOPLEFT;
Vector2 PRINT_CENTER;
Vector2 PRINT_BOTTOMRIGHT;

std::wstring TEXT_LEFT = 	L"Left align.";
std::wstring TEXT_RIGHT = 	L"Right align.";
std::wstring TEXT_CENTER = 	L"Center.";

int main(s32 argc, const char* argv[]) {

	// Load Mini2D
	mini = new Mini2D((Mini2D::PadCallback_f)&padUpdate, (Mini2D::DrawCallback_f)&drawUpdate, (Mini2D::ExitCallback_f)&exit);

	// Initialize location and size vectors
	FONT_SMALL = Vector2(0.03*mini->MAXW, 0.03*mini->MAXH);

	PRINT_TOPLEFT = Vector2(0,0);
	PRINT_CENTER = Vector2(0.5*mini->MAXW, 0.5*mini->MAXH);
	PRINT_BOTTOMRIGHT = Vector2(mini->MAXW, mini->MAXH);

	// Load comfortaa font
	font1 = new Font(mini);
	if (font1->Load((void*)comfortaa_regular_ttf, comfortaa_regular_ttf_size))
		printf("error loading font\n");

	mini->SetAnalogDeadzone(15);
	mini->SetClearColor(0xFFFFFFFF);
	mini->SetAlphaState(1);
	mini->BeginDrawLoop();

	return 0;
}

int drawUpdate(float deltaTime, unsigned int frame) {
	font1->TextAlign = Font::PRINT_ALIGN_TOPLEFT;
	font1->PrintLine(NULL, &TEXT_LEFT, NULL, PRINT_TOPLEFT, FONT_SMALL);

	font1->TextAlign = Font::PRINT_ALIGN_BOTTOMRIGHT;
	font1->PrintLine(NULL, &TEXT_RIGHT, NULL, PRINT_BOTTOMRIGHT, FONT_SMALL);

	font1->TextAlign = Font::PRINT_ALIGN_CENTER;
	font1->PrintLine(NULL, &TEXT_CENTER, NULL, PRINT_CENTER, FONT_SMALL);
	
	return doExit;
}

void padUpdate(int changed, int port, padData pData) {
	if (pData.BTN_START && changed & Mini2D::BTN_CHANGED_START)
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
