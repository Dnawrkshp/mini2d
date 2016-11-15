/*
 * RectangleF.cpp
 *
 *  Created on: November 10th, 2016
 *  Author: Daniel Gerendasy
 */

#include <stdlib.h>							// abs()
#include <math.h>							// Trig functions
#include <stdio.h>

#include <Mini2D/Units.hpp>

#define PI 3.14159265
#define DEG2RAD(x) ((x*PI)/180.0)
#define RAD2DEG(x) ((x*180.0)/PI)

// Determine if point exists within polygon
bool polyContainPoint(Vector2 * p, Vector2 * poly[], int polyCount);

//---------------------------------------------------------------------------
// Init Functions
//---------------------------------------------------------------------------
RectangleF::RectangleF() {
	_rCenter = new Vector2(0,0);
	Init(0,0,0,0);
}

RectangleF::RectangleF(float x, float y, float w, float h) {
	_rCenter = new Vector2(0,0);
	Init(x,y,w,h);
}

RectangleF::RectangleF(Vector2 point, Vector2 dimension) {
	_rCenter = new Vector2(0,0);
	Init(point.X, point.Y, dimension.X, dimension.Y);
}

RectangleF::RectangleF(Vector2 point, float w, float h) {
	_rCenter = new Vector2(0,0);
	Init(point.X, point.Y, w, h);
}

RectangleF::RectangleF(RectangleF * rectangle) {
	_rCenter = new Vector2(0,0);
	Init(rectangle->X(), rectangle->Y(), rectangle->W(), rectangle->H());
}

RectangleF::~RectangleF() {
	if (_rCenter)
		delete _rCenter;

	_rCenter = 0;
}

void RectangleF::Init(float x, float y, float w, float h) {
	Location.X = x;
	Location.Y = y;
	Dimension.X = w;
	Dimension.Y = h;

	Update();
}

void RectangleF::Update() {
	if (_lastX == Location.X && _lastY == Location.Y &&
		_lastAX == Anchor.X && _lastAY == Anchor.Y &&
		_lastA == AnchorAngle && _lastUA == UseAnchor)
		return;

	_lastX = Location.X;
	_lastY = Location.Y;
	_lastA = AnchorAngle;
	_lastAX = Anchor.X;
	_lastAY = Anchor.Y;
	_lastUA = UseAnchor;

	_rCenter->Set(Location.X, Location.Y);
	if (AnchorAngle && UseAnchor) {
		float c = cos(DEG2RAD(AnchorAngle));
		float s = sin(DEG2RAD(AnchorAngle));

		_rCenter->X-=Anchor.X;
		_rCenter->Y-=Anchor.Y;

		_rCenter->X = c*_rCenter->X + s*_rCenter->Y + Anchor.X;
		_rCenter->Y = c*_rCenter->Y - s*_rCenter->X + Anchor.Y;
	}
}

//---------------------------------------------------------------------------
// Get/Set Functions
//---------------------------------------------------------------------------
float RectangleF::X() {
	return Location.X;
}

void RectangleF::X(float x) {
	Location.X = x;
	Update();
}

float RectangleF::Y() {
	return Location.Y;
}

void RectangleF::Y(float y) {
	Location.Y = y;
	Update();
}

float RectangleF::W() {
	return Dimension.X;
}

void RectangleF::W(float w) {
	Dimension.X = w;
	Update();
}

float RectangleF::H() {
	return Dimension.Y;
}

void RectangleF::H(float h) {
	Dimension.Y = h;
	Update();
}

void RectangleF::FromCorners(Vector2 point1, Vector2 point2) {
	Init((point1.X+point2.X)/2,
		(point1.Y+point2.Y)/2,
		point1.X<point2.X?point2.X-point1.X:point2.X-point1.X,
		point1.Y<point2.Y?point2.Y-point1.Y:point2.Y-point1.Y);
}

Vector2 * RectangleF::GetRotatedCenter() {
	Update();

	return _rCenter;
}

//---------------------------------------------------------------------------
// Misc Functions
//---------------------------------------------------------------------------
bool RectangleF::Intersect(RectangleF * rectangle) {
	int x,i;
	float cA,sA,cB,sB;
	Vector2 *center;
	Vector2 ** rect1, ** rect2;

	Update();
	center = rectangle->GetRotatedCenter();

	cA = cos(DEG2RAD(RectangleAngle));
	sA = sin(DEG2RAD(RectangleAngle));
	cB = cos(DEG2RAD(rectangle->RectangleAngle));
	sB = sin(DEG2RAD(rectangle->RectangleAngle));

	rect1 = new Vector2*[4];
	rect2 = new Vector2*[4];

	rect1[0] = new Vector2(cA*Dimension.X* 0.5 + sA*Dimension.Y* 0.5 + _rCenter->X, cA*Dimension.Y* 0.5 + sA*Dimension.X* 0.5 + _rCenter->Y);
	rect1[1] = new Vector2(cA*Dimension.X*-0.5 + sA*Dimension.Y* 0.5 + _rCenter->X, cA*Dimension.Y* 0.5 + sA*Dimension.X*-0.5 + _rCenter->Y);
	rect1[2] = new Vector2(cA*Dimension.X*-0.5 + sA*Dimension.Y*-0.5 + _rCenter->X, cA*Dimension.Y*-0.5 + sA*Dimension.X*-0.5 + _rCenter->Y);
	rect1[3] = new Vector2(cA*Dimension.X* 0.5 + sA*Dimension.Y*-0.5 + _rCenter->X, cA*Dimension.Y*-0.5 + sA*Dimension.X* 0.5 + _rCenter->Y);

	rect2[0] = new Vector2(cB*rectangle->W()* 0.5 + sB*rectangle->H()* 0.5 + center->X, cB*rectangle->H()* 0.5 + sB*rectangle->W()* 0.5 + center->Y);
	rect2[1] = new Vector2(cB*rectangle->W()*-0.5 + sB*rectangle->H()* 0.5 + center->X, cB*rectangle->H()* 0.5 + sB*rectangle->W()*-0.5 + center->Y);
	rect2[2] = new Vector2(cB*rectangle->W()*-0.5 + sB*rectangle->H()*-0.5 + center->X, cB*rectangle->H()*-0.5 + sB*rectangle->W()*-0.5 + center->Y);
	rect2[3] = new Vector2(cB*rectangle->W()* 0.5 + sB*rectangle->H()*-0.5 + center->X, cB*rectangle->H()*-0.5 + sB*rectangle->W()* 0.5 + center->Y);

	for (x=0;x<4;x++) {
		if (polyContainPoint(rect2[x], rect1, 4))
			break;
	}

	for (i=0;i<4;i++) {
		delete rect1[i];
		delete rect2[i];
	}
	delete[] rect1;
	delete[] rect2;

	return x!=4;
}

