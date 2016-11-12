/*
 * Units.hpp
 *
 *  Created on: November 10th, 2016
 *  Author: Daniel Gerendasy
 */

#ifndef UNITS_HPP_
#define UNITS_HPP_

class Vector2 {
public:
	// Variables
	float X;
	float Y;

	// Constructors
	Vector2();
	Vector2(float x, float y);
	virtual ~Vector2();

	// Operations
	float Magnitude();
	void Normalize();

	// Comparison Operators
	bool operator==(const Vector2& s) const;
	bool operator!=(const Vector2& s) const;
	
	// Compound Operators
	Vector2& operator+=(const Vector2& s);
	Vector2& operator-=(const Vector2& s);
	Vector2& operator*=(const Vector2& s);
	Vector2& operator/=(const Vector2& s);
	Vector2& operator+=(const float& s);
	Vector2& operator-=(const float& s);
	Vector2& operator*=(const float& s);
	Vector2& operator/=(const float& s);
	Vector2& operator=(const Vector2& s);

	// Arithmetic Operators
	const Vector2 operator+(const Vector2& s) const;
	const Vector2 operator-(const Vector2& s) const;
	const Vector2 operator*(const Vector2& s) const;
	const Vector2 operator/(const Vector2& s) const;
	const Vector2 operator+(const float& s) const;
	const Vector2 operator-(const float& s) const;
	const Vector2 operator*(const float& s) const;
	const Vector2 operator/(const float& s) const;
};

class RectangleF {
public:
	// Variables
	Vector2 Location;
	Vector2 Dimension;

	// Constructors
	RectangleF();
	RectangleF(float x, float y, float w, float h);
	RectangleF(Vector2 point, Vector2 dimension);
	RectangleF(Vector2 point, float w, float h);
	RectangleF(RectangleF * rectangle);
	virtual ~RectangleF();

	// Get X,Y,W,H
	float X();
	float Y();
	float W();
	float H();

	// Set X,Y,W,H
	void X(float x);
	void Y(float y);
	void W(float w);
	void H(float h);
	void FromCorners(Vector2 point1, Vector2 point2);

	// Determines if the rectangles intersects
	bool Intersect(RectangleF * rectangle);
	bool Intersect(Vector2 * location, Vector2 * dimension);

	// Comparison Operators
	bool operator==(const RectangleF& r) const;
	bool operator!=(const RectangleF& r) const;
private:
	void Init(float x, float y, float w, float h);
};

#endif /* UNITS_HPP_ */