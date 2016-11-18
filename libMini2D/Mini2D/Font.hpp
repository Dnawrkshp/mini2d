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

	unsigned int BackColor;					// RGBA background color of text
	unsigned int ForeColor;					// RGBA text color

	float ZIndex;							// Z coordinate

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
	 * w:
	 *		Width of character loaded into RSX (you should set this higher if you want to print with a large font size)
	 * h:
	 *		Height of character loaded into RSX (should match w)
	 * 
	 * Return:
	 * 		Result of load operation
	 */
	FontLoadStatus Load(char * filepath, short w = 32, short h = 32);

	/*
	 * Load:
	 *		Load TTF font
	 * 
	 * buffer:
	 *		Buffer containing TTF font
	 * size:
	 * 		Size of buffer
	 * w:
	 *		Width of character loaded into RSX (you should set this higher if you want to print with a large font size)
	 * h:
	 *		Height of character loaded into RSX (should match w)
	 * 
	 * Return:
	 *		Result of load operation
	 */
	FontLoadStatus Load(void * buffer, unsigned int size, short w = 32, short h = 32);

	/*
	 * PrintLine:
	 *		Print the first line in string from the index held in startIndex
	 * 
	 * cString:
	 *		
	 * string:
	 *		Buffer containing line to print
	 * startIndex:
	 *		Pointer to startIndex. When this function closes this value is set to the index of the next line to print
	 * location:
	 * 		Where to print text
	 * size:
	 * 		Width and height of each character
	 * useContainer:
	 *		Whether or not to use container to clip printed text
	 */
	void PrintLine(const wchar_t * cString, std::wstring * string, int * startIndex, Vector2 location, Vector2 size, bool useContainer = 0);

	/*
	 * PrintLines:
	 *		Prints all the lines in string from the index lineStart onward
	 * 
	 * cString:
	 *		
	 * string:
	 *		Buffer containing lines to print
	 * lineStart:
	 *		Starting line
	 * location:
	 * 		Where to print text
	 * size:
	 * 		Width and height of each character
	 * useContainer:
	 *		Whether or not to use container to clip printed text
	 * wordWrap:
	 *		If useContainer is true. Whether or not to wrap characters drawn outside the container to a new line
	 */
	void PrintLines(const wchar_t * cString, std::wstring * string, int lineStart, Vector2 location, Vector2 size, bool useContainer = 0, bool wordWrap = 0);

	/*
	 * PrintFormat:
	 *		Prints all the lines in string generated from swprintf()
	 *
	 * location:
	 * 		Where to print text
	 * size:
	 * 		Width and height of each character
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
	int PrintFormat(Vector2 location, Vector2 size, bool useContainer, bool wordWrap, std::size_t len, const wchar_t * format, ...);

	/*
	 * GetWidth:
	 *		Returns the width of the current line in string or cString
	 *
	 * cString:
	 *		wide char array (If calculating width of std::wstring, make NULL)
	 * string:
	 *		wide string (If calculating width of wchar_t*, make NULL)
	 * w:
	 *		Width scale (equivalent to the X value of the size Vector2 passed to PrintLine)
	 * offset:
	 *		Offset in string to start calculating width from
	 */
	float GetWidth(const wchar_t * cString, std::wstring * string, float w, int offset = 0);
	
	/*
	 * GetWidth:
	 *		Returns the width of chr
	 *
	 * chr:
	 *		The wide character to determine the length of
	 * w:
	 *		Width scale (equivalent to the X value of the size Vector2 passed to PrintLine)
	 */
	float GetWidth(wchar_t chr, float w);

private:
	typedef struct _fontChar_t {
		wchar_t chr;							// Character code
		u32 rsx;								// Offset in RSX
		u8 fw;									// Width of char
		u8 fy;									// Y correction
		u8 w;									// Width of image
		u8 h;									// Height of image
		short p;								// Pitch of image
	} FontChar;

	std::vector<FontChar> CharMap;				// List of characters

	Mini2D * _mini;

	// Load font
	FontLoadStatus loadFont(char * path, void * buffer, int size, short w, short h);
	// Draw character to frame
	float printChar(FontChar * fontChar, float x, float y, float w, float h);
	// Determine if the character denotes a line break
	bool isNewline(const wchar_t * cString, std::wstring * string, int strLen, int * index);
	// Prints line
	int printLine(const wchar_t * cString, std::wstring * string, int * startIndex, Vector2 location, Vector2 size, bool useContainer, bool draw);
	// Convert the glyph into a bitmap and load into the RSX
	bool ttfToBitmap(FT_Face face, unsigned int chr, u8 * bitmap, u8 *w, short *h, u8 *yCorrection);
	// Load all glyphs into rsx
	u8 * addFontFromTTF(FT_Face face, u8 *texture, short w, short h);
};

#endif /* FONT_HPP_ */