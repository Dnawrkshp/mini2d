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

//---------------------------------------------------------------------------
// Misc Functions
//---------------------------------------------------------------------------
bool CircleF::Intersect(CircleF * circle, Vector2 * normal) {
	if (!circle)
		return 0;

	Vector2 * center = circle->GetRotatedCenter();
	Vector2 direction = _cCenter - (*center);
	float dist = direction.Magnitude();
	if (dist <= (Radius + circle->Radius)) {
		if (normal) {
			normal->X += direction.X;
			normal->Y += direction.Y;
			normal->Normalize();
		}
		return 1;
	}

	return 0;
}

bool CircleF::Intersect(Vector2 * polygon[], int polyCount, Vector2 * normal, int * points) {
	int i = 0, c = 0;
	Vector2 * temp;
	Vector2 dif;
	float dist;

	if (!polygon || polyCount < 2)
		return 0;

	for (i = 0; i < polyCount; i++) {
		temp = polygon[((i==polyCount-1)?0:i+1)];
		dist = abs(Vector2::CrossProduct(polygon[i],temp,&_cCenter))/Vector2::DistanceFrom(temp,polygon[i]);

		if (dist <= Radius) {
			if (normal) {
				dif.Set(*temp - *polygon[i]);
				normal->X -= dif.Y;
				normal->Y += dif.X;
			}
			c++;
		}
	}

	if (points)
		*points = c;
	return c > 0 && c < polyCount;
}
