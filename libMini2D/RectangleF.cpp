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

RectangleF::RectangleF(PointF point1, PointF point2) {
	Init(point1.X<point2.X?point1.X:point2.X,
		point1.Y<point2.Y?point1.Y:point2.Y,
		point1.X<point2.X?point2.X-point1.X:point2.X-point1.X,
		point1.Y<point2.Y?point2.Y-point1.Y:point2.Y-point1.Y);
}

RectangleF::RectangleF(PointF point, SizeF size) {
	Init(point.X, point.Y, size.W, size.H);
}

RectangleF::RectangleF(PointF point, float w, float h) {
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
	Dimension.W = w;
	Dimension.H = h;
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
	return Dimension.W;
}

void RectangleF::W(float w) {
	Dimension.W = w;
}

float RectangleF::H() {
	return Dimension.H;
}

void RectangleF::H(float h) {
	Dimension.H = h;
}

//---------------------------------------------------------------------------
// Misc Functions
//---------------------------------------------------------------------------
bool RectangleF::Intersect(RectangleF rectangle) {
	if (Location.X < (rectangle.X() + rectangle.W()) &&
		(Location.X+Dimension.W) > rectangle.X() &&
		Location.Y < (rectangle.Y()+rectangle.H()) &&
		(Location.Y+Dimension.H) > rectangle.Y())
		return 1;

    return 0;
}

//---------------------------------------------------------------------------
// Comparison Operator Overloads
//---------------------------------------------------------------------------
bool RectangleF::operator==(const RectangleF& r) const { return r.Location == Location && r.Dimension == Dimension; }
bool RectangleF::operator!=(const RectangleF& r) const { return !operator==(r); }
