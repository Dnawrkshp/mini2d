#include <stdio.h>
#include <io/pad.h>
#include <math.h>

#include <vector>
#include <algorithm>
#include <memory>
#include <string>
#include <sstream>

#include <Mini2D/Mini2D.hpp>
#include <Mini2D/Image.hpp>
#include <Mini2D/Font.hpp>

#include "Ball.hpp"

#include "paper_jpg.h"
#include "ball_png.h"
#include "cannon_png.h"

#include "OpenSans_Regular_ttf.h"


int drawUpdate(float deltaTime, unsigned int frame);
void padUpdate(int changed, int port, padData pData);
void exit();

// Called by Ball on Draw()
bool BallCollisionCheck(Ball * ball, Vector2 * normal);

Mini2D * mini = NULL;

Image *paper = NULL;
Image *ball = NULL;
Image *cannon = NULL;

Font *openSansRegular = NULL;

int doExit = 0;
int count = 0;

// The cannon image isn't rendered upright so we have to offset the analog stick's angle by the cannon's starting angle
// This is an estimation that looks good enough rendered
const float CANNON_ANGLE_OFFSET = 	35.f;
float CannonAngle = 				0;

const unsigned int BALL_RGBA = 		0xFFFFFFFF;
const float BALL_MINSPEED = 		50;

const float SLIDER_BALLSPEED_MAX = 	2000;
const float SLIDER_BALLSPEED_MIN = 	500;
float BallSpeed = 					1000;

// Image sizes
Vector2 SIZE_CANNON;
Vector2 SIZE_BALL;
Vector2 SIZE_PAPER;
// Image locations
Vector2 CENTER;
Vector2 BALL_START;

// Font sizes
Vector2 FONT_MEDIUM;
// Font locations
Vector2 PRINT_FPS;
Vector2 PRINT_COUNT;
Vector2 PRINT_SLIDER_POWER;

std::vector<Ball*> balls;

int main(s32 argc, const char* argv[]) {

	// Load Mini2D
	mini = new Mini2D((Mini2D::PadCallback_f)&padUpdate, (Mini2D::DrawCallback_f)&drawUpdate, (Mini2D::ExitCallback_f)&exit);

	// Initialize location and size vectors
	SIZE_CANNON = Vector2(					0.1*mini->MAXW,					0.1*mini->MAXW);
	SIZE_BALL = Vector2(					0.025*mini->MAXW,				0.025*mini->MAXW);
	SIZE_PAPER = Vector2(					mini->MAXW,						mini->MAXH*0.8);
	
	CENTER = Vector2(						0.5*mini->MAXW,					0.5*mini->MAXH);
	BALL_START = Vector2(					CENTER.X+SIZE_CANNON.X/2,		CENTER.Y-SIZE_CANNON.Y/2);

	FONT_MEDIUM = Vector2(					0.04*mini->MAXW,				0.04*mini->MAXH);

	PRINT_FPS = Vector2(					0.1*mini->MAXW,					0.05*mini->MAXH);
	PRINT_COUNT = Vector2(					0.9*mini->MAXW,					0.05*mini->MAXH);
	PRINT_SLIDER_POWER = Vector2(			CENTER.X,						0.95*mini->MAXH);


	// Load background paper
	paper = new Image(mini);
	paper->Load((void*)paper_jpg, paper_jpg_size, Image::IMAGE_TYPE_JPG);
	paper->DrawRegion.Location = CENTER;
	paper->DrawRegion.Dimension = SIZE_PAPER;

	// Load ball
	ball = new Image(mini);
	ball->Load((void*)ball_png, ball_png_size, Image::IMAGE_TYPE_PNG);
	ball->DrawRegion.Dimension = SIZE_BALL;

	// Load cannon
	cannon = new Image(mini);
	cannon->Load((void*)cannon_png, cannon_png_size, Image::IMAGE_TYPE_PNG);
	cannon->DrawRegion.Location = CENTER;
	cannon->DrawRegion.Dimension = SIZE_CANNON;
	// The anchor point is the center of the cannon's wheel
	// I designed it such that the wheel is in the center of the third quadrant
	cannon->DrawRegion.Anchor = CENTER + Vector2(-SIZE_CANNON.X/4, SIZE_CANNON.Y/4);
	cannon->DrawRegion.UseAnchor = 1;

	// Load Open Sans Regular font
	openSansRegular = new Font(mini);
	openSansRegular->Load((void*)OpenSans_Regular_ttf, OpenSans_Regular_ttf_size);
	openSansRegular->ForeColor = 0xFFFFFFFF;

	mini->SetAnalogDeadzone(15);
	mini->SetClearColor(0xFF000000);
	mini->SetAlphaState(1);
	mini->BeginDrawLoop();

	return 0;
}

