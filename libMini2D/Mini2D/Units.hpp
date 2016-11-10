/*
 * Units.hpp
 *
 *  Created on: November 10th, 2016
 *  Author: Daniel Gerendasy
 */

#ifndef UNITS_HPP_
#define UNITS_HPP_

class SizeF {
public:
	// Variables
	float W;
	float H;

	// Constructors
	SizeF();
	SizeF(float w, float h);
	virtual ~SizeF();

	// Comparison Operators
	bool operator==(const SizeF& s) const;
	bool operator!=(const SizeF& s) const;
	bool operator<(const SizeF& s) const;
	bool operator>(const SizeF& s) const;
	bool operator<=(const SizeF& s) const;
	bool operator>=(const SizeF& s) const;
	
	// Compound Operators
	SizeF& operator+=(const SizeF& s);
	SizeF& operator-=(const SizeF& s);
	SizeF& operator*=(const SizeF& s);
	SizeF& operator/=(const SizeF& s);
	SizeF& operator=(const SizeF& s);

	// Arithmetic Operators
	const SizeF operator+(const SizeF& s) const;
	const SizeF operator-(const SizeF& s) const;
	const SizeF operator*(const SizeF& s) const;
	const SizeF operator/(const SizeF& s) const;
};

class PointF {
public:
	// Variables
	float X;
	float Y;

	// Constructors
	PointF();
	PointF(float x, float y);
	virtual ~PointF();

	// Comparison Operators
	bool operator==(const PointF& p) const;
	bool operator!=(const PointF& p) const;
	
	// Compound Operators
	PointF& operator+=(const PointF& p);
	PointF& operator-=(const PointF& p);
	PointF& operator*=(const PointF& p);
	PointF& operator/=(const PointF& p);
	PointF& operator=(const PointF& p);

	// Arithmetic Operators
	const PointF operator+(const PointF& p) const;
	const PointF operator-(const PointF& p) const;
	const PointF operator*(const PointF& p) const;
	const PointF operator/(const PointF& p) const;
};

class RectangleF {
public:
	// Variables
	PointF Location;
	SizeF Dimension;

	// Constructors
	RectangleF();
	RectangleF(float x, float y, float w, float h);
	RectangleF(PointF point1, PointF point2);
	RectangleF(PointF point, SizeF size);
	RectangleF(PointF point, float w, float h);
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

	// Determines if the rectangles intersects
	bool Intersect(RectangleF rectangle);

	// Comparison Operators
	bool operator==(const RectangleF& r) const;
	bool operator!=(const RectangleF& r) const;
private:
	void Init(float x, float y, float w, float h);
};

#endif /* UNITS_HPP_ */