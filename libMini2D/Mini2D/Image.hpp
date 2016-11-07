/*
 * Image.hpp
 *
 *  Created on: November 6th, 2016
 *  Author: Daniel Gerendasy
 */

#ifndef IMAGE_HPP_
#define IMAGE_HPP_

#include <pngdec/pngdec.h>					// PNG load functions
#include <jpgdec/jpgdec.h>					// JPG load functions

#include <Mini2D/Mini2D.hpp>				// Mini2D class

class Image {
public:
	typedef enum _imageType_t {
		IMAGE_TYPE_PNG = 0,
		IMAGE_TYPE_JPG
	} ImageType;

	typedef enum _imageLoadStatus_t {
		IMAGE_SUCCESS = 0,
		IMAGE_INVALID_ARG,
		IMAGE_NO_MEM,
		IMAGE_INVALID_MINI2D
	} ImageLoadStatus;

	typedef enum _imageDrawType_t {
		DRAW_TOPLEFT = 0,
		DRAW_TOPRIGHT,
		DRAW_BOTTOMLEFT,
		DRAW_BOTTOMRIGHT,
		DRAW_CENTER
	} ImageDrawType;

	// Constructors
	Image(Mini2D * mini);
	virtual ~Image();

	ImageLoadStatus Load(char * filename);
	ImageLoadStatus Load(void * buffer, unsigned int size, ImageType type);

	/*
	 * Draw:
	 * 		Draws the image onto the current frame
	 * 
	 * x:
	 * 		X coordinate (0 - 1)
	 * y:
	 *		Y coordinate (0 - 1)
	 * width:
	 *		Width of image
	 * height:
	 *		Height of image
	 * color:
	 *		Color
	 */
	void Draw(float x, float y, float width, float height, unsigned int color, ImageDrawType type);

	/*
	 * Draw:
	 * 		Draws the image onto the current frame
	 * 
	 * x:
	 * 		X coordinate (0 - 1)
	 * y:
	 *		Y coordinate (0 - 1)
	 * width:
	 *		Width of image
	 * height:
	 *		Height of image
	 */
	void Draw(float x, float y, float width, float height, ImageDrawType type);

	/*
	 * Draw:
	 * 		Draws the image onto the current frame
	 * 
	 * x:
	 * 		X coordinate (0 - 1)
	 * y:
	 *		Y coordinate (0 - 1)
	 * rgba:
	 *		Color
	 */
	void Draw(float x, float y, unsigned int rgba, ImageDrawType type);

	/*
	 * Draw:
	 * 		Draws the image onto the current frame
	 * 
	 * x:
	 * 		X coordinate (0 - 1)
	 * y:
	 *		Y coordinate (0 - 1)
	 */
	void Draw(float x, float y, ImageDrawType type);

private:
	Mini2D * _mini;

	int _width, _height, _pitch;
	float _sWidth, _sHeight;
	unsigned int _textureOff;

	void loadModules();
};

#endif /* IMAGE_HPP_ */