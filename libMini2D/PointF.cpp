/*
 * PointF.cpp
 *
 *  Created on: November 10th, 2016
 *  Author: Daniel Gerendasy
 */

#include <Mini2D/Units.hpp>


//---------------------------------------------------------------------------
// Init Functions
//---------------------------------------------------------------------------
PointF::PointF() {
	X = 0;
	Y = 0;
}

PointF::PointF(float x, float y) {
	X = x;
	Y = y;
}

PointF::~PointF() {

}

//---------------------------------------------------------------------------
// Comparison Operator Overloads
//---------------------------------------------------------------------------
bool PointF::operator==(const PointF& p) const { return p.X == X && p.Y == Y; }
bool PointF::operator!=(const PointF& p) const { return !operator==(p); }

//---------------------------------------------------------------------------
// Compound Assignment Operator Overloads
//---------------------------------------------------------------------------
PointF& PointF::operator+=(const PointF& p) { X+=p.X;Y+=p.Y;return *this; }
PointF& PointF::operator-=(const PointF& p) { X-=p.X;Y-=p.Y;return *this; }
PointF& PointF::operator*=(const PointF& p) { X*=p.X;Y*=p.Y;return *this; }
PointF& PointF::operator/=(const PointF& p) { X/=p.X;Y/=p.Y;return *this; }
PointF& PointF::operator=(const PointF& p) { X=p.X;Y=p.Y;return *this; }

//---------------------------------------------------------------------------
// Arithmetic Operator Overloads
//---------------------------------------------------------------------------
const PointF PointF::operator+(const PointF& p) const { PointF p1 = *this;p1 += p;return p1; }
const PointF PointF::operator-(const PointF& p) const { PointF p1 = *this;p1 -= p;return p1; }
const PointF PointF::operator*(const PointF& p) const { PointF p1 = *this;p1 *= p;return p1; }
const PointF PointF::operator/(const PointF& p) const { PointF p1 = *this;p1 /= p;return p1; }
