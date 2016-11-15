#include <stdio.h>
#include <io/pad.h>

#include <Mini2D/Mini2D.hpp>
#include <Mini2D/Dialog.hpp>
#include <Mini2D/Font.hpp>

#include "comfortaa_regular_ttf.h"

int drawUpdate(float deltaTime, unsigned int frame);
void padUpdate(int changed, int port, padData pData);
void exit();

void ProgressSingle(float deltaTime);
void ProgressDouble(float deltaTime);

Mini2D mini((Mini2D::PadCallback_f)&padUpdate, (Mini2D::DrawCallback_f)&drawUpdate, (Mini2D::ExitCallback_f)&exit);

int doExit = 0, doDialog = 0;
float doProgress[2];
Dialog *dialog1;
Font *font1;

const Vector2 FONT_MEDIUM(0.04*mini.MAXW, 0.04*mini.MAXH);

const Vector2 PRINT_ITEM0(0.15*mini.MAXW, 0.45*mini.MAXH);
const Vector2 PRINT_ITEM1(PRINT_ITEM0.X,  0.50*mini.MAXH);
const Vector2 PRINT_ITEM2(PRINT_ITEM0.X,  0.55*mini.MAXH);
const Vector2 PRINT_ITEM3(PRINT_ITEM0.X,  0.60*mini.MAXH);

int main(s32 argc, const char* argv[]) {
	doProgress[0] = -1.f;
	doProgress[1] = -1.f;

	dialog1 = new Dialog(&mini);
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
	font1->Print((char*)"Press Triangle to open Message Dialog", PRINT_ITEM0, FONT_MEDIUM);
	font1->Print((char*)"Press Square to open Error Dialog", PRINT_ITEM1, FONT_MEDIUM);
	font1->Print((char*)"Press Circle to open Single Progress Dialog", PRINT_ITEM2, FONT_MEDIUM);
	font1->Print((char*)"Press Cross to open Double Progress Dialog", PRINT_ITEM3, FONT_MEDIUM);

	switch (doDialog) {
		case 0:
			break;
		case 1:
			// Display a message dialog with yes/no options and wait for it to exit
			printf ("Pop: %d\n", dialog1->Pop((msgType)(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_YESNO | MSG_DIALOG_DISABLE_CANCEL_ON), "Would you rather select 'Yes' or 'No'?", 1));
			break;
		case 2:
			// Display an error message that will close in 5000 milliseconds and wait for it to exit
			printf ("Pop: %d\n", dialog1->PopError(0x1337C0DE, 1, 5000));
			break;
		case 3:
			// Display a single progress bar
			// Once complete, have it close automatically after 1000 milliseconds
			dialog1->PopSingleProgressBar("We are actively progressing.", "This should take about 10 seconds.", 1000);
			doProgress[0] = 0;
			break;
		case 4:
			// Display a double progress bar
			// Once complete, have it close automatically after 3000 milliseconds
			dialog1->PopDoubleProgressBar("We are actively progressing.", "This should take about 100 seconds.", "This should take about 10 seconds.", 3000);
			doProgress[0] = 0;
			doProgress[1] = 0;
			break;
	}
	doDialog = 0;

	if (doProgress[0] > -1 && doProgress[1] > -1)
		ProgressDouble(deltaTime);
	else if (doProgress[0] > -1)
		ProgressSingle(deltaTime);


	return doExit;
}

void padUpdate(int changed, int port, padData pData) {
	if (pData.BTN_START && changed & Mini2D::BTN_CHANGED_START)
		doExit = -1;

	if (pData.BTN_TRIANGLE && changed & Mini2D::BTN_CHANGED_TRIANGLE)
		doDialog = 1;
	if (pData.BTN_SQUARE && changed & Mini2D::BTN_CHANGED_SQUARE)
		doDialog = 2;
	if (pData.BTN_CIRCLE && changed & Mini2D::BTN_CHANGED_CIRCLE)
		doDialog = 3;
	if (pData.BTN_CROSS && changed & Mini2D::BTN_CHANGED_CROSS)
		doDialog = 4;
}

void exit() {
	printf("exiting\n");
}

void ProgressSingle(float deltaTime) {
	doProgress[0] += deltaTime * 10;
	dialog1->ProgressBarSet(0, (unsigned int)doProgress[0]);
	if (doProgress[0] >= 100)
		doProgress[0] = -1;
}

void ProgressDouble(float deltaTime) {
	doProgress[1] += deltaTime * 10;
	dialog1->ProgressBarSet(1, (unsigned int)doProgress[1]);
	if (doProgress[1] >= 100) {
		doProgress[1] = 0;
		dialog1->ProgressBarReset(1);

		doProgress[0] += 10.f;
		dialog1->ProgressBarSet(0, (unsigned int)doProgress[0]);

		if (doProgress[0] >= 100) {
			doProgress[0] = -1;
			doProgress[1] = -1;
		}
	}
}