int drawUpdate(float deltaTime, unsigned int frame) {
	paper->Draw(0xFFFFFFFF);
	cannon->Draw(0xFFFFFFFF);

	count = 0;
	for(std::vector<Ball*>::iterator it = balls.begin(); it != balls.end();) {
    	if (!((*it)->Draw(deltaTime))) {
    		delete *it;
    		it = balls.erase(it);
    	}
    	else {
    		it++;
    	}

    	count++;
 	}

 	// Draw power scale bar
	float scaleW = BallSpeed/SLIDER_BALLSPEED_MAX;
	mini->DrawRectangle(PRINT_SLIDER_POWER.X,PRINT_SLIDER_POWER.Y,PRINT_SLIDER_POWER.X,PRINT_SLIDER_POWER.Y,0,scaleW*mini->MAXW,0.1*mini->MAXH,0xC00000FF,0);

 	// Print FPS
 	openSansRegular->TextAlign = Font::PRINT_ALIGN_CENTERLEFT;
	openSansRegular->PrintFormat(PRINT_FPS, FONT_MEDIUM, 0, 0, 11, L"FPS: %.2f", 1/deltaTime);

	// Print number of drawn balls
	openSansRegular->TextAlign = Font::PRINT_ALIGN_CENTERRIGHT;
	openSansRegular->PrintFormat(PRINT_COUNT, FONT_MEDIUM, 0, 0, 10, L"Balls: %d", count);

	// Print power %
	openSansRegular->TextAlign = Font::PRINT_ALIGN_CENTER;
	openSansRegular->PrintFormat(PRINT_SLIDER_POWER, FONT_MEDIUM, 0, 0, 12, L"Power: %.0f%%", scaleW * 100);

	return doExit;
}

void padUpdate(int changed, int port, padData pData) {
	short x,y,dead=10;
	float theta;

	if (pData.BTN_START && changed & Mini2D::BTN_CHANGED_START)
		doExit = -1;

	// Direct cannon
	x = 0x80 - pData.ANA_L_H;
	y = 0x80 - pData.ANA_L_V;
	if (x <= dead && x >= -dead)
		x = 0;
	if (y <= dead && y >= -dead)
		y = 0;

	if (x || y) {
		theta = atan2(pData.ANA_L_V-0x80,pData.ANA_L_H-0x80);
		CannonAngle = CANNON_ANGLE_OFFSET + (theta * 180.f / 3.14159);
		cannon->DrawRegion.RectangleAngle = CannonAngle;
		cannon->DrawRegion.AnchorAngle = CannonAngle;
	}

	// Adjust power slider bar
	x = 0x80 - pData.ANA_R_H;
	if (x <= dead && x >= -dead)
		x = 0;

	BallSpeed += x;
	if (BallSpeed > SLIDER_BALLSPEED_MAX)
		BallSpeed = SLIDER_BALLSPEED_MAX;
	else if (BallSpeed < SLIDER_BALLSPEED_MIN)
		BallSpeed = SLIDER_BALLSPEED_MIN;

	// If CROSS is pressed and we haven't hit the max number of balls
	if (pData.BTN_CROSS && changed & Mini2D::BTN_CHANGED_CROSS && count < 50) {
		Ball * newBall = new Ball(mini, ball, BallCollisionCheck);										// Create new ball
		newBall->MinSpeed = BALL_MINSPEED;																// Set minimum speed
		newBall->DrawRegion.Location.Set(BALL_START);													// Set location to start location
		newBall->DrawRegion.Location.RotateAroundPoint(&cannon->DrawRegion.Anchor, CannonAngle);		// Rotate location around anchor point to get new start location
		newBall->Velocity.Set(newBall->DrawRegion.Location - cannon->DrawRegion.Anchor);				// Calculate velocity by subtracting the new location from the anchor point
		newBall->Velocity.Normalize();																	// Normalize to ensure consistent speed before scaling
		newBall->Velocity *= BallSpeed;																	// Scale velocity
		newBall->RGBA = BALL_RGBA;																		// Set color
		balls.push_back(newBall);																		// Add to vector<Ball*>
	}
}

void exit() {
	printf("exiting\n");

	for(std::vector<Ball*>::iterator it = balls.begin(); it != balls.end();) {
		if (*it)
			delete *it;
	}
	balls.clear();

	// Clean up
	if (paper) {
		delete paper;
		paper = NULL;
	}

	if (ball) {
		delete ball;
		ball = NULL;
	}

	if (cannon) {
		delete cannon;
		cannon = NULL;
	}

	if (openSansRegular) {
		delete openSansRegular;
		openSansRegular = NULL;
	}

	if (mini) {
		delete mini;
		mini = NULL;
	}
}

bool BallCollisionCheck(Ball * ball, Vector2 * normal) {
	bool ballCollide = 0;

	// Check if the ball goes out of bounds
	int boundsCheck = paper->DrawRegion.Intersect(&ball->DrawRegion, normal);
	if (boundsCheck)
		return 1;
	
	// If the ball is fully out of bounds (velocity must be too high)
	if (boundsCheck == -1) {
		normal->Set(0,0);
		return 1;
	}

	// Check if the ball collides with another ball
	for(std::vector<Ball*>::iterator it = balls.begin(); it != balls.end(); it++) {
    	if ((*it) != ball && (*it)->DrawRegion.Intersect(&ball->DrawRegion, normal)) {
    		ballCollide = 1;

    		// Here we can affect the velocity of the other ball
    		Vector2 n = (*it)->DrawRegion.Location - ball->DrawRegion.Location;
    		n.Normalize();
    		if (n.Magnitude() == 0) {
    			n.X = 1;
    			n.Y = 0;
    		}

    		float dA = Vector2::DotProduct(&(*it)->Velocity,&n);
    		float dB = Vector2::DotProduct(&ball->Velocity,&n);

    		(*it)->Velocity += n * (dB - dA);
    		ball->Velocity += n * (dA - dB);
    	}
 	}

	return ballCollide;
}
