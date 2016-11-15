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
#include <Mini2D/Units.hpp>					// Vector2

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

	unsigned int * TexturePointer;			// Pointer to texture in RSX memory

	RectangleF DrawRegion;					// Where to draw image to
	float ZIndex;							// Z coordinate (lower numbers are drawn over higher numbers)

	// Constructors
	Image(Mini2D * mini);
	virtual ~Image();

	/*
	 * Load:
	 *		Load PNG/JPG image
	 * 
	 * filepath:
	 *		Path to image
	 * 
	 * Return:
	 *		Result of load operation
	 */
	ImageLoadStatus Load(char * filename);

	/*
	 * Load:
	 *		Load PNG/JPG image
	 * 
	 * buffer:
	 *		Buffer containing image
	 * size:
	 *		Size of buffer
	 * type:
	 *		Type of image the buffer contains
	 * 
	 * Return:
	 *		Result of load operation
	 */
	ImageLoadStatus Load(void * buffer, unsigned int size, ImageType type);

	/*
	 * Load:
	 *		Load ARGB image from buffer
	 * 
	 * argbBuffer:
	 *		Array of ARGB pixels
	 * w:
	 *		Width of image
	 * h:
	 *		Height of image
	 */
	void Load(void * argbBuffer, int w, int h);

	/*
	 * Draw:
	 * 		Draws the image onto the current frame
	 * 
	 * rgba:
	 *		Color
	 */
	void Draw(unsigned int rgba = 0xFFFFFFFF);

private:
	Mini2D * _mini;

	int _width, _height, _pitch;			// Width, height, and pitch of loaded image
	float _sWidth, _sHeight;				// Width and height in terms of screen scale
	unsigned int _textureOff;				// RSX offset to texture

	// Load PNG and JPG dec modules
	void loadModules();
	// Move loaded image to RSX
	void toRSX(void * buffer);
};

#endif /* IMAGE_HPP_ */