/*
 * Font.hpp
 *
 *  Created on: November 6th, 2016
 *  Author: Daniel Gerendasy
 */

#ifndef FONT_HPP_
#define FONT_HPP_

#include <vector>							// std::vector<>
#include <string>							// std::wstring

#include <Mini2D/Mini2D.hpp>				// Class definition
#include <Mini2D/Image.hpp>					// Image class
#include <Mini2D/Units.hpp>					// Vector2

class Font {
public:
	typedef enum _fontLoadStatus_t {
		FONT_SUCCESS = 0,
		FONT_INVALID_ARG,
		FONT_INVALID_TTF,
		FONT_LIBRARY_INIT_FAILED,
		FONT_INVALID_MINI2D
	} FontLoadStatus;

	typedef enum _fontPrintAlign_t {
		PRINT_ALIGN_TOPLEFT = 0,
		PRINT_ALIGN_TOPCENTER,
		PRINT_ALIGN_TOPRIGHT,
		PRINT_ALIGN_CENTERLEFT,
		PRINT_ALIGN_CENTER,
		PRINT_ALIGN_CENTERRIGHT,
		PRINT_ALIGN_BOTTOMLEFT,
		PRINT_ALIGN_BOTTOMCENTER,
		PRINT_ALIGN_BOTTOMRIGHT
	} FontPrintAlign;

	u32 BackColor;							// RGBA background color of text
	u32 ForeColor;							// RGBA text color

	float ZIndex;							// Z coordinate
	float SpacingOffset;					// Offset added to draw location after each character

	FontPrintAlign TextAlign;				// Text alignment

	RectangleF Container;					// Print container. Text only prints in here (default is full screen)

	// Constructors
	Font(Mini2D * mini);
	virtual ~Font();

	/*
	 * Load:
	 *		Load TTF font
	 * 
	 * filepath:
	 *		Path to TTF
	 * r:
	 *		Pixel resolution of character loaded into RSX
	 * 
	 * Return:
	 * 		Result of load operation
	 */
	FontLoadStatus Load(const char * filepath, u16 r = 32);

	/*
	 * Load:
	 *		Load TTF font
	 * 
	 * buffer:
	 *		Buffer containing TTF font
	 * size:
	 * 		Size of buffer
	 * r:
	 *		Pixel resolution of character loaded into RSX
	 * 
	 * Return:
	 *		Result of load operation
	 */
	FontLoadStatus Load(const void * buffer, u32 size, u16 r = 32);

	/*
	 * PrintLine:
	 *		Print the first line in string from the index held in startIndex
	 * 
	 * string:
	 *		Buffer containing std::wstring to print
	 * startIndex:
	 *		Pointer to startIndex. When this function closes this value is set to the index of the next line to print
	 * location:
	 * 		Where to print text
	 * size:
	 * 		Pixel size per character
	 * useContainer:
	 *		Whether or not to use container to clip printed text
	 */
	void PrintLine(const std::wstring& string, int * startIndex, const Vector2& location, float size, bool useContainer = 0);

	/*
	 * PrintLines:
	 *		Prints all the lines in string from the index lineStart onward
	 * 
	 * string:
	 *		Buffer containing std::wstring to print
	 * lineStart:
	 *		Starting line
	 * location:
	 * 		Where to print text
	 * size:
	 * 		Pixel size per character
	 * useContainer:
	 *		Whether or not to use container to clip printed text
	 * wordWrap:
	 *		If useContainer is true. Whether or not to wrap characters drawn outside the container to a new line
	 */
	void PrintLines(const std::wstring& string, int lineStart, const Vector2& location, float size, bool useContainer = 0, bool wordWrap = 0);

	/*
	 * PrintFormat:
	 *		Prints all the lines in string generated from swprintf()
	 *
	 * location:
	 * 		Where to print text
	 * size:
	 * 		Pixel size per character
	 * useContainer:
	 *		Whether or not to use container to clip printed text
	 * wordWrap:
	 *		If useContainer is true. Whether or not to wrap characters drawn outside the container to a new line
	 * len:
	 *		Max number of characters written
	 * format:
	 *		Pointer to a null-terminated wide string specifying how to interpret the data
	 * ...:
	 *		Arguments
	 *
	 * Return:
	 *		Negative value if failed. Otherwise, number of characters written
	 */
	int PrintFormat(const Vector2& location, float size, bool useContainer, bool wordWrap, std::size_t len, const wchar_t * format, ...);

	/*
	 * AddChar:
	 *		Defines the given wchar_t as the given image within this font
	 *
	 * chr:
	 *		Wide character to associate this image with
	 * image:
	 *		Image to draw
	 * yCorrection:
	 *		Y offset from top
	 * 
	 * Return:
	 *		True if successful. False if unsuccessful.
	 */
	bool AddChar(wchar_t chr, Image * image, int yCorrection = 0);

	/*
	 * GetWidth:
	 *		Returns the width of the current line in string
	 *
	 * string:
	 *		wide string
	 * size:
	 *		Pixel size per character
	 * offset:
	 *		Offset in string to start calculating width from
	 */
	float GetWidth(const std::wstring& string, float size, int offset = 0);
	
	/*
	 * GetWidth:
	 *		Returns the width of chr
	 *
	 * chr:
	 *		The wide character to determine the length of
	 * size:
	 *		Pixel size per character
	 */
	float GetWidth(wchar_t chr, float size);

private:
	typedef struct _fontChar_t {
		wchar_t chr;							// Character code
		u32 rsx;								// Offset in RSX
		u32 format;								// Color format of image
		u16 fr;									// Original resolution of char
		u16 fw;									// Character width
		u16 fy;									// Y correction
		u16 w;									// Width of image
		u16 h;									// Height of image
		u16 p;									// Pitch of image
	} FontChar;

	std::vector<FontChar*> CharMap;				// List of characters

	Mini2D * _mini;

	// Load font
	FontLoadStatus loadFont(const char * path, const void * buffer, int size, u16 r);
	// Get the scale dimension of given the image dimension, load resolution, and the font size
	float getDimension(float d, float r, float s);
	// Get the fontChar associated with chr
	FontChar * getFontChar(wchar_t chr);
	// Draw character to frame
	float printChar(FontChar * fontChar, float x, float y, float size);
	// Determine if the character denotes a line break
	bool isNewline(const std::wstring& string, int strLen, int * index);
	// Prints line
	int printLine(const std::wstring& string, int * startIndex, const Vector2& location, float size, bool useContainer, bool draw);
	// Convert the glyph into a bitmap and load into the RSX
	bool ttfToBitmap(FT_Face face, u32 chr, u8 ** bitmap, u16 * w, u16 * h, u16 * fw, u16 * yCorrection);
	// Load all glyphs into rsx
	u8 * addFontFromTTF(FT_Face face, u8 *texture, u16 w, u16 h);
	// Unload all FontChars
	void unloadCharMap();
};

#endif /* FONT_HPP_ */