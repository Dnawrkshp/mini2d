/*
 * Mini2D.cpp
 *
 *  Created on: November 4th, 2016
 *  Author: Daniel Gerendasy
 */

#include <string.h>							// memcpy
#include <stdio.h>							// printf
#include <sys/time.h>

#include <sysutil/sysutil.h>				// Register sysutil callback (when program exits, xmb menu opens/closes, etc)
#include <sysmodule/sysmodule.h>			// Load and unload PNG,JPG modules
#include <lv2/process.h>					// sysProcessExit()

#include <tiny3d.h>							// Tiny3D functions
#include <Mini2D/Mini2D.hpp>				// Class definition


#define ANA_DIF_SHIFT(a,b,d,r) ((a==b) ? 0 : ((a<b) ? ((b-a)>=d ? r : 0) : (((a-b)>=d ? r : 0))))

// Exit callback
static Mini2D::ExitCallback_f _exitCallback;

// Called when program unload
void unload();

// Callback for system events
static void sys_callback(uint64_t status, uint64_t param, void* userdata);


//---------------------------------------------------------------------------
// Init Functions
//---------------------------------------------------------------------------
Mini2D::Mini2D(PadCallback_f pCallback, DrawCallback_f dCallback, ExitCallback_f eCallback) : 
				_padCallback(pCallback), _drawCallback(dCallback) {
	// Init tiny3d
	tiny3d_Init(1024*1024);
	tiny3d_UserViewport(1, (float)Video_Resolution.width/2.f, (float)Video_Resolution.height/2.f, (float)Video_Resolution.width/2.f, (float)Video_Resolution.height/2.f, 0, 0);

	_textureMem = tiny3d_AllocTexture(64*1024*1024);
	TexturePointer = (unsigned int*)_textureMem;

	// Init pad
	ioPadInit(7);
	ioPadSetPressMode(0, 1);

	// Load modules
	sysModuleLoad(SYSMODULE_PNGDEC);
	sysModuleLoad(SYSMODULE_JPGDEC);

	// Setup sysUtil callback
	sysUtilRegisterCallback(SYSUTIL_EVENT_SLOT0, sys_callback, NULL);

	// Private variables
	_maxFrameCount = 0xFFFFFFFA;
	_clearColor = 0xFF000000;
	_alphaEnabled = 0;
	_analogDeadzone = 0;

	_exitCallback = eCallback;
}

Mini2D::~Mini2D() {
	unload();
}

void unload() {
	// Unload modules
	if (!sysModuleIsLoaded(SYSMODULE_PNGDEC))
		sysModuleUnload(SYSMODULE_PNGDEC);
	if (!sysModuleIsLoaded(SYSMODULE_JPGDEC))
		sysModuleUnload(SYSMODULE_JPGDEC);
}

static void sys_callback(uint64_t status, uint64_t param, void* userdata) {

	 switch (status) {
		case SYSUTIL_EXIT_GAME: //0x0101
			unload();
			
			if (_exitCallback != NULL)
				_exitCallback();

			sysProcessExit(1);
			break;
	  
		case SYSUTIL_MENU_OPEN: //0x0131

			break;
		case SYSUTIL_MENU_CLOSE: //0x0132

			break;
	   default:
		   break;
		 
	}
}

//---------------------------------------------------------------------------
// Property Get/Set Functions
//---------------------------------------------------------------------------
void Mini2D::SetClearColor(unsigned int color) {
	_clearColor = color;
}

void Mini2D::SetAlphaState(bool enable) {
	_alphaEnabled = enable;
}

void Mini2D::SetAnalogDeadzone(unsigned char deadzone) {
	_analogDeadzone = (unsigned short)deadzone;
}

void Mini2D::ResetTexturePointer() {
	TexturePointer = (unsigned int*)_textureMem;
}

