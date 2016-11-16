#include <stdio.h>
#include <io/pad.h>
#include <math.h>

#include <vector>
#include <algorithm>
#include <memory>

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

Mini2D mini((Mini2D::PadCallback_f)&padUpdate, (Mini2D::DrawCallback_f)&drawUpdate, (Mini2D::ExitCallback_f)&exit);

int doExit = 0;
int count = 0;
Image *paper;
Image *ball;
Image *cannon;
Font *openSansRegular;

// The cannon image isn't rendered upright so we have to offset the analog stick's angle by the cannon's starting angle
// This is an estimation that looks good enough rendered
const float CANNON_ANGLE_OFFSET = 35.f;
float CannonAngle = 0;

const unsigned int BALL_RGBA = 0xFFFFFFFF;
const float BALL_MINSPEED = 50;

const float SLIDER_BALLSPEED_MAX = 2000;
const float SLIDER_BALLSPEED_MIN = 500;
float BallSpeed = 1000;

Vector2 FONT_MEDIUM(0.04*mini.MAXW,0.04*mini.MAXH);

Vector2 SIZE_CANNON(0.1*mini.MAXW,0.1*mini.MAXW);
Vector2 SIZE_BALL(0.025*mini.MAXW,0.025*mini.MAXW);
Vector2 SIZE_PAPER(mini.MAXW,mini.MAXH*0.8);

Vector2 CENTER(0.5*mini.MAXW,0.5*mini.MAXH);
Vector2 BALL_START(CENTER.X+SIZE_CANNON.X/2,CENTER.Y-SIZE_CANNON.Y/2);

Vector2 PRINT_FPS(0.1*mini.MAXW, 0.05*mini.MAXH);
Vector2 PRINT_COUNT(0.9*mini.MAXW, 0.05*mini.MAXH);
Vector2 SLIDER_POWER(CENTER.X, 0.95*mini.MAXH);

std::vector<Ball*> balls;

int main(s32 argc, const char* argv[]) {
	// Load background paper
	paper = new Image(&mini);
	paper->Load((void*)paper_jpg, paper_jpg_size, Image::IMAGE_TYPE_JPG);
	paper->DrawRegion.Location = CENTER;
	paper->DrawRegion.Dimension = SIZE_PAPER;

	// Load ball
	ball = new Image(&mini);
	ball->Load((void*)ball_png, ball_png_size, Image::IMAGE_TYPE_PNG);
	ball->DrawRegion.Dimension = SIZE_BALL;

	// Load cannon
	cannon = new Image(&mini);
	cannon->Load((void*)cannon_png, cannon_png_size, Image::IMAGE_TYPE_PNG);
	cannon->DrawRegion.Location = CENTER;
	cannon->DrawRegion.Dimension = SIZE_CANNON;
	// The anchor point is the center of the cannon's wheel
	// I designed it such that the wheel is in the center of the third quadrant
	cannon->DrawRegion.Anchor = CENTER + Vector2(-SIZE_CANNON.X/4, SIZE_CANNON.Y/4);
	cannon->DrawRegion.UseAnchor = 1;

	// Load Open Sans Regular font
	openSansRegular = new Font(&mini);
	openSansRegular->Load((void*)OpenSans_Regular_ttf, OpenSans_Regular_ttf_size);

	mini.SetAnalogDeadzone(15);
	mini.SetClearColor(0xFF000000);
	mini.SetAlphaState(1);
	mini.BeginDrawLoop();

	return 0;
}

int drawUpdate(float deltaTime, unsigned int frame) {
	char buffer[11];

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

 	// Print FPS
	sprintf((char*)buffer, "FPS: %.2f", 1/deltaTime);
	openSansRegular->Print((char*)buffer, PRINT_FPS, FONT_MEDIUM, 0xFFFFFFFF, Font::PRINT_ALIGN_CENTERLEFT);

	// Print number of drawn balls
	sprintf((char*)buffer, "Balls: %d", count);
	openSansRegular->Print((char*)buffer, PRINT_COUNT, FONT_MEDIUM, 0xFFFFFFFF, Font::PRINT_ALIGN_CENTERRIGHT);

	// Draw power scale bar
	float scaleW = BallSpeed/SLIDER_BALLSPEED_MAX;
	mini.DrawRectangle(SLIDER_POWER.X,SLIDER_POWER.Y,SLIDER_POWER.X,SLIDER_POWER.Y,0,scaleW*mini.MAXW,0.1*mini.MAXH,0xC00000FF,0);

	// Print power %
	sprintf((char*)buffer, "Power: %.0f%%", scaleW*100);
	openSansRegular->Print((char*)buffer, SLIDER_POWER, FONT_MEDIUM, 0xFFFFFFFF, Font::PRINT_ALIGN_CENTER);

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
		Ball * newBall = new Ball(&mini, ball, BallCollisionCheck);										// Create new ball
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
}

bool BallCollisionCheck(Ball * ball, Vector2 * normal) {
	bool ballCollide = 0;
	int points = 0;

	// Check if the ball goes out of bounds
	if (paper->DrawRegion.Intersect(&ball->DrawRegion, normal, &points))
		return 1;
	// If the ball is fully out of bounds (velocity must be too high)
	if (points == 0) {
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

