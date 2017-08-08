
#include <math.h>

#include "Ball.hpp"

Ball::Ball(Mini2D * mini, Image * image, Ball::BallCollision_f ballCollision) : _mini(mini), _image(image), _ballCollision(ballCollision) {
	if (!mini || !image || !ballCollision)
		return;

	DrawRegion.R(image->DrawRegion.W()/2);
	Friction = 0.9f;
}

Ball::~Ball() {
	
}

bool Ball::Draw(float deltaTime) {
	float x,y;
	Vector2 normal;

	if (!_mini || !_image || !_ballCollision)
		return 0;

	x = DrawRegion.X();
	y = DrawRegion.Y();

	// Move
	DrawRegion.Location += Velocity * deltaTime;

	// Rotate
	DrawRegion.CircleAngle += (Velocity.X<0?-1:1) * Velocity.Magnitude() * deltaTime;
	while (DrawRegion.CircleAngle > 180)
		DrawRegion.CircleAngle -= 360;
	while (DrawRegion.CircleAngle < -180)
		DrawRegion.CircleAngle += 360;

	// If there was a collision then we must go back to before the collision
	// Then we must calculate the new velocity using the normal
	if (_ballCollision(this, &normal)) {
		DrawRegion.Location.Set(x,y);
		if (normal.X || normal.Y) {			// If collided
			Vector2::Reflect(&Velocity, &Velocity, &normal);
			Velocity *= Friction;			// reduce the velocity after a collision
		}
		else								// If out of bounds
			return 0;

		_lastMove += deltaTime;
	}
	else {
		_lastMove = 0;
		// If the speed is less than the minimum
		if (Velocity.Magnitude() <= MinSpeed)
			return 0;
	}

	// If the ball hasn't moved in a second
	if (_lastMove > 1)
		return 0;

	_image->DrawRegion.X(DrawRegion.X());
	_image->DrawRegion.Y(DrawRegion.Y());
	_image->DrawRegion.W(DrawRegion.R()*2);
	_image->DrawRegion.H(DrawRegion.R()*2);
	_image->DrawRegion.UseAnchor = 0;
	_image->DrawRegion.RectangleAngle = DrawRegion.CircleAngle;
	_image->Draw(RGBA);

	return 1;
}

bool Ball::Intersect(CircleF * circle, Vector2 * normal) {
	Vector2 * c0 = DrawRegion.GetRotatedCenter();
	Vector2 * c1 = circle->GetRotatedCenter();
	float maxDistance = DrawRegion.Radius + circle->Radius;

	if (Vector2::DistanceFrom(c0, c1) < maxDistance) {
		if (normal) {
			normal->Set((*c1 - *c0));
			normal->Normalize();
		}

		return 1;
	}

	return 0;
}

int Ball::Intersect(RectangleF * rectangle, Vector2 * normal) {
	Vector2 * rect[4];

	// Check if the rectangle contains this ball
	if (!contains(rectangle))
		return -1;

	rect[0] = &rectangle->BottomRight;
	rect[1] = &rectangle->BottomLeft;
	rect[2] = &rectangle->TopLeft;
	rect[3] = &rectangle->TopRight;

	return Intersect(rect, 4, normal, NULL);
}

bool Ball::Intersect(Vector2 * polygon[], int polyCount, Vector2 * normal, int * points) {
	int i = 0, c = 0;
	Vector2 * temp;
	Vector2 dif;
	Vector2 * center = DrawRegion.GetRotatedCenter();
	float dist;

	if (!polygon || polyCount < 2)
		return 0;

	for (i = 0; i < polyCount; i++) {
		temp = polygon[((i==polyCount-1)?0:i+1)];
		dist = abs(Vector2::CrossProduct(polygon[i],temp,center))/Vector2::DistanceFrom(temp,polygon[i]);

		if (dist <= DrawRegion.Radius) {
			if (normal) {
				dif.Set(*temp - *polygon[i]);
				normal->X -= dif.Y;
				normal->Y += dif.X;
			}
			c++;
		}
	}

	// Normalize normal
	if (normal && c > 0) {
		// If the normal is invalid, return false
		if (!normal->X && !normal->Y)
			return 0;

		normal->Normalize();
	}

	// Number of points found in polygon
	if (points)
		*points = c;

	return c!=0;
}

bool Ball::contains(RectangleF * rectangle) {
	// Update coordinates
	rectangle->GetRotatedCenter();

	if ((DrawRegion.X()-DrawRegion.R()) <= rectangle->TopRight.X &&
		(DrawRegion.X()+DrawRegion.R()) >= rectangle->TopLeft.X &&
		(DrawRegion.Y()-DrawRegion.R()) <= rectangle->BottomLeft.Y &&
		(DrawRegion.Y()+DrawRegion.R()) >= rectangle->TopLeft.Y)
		return 1;

	if (rectangle->TopLeft.X <= (DrawRegion.X()+DrawRegion.R()) &&
		rectangle->TopRight.X >= (DrawRegion.X()-DrawRegion.R()) &&
		rectangle->TopLeft.Y <= (DrawRegion.Y()+DrawRegion.R()) &&
		rectangle->BottomLeft.Y >= (DrawRegion.Y()-DrawRegion.R()))
		return 1;

	return 0;
}