/*
 * RectangleF.cpp
 *
 *  Created on: November 10th, 2016
 *  Author: Daniel Gerendasy
 */

#include <Mini2D/Units.hpp>


//---------------------------------------------------------------------------
// Init Functions
//---------------------------------------------------------------------------
RectangleF::RectangleF() {
	Init(0,0,0,0);
}

RectangleF::RectangleF(float x, float y, float w, float h) {
	Init(x,y,w,h);
}

RectangleF::RectangleF(Vector2 point, Vector2 dimension) {
	Init(point.X, point.Y, dimension.X, dimension.Y);
}

RectangleF::RectangleF(Vector2 point, float w, float h) {
	Init(point.X, point.Y, w, h);
}

RectangleF::RectangleF(RectangleF * rectangle) {
	Init(rectangle->X(), rectangle->Y(), rectangle->W(), rectangle->H());
}

RectangleF::~RectangleF() {
	
}

void RectangleF::Init(float x, float y, float w, float h) {
	Location.X = x;
	Location.Y = y;
	Dimension.X = w;
	Dimension.Y = h;
}

//---------------------------------------------------------------------------
// Get/Set Functions
//---------------------------------------------------------------------------
float RectangleF::X() {
	return Location.X;
}

void RectangleF::X(float x) {
	Location.X = x;
}

float RectangleF::Y() {
	return Location.Y;
}

void RectangleF::Y(float y) {
	Location.Y = y;
}

float RectangleF::W() {
	return Dimension.X;
}

void RectangleF::W(float w) {
	Dimension.X = w;
}

float RectangleF::H() {
	return Dimension.Y;
}

void RectangleF::H(float h) {
	Dimension.Y = h;
}

void RectangleF::FromCorners(Vector2 point1, Vector2 point2) {
	Init(point1.X<point2.X?point1.X:point2.X,
		point1.Y<point2.Y?point1.Y:point2.Y,
		point1.X<point2.X?point2.X-point1.X:point2.X-point1.X,
		point1.Y<point2.Y?point2.Y-point1.Y:point2.Y-point1.Y);
}

//---------------------------------------------------------------------------
// Misc Functions
//---------------------------------------------------------------------------
bool RectangleF::Intersect(RectangleF * rectangle) {
	if (Location.X < (rectangle->X() + rectangle->W()) &&
		(Location.X+Dimension.X) > rectangle->X() &&
		Location.Y < (rectangle->Y()+rectangle->H()) &&
		(Location.Y+Dimension.Y) > rectangle->Y())
		return 1;

    return 0;
}

bool RectangleF::Intersect(Vector2 * location, Vector2 * dimension) {
	if (Location.X < (location->X + dimension->X) &&
		(Location.X+Dimension.X) > location->X &&
		Location.Y < (location->Y+dimension->Y) &&
		(Location.Y+Dimension.Y) > location->Y)
		return 1;

    return 0;
}

//---------------------------------------------------------------------------
// Comparison Operator Overloads
//---------------------------------------------------------------------------
bool RectangleF::operator==(const RectangleF& r) const { return r.Location == Location && r.Dimension == Dimension; }
bool RectangleF::operator!=(const RectangleF& r) const { return !operator==(r); }
