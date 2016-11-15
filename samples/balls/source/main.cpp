#include <stdio.h>
#include <io/pad.h>
#include <math.h>

#include <vector>
#include <algorithm>
#include <memory>

#include <Mini2D/Mini2D.hpp>
#include <Mini2D/Image.hpp>

#include "Ball.hpp"

#include "paper_jpg.h"
#include "ball_png.h"
#include "cannon_png.h"

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

// The cannon image isn't rendered upright so we have to offset the analog stick's angle by the cannon's starting angle
// This is an estimation that looks good enough rendered
const float CANNON_ANGLE_OFFSET = 35.f;
float CannonAngle = 0;

const unsigned int BALL_RGBA = 0xFF00FFFF;
const float BALL_MINSPEED = 50;

float BallSpeed = 1000;

Vector2 SIZE_CANNON(0.1*mini.MAXW,0.1*mini.MAXW);
Vector2 SIZE_BALL(0.025*mini.MAXW,0.025*mini.MAXW);
Vector2 SIZE_FULL(mini.MAXW,mini.MAXH);

Vector2 CENTER(0.5*mini.MAXW,0.5*mini.MAXH);
Vector2 BALL_START(CENTER.X+SIZE_CANNON.X/2,CENTER.Y-SIZE_CANNON.Y/2);


std::vector<Ball*> balls;

int main(s32 argc, const char* argv[]) {
	paper = new Image(&mini);
	paper->Load((void*)paper_jpg, paper_jpg_size, Image::IMAGE_TYPE_JPG);
	paper->DrawRegion.Location = CENTER;
	paper->DrawRegion.Dimension = SIZE_FULL;

	ball = new Image(&mini);
	ball->Load((void*)ball_png, ball_png_size, Image::IMAGE_TYPE_PNG);
	ball->DrawRegion.Dimension = SIZE_BALL;

	cannon = new Image(&mini);
	cannon->Load((void*)cannon_png, cannon_png_size, Image::IMAGE_TYPE_PNG);
	cannon->DrawRegion.Location = CENTER;
	cannon->DrawRegion.Dimension = SIZE_CANNON;
	// The anchor point is the center of the cannon's wheel
	// I designed it such that the wheel is in the center of the third quadrant
	cannon->DrawRegion.Anchor = CENTER + Vector2(-SIZE_CANNON.X/4, SIZE_CANNON.Y/4);
	cannon->DrawRegion.UseAnchor = 1;

	mini.SetAnalogDeadzone(15);
	mini.SetClearColor(0x23B2D7FF);
	mini.SetAlphaState(1);
	mini.BeginDrawLoop();

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
	float magnitude;

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

