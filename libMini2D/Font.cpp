/*
 * Font.cpp
 *
 *  Created on: November 6th, 2016
 *  Author: Daniel Gerendasy
 */

#include <freetype/ftglyph.h>				// For loading TTF character and converting to raw RGBA
#include <wchar.h>							// wcslen(), wprintf()

#include <tiny3d.h>							// Tiny3D functions
#include <Mini2D/Font.hpp>					// Class definition

//---------------------------------------------------------------------------
// Init Functions
//---------------------------------------------------------------------------
Font::Font(Mini2D * mini) :
		_mini(mini) {

	if (!mini)
		return;

	Container.X(mini->MAXW/2);
	Container.Y(mini->MAXH/2);
	Container.W(mini->MAXW);
	Container.H(mini->MAXH);

	ForeColor = 0x000000FF;
	BackColor = 0x00000000;
	ZIndex = 0;
	SpacingOffset = 0;
	TextAlign = PRINT_ALIGN_TOPLEFT;
}

Font::~Font() {
	unloadCharMap();
	_mini = NULL;
}

void Font::unloadCharMap() {
	for(std::vector<FontChar*>::iterator it = CharMap.begin(); it != CharMap.end(); it++) { if (*it) {delete *it;} }
	CharMap.clear();
}

//---------------------------------------------------------------------------
// Print Functions
//---------------------------------------------------------------------------
int Font::PrintFormat(const Vector2& location, float size, bool useContainer, bool wordWrap, std::size_t len, const wchar_t * format, ...) {
	va_list args;
	wchar_t * buffer;
	std::wstring string;
	int result = 0;

	if (!_mini || !format || !ForeColor || !len)
		return -1;

	// Allocate room
	buffer = new wchar_t[len+1]();

    va_start(args, format);
    result = vswprintf(buffer, len, format, args);
    va_end(args);

    if (result <= 0) {
    	delete [] buffer;
    	return result;
    }

    string = buffer;
    delete [] buffer;

    PrintLines(string, 0, location, size, useContainer, wordWrap);

    
    return result;
}

void Font::PrintLines(const std::wstring& string, int lineStart, const Vector2& location, float size, bool useContainer, bool wordWrap) {
	int i,len,line,last=-1;
	bool draw = lineStart <= 0,wrap=0;
	Vector2 loc = location;
	if (!_mini || string.empty() || !ForeColor)
		return;

	len = string.length();
	line = 0;
	for (i=0;i<len;) {
		wrap = printLine(string, &i, loc, size, useContainer, draw && (!wrap || wrap==wordWrap)) == 1;

		if (!wrap || wrap==wordWrap)
			loc.Y += size;

		line++;
		if (line == lineStart)
			draw = 1;

		if (last == i) {
			wprintf(L"Endless loop detected in Font::PrintLines():\n\tstring: %ls\n\tindex %d\n\tDrawing past container width perhaps?\n", string.c_str(), i);
			return;
		}
		last = i;
	}
}

void Font::PrintLine(const std::wstring& string, int * startIndex, const Vector2& location, float size, bool useContainer) {
	printLine(string, startIndex, location, size, useContainer, 1);
}

int Font::printLine(const std::wstring& string, int * startIndex, const Vector2& location, float size, bool useContainer, bool draw) {
	int j,len;
	float cRight=0,cLeft=0,cTop=0,cBottom=0,w;
	float cw = 0, ch = 0;
	bool wrap = 0;
	FontChar * fc = NULL;
	Vector2 loc = location;

	if (!_mini || string.empty() || !ForeColor)
		return -1;

	// Calculate left and right bounds
	if (useContainer) {
		cRight = Container.Location.X + Container.Dimension.X/2;
		cLeft = Container.Location.X - Container.Dimension.X/2;
		cTop = Container.Location.Y - Container.Dimension.Y/2;
		cBottom = Container.Location.Y + Container.Dimension.Y/2;
	}

	// Start index
	j = (startIndex?(*startIndex):0);

	// Align X
	// If useContainer: only align if the string width is less than the container width
	w = GetWidth(string,size,j);
	if (w < (useContainer?Container.Dimension.X:w+1)) {
		switch (TextAlign) {
			case PRINT_ALIGN_TOPCENTER:
			case PRINT_ALIGN_CENTER:
			case PRINT_ALIGN_BOTTOMCENTER:
				loc.X -= w / 2.f;
				break;
			case PRINT_ALIGN_TOPRIGHT:
			case PRINT_ALIGN_CENTERRIGHT:
			case PRINT_ALIGN_BOTTOMRIGHT:
				loc.X -= w;
				break;
			default:
				break;
		}
	}
	else
		loc.X = cLeft;

	// Align Y
	switch (TextAlign) {
		case PRINT_ALIGN_CENTERRIGHT:
		case PRINT_ALIGN_CENTER:
		case PRINT_ALIGN_CENTERLEFT:
			loc.Y -= size/2;
			break;
		case PRINT_ALIGN_BOTTOMRIGHT:
		case PRINT_ALIGN_BOTTOMCENTER:
		case PRINT_ALIGN_BOTTOMLEFT:
			loc.Y -= size;
			break;
		default:
			break;
	}

	len = string.length();
	for (; j < len; j++) {
		if (isNewline(string, len, &j)) {
			j++;
			break;
		}

		fc = getFontChar(string.at(j));
		if (fc) {
			cw = getDimension(fc->w, fc->fr, size);
			ch = getDimension(fc->h, fc->fr, size);

			// If we are drawing past the container, stop
			if (useContainer && (loc.X + cw) > cRight) {
				wrap = 1;
				break;
			}

			// If draw is false or we are drawing before the container, skip
			if (!draw || (useContainer && (loc.X < cLeft || loc.Y+ch > cBottom || loc.Y < cTop)))
				loc.X += cw + SpacingOffset;
			else
				loc.X += printChar(fc, loc.X, loc.Y, size);
		}
	}

	if (startIndex)
		*startIndex = j;

	return wrap;
}

