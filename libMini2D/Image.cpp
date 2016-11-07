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

Image::Image(Mini2D * mini) :
		_mini(mini) {

	if (_mini == NULL)
		return;
	loadModules();
}

Image::~Image() {
	_mini = NULL;
	_textureOff = 0;
}

Image::ImageLoadStatus Image::Load(char * filepath) {
	if (_mini == NULL)
		return IMAGE_INVALID_MINI2D;

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
			_textureOff = _mini->AddTexture(png.bmp_out, png.pitch, png.height);
			free(png.bmp_out);

			_pitch = png.pitch;
			_width = png.width;
			_height = png.height;
			break;
		case IMAGE_TYPE_JPG:
			if (jpgLoadFromBuffer(buffer, size, &jpg))
				return IMAGE_INVALID_ARG;
			_textureOff = _mini->AddTexture(jpg.bmp_out, jpg.pitch, jpg.height);
			free(jpg.bmp_out);

			_pitch = jpg.pitch;
			_width = jpg.width;
			_height = jpg.height;
			break;
	}
	if (!_textureOff)
		return IMAGE_NO_MEM;

	_sWidth = (float)_width / 848.f;
	_sHeight = (float)_height / 512.f;

	return IMAGE_SUCCESS;
}

void Image::Draw(float x, float y, float width, float height, unsigned int rgba, Image::ImageDrawType type) {
	if (_mini == NULL || !_textureOff)
		return;

	float nx = x, ny = y;
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

	_mini->DrawTexture(_textureOff, _pitch, _width, _height, nx, ny, width, height, rgba, 0);
}

void Image::Draw(float x, float y, float width, float height, Image::ImageDrawType type) {
	Draw(x,y,width,height,0xFFFFFFFF,type);
}

void Image::Draw(float x, float y, unsigned int rgba, Image::ImageDrawType type) {
	Draw(x,y,_sWidth,_sHeight,rgba,type);
}

void Image::Draw(float x, float y, Image::ImageDrawType type) {
	Draw(x,y,_sWidth,_sHeight,0xFFFFFFFF,type);
}


void Image::loadModules() {
	if (sysModuleIsLoaded(SYSMODULE_PNGDEC))
		sysModuleLoad(SYSMODULE_PNGDEC);
	if (sysModuleIsLoaded(SYSMODULE_JPGDEC))
    	sysModuleLoad(SYSMODULE_JPGDEC);
}
