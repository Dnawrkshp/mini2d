

#ifndef BALL_HPP_
#define BALL_HPP_

#include <Mini2D/Mini2D.hpp>				// Mini2D class
#include <Mini2D/Image.hpp>					// Image class
#include <Mini2D/Units.hpp>					// RectangleF, Vector2

class Ball
{
public:
	/*
	 * BallCollision_f: 
	 * 		Check if this ball is colliding with any other object in scene.
	 * 
	 * Ball*:
	 * 		This
	 * Vector2*:
	 *		The collision normal if there was a collision
	 *
	 * Return:
	 *		1 if collided
	 */
	typedef bool (*BallCollision_f) (Ball *, Vector2 *);

	Vector2 Velocity;						// Current velocity
	CircleF DrawRegion;						// Current location
	float MinSpeed;							// Minimum speed before ball is considered dead
	float Friction;							// Multiplies with velocity after every collision
	unsigned int RGBA;						// Color of ball

	Ball(Mini2D * mini, Image * image, BallCollision_f ballCollision);
	~Ball();

	// Returns 0 if the ball is dead
	bool Draw(float deltaTime);

private:
	Mini2D * _mini;
	Image * _image;

	float _lastMove;

	BallCollision_f _ballCollision;
};

#endif /* BALL_HPP_ */