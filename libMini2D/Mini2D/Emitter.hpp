/*
 * Emitter.hpp
 *
 *  Created on: November 12th, 2016
 *  Author: Daniel Gerendasy
 */

#ifndef EMITTER_HPP_
#define EMITTER_HPP_

#include <Mini2D/Mini2D.hpp>				// Mini2D class
#include <Mini2D/Image.hpp>					// Image class
#include <Mini2D/Units.hpp>					// RectangleF, Vector2

class Emitter {
public:

	Vector2 RangeVelocityX;					// Minimum and maximum X velocity
	Vector2 RangeVelocityY;					// Minimum and maximum Y velocity
	Vector2 RangeStartX;					// Minimum and maximum X offset from emission location (locX + (X*randomNumber))
	Vector2 RangeStartY;					// Minimum and maximum Y offset from emission location (locY + (Y*randomNumber))
	Vector2 RangeDimensionW;				// Minimum and maximum Width
	Vector2 RangeDimensionH;				// Minimum and maximum Height
	Vector2 RangeTTL;						// Minimum and maximum lifetime of particles
	Vector2 RangeRotation;					// Minimum and maximum rotation (degrees)
	Vector2 RangeColorRed;					// Minimum and maximum red
	Vector2 RangeColorGreen;				// Minimum and maximum green
	Vector2 RangeColorBlue;					// Minimum and maximum blue
	Vector2 RangeColorAlpha;				// Minimum and maximum alpha

	float ZIndex;							// Z coordinate
	unsigned int MinParticles;				// Minimum number of particles
	bool Revive;							// If particle dies before timeToLive hits 0, restart particle

	Image * ParticleImage;					// Optional image to draw particles with. Otherwise RGBA will be used



	Emitter(Mini2D * mini, unsigned int maxParticles);
	virtual ~Emitter();

	bool Start(Vector2 location, float timeToLive = 0);
	void Draw(float deltaTime);
private:
	typedef struct _emitterParticle_t {
		RectangleF rect;
		Vector2 velocity;
		float angleVelocity;

		float angle;
		float TTL;
		unsigned int RGBA;
		bool spawn;
	} EmitterParticle;

	Mini2D * _mini;

	int _maxP;								// Maximum number of particles
	int _activeP;							// Current number of particles

	float _timeLeft;						// Number of seconds before draw is finished

	EmitterParticle * _particles;			// Array of particles

	// Create particle
	void CreateParticle(Vector2 location, float timeToLive, int i);
};

#endif /* EMITTER_HPP_ */