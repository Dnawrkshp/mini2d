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
int Font::PrintFormat(Vector2 location, Vector2 size, bool useContainer, bool wordWrap, std::size_t len, const wchar_t * format, ...) {
	va_list args;
	wchar_t * buffer;
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


    PrintLines((const wchar_t*)buffer, NULL, 0, location, size, useContainer, wordWrap);
    delete [] buffer;
    return result;
}

void Font::PrintLines(const wchar_t * cString, std::wstring * string, int lineStart, Vector2 location, Vector2 size, bool useContainer, bool wordWrap) {
	int i,len,line,last=-1;
	bool draw = lineStart <= 0,wrap=0;
	if (!_mini || (!string && !cString) || !ForeColor)
		return;

	len = string?string->length():wcslen(cString);
	line = 0;
	for (i=0;i<len;) {
		wrap = printLine(cString, string, &i, location, size, useContainer, draw && (!wrap || wrap==wordWrap)) == 1;

		if (!wrap || wrap==wordWrap)
			location.Y += size.Y;

		line++;
		if (line == lineStart)
			draw = 1;

		if (last == i) {
			wprintf(L"Endless loop detected in Font::PrintLines():\n\tstring: %ls\n\tindex %d\n\tDrawing past container width perhaps?\n", string?string->c_str():cString, i);
			return;
		}
		last = i;
	}
}

void Font::PrintLine(const wchar_t * cString, std::wstring * string, int * startIndex, Vector2 location, Vector2 size, bool useContainer) {
	printLine(cString, string, startIndex, location, size, useContainer, 1);
}

int Font::printLine(const wchar_t * cString, std::wstring * string, int * startIndex, Vector2 location, Vector2 size, bool useContainer, bool draw) {
	int j,len;
	float cRight=0,cLeft=0,cTop=0,cBottom=0,w;
	wchar_t chr;
	bool wrap = 0;
	FontChar * fc = NULL;

	if (!_mini || (!string && !cString) || !ForeColor)
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
	w = GetWidth(cString,string,size.X,j);
	if (w < (useContainer?Container.Dimension.X:w+1)) {
		switch (TextAlign) {
			case PRINT_ALIGN_TOPCENTER:
			case PRINT_ALIGN_CENTER:
			case PRINT_ALIGN_BOTTOMCENTER:
				location.X -= w / 2.f;
				break;
			case PRINT_ALIGN_TOPRIGHT:
			case PRINT_ALIGN_CENTERRIGHT:
			case PRINT_ALIGN_BOTTOMRIGHT:
				location.X -= w;
				break;
			default:
				break;
		}
	}
	else
		location.X = cLeft;

	// Align Y
	switch (TextAlign) {
		case PRINT_ALIGN_CENTERRIGHT:
		case PRINT_ALIGN_CENTER:
		case PRINT_ALIGN_CENTERLEFT:
			location.Y -= size.Y/2;
			break;
		case PRINT_ALIGN_BOTTOMRIGHT:
		case PRINT_ALIGN_BOTTOMCENTER:
		case PRINT_ALIGN_BOTTOMLEFT:
			location.Y -= size.Y;
			break;
		default:
			break;
	}

	len = string?string->length():wcslen(cString);
	for (; j < len; j++) {
		if (isNewline(cString, string, len, &j)) {
			j++;
			break;
		}

		// If we are drawing past the container, stop
		if (useContainer && (location.X+size.X) > cRight) {
			wrap = 1;
			break;
		}

		chr = string?string->at(j):cString[j];
		for(std::vector<FontChar*>::iterator it = CharMap.begin(); it != CharMap.end(); it++) {
			fc = *it;
			if (fc->chr == chr) {
				// If draw is false or we are drawing before the container, skip
				if (!draw || (useContainer && (location.X < cLeft || location.Y+size.Y > cBottom || location.Y < cTop)))
					location.X += size.X * ((float)fc->fw / (float)(fc->w+1));
				else
					location.X += printChar(fc, location.X, location.Y, size.X, size.Y);
			}
		}
	}

	if (startIndex)
		*startIndex = j;

	return wrap;
}

float Font::printChar(FontChar * fontChar, float x, float y, float w, float h) {
	float dx2 = w * ((float)fontChar->fw / (float)(fontChar->w+1));
	if (!_mini || !fontChar)
		return 0;
	if (!fontChar->rsx)
		return dx2;

	x+=dx2/2;
	y+=h/2;

	// Draw background
	if (BackColor)
		_mini->DrawRectangle(x, y, x, y, ZIndex, dx2, h, BackColor, 0);

	y +=  (float)fontChar->fy * (h / (float)(fontChar->h+1));

	_mini->DrawTexture(fontChar->rsx,
					fontChar->p,
					fontChar->fw,
					fontChar->h+1,
					x,
					y,
					ZIndex,
					dx2,
					h,
					ForeColor,
					0,
					TINY3D_TEX_FORMAT_A4R4G4B4);

	return dx2;
}

float Font::GetWidth(const wchar_t * cString, std::wstring * string, float w, int offset) {
	int i,l;
	float width = 0.f;

	if (!string && !cString)
		return 0.f;

	if (offset < 0)
		offset = 0;

	l = string?string->length():wcslen(cString);
	for (i=offset;i<l;i++) {
		if (isNewline(cString, string, l, &i))
			break;
		width += GetWidth(string?string->at(i):cString[i], w);
	}

	return width;
}