//---------------------------------------------------------------------------
// Draw Loop/Pad Functions
//---------------------------------------------------------------------------
void Mini2D::BeginDrawLoop() {
	float deltaTime = 0.f;
	struct timeval start, end;

	if (_drawCallback == NULL)
		return;

	gettimeofday(&end, NULL);
	while (1) {
		gettimeofday(&start, NULL);
		deltaTime = (start.tv_sec - end.tv_sec) * 1.f + (start.tv_usec - end.tv_usec) / 1000000.f;
		memcpy(&end, &start, sizeof(timeval));

		// Setup frame
		tiny3d_Clear(_clearColor, TINY3D_CLEAR_ALL);

		// Enable alpha Test
		tiny3d_AlphaTest(_alphaEnabled, 0, TINY3D_ALPHA_FUNC_GEQUAL);
		tiny3d_BlendFunc(_alphaEnabled, (blend_src_func)(TINY3D_BLEND_FUNC_SRC_RGB_SRC_ALPHA | TINY3D_BLEND_FUNC_SRC_ALPHA_SRC_ALPHA),
							(blend_dst_func)(TINY3D_BLEND_FUNC_DST_RGB_ONE_MINUS_SRC_ALPHA | TINY3D_BLEND_FUNC_DST_ALPHA_ZERO),
							(blend_func)(TINY3D_BLEND_RGB_FUNC_ADD | TINY3D_BLEND_ALPHA_FUNC_ADD));

		// 2D context
		tiny3d_Project2D();

		// Process pad
		if (_padCallback != NULL)
			Pad();

		// Call user draw
		if (_drawCallback(deltaTime==0?(1/60.f):deltaTime, _frameCount) < 0)
			break;

		// Flip frame
		tiny3d_Flip();

		_frameCount++;
		if (_frameCount >= _maxFrameCount)
			_frameCount = 0;
	}
}

void Mini2D::Pad() {
	int p;
	padInfo padinfo;
	padData pData;
	int changed;
	
	// Check the pads.
	ioPadGetInfo(&padinfo);
	for (p=0;p<MAX_PORT_NUM;p++) {
		memset(&pData, 0, sizeof(padData));
		if (padinfo.status[p]) {
			if (!ioPadGetData(p, &pData)) {
				if (!pData.len) {
					if (_padData[p].len && _padCallback) {
						_padCallback(0, p, _padData[p]);
					}
				}
				else {
					// Create changed value
					changed = (pData.BTN_LEFT == _padData[p].BTN_LEFT ? 0 : BTN_CHANGED_LEFT) |
							(pData.BTN_RIGHT == _padData[p].BTN_RIGHT ? 0 : BTN_CHANGED_RIGHT) |
							(pData.BTN_UP == _padData[p].BTN_UP ? 0 : BTN_CHANGED_UP) |
							(pData.BTN_DOWN == _padData[p].BTN_DOWN ? 0 : BTN_CHANGED_DOWN) |
							(pData.BTN_R1 == _padData[p].BTN_R1 ? 0 : BTN_CHANGED_R1) |
							(pData.BTN_L1 == _padData[p].BTN_L1 ? 0 : BTN_CHANGED_L1) |
							(pData.BTN_R2 == _padData[p].BTN_R2 ? 0 : BTN_CHANGED_R2) |
							(pData.BTN_L2 == _padData[p].BTN_L2 ? 0 : BTN_CHANGED_L2) |
							(pData.BTN_CROSS == _padData[p].BTN_CROSS ? 0 : BTN_CHANGED_CROSS) |
							(pData.BTN_SQUARE == _padData[p].BTN_SQUARE ? 0 : BTN_CHANGED_SQUARE) |
							(pData.BTN_TRIANGLE == _padData[p].BTN_TRIANGLE ? 0 : BTN_CHANGED_TRIANGLE) |
							(pData.BTN_CIRCLE == _padData[p].BTN_CIRCLE ? 0 : BTN_CHANGED_CIRCLE) |
							(pData.BTN_START == _padData[p].BTN_START ? 0 : BTN_CHANGED_START) |
							(pData.BTN_SELECT == _padData[p].BTN_SELECT ? 0 : BTN_CHANGED_SELECT) |
							(pData.BTN_R3 == _padData[p].BTN_R3 ? 0 : BTN_CHANGED_R3) |
							(pData.BTN_L3 == _padData[p].BTN_L3 ? 0 : BTN_CHANGED_L3) |
							ANA_DIF_SHIFT(pData.ANA_R_H, _padData[p].ANA_R_H, _analogDeadzone, ANA_CHANGED_R_H) |
							ANA_DIF_SHIFT(pData.ANA_R_V, _padData[p].ANA_R_V, _analogDeadzone, ANA_CHANGED_R_V) |
							ANA_DIF_SHIFT(pData.ANA_L_H, _padData[p].ANA_L_H, _analogDeadzone, ANA_CHANGED_L_H) |
							ANA_DIF_SHIFT(pData.ANA_L_V, _padData[p].ANA_L_V, _analogDeadzone, ANA_CHANGED_L_V);

					memcpy((void*)&_padData[p], (void*)&pData, sizeof(padData));
					if (_padCallback != NULL)
						_padCallback(changed, p, pData);
				}
			}
		}
	}
}

