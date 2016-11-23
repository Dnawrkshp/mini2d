/*
 * Units.hpp
 *
 *  Created on: November 10th, 2016
 *  Author: Daniel Gerendasy
 */

#ifndef UNITS_HPP_
#define UNITS_HPP_

#include <stdlib.h>							// NULL

class Vector2 {
public:
	// Variables
	float X;
	float Y;

	// Constructors
	Vector2();
	Vector2(float x, float y);
	virtual ~Vector2();

	/*
	 * Magnitude:
	 *		Returns the magnitude of the vector
	 */
	float Magnitude();

	/*
	 * Normalize:
	 *		Normalizes the vector by dividing itself by its magnitude
	 */
	void Normalize();

	/*
	 * RotateAroundPoint:
	 *		Rotates the current vector around a point
	 *
	 * point:
	 *		The X,Y position of which to rotate this X,Y position around
	 * angle:
	 *		The number of degrees to rotate
	 */
	void RotateAroundPoint(Vector2 * point, float angle);

	// Set
	void SetX(float x);
	void SetY(float y);
	void Set(float x, float y);
	void Set(const Vector2& v);

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

	static float Determinant(Vector2 * u, Vector2 * v);
	static float CrossProduct(Vector2 * p, Vector2 * q, Vector2 * r);
	static float DotProduct(Vector2 * a, Vector2 * b);
	static float DistanceFrom(Vector2 * a, Vector2 * b);
	static float Magnitude(Vector2 * a);
	static void Reflect(Vector2 * out, Vector2 * direction, Vector2 * normal);
};

class RectangleF {
public:
	// Variables
	Vector2 Location;						// Center of rectangle
	Vector2 Dimension;						// Size of rectangle
	Vector2 Anchor;							// Point to rotate around

	// Points
	Vector2 TopLeft;						// Top Left corner of rectangle (for reading)
	Vector2 TopRight;						// Top Right corner of rectangle (for reading)
	Vector2 BottomLeft;						// Bottom Left corner of rectangle (for reading)
	Vector2 BottomRight;					// Bottom Right corner of rectangle (for reading)

	float AnchorAngle;						// Angle of rotation around anchor point (degrees)
	float RectangleAngle;					// Angle of rotation around rectangle center (degrees)

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

	/*
	 * GetRotatedCenter:
	 *		Returns a pointer to the position of the rectangle after rotation has been calculated.
	 *		If UseAnchor is false, this will be the same value as Location.
	 */
	Vector2 *GetRotatedCenter();

	// Set X,Y,W,H
	void X(float x);
	void Y(float y);
	void W(float w);
	void H(float h);

	/*
	 * FromCorners:
	 *		Set the X, Y, Width, and Height of the rectangle from 2 corners
	 * 
	 * point1:
	 *		First corner
	 * point2:
	 *		Second corner
	 */
	void FromCorners(Vector2 point1, Vector2 point2);

	/*
	 * Intersect:
	 *		Determines if the passed rectangle intersects with this rectangle.
	 *		If one is contained within the other, this returns 0
	 * 
	 * rectangle:
	 *		Rectangle to compare with
	 * normal:
	 *		The resulting normal of the intersected segment (can be null)
	 * points:
	 *		The number of points contained within this rectangle
	 * 
	 * Return:
	 *		1 if they intersect. 0 if not.
	 */
	bool Intersect(RectangleF * rectangle, Vector2 * normal = NULL, int * points = NULL);

	/*
	 * IntersectFast:
	 *		Determines if the passed rectangle intersects with this rectangle.
	 * 
	 * rectangle:
	 *		Rectangle to compare with
	 * 
	 * Return:
	 *		1 if they intersect. 0 if not.
	 */
	bool IntersectFast(RectangleF * rectangle);
	
	/*
	 * Contain:
	 *		Determines if the passed rectangle is contained within this rectangle
	 * 
	 * rectangle:
	 *		Rectangle to compare with
	 * 
	 * Return:
	 *		1 if rectangle is within container. 0 if not.
	 */
	bool Contain(RectangleF * rectangle);

	// Compound Operators
	RectangleF& operator=(const RectangleF& r);

	// Comparison Operators
	bool operator==(const RectangleF& r) const;
	bool operator!=(const RectangleF& r) const;
private:
	Vector2 _rCenter;						// Center of rotated rectangle

	float _lastX;							// X value from last Update()
	float _lastY;							// Y value from last Update()
	float _lastA;							// AnchorAngle from last Update()
	float _lastAX;							// Anchor X value from last Update()
	float _lastAY;							// Anchor Y value from last Update()
	float _lastUA;							// UseAnchor value from last Update()

	// Load
	void Init(float x, float y, float w, float h, bool init = 1);

	// Calculate Center
	void Update();
	// Determine how many points from rectangle are within this rectangle
	int CheckCollision(RectangleF * rectangle, Vector2 * slopes[]);
};

#endif /* UNITS_HPP_ */