/*
 * Image.cpp
 *
 *  Created on: November 6th, 2016
 *  Author: Daniel Gerendasy
 */

#include <sysmodule/sysmodule.h>                 // Load and unload PNG,JPG modules
#include <string.h>                              // memcpy, memset
#include <math.h>                                // Trig functions

#include <tiny3d.h>                              // Tiny3D functions
#include <Mini2D/Image.hpp>                      // Class definition

#define PI 3.14159265
#define DEG2RAD(x) ((x*PI)/180.0)
#define RAD2DEG(x) ((x*180.0)/PI)

namespace Mini2D {

	//---------------------------------------------------------------------------
	// Init Functions
	//---------------------------------------------------------------------------
	Image::Image(Mini * mini) :
			_mini(mini) {

		if (_mini == NULL)
			return;
		loadModules();

		TexturePointer = 0;
		ZIndex = 0;

		_width = 0;
		_height = 0;
		_pitch = 0;
		_sWidth = 0;
		_sHeight = 0;
		_textureOff = 0;
	}

	Image::~Image() {
		_mini = NULL;
		_textureOff = 0;
	}

	void Image::loadModules() {
		if (sysModuleIsLoaded(SYSMODULE_PNGDEC))
			sysModuleLoad(SYSMODULE_PNGDEC);
		if (sysModuleIsLoaded(SYSMODULE_JPGDEC))
			sysModuleLoad(SYSMODULE_JPGDEC);
	}

	//---------------------------------------------------------------------------
	// Load Functions
	//---------------------------------------------------------------------------
	Image::ImageLoadStatus Image::Load(char * filepath) {
		int len, type;
		pngData png;
		jpgData jpg;

		if (_mini == NULL)
			return IMAGE_INVALID_MINI2D;
		if (!filepath)
			return IMAGE_INVALID_ARG;

		len = strlen(filepath);
		if (len < 4)
			return IMAGE_INVALID_ARG;

		if (filepath[len-1] != 'g' && filepath[len-1] != 'G')
			return IMAGE_INVALID_ARG;

		// PNG
		if ((filepath[len-2] == 'n' || filepath[len-2] == 'N') &&
			(filepath[len-3] == 'p' || filepath[len-3] == 'P') && (filepath[len-4] == '.')) {
			type = 1;
			goto process;
		}
		// JPG
		if ((filepath[len-2] == 'p' || filepath[len-2] == 'P') &&
			(filepath[len-3] == 'j' || filepath[len-3] == 'J') && (filepath[len-4] == '.')) {
			type = 0;
			goto process;
		}

		if (len < 5)
			return IMAGE_INVALID_ARG;

		// JPEG
		if ((filepath[len-2] == 'e' || filepath[len-2] == 'E') &&
			(filepath[len-3] == 'p' || filepath[len-3] == 'P') &&
			(filepath[len-4] == 'j' || filepath[len-4] == 'J') && (filepath[len-5] == '.')) {
			type = 0;
			goto process;
		}

		return IMAGE_INVALID_ARG;

		process: ;
		if (type) {
			if (pngLoadFromFile(filepath, &png))
				return IMAGE_INVALID_ARG;
			toRSX((void*)&png);
		}
		else {
			if (jpgLoadFromFile(filepath, &jpg))
				return IMAGE_INVALID_ARG;
			toRSX((void*)&jpg);
		}

		return IMAGE_SUCCESS;
	}

	Image::ImageLoadStatus Image::Load(void * buffer, unsigned int size, ImageType type) {
		pngData png;
		jpgData jpg;

		if (_mini == NULL)
			return IMAGE_INVALID_MINI2D;

		switch (type) {
			case IMAGE_TYPE_PNG:
				if (pngLoadFromBuffer(buffer, size, &png))
					return IMAGE_INVALID_ARG;
				toRSX((void*)&png);
				break;
			case IMAGE_TYPE_JPG:
				if (jpgLoadFromBuffer(buffer, size, &jpg))
					return IMAGE_INVALID_ARG;
				toRSX((void*)&jpg);
				break;
		}
		if (!_textureOff)
			return IMAGE_NO_MEM;

		_sWidth = (float)_width / (float)Video_Resolution.width;
		_sHeight = (float)_height / (float)Video_Resolution.height;

		return IMAGE_SUCCESS;
	}

	void Image::Load(void * argbBuffer, int w, int h) {
		_textureOff = _mini->AddTexture(argbBuffer, w * 4, h);

		_pitch = w * 4;
		_width = w;
		_height = h;

		_sWidth = (float)w / (float)Video_Resolution.width;
		_sHeight = (float)h / (float)Video_Resolution.height;
	}

	void Image::toRSX(void * buffer) {
		jpgData img = *(jpgData*)buffer;
		TexturePointer = _mini->TexturePointer;
		_textureOff = _mini->AddTexture(img.bmp_out, img.pitch, img.height);
		free(img.bmp_out);

		_pitch = img.pitch;
		_width = img.width;
		_height = img.height;
	}

	//---------------------------------------------------------------------------
	// Draw Functions
	//---------------------------------------------------------------------------
	void Image::Draw(unsigned int rgbaTL, unsigned int rgbaTR, unsigned int rgbaBR, unsigned int rgbaBL) {
		if (_mini == NULL || !_textureOff)
			return;

		float w = DrawRegion.W(), h = DrawRegion.H();
		float nx = DrawRegion.X(), ny = DrawRegion.Y();
		if (DrawRegion.UseAnchor) {
			Vector2 * pos = DrawRegion.GetRotatedCenter();
			nx = pos->X;
			ny = pos->Y;
		}

		float ax = nx, ay = ny;

		_mini->DrawTexture(_textureOff, _pitch, _width, _height, ax, ay, nx, ny, ZIndex, w, h, rgbaTL, rgbaTR, rgbaBR, rgbaBL, -DrawRegion.RectangleAngle, TINY3D_TEX_FORMAT_A8R8G8B8);
	}

	void Image::Draw(unsigned int rgba) {
		Draw(rgba, rgba, rgba, rgba);
	}

	//---------------------------------------------------------------------------
	// Get Functions
	//---------------------------------------------------------------------------
	int Image::GetWidth() {
		return _width;
	}

	int Image::GetHeight() {
		return _height;
	}

	int Image::GetPitch() {
		return _pitch;
	}

}
