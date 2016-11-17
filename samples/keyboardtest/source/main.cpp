#include <stdio.h>
#include <io/pad.h>
#include <stdlib.h>
#include <string>

#include <Mini2D/Mini2D.hpp>
#include <Mini2D/Font.hpp>
#include <Mini2D/Keyboard.hpp>

#include "OpenSans_Regular_ttf.h"

int drawUpdate(float deltaTime, unsigned int frame);
void padUpdate(int changed, int port, padData pData);
void exit();

Mini2D mini((Mini2D::PadCallback_f)&padUpdate, (Mini2D::DrawCallback_f)&drawUpdate, (Mini2D::ExitCallback_f)&exit);

int doExit = 0;
bool kbWait = 0;

Font * OpenSans;
Keyboard * kb1;

const Vector2 FONT_SMALL(0.03*mini.MAXW, 0.03*mini.MAXH);

const Vector2 LOC_CONTAINER(0.5*mini.MAXW, 0.5*mini.MAXH);
const Vector2 SIZE_CONTAINER(0.5*mini.MAXW, 0.25*mini.MAXH);
const Vector2 LOC_TEXT(LOC_CONTAINER.X-SIZE_CONTAINER.X/2 + 2,LOC_CONTAINER.Y-SIZE_CONTAINER.Y/2);

std::wstring title = L"Enter something please";
std::wstring start = L"Press TRIANGLE to open Keyboard";

int main(s32 argc, const char* argv[]) {
	OpenSans = new Font(&mini);
	if (OpenSans->Load((void*)OpenSans_Regular_ttf, OpenSans_Regular_ttf_size, 64, 64))
		printf("error loading font\n");

	OpenSans->Container.Location.Set(LOC_CONTAINER);
	OpenSans->Container.Dimension.Set(SIZE_CONTAINER);

	kb1 = new Keyboard(&mini);
	kb1->MaxLength = 128;
	kb1->Languages = 	Keyboard::PANEL_ALPHABET_FULL_WIDTH |
						Keyboard::PANEL_ALPHABET |
						Keyboard::PANEL_NUMERAL_FULL_WIDTH |
						Keyboard::PANEL_NUMERAL |
						Keyboard::PANEL_ENGLISH |
						Keyboard::PANEL_FRENCH |
						Keyboard::PANEL_SPANISH |
						Keyboard::PANEL_DUTCH |
						Keyboard::PANEL_GERMAN |
						Keyboard::PANEL_RUSSIAN |
						Keyboard::PANEL_ITALIAN |
						Keyboard::PANEL_PORTUGUESE |
						Keyboard::PANEL_KOREAN |
						Keyboard::PANEL_DANISH |
						Keyboard::PANEL_SWEDISH |
						Keyboard::PANEL_NORWEGIAN |
						Keyboard::PANEL_FINNISH;
	kb1->AllowedPanels = kb1->Languages;
	kb1->InitialPanel = Keyboard::PANEL_ENGLISH;
	kb1->Prohibit = 0;

	mini.SetAnalogDeadzone(15);
	mini.SetClearColor(0xFF000000);
	mini.SetAlphaState(1);
	mini.BeginDrawLoop();

	return 0;
}

int drawUpdate(float deltaTime, unsigned int frame) {
	mini.DrawRectangle(OpenSans->Container.X(), OpenSans->Container.Y(), OpenSans->Container.X(), OpenSans->Container.Y(), 0, OpenSans->Container.W(), OpenSans->Container.H(), 0xC0C0C0FF, 0);
	OpenSans->PrintLines(&start, 0, LOC_TEXT, FONT_SMALL, 1, 1, 0x000000FF, Font::PRINT_ALIGN_TOPLEFT);
	
	if (kbWait && !kb1->Alive()) {
		// If user submitted the text
		if (kb1->ValidResult()) {
			// Clear the buffer so we don't append
			start.clear();
			kb1->GetResult(&start);
		}

		kbWait = 0;
	}
	else if (kbWait) {
		// Draw background items when OSK is active
	}
	else {
		// Normal draw
	}

	return doExit;
}

void padUpdate(int changed, int port, padData pData) {
	if (pData.BTN_START && changed & Mini2D::BTN_CHANGED_START)
		doExit = -1;

	if (pData.BTN_TRIANGLE && changed & Mini2D::BTN_CHANGED_TRIANGLE) {
		kb1->Display(&title, &start);
		kbWait = 1;
	}
}

void exit() {
	printf("exiting\n");
}
