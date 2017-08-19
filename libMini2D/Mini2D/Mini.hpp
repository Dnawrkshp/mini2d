/*
 * Mini2D.hpp
 *
 *  Created on: November 4th, 2016
 *  Author: Daniel Gerendasy
 */

#ifndef MINI2D_MINI_HPP_
#define MINI2D_MINI_HPP_

#include <io/pad.h>                              // Pad functions
#include <sys/time.h>                            // For FPS / deltaTime calculation
#include <sys/spu.h>                             // sysSpuImage type

extern "C" {
    #include <soundlib/audioplayer.h>            // SPU sound playback
}

#include <ft2build.h>                            // Freetype header
#include <freetype/freetype.h>                   // FT_Library

#define SPU_SIZE(x) (((x)+127) & ~127)

namespace Mini2D {

	class Mini {
	public:
		typedef enum _padChangedInfo_t {
			UNCHANGED = 						0,
			BTN_CHANGED_LEFT = 					1 << 0,
			BTN_CHANGED_RIGHT = 				1 << 1,
			BTN_CHANGED_UP = 					1 << 2,
			BTN_CHANGED_DOWN = 					1 << 3,
			BTN_CHANGED_R1 = 					1 << 4,
			BTN_CHANGED_L1 = 					1 << 5,
			BTN_CHANGED_R2 = 					1 << 6,
			BTN_CHANGED_L2 = 					1 << 7,
			BTN_CHANGED_CROSS = 				1 << 8,
			BTN_CHANGED_SQUARE = 				1 << 9,
			BTN_CHANGED_TRIANGLE = 				1 << 10,
			BTN_CHANGED_CIRCLE = 				1 << 11,
			BTN_CHANGED_START = 				1 << 12,
			BTN_CHANGED_SELECT = 				1 << 13,
			BTN_CHANGED_L3 = 					1 << 14,
			BTN_CHANGED_R3 = 					1 << 15,
			ANA_CHANGED_R_H =					1 << 16,
			ANA_CHANGED_R_V	=					1 << 17,
			ANA_CHANGED_L_H	=					1 << 18,
			ANA_CHANGED_L_V	=					1 << 19,

			// groups
			DPAD_CHANGED =						0x0000F,
			TRIGGER_CHANGED =					0x000F0,
			SYMBOLS_CHANGED = 					0x00F00,
			START_SELECT_R3_L3_CHANGED = 		0x0F000,
			ANA_CHANGED =						0xF0000,
		} PadChangedInfo;

		/*
		 * PadCallback_f:
		 * 		Callback when pad updates or changes
		 *
		 * int:
		 * 		What has changed (if any). Perform an AND operation with Mini2D::PadChangedInfo enum elements
		 * int:
		 *		Pad port
		 * padData:
		 * 		The pad data
		 */
		typedef void (*PadCallback_f) (int, int, padData);

		/*
		 * DrawCallback_f:
		 * 		Callback when on draw (user should insert draw logic here)
		 *
		 * float:
		 * 		Delta time. Amount of time passed since last frame
		 * unsigned long:
		 * 		Frame.
		 *
		 * Return:
		 * 		If return is less than 0 the draw loop stops
		 */
		typedef int (*DrawCallback_f) (float, unsigned long);

		/*
		 * ExitCallback_f:
		 * 		Callback when program exits
		 */
		typedef void (*ExitCallback_f) ();


		u32 * TexturePointer;                    // Active pointer to unused texture vram
		bool XMB;                                // Whether or not the in-game XMB is currently being displayed

		const float &MAXW;                       // Max width of screen
		const float &MINW;                       // Min width of screen
		const float &MAXH;                       // Max height of screen
		const float &MINH;                       // Min height of screen

		// Constructor
		Mini(PadCallback_f pCallback, DrawCallback_f dCallback, ExitCallback_f eCallback);
		virtual ~Mini();

		/*
		 * BeginDrawLoop:
		 * 		Begins the draw loop. Loops endlessly until your DrawCallback_f function returns a value less than 0
		 */
		void BeginDrawLoop();

		/*
		 * Flip:
		 * 		Flips the frame and calculates deltaTime
		 */
		void Flip();

		/*
		 * Pad:
		 *		Process all connected controllers and pass information to _padCallback
		 */
		void Pad();

		/*
		 * SetAnalogDeadzone:
		 * 		Set the deadzone Pad() uses to detect whether the analog sticks have changed
		 *
		 * deadzone:
		 * 		The unsigned 8-bit deadzone
		 */
		void SetAnalogDeadzone(unsigned char deadzone);

