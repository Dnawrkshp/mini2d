/*
 * SizeF.cpp
 *
 *  Created on: November 10th, 2016
 *  Author: Daniel Gerendasy
 */

#include <Mini2D/Units.hpp>


//---------------------------------------------------------------------------
// Init Functions
//---------------------------------------------------------------------------
SizeF::SizeF() {
	W = 0;
	H = 0;
}

SizeF::SizeF(float w, float h) {
	W = w;
	H = h;
}

SizeF::~SizeF() {

}

//---------------------------------------------------------------------------
// Comparison Operator Overloads
//---------------------------------------------------------------------------
bool SizeF::operator==(const SizeF& s) const { return s.W == W && s.H == H; }
bool SizeF::operator!=(const SizeF& s) const { return !operator==(s); }

//---------------------------------------------------------------------------
// Compound Assignment Operator Overloads
//---------------------------------------------------------------------------
SizeF& SizeF::operator+=(const SizeF& s) { W+=s.W;H+=s.H;return *this; }
SizeF& SizeF::operator-=(const SizeF& s) { W-=s.W;H-=s.H;return *this; }
SizeF& SizeF::operator*=(const SizeF& s) { W*=s.W;H*=s.H;return *this; }
SizeF& SizeF::operator/=(const SizeF& s) { W/=s.W;H/=s.H;return *this; }
SizeF& SizeF::operator=(const SizeF& s) { W=s.W;H=s.H;return *this; }

//---------------------------------------------------------------------------
// Arithmetic Operator Overloads
//---------------------------------------------------------------------------
const SizeF SizeF::operator+(const SizeF& s) const { SizeF s1 = *this;s1 += s;return s1; }
const SizeF SizeF::operator-(const SizeF& s) const { SizeF s1 = *this;s1 -= s;return s1; }
const SizeF SizeF::operator*(const SizeF& s) const { SizeF s1 = *this;s1 *= s;return s1; }
const SizeF SizeF::operator/(const SizeF& s) const { SizeF s1 = *this;s1 /= s;return s1; }
