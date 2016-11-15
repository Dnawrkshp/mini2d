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

	// Set
	void SetX(float x);
	void SetY(float y);
	void Set(float x, float y);

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
	Vector2 Location;						// Center of rectangle
	Vector2 Dimension;						// Size of rectangle
	Vector2 Anchor;							// Point to rotate around
	float Angle;							// Angle of rotation (degrees)
	bool UseAnchor;							// Whether or not to use anchor point in rotation calculation

	// Constructors
	RectangleF();
	RectangleF(float x, float y, float w, float h);
	RectangleF(Vector2 center, Vector2 dimension);
	RectangleF(Vector2 center, float w, float h);
	RectangleF(RectangleF * rectangle);
	virtual ~RectangleF();

	// Get X,Y,W,H
	float X();
	float Y();
	float W();
	float H();

	Vector2 *GetRotatedCenter();

	// Set X,Y,W,H
	void X(float x);
	void Y(float y);
	void W(float w);
	void H(float h);
	void FromCorners(Vector2 point1, Vector2 point2);

	// Determines if the rectangles intersect
	bool Intersect(RectangleF * rectangle);
	// Determines if this rectangle contains the passed rectangle
	bool Contain(RectangleF * rectangle);

	// Compound Operators
	RectangleF& operator=(const RectangleF& r);

	// Comparison Operators
	bool operator==(const RectangleF& r) const;
	bool operator!=(const RectangleF& r) const;
private:
	Vector2 * _rCenter;						// Center of rotated rectangle

	float _lastX;							// X value from last Update()
	float _lastY;							// Y value from last Update()
	float _lastW;							// Width from last Update()
	float _lastH;							// Height from last Update()
	float _lastA;							// Angle from last Update()
	float _lastAX;							// Anchor X value from last Update()
	float _lastAY;							// Anchor Y value from last Update()

	// Load
	void Init(float x, float y, float w, float h);

	// Calculate Center
	void Update();
};

#endif /* UNITS_HPP_ */