		/*
		 * SetClearColor:
		 * 		Sets the clear color used to flush the display buffers every frame
		 *
		 * color:
		 * 		ARGB color
		 */
		void SetClearColor(unsigned int color);

		/*
		 * SetAlphaState:
		 * 		Enable or disable alpha testing
		 *
		 * enable:
		 * 		True to enable alpha blending
		 */
		void SetAlphaState(bool enable);

		/*
		 * ResetTexturePointer:
		 *		Resets texture pointer. Used to pop the entire texture heap
		 */
		void ResetTexturePointer();

		/*
		 * AddTexture:
		 *		Copy raw ARGB pixel data into RSX
		 *
		 * pixelData:
		 * 		Pointer to an array of ARGB values
		 * pitch:
		 * 		Number of bytes per row
		 * height:
		 * 		Height of the texture
		 *
		 * Return:
		 * 		Texture offset in RSX (do not directly access this offset)
		 */
		unsigned int AddTexture(void * pixelData, int pitch, int height);

		/*
		 * DrawTexture:
		 * 		Draws the texture onto the currect frame
		 *
		 * textureOff:
		 * 		RSX offset to texture
		 * pitch:
		 * 		Number of bytes per row
		 * width
		 * 		Width of image
		 * height:
		 * 		Height of image
		 * xAnchor:
		 *		X coordinate to rotate around
		 * yAnchor:
		 *		Y coordinate to rotate around
		 * x:
		 * 		X coordinate
		 * y:
		 * 		Y coordinate
		 * z:
		 *		Z coordinate
		 * w:
		 * 		New width
		 * h:
		 * 		New height
		 * rgba:
		 * 		RGBA color
		 * angle:
		 * 		Angle to rotate image (degrees)
		 * colorFormat:
		 *		Tiny3D pixel format
		 */
		void DrawTexture(u32 textureOff, int pitch, int width, int height, float xAnchor, float yAnchor, float x, float y, float z, float w, float h, unsigned int rgbaTL, unsigned int rgbaTR, unsigned int rgbaBR, unsigned int rgbaBL, float angle, unsigned int colorFormat);
		void DrawTexture(unsigned int textureOff, int pitch, int width, int height, float xAnchor, float yAnchor, float x, float y, float z, float w, float h, unsigned int rgba, float angle, unsigned int colorFormat);
		void DrawTexture(unsigned int textureOff, int pitch, int width, int height, float x, float y, float z, float w, float h, unsigned int rgba, float angle, unsigned int colorFormat);

		/*
		 * DrawRectangle:
		 *		Draws the rectangle onto the current frame
		 *
		 * xAnchor:
		 *		X coordinate to rotate around
		 * yAnchor:
		 *		Y coordinate to rotate around
		 * x:
		 * 		X coordinate
		 * y:
		 * 		Y coordinate
		 * layer:
		 *		Z coordinate
		 * dx:
		 * 		Width
		 * dy:
		 * 		Height
		 * rgba:
		 * 		Color
		 * angle:
		 * 		Angle of rotation (degrees)
		 */
		void DrawRectangle(float xAnchor, float yAnchor, float x, float y, float layer, float dx, float dy, unsigned int rgba, float angle);


	private:
		PadCallback_f _padCallback;
		DrawCallback_f _drawCallback;

		unsigned int _clearColor;                // Value to clear screen to on every draw
		int _alphaEnabled;                       // Whether alpha testing is enabled

		float _maxW;                             // MAXW refers to
		float _minW;                             // MINW refers to
		float _maxH;                             // MAXH refers to
		float _minH;                             // MINH refers to

		unsigned long _maxFrameCount;            // Max value of _frameCount before reset. Value is divisible by 10
		unsigned long _frameCount;               // Number of frames drawn, resets on _maxFrameCount
		struct timeval _start;                   // Start of draw
		struct timeval _end;                     // End of draw
		float _deltaTime;                        // Time between start and end of draw

		padData _padData[MAX_PORT_NUM];          // Pad data
		unsigned short _analogDeadzone;          // Deadzone of analog sticks

		void * _textureMem;                      // Pointer to vram

		u32 _spu;                                //
		u32 _spuInited;                          // SPU Init status
		sysSpuImage _spuImage;                   // Image holding sound module

		// Draw texture with rotation
		void drawSpriteRot(float xAnchor, float yAnchor, float x, float y, float layer, float dx, float dy, u32 rgba0, u32 rgba1, u32 rgba2, u32 rgba3, float angle);

		// Initialize SPU and sound modules
		void initSPU();
	};

}

#endif /* MINI2D_MINI2D_HPP_ */