bool RectangleF::Contain(RectangleF * rectangle) {
	int x,i;
	float cA,sA,cB,sB;
	Vector2 *center;
	Vector2 ** rect1, ** rect2;

	Update();
	center = rectangle->GetRotatedCenter();

	cA = cos(DEG2RAD(RectangleAngle));
	sA = sin(DEG2RAD(RectangleAngle));
	cB = cos(DEG2RAD(rectangle->RectangleAngle));
	sB = sin(DEG2RAD(rectangle->RectangleAngle));

	rect1 = new Vector2*[4];
	rect2 = new Vector2*[4];

	rect1[0] = new Vector2(cA*Dimension.X* 0.5 + sA*Dimension.Y* 0.5 + _rCenter->X, cA*Dimension.Y* 0.5 + sA*Dimension.X* 0.5 + _rCenter->Y);
	rect1[1] = new Vector2(cA*Dimension.X*-0.5 + sA*Dimension.Y* 0.5 + _rCenter->X, cA*Dimension.Y* 0.5 + sA*Dimension.X*-0.5 + _rCenter->Y);
	rect1[2] = new Vector2(cA*Dimension.X*-0.5 + sA*Dimension.Y*-0.5 + _rCenter->X, cA*Dimension.Y*-0.5 + sA*Dimension.X*-0.5 + _rCenter->Y);
	rect1[3] = new Vector2(cA*Dimension.X* 0.5 + sA*Dimension.Y*-0.5 + _rCenter->X, cA*Dimension.Y*-0.5 + sA*Dimension.X* 0.5 + _rCenter->Y);

	rect2[0] = new Vector2(cB*rectangle->W()* 0.5 + sB*rectangle->H()* 0.5 + center->X, cB*rectangle->H()* 0.5 + sB*rectangle->W()* 0.5 + center->Y);
	rect2[1] = new Vector2(cB*rectangle->W()*-0.5 + sB*rectangle->H()* 0.5 + center->X, cB*rectangle->H()* 0.5 + sB*rectangle->W()*-0.5 + center->Y);
	rect2[2] = new Vector2(cB*rectangle->W()*-0.5 + sB*rectangle->H()*-0.5 + center->X, cB*rectangle->H()*-0.5 + sB*rectangle->W()*-0.5 + center->Y);
	rect2[3] = new Vector2(cB*rectangle->W()* 0.5 + sB*rectangle->H()*-0.5 + center->X, cB*rectangle->H()*-0.5 + sB*rectangle->W()* 0.5 + center->Y);

	for (x=0;x<4;x++) {
		if (!polyContainPoint(rect2[x], rect1, 4))
			break;
	}

	for (i=0;i<4;i++) {
		delete rect1[i];
		delete rect2[i];
	}
	delete[] rect1;
	delete[] rect2;

	return x==4;
}

/*
 * From: http://blog.theliuy.com/determine-if-two-rotated-rectangles-overlap-each-other/
 */
int orientation(Vector2 *p, Vector2 *q, Vector2 *r)
{
    int val = (q->Y - p->Y) * (r->X - q->X) - 
              (q->X - p->X) * (r->Y - q->Y);
 
    if (0 == val)
        return 0;
    return val > 0 ? 1: 2;
}

bool polyContainPoint(Vector2 * p, Vector2 *poly[], int polyCount) {
	int x,o;
	Vector2 * rp;
	if (polyCount < 2)
		return 0;

	rp = poly[0];
    // orientation
    o = orientation(poly[0], poly[1], p);

    if (o == 0)
        return 1;

    for (x=1;x<polyCount;x++) {
    	if (o != orientation(poly[x], poly[((x==polyCount-1)?0:x+1)], p))
    		return 0;
    }
    return 1;
}

//---------------------------------------------------------------------------
// Compound Assignment Operator Overloads
//---------------------------------------------------------------------------
RectangleF& RectangleF::operator=(const RectangleF& r) { Location=r.Location;Dimension=r.Dimension;return *this; }

//---------------------------------------------------------------------------
// Comparison Operator Overloads
//---------------------------------------------------------------------------
bool RectangleF::operator==(const RectangleF& r) const { return r.Location == Location && r.Dimension == Dimension; }
bool RectangleF::operator!=(const RectangleF& r) const { return !operator==(r); }
