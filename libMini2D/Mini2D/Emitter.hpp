/*
 * Emitter.hpp
 *
 *  Created on: November 12th, 2016
 *  Author: Daniel Gerendasy
 */

#ifndef EMITTER_HPP_
#define EMITTER_HPP_

#include <vector>                           // std::vector<>

#include <Mini2D/Mini.hpp>                  // Mini class
#include <Mini2D/Image.hpp>                 // Image class
#include <Mini2D/Units.hpp>                 // RectangleF, Vector2

namespace Mini2D {

	class Emitter {
	public:

		typedef enum _emitterStatus_t {
			EMITTER_DEAD = 0,
			EMITTER_ALIVE,
			EMITTER_PAUSED
		} EmitterStatus;

		Vector2 RangeVelocityTheta;               // Minimum and maximum angle of velocity
		Vector2 RangeVelocity;                    // Minimum and maximum speed
		Vector2 RangeStartX;                      // Minimum and maximum X offset from emission location (locX + (X*randomNumber))
		Vector2 RangeStartY;                      // Minimum and maximum Y offset from emission location (locY + (Y*randomNumber))
		Vector2 RangeDimensionW;                  // Minimum and maximum Width
		Vector2 RangeDimensionH;                  // Minimum and maximum Height
		Vector2 RangeTTL;                         // Minimum and maximum lifetime of particles
		Vector2 RangeRotation;                    // Minimum and maximum rotation (degrees)
		Vector2 RangeColorRed;				// Minimum and maximum red
		Vector2 RangeColorGreen;				// Minimum and maximum green
		Vector2 RangeColorBlue;				// Minimum and maximum blue
		Vector2 RangeColorAlpha;				// Minimum and maximum alpha
		RectangleF Clip;					// Particle draw region. Any particles outside this box will not be drawn

		float ZIndex;						// Z coordinate
		unsigned int MinParticles;				// Minimum number of particles
		bool Revive;						// If particle dies before timeToLive hits 0, restart particle
		bool SkipExplosion;					// Skip initial explosion of particles (pair with revive)

		Image * ParticleImage;				// Optional image to draw particles with. Otherwise RGBA will be used


		// Constructors
		Emitter(Mini * mini, unsigned int maxParticles);
		virtual ~Emitter();

		/*
		 * Start:
		 *		Setup the all particles for drawing
		 *
		 * location:
		 *		Location to start spawning particles
		 * timeToLive:
		 *		How long this emission lasts.
		 *		If 0, uses RangeTTL max (Y).
		 *		If < 0, lasts until Stop() is called.
		 *
		 * Return:
		 *		Returns 0 if invalid arguments/setup parameters
		 */
		bool Start(Vector2 location, float timeToLive = 0);

		/*
		 * Draw:
		 *		Draws particles
		 *
		 * deltaTime:
		 *		Time since last frame
		 */
		void Draw(float deltaTime);

		/*
		 * Status:
		 *		Gets the status of the emitter
		 */
		EmitterStatus Status();

		/*
		 * Pause the particles (but still draw them)
		 */
		void Pause();

		/*
		 * Resume particle movement
		 */
		void Resume();

		/*
		 * Stop emission
		 */
		void Stop();

	private:
		typedef struct _emitterParticle_t {
			RectangleF rect;
			Vector2 velocity;
			float angleVelocity;

			float TTL;
			unsigned int RGBA;
			bool spawn;
		} EmitterParticle;

		Mini * _mini;

		bool _pause;							// Whether or not to update the particle locations/angles
		bool _loop;								// Whether or not to continuously draw particles

		int _maxP;								// Maximum number of particles
		int _activeP;							// Current number of particles

		float _timeLeft;						// Number of seconds before draw is finished

		Vector2 _startLocation;					// Start location for the given emission session

		//EmitterParticle * _particles;			// Array of particles
		std::vector<EmitterParticle> _particles;

		// Create particle
		void createParticle(Vector2 location);
	};

}

#endif /* EMITTER_HPP_ */
