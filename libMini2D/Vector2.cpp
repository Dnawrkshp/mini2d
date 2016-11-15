/*
 * Vector2.cpp
 *
 *  Created on: November 12th, 2016
 *  Author: Daniel Gerendasy
 */

#include <math.h>							// pow()

#include <Mini2D/Units.hpp>					// class definition


//---------------------------------------------------------------------------
// Init Functions
//---------------------------------------------------------------------------
Vector2::Vector2() {
	X = 0;
	Y = 0;
}

Vector2::Vector2(float x, float y) {
	X = x;
	Y = y;
}

Vector2::~Vector2() {
}

//---------------------------------------------------------------------------
// Misc Functions
//---------------------------------------------------------------------------
float Vector2::Magnitude() {
	return pow(pow(X,2) + pow(Y,2), 0.5);
}

void Vector2::Normalize() {
	float m = Magnitude();
	X/=m;
	Y/=m;
}

//---------------------------------------------------------------------------
// Set Functions
//---------------------------------------------------------------------------
void Vector2::Set(float x, float y) {
	X = x;
	Y = y;
}

void Vector2::Set(const Vector2& v) {
	X = v.X;
	Y = v.Y;
}

void Vector2::SetX(float x) {
	Set(x,Y);
}

void Vector2::SetY(float y) {
	Set(X,y);
}

//---------------------------------------------------------------------------
// Comparison Operator Overloads
//---------------------------------------------------------------------------
bool Vector2::operator==(const Vector2& s) const { return s.X == X && s.Y == Y; }
bool Vector2::operator!=(const Vector2& s) const { return !operator==(s); }

//---------------------------------------------------------------------------
// Compound Assignment Operator Overloads
//---------------------------------------------------------------------------
Vector2& Vector2::operator+=(const Vector2& s) { X+=s.X;Y+=s.Y;return *this; }
Vector2& Vector2::operator-=(const Vector2& s) { X-=s.X;Y-=s.Y;return *this; }
Vector2& Vector2::operator*=(const Vector2& s) { X*=s.X;Y*=s.Y;return *this; }
Vector2& Vector2::operator/=(const Vector2& s) { X/=s.X;Y/=s.Y;return *this; }
Vector2& Vector2::operator+=(const float& s) { X+=s;Y+=s;return *this; }
Vector2& Vector2::operator-=(const float& s) { X-=s;Y-=s;return *this; }
Vector2& Vector2::operator*=(const float& s) { X*=s;Y*=s;return *this; }
Vector2& Vector2::operator/=(const float& s) { X/=s;Y/=s;return *this; }
Vector2& Vector2::operator=(const Vector2& s) { X=s.X;Y=s.Y;return *this; }

//---------------------------------------------------------------------------
// Arithmetic Operator Overloads
//---------------------------------------------------------------------------
const Vector2 Vector2::operator+(const Vector2& s) const { Vector2 s1 = *this;s1 += s;return s1; }
const Vector2 Vector2::operator-(const Vector2& s) const { Vector2 s1 = *this;s1 -= s;return s1; }
const Vector2 Vector2::operator*(const Vector2& s) const { Vector2 s1 = *this;s1 *= s;return s1; }
const Vector2 Vector2::operator/(const Vector2& s) const { Vector2 s1 = *this;s1 /= s;return s1; }
const Vector2 Vector2::operator+(const float& s) const { Vector2 s1 = *this;s1 += s;return s1; }
const Vector2 Vector2::operator-(const float& s) const { Vector2 s1 = *this;s1 -= s;return s1; }
const Vector2 Vector2::operator*(const float& s) const { Vector2 s1 = *this;s1 *= s;return s1; }
const Vector2 Vector2::operator/(const float& s) const { Vector2 s1 = *this;s1 /= s;return s1; }

//---------------------------------------------------------------------------
// Static Functions
//-------------------------------------------------------------------------
float Vector2::Determinant(Vector2 * u, Vector2 * v) {
	return u->X*v->Y - u->Y*v->X;
}

float Vector2::CrossProduct(Vector2 * p, Vector2 * q, Vector2 * r) {
	return (q->Y - p->Y) * (r->X - q->X) - 
		   (q->X - p->X) * (r->Y - q->Y);
}

float Vector2::DotProduct(Vector2 * a, Vector2 * b) {
	return a->X*b->X + a->Y*b->Y;
}

float Vector2::DistanceFrom(Vector2 * a, Vector2 * b) {
	return pow(pow(a->X-b->X,2) + pow(a->Y-b->Y,2), 0.5);
}

float Vector2::Magnitude(Vector2 * a) {
	return pow(pow(a->X,2) + pow(a->Y,2), 0.5);
}

void Vector2::Reflect(Vector2 * out, Vector2 * direction, Vector2 * normal) {
	if (!out || !direction || !normal)
		return;

	Vector2 n = (*normal);
	n.Normalize();
	out->Set(*direction - (n * 2.0f * DotProduct(direction,&n)));
}