float Font::printChar(FontChar * fontChar, float x, float y, float size) {
	float dx2, dy2, dx;

	if (!_mini || !fontChar)
		return 0;

	dx = getDimension(fontChar->w, fontChar->fr, size);
	dx2 = getDimension(fontChar->fw, fontChar->fr, size);
	dy2 = getDimension(fontChar->h, fontChar->fr, size);

	if (!fontChar->rsx)
		return dx2;

	x+=dx2/2;
	y+=dy2/2;

	// Draw background
	if (BackColor)
		_mini->DrawRectangle(x, y, x, y, ZIndex, dx2, dy2, BackColor, 0);

	 y += (float)fontChar->fy * (size / (float)fontChar->fr);

	_mini->DrawTexture(fontChar->rsx,
					fontChar->p,
					fontChar->w,
					fontChar->h,
					x,
					y,
					ZIndex,
					dx,
					dy2,
					ForeColor,
					0,
					fontChar->format);

	return dx2 + SpacingOffset;
}

float Font::GetWidth(const std::wstring& string, float size, int offset) {
	int i,l;
	float width = 0.f;

	if (string.empty())
		return 0.f;

	if (offset < 0)
		offset = 0;

	l = string.length();
	for (i=offset;i<l;i++) {
		if (isNewline(string, l, &i))
			break;
		width += GetWidth(string.at(i), size);
	}

	return width;
}

float Font::GetWidth(wchar_t chr, float size) {
	for(std::vector<FontChar*>::iterator it = CharMap.begin(); it != CharMap.end(); it++)
		if ((*it)->chr == chr)
			return getDimension((*it)->fw, (*it)->fr, size) + SpacingOffset;

	return 0;
}

float Font::getDimension(float d, float r, float s) {
	return s * (d / r);
}


bool Font::isNewline(const std::wstring& string, int strLen, int * index) {
	wchar_t chr = string.at(*index);
	if (chr == 0x000D && (*index) < (strLen-1) && string.at((*index)+1) == 0x000A)
	{
		(*index) ++;
		return 1;
	}
	if (chr == 0x000D || chr == 0x000A ||
		chr == 0x000B || chr == 0x000C ||
		chr == 0x0085 || chr == 0x2028 ||
		chr == 0x2029) {
		return 1;
	}
	return 0;
}

Font::FontChar * Font::getFontChar(wchar_t chr) {
	for(std::vector<FontChar*>::iterator it = CharMap.begin(); it != CharMap.end(); it++)
		if ((*it)->chr == chr)
			return *it;

	return NULL;
}

//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
bool Font::AddChar(wchar_t chr, Image * image, int yCorrection) {
	FontChar * fontChar = NULL;

	if (!image || !chr)
		return false;

	fontChar = new FontChar();
	fontChar->chr = chr;
	fontChar->fr = image->GetHeight();
	fontChar->w = image->GetWidth();
	fontChar->h = image->GetHeight();
	fontChar->p = image->GetPitch();
	fontChar->fw = fontChar->w + 2;
	fontChar->fy = yCorrection;
	fontChar->rsx = tiny3d_TextureOffset(image->TexturePointer);
	fontChar->format = TINY3D_TEX_FORMAT_A8R8G8B8;

	// If this wchar is already mapped let's remove it
	for(std::vector<FontChar*>::iterator it = CharMap.begin(); it != CharMap.end(); it++) {
		if ((*it)->chr == chr) {
			delete *it;
			CharMap.erase(it);
			break;
		}
	}

	CharMap.push_back(fontChar);

	return true;
}

