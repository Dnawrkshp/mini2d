/*
 * RectangleF.cpp
 *
 *  Created on: November 10th, 2016
 *  Author: Daniel Gerendasy
 */

#include <stdlib.h>							// abs()
#include <math.h>							// Trig functions
#include <stdio.h>							// printf()

#include <Mini2D/Units.hpp>					// Class definition

namespace Mini2D {

	//---------------------------------------------------------------------------
	// Init Functions
	//---------------------------------------------------------------------------
	RectangleF::RectangleF() {
		init(0,0,0,0);
	}

	RectangleF::RectangleF(float x, float y, float w, float h) {
		init(x,y,w,h);
	}

	RectangleF::RectangleF(Vector2 point, Vector2 dimension) {
		init(point.X, point.Y, dimension.X, dimension.Y);
	}

	RectangleF::RectangleF(Vector2 point, float w, float h) {
		init(point.X, point.Y, w, h);
	}

	RectangleF::RectangleF(RectangleF * rectangle) {
		init(rectangle->X(), rectangle->Y(), rectangle->W(), rectangle->H());
	}

	RectangleF::~RectangleF() {

	}

	void RectangleF::init(float x, float y, float w, float h, bool init) {
		Location.X = x;
		Location.Y = y;
		Dimension.X = w;
		Dimension.Y = h;

		if (init) {
			AnchorAngle = 0;
			RectangleAngle = 0;
			UseAnchor = 0;
			_lastUA = 1;
		}

		update();
	}

	void RectangleF::update() {
		float w2, h2;
		if (_lastX == Location.X && _lastY == Location.Y &&
			_lastAX == Anchor.X && _lastAY == Anchor.Y &&
			_lastA == AnchorAngle && _lastUA == UseAnchor && 
			_lastRA == RectangleAngle)
			return;

		_lastX = Location.X;
		_lastY = Location.Y;
		_lastA = AnchorAngle;
		_lastRA = RectangleAngle;
		_lastAX = Anchor.X;
		_lastAY = Anchor.Y;
		_lastUA = UseAnchor;

		_rCenter.Set(Location.X, Location.Y);

		if (AnchorAngle && UseAnchor) {
			_rCenter.RotateAroundPoint(&Anchor, AnchorAngle);
		}

		w2 = Dimension.X/2;
		h2 = Dimension.Y/2;

		TopLeft.Set(_rCenter.X-w2, _rCenter.Y-h2);
		TopRight.Set(_rCenter.X+w2, _rCenter.Y-h2);
		BottomLeft.Set(_rCenter.X-w2, _rCenter.Y+h2);
		BottomRight.Set(_rCenter.X+w2, _rCenter.Y+h2);

		if (RectangleAngle) {
			TopLeft.RotateAroundPoint(&_rCenter, RectangleAngle);
			TopRight.RotateAroundPoint(&_rCenter, RectangleAngle);
			BottomLeft.RotateAroundPoint(&_rCenter, RectangleAngle);
			BottomRight.RotateAroundPoint(&_rCenter, RectangleAngle);
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
		update();
	}

	float RectangleF::Y() {
		return Location.Y;
	}

	void RectangleF::Y(float y) {
		Location.Y = y;
		update();
	}

	float RectangleF::W() {
		return Dimension.X;
	}

	void RectangleF::W(float w) {
		Dimension.X = w;
		update();
	}

	float RectangleF::H() {
		return Dimension.Y;
	}

	void RectangleF::H(float h) {
		Dimension.Y = h;
		update();
	}

	void RectangleF::FromCorners(Vector2 point1, Vector2 point2) {
		init((point1.X+point2.X)/2,
			(point1.Y+point2.Y)/2,
			point1.X<point2.X?point2.X-point1.X:point2.X-point1.X,
			point1.Y<point2.Y?point2.Y-point1.Y:point2.Y-point1.Y,
			0);
	}

	Vector2 * RectangleF::GetRotatedCenter() {
		update();

		return &_rCenter;
	}

	//---------------------------------------------------------------------------
	// Misc Functions
	//---------------------------------------------------------------------------
	bool RectangleF::Contain(RectangleF * rectangle) {
		update();
		rectangle->GetRotatedCenter();
		
		if (TopLeft.X <= rectangle->TopLeft.X &&
			BottomRight.X >= rectangle->BottomRight.X &&
			TopLeft.Y <= rectangle->TopLeft.Y &&
			BottomRight.Y >= rectangle->BottomRight.Y)
			return 1;

		return 0;
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

}
