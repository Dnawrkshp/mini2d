/*
 * Keyboard.hpp
 *
 * Created on: November 15th, 2016
 * Author: Daniel Gerendasy
 */

#ifndef KEYBOARD_HPP_
#define KEYBOARD_HPP_

#include <string>							// wstring

#include <Mini2D/Mini2D.hpp>				// Mini2D definition
#include <Mini2D/Units.hpp>					// Vector2

class Keyboard
{
public:
	// The following enums are from sysutil/osk.h
	// I'm just wrapping them nicely in here
	typedef enum _kbLayoutMode_t {
		LAYOUTMODE_H_RIGHT =		0x0200,
		LAYOUTMODE_H_CENTER = 		0x0400,
		LAYOUTMODE_H_LEFT =			0x0800,
		LAYOUTMODE_V_BOTTOM =		0x1000,
		LAYOUTMODE_V_CENTER =		0x2000,
		LAYOUTMODE_V_TOP =			0x4000
	} KBLayoutMode;

	typedef enum _kbProhibit_t {
		PROHIBIT_SPACE =			0x0001,
		PROHIBIT_RETURN =			0x0002,
		PROHIBIT_INPUT_ANALOG =		0x0008
	} KBProhibit;

	typedef enum _kbPanelType_t {
		PANEL_DEFAULT = 			0x00000000,
		PANEL_DEFAULT_NO_JAPANESE =	0x00000200,
		PANEL_ALPHABET_FULL_WIDTH =	0x00800000,
		PANEL_ALPHABET =			0x01000000,
		PANEL_NUMERAL_FULL_WIDTH = 	0x04000000,
		PANEL_NUMERAL = 			0x08000000,
		PANEL_JAPANESE = 			0x00000100,
		PANEL_JAPANESE_HIRAGANA = 	0x00200000,
		PANEL_JAPANESE_KATAKANA = 	0x00400000,
		PANEL_ENGLISH = 			0x00000002,
		PANEL_FRENCH = 				0x00000008,
		PANEL_SPANISH = 			0x00000004,
		PANEL_DUTCH = 				0x00000020,
		PANEL_GERMAN = 				0x00000001,
		PANEL_RUSSIAN = 			0x00000080,
		PANEL_ITALIAN = 			0x00000010,
		PANEL_PORTUGUESE = 			0x00000040,
		PANEL_KOREAN = 				0x00001000,
		PANEL_DANISH = 				0x00020000,
		PANEL_SWEDISH = 			0x00040000,
		PANEL_NORWEGIAN = 			0x00080000,
		PANEL_FINNISH = 			0x00100000,
		PANEL_TRADITIONAL_CHINESE =	0x00004000,
		PANEL_SIMPLIFIED_CHINESE = 	0x00008000,
		PANEL_LATIN = 				0x02000000,
		PANEL_URL = 				0x10000000,
		PANEL_PASSWORD = 			0x20000000
	} KBPanelType;

	typedef enum _kbLanguage_t {
		LANG_KOREAN = 				0x00001000,
		LANG_DANISH = 				0x00020000,
		LANG_SWEDISH = 				0x00040000,
		LANG_NORWEGIAN = 			0x00080000,
		LANG_FINNISH = 				0x00100000
	} KBLanguage;

	typedef enum _kbKeyPanel_t {
		KEYPANEL_10 =				1,
		KEYPANEL_FULL =				2
	} KBKeyPanel;

	Keyboard(Mini2D * mini);
	~Keyboard();

	int MaxLength;							// Max number of characters					(default is 0)
	bool MaskPad;							// OSK Device mask (maskpad = 0xff)			(default is false)
	bool Dim;								// Whether or not to dim the background		(default is true)
	int KeyPanel;							// Key panel displayed						(default is KEYPANEL_FULL|KEYPANEL_10)
	int LayoutMode;							// Vertical and Horizontal alignment		(default is V_CENTER|H_CENTER)
	int Prohibit;							// Prohibit flags							(default is PROHIBIT_RETURN)
	KBPanelType InitialPanel;				// Panel to display when keyboard opens 	(default is PANEL_DEFAULT)
	unsigned int AllowedPanels;				// Panels keyboard may use 					(default is PANEL_DEFAULT)
	unsigned int Languages;					// Supported Languages						(default is PANEL_DEFAULT)

	Vector2 Location;						// Where to pop the keyboard

	/*
	 * Display:
	 *		Pops the On-screen keyboard
	 *
	 * title:
	 *		The title displayed on the top of the OSK (pass NULL for no string)
	 * startStr:
	 *		The starting string placed in the OSK's input box (pass NULL for no string)
	 */
	void Display(const std::wstring * title, const std::wstring * startStr);

	/*
	 * Abort:
	 *		Force closes the OSK
	 */
	void Abort();

	/*
	 * GetResult:
	 *		Get the result and append it to str
	 *
	 * str:
	 *		std::wstring to append result to
	 *
	 * Return:
	 *		Returns 0 if OSK is still open or the user aborted the input
	 */
	bool GetResult(std::wstring * str);

	/*
	 * WaitResult:
	 *		Waits for the OSK to close before getting the result and appending it to str
	 *
	 * str:
	 *		std::wstring to append result to
	 *
	 * Return:
	 *		Returns 0 if OSK is closed or the user aborted the input
	 */
	bool WaitResult(std::wstring * str);

	/*
	 * ValidResult:
	 *		Whether or not the user aborted the input
	 */
	bool ValidResult();

	/*
	 * Alive:
	 *		Whether or not the OSK is open
	 */
	bool Alive();

	/*
	 * SetDefault:
	 *		Sets default parameters
	 */
	void SetDefault();

	/*
	 * SetNumeric:
	 *		Sets OSK parameters to only accept numbers
	 */
	void SetNumeric();

	/*
	 * SetAlphanumeric:
	 *		Sets OSK parameters to only accept numbers and english alphabet characters
	 */
	void SetAlphanumeric();

	/*
	 * SetPassword:
	 *		Sets OSK parameters to display a password panel
	 */
	void SetPassword();

	/*
	 * SetURL:
	 *		Sets OSK parameters to only accept valid URLs
	 */
	void SetURL();

	/*
	 * SetMultiline:
	 *		Sets OSK parameters to enable multiline text input
	 */
	void SetMultiline();

	/*
	 * SetSingleline:
	 *		Sets OSK parameters to disable multiline text input
	 */
	void SetSingleline();
	
private:
	Mini2D * _mini;

	bool _newMessage;						// Whether or not inputFieldInfo.message points to dynamically allocated memory
	bool _newStart;							// Whether or not inputFieldInfo.startText points to dynamically allocated memory

	// Closes OSK politely
	void Close();
	// Cleanup allocations from last Display()
	void CleanOSK();
};


#endif /* KEYBOARD_HPP_ */