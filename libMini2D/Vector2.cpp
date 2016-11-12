/*
 * Vector2.cpp
 *
 *  Created on: November 12th, 2016
 *  Author: Daniel Gerendasy
 */

#include <math.h>							// pow()

#include <Mini2D/Units.Hpp>


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
