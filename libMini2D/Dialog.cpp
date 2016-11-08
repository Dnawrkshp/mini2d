/*
 * Dialog.cpp
 *
 *  Created on: November 8th, 2016
 *  Author: Daniel Gerendasy
 */

#include <sysutil/sysutil.h>					// sysUtilCheckCallback()
#include <stdio.h>

#include <tiny3d.h>
#include <Mini2D/Dialog.hpp>					// Class definition

static void msgDialogHandler(msgButton button, void *usrdata);

//---------------------------------------------------------------------------
// Init Functions
//---------------------------------------------------------------------------
Dialog::Dialog(Mini2D * mini) : _mini(mini) {
	DialogResponse = MSG_DIALOG_BTN_INVALID;
}

Dialog::~Dialog() {

}

//---------------------------------------------------------------------------
// Pop Functions
//---------------------------------------------------------------------------
msgButton Dialog::Pop(msgType type, const char * message, bool wait) {
	msgDialogOpen2(type, message, msgDialogHandler, this, NULL);
	DialogResponse = MSG_DIALOG_BTN_INVALID;
	while(DialogResponse == MSG_DIALOG_BTN_INVALID && wait){
		Flip(1);
	}

	return DialogResponse;
}

msgButton Dialog::Pop(msgType type, const char * message, bool wait, unsigned int ms) {
	msgDialogOpen2(type, message, msgDialogHandler, this, NULL);
	msgDialogClose(ms);
	DialogResponse = MSG_DIALOG_BTN_INVALID;
	while(DialogResponse == MSG_DIALOG_BTN_INVALID && wait){
		Flip(1);
	}

	return DialogResponse;
}

msgButton Dialog::PopError(unsigned int errorCode, bool wait) {
	msgDialogOpenErrorCode(errorCode,msgDialogHandler, this, NULL);
	DialogResponse = MSG_DIALOG_BTN_INVALID;
	while(DialogResponse == MSG_DIALOG_BTN_INVALID && wait){
		Flip(1);
	}

	return DialogResponse;
}

msgButton Dialog::PopError(unsigned int errorCode, bool wait, unsigned int ms) {
	msgDialogOpenErrorCode(errorCode,msgDialogHandler, this, NULL);
	msgDialogClose(ms);
	DialogResponse = MSG_DIALOG_BTN_INVALID;
	while(DialogResponse == MSG_DIALOG_BTN_INVALID && wait){
		Flip(1);
	}

	return DialogResponse;
}

void Dialog::PopSingleProgressBar(const char * dialogMessage, const char * progressMessage, unsigned int msAfterComplete) {
	_msAfterComplete = msAfterComplete;

	msgDialogOpen2((msgType)(MSG_DIALOG_SINGLE_PROGRESSBAR | MSG_DIALOG_DISABLE_CANCEL_ON), dialogMessage, msgDialogHandler, this, NULL);
	msgDialogProgressBarSetMsg(MSG_PROGRESSBAR_INDEX0, progressMessage);
	msgDialogProgressBarReset(MSG_PROGRESSBAR_INDEX0);
	msgDialogProgressBarInc(MSG_PROGRESSBAR_INDEX0, 0);
	Flip();
}

void Dialog::PopDoubleProgressBar(const char * dialogMessage, const char * progressMessage1, const char * progressMessage2, unsigned int msAfterComplete) {
	_msAfterComplete = msAfterComplete;

	msgDialogOpen2((msgType)(MSG_DIALOG_DOUBLE_PROGRESSBAR | MSG_DIALOG_DISABLE_CANCEL_ON), dialogMessage, msgDialogHandler, this, NULL);
	msgDialogProgressBarSetMsg(MSG_PROGRESSBAR_INDEX0, progressMessage1);
	msgDialogProgressBarSetMsg(MSG_PROGRESSBAR_INDEX1, progressMessage2);
	msgDialogProgressBarReset(MSG_PROGRESSBAR_INDEX0);
	msgDialogProgressBarReset(MSG_PROGRESSBAR_INDEX1);
	msgDialogProgressBarInc(MSG_PROGRESSBAR_INDEX0, 0);
	msgDialogProgressBarInc(MSG_PROGRESSBAR_INDEX1, 0);
	Flip();
}

//---------------------------------------------------------------------------
// Progress Bar Post-Pop Functions
//---------------------------------------------------------------------------
void Dialog::ProgressBarIncrease(bool index, unsigned int percent) {
	_progressBarPercent[index] += percent;

	msgDialogProgressBarInc(index ? MSG_PROGRESSBAR_INDEX1 : MSG_PROGRESSBAR_INDEX0, percent);
	Flip();

	if (!index && _progressBarPercent[index?1:0] >= 100 && _msAfterComplete)
		msgDialogClose(_msAfterComplete);
}

void Dialog::ProgressBarSet(bool index, unsigned int percent) {
	ProgressBarIncrease(index, percent - _progressBarPercent[index]);
}

void Dialog::ProgressBarSetMessage(bool index, const char * progressMessage) {
	msgDialogProgressBarSetMsg(index ? MSG_PROGRESSBAR_INDEX1 : MSG_PROGRESSBAR_INDEX0, progressMessage);
	Flip();
}

void Dialog::ProgressBarReset(bool index) {
	_progressBarPercent[index] = 0;
	msgDialogProgressBarReset(index ? MSG_PROGRESSBAR_INDEX1 : MSG_PROGRESSBAR_INDEX0);
	Flip();
}

void Dialog::ProgressBarClose() {
	msgDialogAbort();
}


//---------------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------------
void Dialog::Flip(bool flip) {
	sysUtilCheckCallback();
	if (flip)
		_mini->Flip();
}

static void msgDialogHandler(msgButton button, void *usrdata) {
	if (!usrdata)
		return;

	printf("button=%d\n", button);
	Dialog * dlg = (Dialog *)usrdata;
	dlg->DialogResponse = button;
	if (button)
		msgDialogAbort();
}
