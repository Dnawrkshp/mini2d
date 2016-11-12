/*
 * Font.cpp
 *
 *  Created on: November 6th, 2016
 *  Author: Daniel Gerendasy
 */

#include <freetype/ftglyph.h>				// For loading TTF character and converting to raw RGBA

#include <tiny3d.h>							// Tiny3D functions
#include <Mini2D/Font.hpp>					// Class definition

void TTFToBitmap(FT_Face face, u8 chr, u8 * bitmap, short *w, short *h, short *yCorrection);
u8 * AddFontFromTTF(FT_Face face, u8 *texture, u8 firstChar, u8 lastChar, short w, short h);

struct _fontInfo_t
{
	short w, h, bh;
	
	u8 firstChar;
	u8 lastChar;
	
	u32 rsxOffset;
	u32 bytesPerChar; 
	u32 colorFormat;

	short fw[256]; // chr width
	short fy[256]; // chr y correction
};

struct _fontData_t
{
	struct _fontInfo_t font;

	float sx, sy;

	u32 color, bColor;

	float X,Y,Z;

} FontData;


//---------------------------------------------------------------------------
// Init Functions
//---------------------------------------------------------------------------
Font::Font(Mini2D * mini) :
		_mini(mini) {

}

Font::~Font() {
	_mini = NULL;
}

//---------------------------------------------------------------------------
// Print Functions
//---------------------------------------------------------------------------
void Font::Print(char * string, Vector2 location, Vector2 size, unsigned int rgba, FontPrintAlign align, float layer) {
	int i;
	if (!_mini || !string)
		return;
	if (rgba)
		FontData.color = rgba;

	FontData.sx = size.X;
	FontData.sy = size.Y;
	switch (align) {
		case PRINT_ALIGN_LEFT:
			break;
		case PRINT_ALIGN_RIGHT:
			location.X -= getWidth(string);
			break;
		case PRINT_ALIGN_CENTER:
			location.X -= getWidth(string) / 2.f;
			break;
	}

	for (i=0;i<(int)strlen(string);i++) {
		location.X += printChar(string[i], location.X, location.Y, layer, FontData.color);
	}
}

float Font::printChar(char c, float x, float y, float z, unsigned int rgba) {
	float dx2 = FontData.sx * ((float)FontData.font.fw[(int)c] / (float)FontData.font.w);  
	if (!_mini)
		return 0;
	if (c > FontData.font.lastChar || c < FontData.font.firstChar)
		return 0;

	// Draw background
	if (FontData.bColor)
		_mini->DrawRectangle(x, y, x, y, z, dx2, FontData.sy, FontData.bColor, 0);

	y += (float)FontData.font.fy[(int)c] * (FontData.sy / (float)FontData.font.h);

	_mini->DrawTexture(FontData.font.rsxOffset + FontData.font.bytesPerChar * (c - FontData.font.firstChar),
					FontData.font.w * (FontData.font.colorFormat == TINY3D_TEX_FORMAT_A8R8G8B8 ? 4 : 2),
					FontData.font.fw[(int)c],
					FontData.font.h,
					x,
					y,
					z,
					dx2,
					FontData.sy,
					rgba,
					0,
					FontData.font.colorFormat);

	return dx2;
}

float Font::getWidth(char * string) {
	if (!string)
		return 0.f;

	float width = 0.f;

	while(*string) {
		width += FontData.sx * ((float)FontData.font.fw[(int)*string++] / (float)FontData.font.w);
	}

	return width;
}

//---------------------------------------------------------------------------
// Load TTF Functions
//---------------------------------------------------------------------------
Font::FontLoadStatus Font::Load(char * filepath) {
	if (!filepath)
		return FONT_INVALID_ARG;
	if (!_mini)
		return FONT_INVALID_MINI2D;

	return loadFont(filepath, NULL, 0);
}

Font::FontLoadStatus Font::Load(void * buffer, unsigned int size) {
	if (!buffer || size == 0)
		return FONT_INVALID_ARG;
	if (!_mini)
		return FONT_INVALID_MINI2D;

	return loadFont(NULL, buffer, size);
}


Font::FontLoadStatus Font::loadFont(char * path, void * buffer, int size)
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


	_mini->TexturePointer = (u32*)AddFontFromTTF(face, (u8*)_mini->TexturePointer, 32, 255, 32, 32);

	FT_Done_Face(face);
	FT_Done_FreeType(freetype);
	return FONT_SUCCESS;
}

