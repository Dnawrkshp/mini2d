/*
 * Keyboard.cpp
 *
 *  Created on: November 15th, 2016
 *  Author: Daniel Gerendasy
 */

#include <sysutil/osk.h>                         // On-screen keyboard functions
#include <sysutil/sysutil.h>                     // SysUtil Callback status definitions
#include <io/camera.h>                           // sysMemContainerCreate(), sysMemContainerDestroy()
#include <wchar.h>                               // wprintf


#include <Mini2D/Keyboard.hpp>                   // Class definition

namespace Mini2D {

	// Handles SysUtil event callback
	static void sys_callback(uint64_t status, uint64_t param, void* userdata);

	// Whether or not the On-screen keyboard is open
	static bool oskOpen = 0;

	// Parameters for OSK
	static sys_mem_container_t container = 0;
	static oskParam param;
	static oskInputFieldInfo inputFieldInfo;
	static oskCallbackReturnParam result;


	//---------------------------------------------------------------------------
	// Init Functions
	//---------------------------------------------------------------------------
	Keyboard::Keyboard(Mini * mini) : _mini(mini) {
		// Allocate 2 mb
		if (!container && sysMemContainerCreate(&container, 0x200000)) {
			_mini = NULL;
			printf("ERROR sysMemContainerCreate() in Keyboard::Keyboard()\n");
			return;
		}

		MaxLength = 0;
		_newMessage = 0;
		_newStart = 0;

		SetDefault();

		result.str = NULL;

		sysUtilRegisterCallback(SYSUTIL_EVENT_SLOT1, sys_callback, this);
	}

	Keyboard::~Keyboard() {
		if (oskOpen)
			Abort();

		if (container)
			sysMemContainerDestroy (container);
		if (result.str) {
			delete [] result.str;
			result.str = NULL;
		}
		_mini = NULL;
	}

	//---------------------------------------------------------------------------
	// Set Standard OSK Functions
	//---------------------------------------------------------------------------
	void Keyboard::SetDefault() {
		KeyPanel = Keyboard::KEYPANEL_FULL | Keyboard::KEYPANEL_10;
		LayoutMode = Keyboard::LAYOUTMODE_H_CENTER | Keyboard::LAYOUTMODE_V_CENTER;
		InitialPanel = Keyboard::PANEL_DEFAULT;
		AllowedPanels = Keyboard::PANEL_DEFAULT;
		Dim = 1;
		MaskPad = 0;
		Prohibit = Keyboard::PROHIBIT_RETURN;
		Languages = Keyboard::PANEL_DEFAULT;
	}

	void Keyboard::SetNumeric() {
		InitialPanel = Keyboard::PANEL_NUMERAL;
		AllowedPanels = Keyboard::PANEL_NUMERAL;
	}

	void Keyboard::SetAlphanumeric() {
		InitialPanel = Keyboard::PANEL_ENGLISH;
		AllowedPanels = Keyboard::PANEL_ENGLISH;
	}

	void Keyboard::SetPassword() {
		InitialPanel = Keyboard::PANEL_PASSWORD;
		AllowedPanels = Keyboard::PANEL_PASSWORD;
		Prohibit = 0;
	}

	void Keyboard::SetURL() {
		InitialPanel = Keyboard::PANEL_URL;
		AllowedPanels = Keyboard::PANEL_URL;
		Prohibit = 0;
	}

	void Keyboard::SetMultiline() {
		Prohibit &= ~Keyboard::PROHIBIT_RETURN;
	}

	void Keyboard::SetSingleline() {
		Prohibit |= Keyboard::PROHIBIT_RETURN;
	}