//---------------------------------------------------------------------------
// Draw/Add Texture Functions
//---------------------------------------------------------------------------
unsigned int Mini2D::AddTexture(void * pixelData, int pitch, int height) {
	unsigned int textureOff = 0;
	// copy texture datas from pixelData to the RSX memory allocated for textures
	if (pixelData) {
		memcpy(TexturePointer, pixelData, pitch * height);
		textureOff = tiny3d_TextureOffset(TexturePointer);	  		// get the offset (RSX use offset instead address)
		TexturePointer += ((pitch * height + 15) & ~15) / 4; 			// aligned to 16 bytes (it is u32) and update the pointer
	}
	return textureOff;
}

void Mini2D::DrawTexture(u32 textureOff, int pitch, int width, int height, float x, float y, float w, float h, unsigned int rgba, float angle, unsigned int colorFormat) {
	tiny3d_SetTextureWrap(0, textureOff, width, height, pitch,  
		(text_format)colorFormat, TEXTWRAP_CLAMP, TEXTWRAP_CLAMP, TEXTURE_LINEAR);
	drawSpriteRot(x, y, 0, w, h, rgba, angle);
}

void Mini2D::DrawRectangle(float x, float y, float dx, float dy, unsigned int rgba, float angle) {
	MATRIX matrix;
	float z = 0.f;

	dx /= 2.0f; dy /= 2.0f;

	// rotate and translate the sprite
	matrix = MatrixRotationZ(angle);
	matrix = MatrixMultiply(matrix, MatrixTranslation(x + dx, y + dy, 0.0f));

	tiny3d_SetMatrixModelView(&matrix);

	tiny3d_SetPolygon(TINY3D_QUADS);

	tiny3d_VertexPos(x	 , y	 , z);
	tiny3d_VertexColor(rgba);

	tiny3d_VertexPos(x + dx, y	 , z);

	tiny3d_VertexPos(x + dx, y + dy, z);

	tiny3d_VertexPos(x	 , y + dy, z);

	tiny3d_End();
}

void Mini2D::drawSpriteRot(float x, float y, float layer, float dx, float dy, u32 rgba, float angle) {
	dx /= 2.0f; dy /= 2.0f;

	MATRIX matrix;
	
	// rotate and translate the sprite
	matrix = MatrixRotationZ(angle);
	matrix = MatrixMultiply(matrix, MatrixTranslation(x + dx, y + dy, 0.0f));
	
	// fix ModelView Matrix
	tiny3d_SetMatrixModelView(&matrix);
   
	tiny3d_SetPolygon(TINY3D_QUADS);

	tiny3d_VertexPos(-dx, -dy, layer);
	tiny3d_VertexColor(rgba);
	tiny3d_VertexTexture(0.0f , 0.0f);

	tiny3d_VertexPos(dx , -dy, layer);
	tiny3d_VertexTexture(0.99f, 0.0f);

	tiny3d_VertexPos(dx , dy , layer);
	tiny3d_VertexTexture(0.99f, 0.99f);

	tiny3d_VertexPos(-dx, dy , layer);
	tiny3d_VertexTexture(0.0f , 0.99f);

	tiny3d_End();

	tiny3d_SetMatrixModelView(NULL); // set matrix identity

}

