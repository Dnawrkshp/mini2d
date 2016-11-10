/*
 * Image.cpp
 *
 *  Created on: November 6th, 2016
 *  Author: Daniel Gerendasy
 */

#include <sysmodule/sysmodule.h>			// Load and unload PNG,JPG modules
#include <string.h>							// memcpy, memset

#include <tiny3d.h>							// Tiny3D functions
#include <Mini2D/Image.hpp>					// Class definition

//---------------------------------------------------------------------------
// Init Functions
//---------------------------------------------------------------------------
Image::Image(Mini2D * mini) :
		_mini(mini) {

	if (_mini == NULL)
		return;
	loadModules();

	TexturePointer = 0;
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
void Image::Draw(float width, float height, unsigned int rgba, ImageDrawType type, float angle) {
	if (_mini == NULL || !_textureOff)
		return;

	float nx = Location.X, ny = Location.Y;
	switch (type) {
		case DRAW_TOPRIGHT:
			nx -= width;
			break;
		case DRAW_BOTTOMLEFT:
			ny -= height;
			break;
		case DRAW_BOTTOMRIGHT:
			nx -= width;
			ny -= height;
			break;
		case DRAW_CENTER:
			nx -= width/2;
			ny -= height/2;
			break;
		case DRAW_TOPLEFT:
			break;
	}

	_mini->DrawTexture(_textureOff, _pitch, _width, _height, nx, ny, width, height, rgba, angle, TINY3D_TEX_FORMAT_A8R8G8B8);
}

void Image::Draw(unsigned int rgba, ImageDrawType type, float angle) {
	Draw(_sWidth, _sHeight, rgba, type, angle);
}