float Font::GetWidth(wchar_t chr, float w) {
	FontChar * fc = NULL;

	for(std::vector<FontChar*>::iterator it = CharMap.begin(); it != CharMap.end(); it++) {
		fc = *it;
		if (fc->chr == chr) {
			return w * ((float)fc->fw / (float)(fc->w+1));
		}
	}

	return 0;
}

bool Font::isNewline(const wchar_t * cString, std::wstring * string, int strLen, int * index) {
	wchar_t chr = string?string->at(*index):cString[*index];
	if (chr == 0x000D && (*index) < (strLen-1) && (string?string->at((*index)+1):cString[(*index)+1]) == 0x000A)
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

//---------------------------------------------------------------------------
// Load TTF Functions
//---------------------------------------------------------------------------
Font::FontLoadStatus Font::Load(char * filepath, short w, short h) {
	if (!filepath)
		return FONT_INVALID_ARG;
	if (!_mini)
		return FONT_INVALID_MINI2D;

	return loadFont(filepath, NULL, 0, w, h);
}

Font::FontLoadStatus Font::Load(void * buffer, unsigned int size, short w, short h) {
	if (!buffer || size == 0)
		return FONT_INVALID_ARG;
	if (!_mini)
		return FONT_INVALID_MINI2D;

	return loadFont(NULL, buffer, size, w, h);
}


Font::FontLoadStatus Font::loadFont(char * path, void * buffer, int size, short w, short h)
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

	_mini->TexturePointer = (u32*)addFontFromTTF(face, (u8*)_mini->TexturePointer, w, h);

	FT_Done_Face(face);
	FT_Done_FreeType(freetype);
	return FONT_SUCCESS;
}

/*
 * addFontFromTTF:
 * 		Moves font glyphs into RSX
 * 
 * Taken from https://github.com/wargio/tiny3D
 * (c) 2010 Hermes  <www.elotrolado.net>
 */
u8 * Font::addFontFromTTF(FT_Face face, u8 *texture, short w, short h)
{
	int chr, a, b;
	FT_UInt index = 0;
	u8 i;
	u8 *font;
	u8 * bitmap;
	FontChar * fontChar = NULL;
	int bpp = 8;
	
	if(h < 8) h = 8;
	if(w < 8) w = 8;
	if(h > 256) h = 256;
	if(w > 256) w = 256;

	bitmap = new u8[257 * 256]();

	unloadCharMap(); 
	
	chr = FT_Get_First_Char(face, &index);
	while (index > 0) {
		fontChar = new FontChar();
		fontChar->chr = chr;
		fontChar->fw = w;
		fontChar->w = w-1;
		fontChar->h = h-1;
		fontChar->p = w*2;
		fontChar->fy = 0;
		fontChar->rsx = 0;

		font = bitmap;
		
		// If the bitmap contains opaque pixels then add to rsx
		if (ttfToBitmap(face, chr, bitmap, &fontChar->fw, &h,  &fontChar->fy)) {
			texture = (u8 *) ((((long) texture) + 15) & ~15);
			fontChar->rsx = tiny3d_TextureOffset(texture);

			// Convert to A4R4G4B4
			for(a = 0; a < h; a++) {
				for(b = 0; b < w; b++) {
					i = font[(b * bpp)/8];
					i >>= (b & (7/bpp)) * bpp;
					i = (i & ((1 << bpp)-1)) * 255 / ((1 << bpp)-1);

					if(i) { //TINY3D_TEX_FORMAT_A4R4G4B4
						i>>=4;
						*((u16 *) texture) = (i<<12) | 0xfff;
					} else {
				  
						texture[0] = texture[1] = 0x0;
					}

					texture+=2;
				   
				}
				font += ((short)w * bpp) / 8;
			}
		}

		CharMap.push_back(fontChar);
		chr = FT_Get_Next_Char(face, chr, &index);
	}

	texture = (u8 *) ((((long) texture) + 15) & ~15);

	delete [] bitmap;

	return texture;
}

bool Font::ttfToBitmap(FT_Face face, unsigned int chr, u8 * bitmap, u8 *w, short *h, u8 *yCorrection)
{
	bool hasPixel = 0;
	int n, m, ww = 0;

	FT_Set_Pixel_Sizes(face, *w, *h);
	FT_GlyphSlot slot = face->glyph;

	if (FT_Load_Char(face, chr, FT_LOAD_RENDER )) {
		*w = 0;
		return 0;
	}
	
	memset(bitmap, 0, (*w) * (*h));

	*yCorrection = (*h) - 1 - slot->bitmap_top;
	
	for(n = 0; n < slot->bitmap.rows; n++) {
		for (m = 0; m < slot->bitmap.width; m++) {
			if(m >= (*w) || n >= (*h))
				continue;
			bitmap[m] = (u8) slot->bitmap.buffer[ww + m];

			if (!hasPixel) {
				if (!bitmap[m] || !bitmap[m+2] || !bitmap[m+3] || !bitmap[m+4])
					hasPixel = 1;
			}
		}
	
		bitmap += *w;
		ww += slot->bitmap.width;
	}
	
	//*w = ((slot->advance.x + 31) >> 6) + ((slot->bitmap_left < 0) ? -slot->bitmap_left : 0) - 0;
	*w = ((slot->advance.x + 31) >> 6) - (slot->bitmap_left) - 1;

	return hasPixel;
}
