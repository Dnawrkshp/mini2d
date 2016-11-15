
#include <math.h>

#include "Ball.hpp"

Ball::Ball(Mini2D * mini, Image * image, Ball::BallCollision_f ballCollision) : _mini(mini), _image(image), _ballCollision(ballCollision) {
	if (!mini || !image || !ballCollision)
		return;

	DrawRegion.W(image->DrawRegion.W());
	DrawRegion.H(image->DrawRegion.H());
	Friction = 0.9f;
}

Ball::~Ball() {
	
}

bool Ball::Draw(float deltaTime) {
	float x,y;
	Vector2 normal;

	if (!_mini || !_image || !_ballCollision){
		printf("AAA\n");
		return 0;
	}

	x = DrawRegion.X();
	y = DrawRegion.Y();

	DrawRegion.Location += Velocity * deltaTime;
	if (isnan(DrawRegion.Location.X) || isnan(DrawRegion.Location.Y)) {
		printf("1 isnan from (%.2f, %.2f) += (%.2f, %.2f) * %f\n", x,y,Velocity.X,Velocity.Y,deltaTime);
		return 0;
	}

	// If there was a collision then we must go back to before the collision
	// Then we must calculate the new velocity using the normal
	if (_ballCollision(this, &normal)) {
		DrawRegion.Location.Set(x,y);
		if (normal.X || normal.Y) {			// If collided
			Vector2::Reflect(&Velocity, &Velocity, &normal);
			Velocity *= Friction;			// reduce the velocity after a collision
		}
		else {								// If out of bounds
			printf("out of bounds\n");
			//DrawRegion.Location.Set(_mini->MAXW*0.5,_mini->MAXH*0.5);
			Velocity *= -Friction;
		}

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
	_image->DrawRegion.W(DrawRegion.W());
	_image->DrawRegion.H(DrawRegion.H());
	_image->DrawRegion.UseAnchor = 0;
	_image->DrawRegion.RectangleAngle = DrawRegion.RectangleAngle;
	_image->Draw(RGBA);

	return 1;
}
