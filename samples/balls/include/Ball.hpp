

#ifndef BALL_HPP_
#define BALL_HPP_

#include <Mini2D/Mini.hpp>					// Mini class
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
	 * Mini2D::Vector2*:
	 *		The collision normal if there was a collision
	 *
	 * Return:
	 *		1 if collided
	 */
	typedef bool (*BallCollision_f) (Ball *, Mini2D::Vector2 *);

	Mini2D::Vector2 Velocity;				// Current velocity
	Mini2D::CircleF DrawRegion;				// Current location
	float MinSpeed;							// Minimum speed before ball is considered dead
	float Friction;							// Multiplies with velocity after every collision
	unsigned int RGBA;						// Color of ball

	Ball(Mini2D::Mini * mini, Mini2D::Image * image, BallCollision_f ballCollision);
	~Ball();

	// Returns 0 if the ball is dead
	bool Draw(float deltaTime);

	/*
	 * Intersect:
	 *		Determines if the passed circle intersects with this ball.
	 * 
	 * circle:
	 *		CircleF to compare with
	 * normal [OUT]:
	 *		The resulting normal of the intersected segment (can be null)
	 * 
	 * Return:
	 *		1 if they intersect. 0 if not.
	 */
	bool Intersect(Mini2D::CircleF * circle, Mini2D::Vector2 * normal);

	/*
	 * Intersect:
	 *		Determines if the passed rectangle intersects with this ball.
	 * 
	 * rectangle:
	 *		RectangleF to compare with
	 * normal [OUT]:
	 *		The resulting normal of the intersected segment (can be null)
	 * 
	 * Return:
	 *		1 if they intersect. 0 if not.
	 */
	int Intersect(Mini2D::RectangleF * rectangle, Mini2D::Vector2 * normal);

	/*
	 * Intersect:
	 *		Determines if the passed polygon intersects with this ball.
	 * 
	 * polygon:
	 *		Array of points
	 * polyCount:
	 *		Number of points in polygon
	 * normal [OUT]:
	 *		The resulting normal of the intersected segment (can be null)
	 * points [OUT]:
	 *		The number of points of polygon contained within this circle (can be null)
	 * 
	 * Return:
	 *		1 if they intersect. 0 if not.
	 */
	bool Intersect(Mini2D::Vector2 * polygon[], int polyCount, Mini2D::Vector2 * normal, int * points);

private:
	Mini2D::Mini * _mini;
	Mini2D::Image * _image;

	float _lastMove;

	BallCollision_f _ballCollision;

	// Checks if ball or rectangle is contained within each other
	bool contains(Mini2D::RectangleF * rectangle);
};

#endif /* BALL_HPP_ */