	//---------------------------------------------------------------------------
	// Display/Close Functions
	//---------------------------------------------------------------------------
	void Keyboard::Display(const std::wstring * title, const std::wstring * startStr) {
		int lang,i;
		int tLen = title?title->length():0;
		int sLen = startStr?startStr->length():0;

		if (!_mini || oskOpen)
			return;

		// Set maxlength
		inputFieldInfo.maxLength = MaxLength;

		// If a title is given, copy it to a new buffer
		if (tLen>0) {
			if (_newMessage && inputFieldInfo.message)
				delete [] inputFieldInfo.message;
			inputFieldInfo.message = new u16[tLen+1]();
			for (i=0;i<tLen;i++)
				inputFieldInfo.message[i] = title->at(i);

			_newMessage = 1;
		}
		else {
			inputFieldInfo.message = (u16*)L"";
			_newMessage = 0;
		}

		// If a start string is given, copy it to a new buffer
		if (sLen>0) {
			if (_newStart && inputFieldInfo.startText)
				delete [] inputFieldInfo.startText;
			inputFieldInfo.startText = new u16[sLen+1]();
			for (i=0;i<sLen;i++)
				inputFieldInfo.startText[i] = startStr->at(i);

			_newStart = 1;
		}
		else {
			inputFieldInfo.startText = (u16*)L"";
			_newStart = 0;
		}

		if (result.str)
			delete [] result.str;
		result.str = new u16[MaxLength+1]();

		// Set location
		param.controlPoint.x = Location.X;
		param.controlPoint.y = Location.Y;
		oskSetLayoutMode(LayoutMode);

		// Set panels
		param.allowedPanels = AllowedPanels;
		param.firstViewPanel = InitialPanel;
		oskSetKeyLayoutOption(KeyPanel);

		// Set languages
		lang = Languages;
		for (i=0;i<32;i++) {
			if (lang & 1)
				oskAddSupportLanguage(1 << i);
			lang>>=1;
		}

		// Set prohibit flags
		param.prohibitFlags = Prohibit;

		oskLoadAsync(container, &param, &inputFieldInfo);

		// Disable dimmer
		if (!Dim)
			oskDisableDimmer();
		// Mask pad
		if (MaskPad)
			oskSetDeviceMask(OSK_DEVICE_MASK_PAD);

		oskOpen = 1;
	}

	void Keyboard::Close() {
		if (!oskOpen)
			return;

		result.len = MaxLength;
		oskUnloadAsync(&result);
		oskOpen = 0;
		CleanOSK();
	}

	void Keyboard::Abort() {
		if (oskOpen) {
			oskAbort();
			oskOpen = 0;
		}

		CleanOSK();
	}

	void Keyboard::CleanOSK() {
		if (_newMessage && inputFieldInfo.message) {
			delete [] inputFieldInfo.message;
			inputFieldInfo.message = NULL;
			_newMessage = 0;
		}
		if (_newStart && inputFieldInfo.startText) {
			delete [] inputFieldInfo.startText;
			inputFieldInfo.startText = NULL;
			_newStart = 0;
		}
	}

	//---------------------------------------------------------------------------
	// Get Result/Status Functions
	//---------------------------------------------------------------------------
	bool Keyboard::GetResult(std::wstring * str) {
		int i = 0;

		if (oskOpen || !str || !result.str)
			return 0;
		if (result.res != 0)
			return 0;

		while (i < result.len && result.str[i]) {
			(*str) += (wchar_t)result.str[i];
			i++;
		}
		return 1;

		return 0;
	}

	bool Keyboard::WaitResult(std::wstring * str) {
		if (!oskOpen)
			return GetResult(str);

		while (oskOpen) {
			sysUtilCheckCallback();
			_mini->Flip();
		}

		return GetResult(str);
	}

	bool Keyboard::ValidResult() {
		if (oskOpen)
			return 0;

		return result.res == 0;
	}

	bool Keyboard::Alive() {
		return oskOpen;
	}

	//---------------------------------------------------------------------------
	// SysUtil callback
	//---------------------------------------------------------------------------
	static void sys_callback(uint64_t status, uint64_t param, void* userdata) {
		Keyboard * kb = NULL;
		if (!userdata)
			return;

		kb = (Keyboard*)userdata;

		switch (status) {
			case SYSUTIL_EXIT_GAME: //0x0101
				kb->Abort();
				break;
			case SYSUTIL_MENU_OPEN: //0x0131

				break;
			case SYSUTIL_MENU_CLOSE: //0x0132

				break;
			case SYSUTIL_DRAW_BEGIN:

				break;
			case SYSUTIL_DRAW_END:

				break;
			case SYSUTIL_OSK_DONE:
				result.len = inputFieldInfo.maxLength;
				oskUnloadAsync(&result);
				kb->Abort();
				break;
			case SYSUTIL_OSK_LOADED:
				oskOpen = 1;
				break;
			default:
				break;
		}
	}

}