/*
 * AddFontFromTTF:
 * 		Moves font glyphs into RSX
 * 
 * Taken from https://github.com/wargio/tiny3D
 * (c) 2010 Hermes  <www.elotrolado.net>
 */
u8 * AddFontFromTTF(FT_Face face, u8 *texture, u8 firstChar, u8 lastChar, short w, short h)
{
	int n, a, b;
	u8 i;
	u8 *font;
	u8 bitmap[257 * 256];
	
	int bpp = 8;
	
	if(h < 8) h = 8;
	if(w < 8) w = 8;
	if(h > 256) h = 256;
	if(w > 256) w = 256;

	FontData.font.w = w;
	FontData.font.h = h;
	FontData.font.bh = h+4;
	FontData.font.colorFormat = TINY3D_TEX_FORMAT_A4R4G4B4;
	FontData.font.firstChar = firstChar;
	FontData.font.lastChar  = lastChar;

	FontData.color = 0xffffffff;
	FontData.bColor = 0x0;

	FontData.sx = w/(float)Video_Resolution.width;
	FontData.sy = h/(float)Video_Resolution.height;

	FontData.Z = 0.0f;

	for(n = 0; n < 256; n++) {
		FontData.font.fw[n] = 0; 
		FontData.font.fy[n] = 0;
	}

	   
	for(n = firstChar; n <= lastChar; n++) {
		
		short hh = h;

		font = bitmap;

		FontData.font.fw[n] = w;
		
		//ttf_callback((u8) (n & 255), bitmap, &FontData.font.fw[n], &hh,  &FontData.font.fy[n]);
		TTFToBitmap(face, (u8)(n&0xFF), bitmap, &FontData.font.fw[n], &hh,  &FontData.font.fy[n]);

		// letter background correction
		if((hh + FontData.font.fy[n]) > FontData.font.bh) 
			FontData.font.bh = hh + FontData.font.fy[n];

		texture = (u8 *) ((((long) texture) + 15) & ~15);

		if(n == firstChar) {
			FontData.font.rsxOffset = tiny3d_TextureOffset(texture);
		}
		if(n == firstChar+1) {
			FontData.font.bytesPerChar = tiny3d_TextureOffset(texture) - FontData.font.rsxOffset;
		}



		for(a = 0; a < h; a++) {
			for(b = 0; b < w; b++) {
				
				i = font[(b * bpp)/8];

				i >>= (b & (7/bpp)) * bpp;
				
				i = (i & ((1 << bpp)-1)) * 255 / ((1 << bpp)-1);

				if(i) {//TINY3D_TEX_FORMAT_A4R4G4B4
					i>>=4;
					*((u16 *) texture) = (i<<12) | 0xfff;
				} else {
			  
					texture[0] = texture[1] = 0x0; //texture[2] = 0x0;
					//texture[3] = 0x0; // alpha
				} 
				texture+=2;
			   
			}

			font += ((short)w * bpp) / 8;
		}
	}

	texture = (u8 *) ((((long) texture) + 15) & ~15);

	return texture;
}

void TTFToBitmap(FT_Face face, u8 chr, u8 * bitmap, short *w, short *h, short *yCorrection)
{
	int width = *w;
	
	TTF_to_Bitmap_loop: ;
	FT_Set_Pixel_Sizes(face, (width), (*h));
	
	FT_GlyphSlot slot = face->glyph;

	if(FT_Load_Char(face, (char) chr, FT_LOAD_RENDER )) {(*w) = 0; return;}
	
	if (slot->bitmap.width > *w && width == *w)
	{
		width = (int)((float)*w * ((float)*w / (float)slot->bitmap.width)) - 1;
		goto TTF_to_Bitmap_loop;
	}
	
	memset(bitmap, 0, (*w) * (*h));
	
	int n, m, ww;

	*yCorrection = (*h) - 1 - slot->bitmap_top;
	
	ww = 0;

	for(n = 0; n < slot->bitmap.rows; n++) {
		for (m = 0; m < slot->bitmap.width; m++) {
				
				if(m >= (*w) || n >= (*h)) continue;
				
				bitmap[m] = (u8) slot->bitmap.buffer[ww + m];
			}
	
		bitmap += *w;
		ww += slot->bitmap.width;
	}
	
	*w = ((slot->advance.x + 31) >> 6) + ((slot->bitmap_left < 0) ? -slot->bitmap_left : 0) - 1;
	*h = slot->bitmap.rows;
}
