/*
 * CircleF.cpp
 *
 *  Created on: November 22nd, 2016
 *  Author: Daniel Gerendasy
 */

#include <stdlib.h>							// abs()

#include <Mini2D/Units.hpp>					// Class definition

//---------------------------------------------------------------------------
// Init Functions
//---------------------------------------------------------------------------
CircleF::CircleF() {
	Init(0,0,1);
}

CircleF::CircleF(Vector2 center, float radius) {
	Init(center.X,center.Y,radius);
}

CircleF::CircleF(Vector2 center) {
	Init(center.X,center.Y,1);
}

CircleF::CircleF(float radius) {
	Init(0,0,radius);
}

CircleF::~CircleF() {

}

void CircleF::Init(float x, float y, float r) {
	Location.X = x;
	Location.Y = y;
	Radius = r;

	UseAnchor = 0;
	AnchorAngle = 0;
	CircleAngle = 0;

	_lastUA = 1;

	Update();
}

void CircleF::Update() {
	if (_lastX == Location.X && _lastY == Location.Y &&
		_lastAX == Anchor.X && _lastAY == Anchor.Y &&
		_lastA == AnchorAngle && _lastUA == UseAnchor && 
		_lastCA == CircleAngle && _lastR == Radius)
		return;

	_lastX = Location.X;
	_lastY = Location.Y;
	_lastA = AnchorAngle;
	_lastCA = CircleAngle;
	_lastR = Radius;
	_lastAX = Anchor.X;
	_lastAY = Anchor.Y;
	_lastUA = UseAnchor;

	_cCenter.Set(Location.X, Location.Y);

	if (AnchorAngle && UseAnchor) {
		_cCenter.RotateAroundPoint(&Anchor, AnchorAngle);
	}
}

//---------------------------------------------------------------------------
// Get/Set Functions
//---------------------------------------------------------------------------
float CircleF::X() {
	return Location.X;
}

float CircleF::Y() {
	return Location.Y;
}

float CircleF::R() {
	return Radius;
}

void CircleF::X(float x) {
	Location.X = x;
	Update();
}

void CircleF::Y(float y) {
	Location.Y = y;
	Update();
}

void CircleF::R(float r) {
	Radius = r;
	Update();
}

Vector2 * CircleF::GetRotatedCenter() {
	Update();
	return &_cCenter;
}