//---------------------------------------------------------------------------
// Load TTF Functions
//---------------------------------------------------------------------------
Font::FontLoadStatus Font::Load(const char * filepath, u16 r) {
	if (!filepath)
		return FONT_INVALID_ARG;
	if (!_mini)
		return FONT_INVALID_MINI2D;

	return loadFont(filepath, NULL, 0, r);
}

Font::FontLoadStatus Font::Load(const void * buffer, u32 size, u16 r) {
	if (!buffer || size == 0)
		return FONT_INVALID_ARG;
	if (!_mini)
		return FONT_INVALID_MINI2D;

	return loadFont(NULL, buffer, size, r);
}


Font::FontLoadStatus Font::loadFont(const char * path, const void * buffer, int size, u16 r)
{
	FT_Face face;
	FT_Library freetype;

	if (!_mini)
		return FONT_INVALID_MINI2D;

	if (FT_Init_FreeType(&freetype))
		return FONT_LIBRARY_INIT_FAILED;

	if (path) {
		if (FT_New_Face(freetype, path, 0, &face)) {
			FT_Done_FreeType(freetype);
			return FONT_INVALID_TTF;
		}
	}
	else {
		if (FT_New_Memory_Face(freetype, (FT_Byte*)buffer, size, 0, &face)) {
			FT_Done_FreeType(freetype);
			return FONT_INVALID_TTF;
		}
	}

	// Selecting the unicode charmap
	FT_Select_Charmap(face, FT_ENCODING_UNICODE);

	// Load font
	_mini->TexturePointer = (u32*)addFontFromTTF(face, (u8*)_mini->TexturePointer, r, r);

	FT_Done_Face(face);
	FT_Done_FreeType(freetype);
	return FONT_SUCCESS;
}

u8 * Font::addFontFromTTF(FT_Face face, u8 *texture, u16 w, u16 h)
{
	int chr, a, b;
	FT_UInt index = 0;
	u8 * bitmap;
	FontChar * fontChar = NULL;

	unloadCharMap(); 
	
	chr = FT_Get_First_Char(face, &index);
	while (index > 0) {
		fontChar = new FontChar();
		fontChar->chr = chr;
		fontChar->fr = h;
		fontChar->w = w;
		fontChar->h = h;
		fontChar->p = w*2;
		fontChar->fw = w;
		fontChar->fy = 0;
		fontChar->rsx = 0;
		fontChar->format = TINY3D_TEX_FORMAT_A4R4G4B4;
		
		// If the bitmap contains opaque pixels then add to rsx
		if (ttfToBitmap(face, chr, &bitmap, &fontChar->w, &fontChar->h, &fontChar->fw, &fontChar->fy)) {
			texture = (u8 *) ((((long) texture) + 15) & ~15);
			fontChar->rsx = tiny3d_TextureOffset(texture);
			fontChar->p = fontChar->w * 2;

			// Convert to A4R4G4B4
			for(a = 0; a < fontChar->h; a++) {
				for(b = 0; b < fontChar->w; b++) {
					if(bitmap[b])
						*(u16*)texture = ((bitmap[b]>>4)<<12) | 0xFFF;
					else
						texture[0] = texture[1] = 0x0;

					texture += 2;
				}

				bitmap += (short)fontChar->w;
			}
		}

		CharMap.push_back(fontChar);
		chr = FT_Get_Next_Char(face, chr, &index);
	}

	// Realign
	texture = (u8 *) ((((long) texture) + 15) & ~15);

	return texture;
}

bool Font::ttfToBitmap(FT_Face face, u32 chr, u8 ** bitmap, u16 * w, u16 * h, u16 * fw, u16 * yCorrection)
{
	int x = 0, size = 0;
	long c = 0;

	FT_Set_Pixel_Sizes(face, 0, *h);
	FT_GlyphSlot slot = face->glyph;

	if (FT_Load_Char(face, chr, FT_LOAD_RENDER)) {
		*w = 0;
		*h = 0;
		return 0;
	}

	// Generate y correction
	*yCorrection = *h - 1 - slot->bitmap_top;

	// Generate character width
	*fw = slot->advance.x >> 6;

	// Copy buffer pointer
	*bitmap = (u8*)slot->bitmap.buffer;

	// Update width and height
	*w = slot->bitmap.width;
	*h = slot->bitmap.rows;
	
	// Check if this has any pixels
	// We compare 8 bytes at a time for speed
	size = slot->bitmap.pitch * slot->bitmap.rows;
	for (x = 0; x < size; ) {

		// Compare against 8 bytes or however many bytes are left to compare
		if ((size - x - 1) < 8) {
			if (!memcmp((void*)&slot->bitmap.buffer[x], (void*)&c, size - x - 1))
				return 1;
			x += size - x - 1;
		}
		else {
			if (!memcmp((void*)&slot->bitmap.buffer[x], (void*)&c, 8))
				return 1;
			x += 8;
		}
	}

	return 0;
}
