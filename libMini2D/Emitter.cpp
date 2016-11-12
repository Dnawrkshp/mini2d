/*
 * Emitter.cpp
 *
 *  Created on: November 12th, 2016
 *  Author: Daniel Gerendasy
 */

#include <time.h>							// time()

#include <Mini2D/Emitter.hpp>				// Class definition
#include <Mini2D/Units.hpp>					// vector2

float RandomRange(float min, float max);

//---------------------------------------------------------------------------
// Init Functions
//---------------------------------------------------------------------------
Emitter::Emitter(Mini2D * mini, unsigned int maxParticles) : _mini(mini) {
	_maxP = maxParticles;

	RangeVelocityX.X = -10;
	RangeVelocityX.Y = 10;
	RangeVelocityY.X = -10;
	RangeVelocityY.Y = 10;

	RangeDimensionW.X = 10;
	RangeDimensionW.Y = 50;
	RangeDimensionH.X = 10;
	RangeDimensionH.Y = 50;

	RangeTTL.X = 5;
	RangeTTL.Y = 20;

	RangeColorRed.X = 0;
	RangeColorRed.Y = 255;
	RangeColorGreen.X = 0;
	RangeColorGreen.Y = 255;
	RangeColorBlue.X = 0;
	RangeColorBlue.Y = 255;
	RangeColorAlpha.X = 255;
	RangeColorAlpha.Y = 255;

	ParticleImage = NULL;

	_particles = new EmitterParticle[maxParticles];
}
Emitter::~Emitter() {
	_timeLeft = 0;
	_activeP = 0;
	ParticleImage = NULL;

	if (_particles)
		delete[] _particles;
	_particles = NULL;


}

bool Emitter::Start(Vector2 location, float timeToLive) {
	int i;

	if (!_maxP || !MinParticles ||
		RangeTTL.Y <= 0 || RangeDimensionW.Y <= 0 || RangeDimensionH.Y <= 0 ||
		RangeColorRed.Y <= 0 || RangeColorGreen.Y <= 0 || RangeColorBlue.Y <= 0 || RangeColorAlpha.Y <= 0)
		return 0;

	_activeP = 0;
	while (!_activeP)
		_activeP = (int)RandomRange(MinParticles, _maxP);

	_timeLeft = timeToLive;
	if (_timeLeft <= 0)
		_timeLeft = RangeTTL.Y;

	for (i = 0; i < _activeP; i++) {
		CreateParticle(location, _timeLeft, i);
	}

	return 1;
}

void Emitter::Draw(float deltaTime) {
	int i;
	float z = ParticleImage ? ParticleImage->ZIndex : 0; // Save ZIndex of ParticleImage

	if (_timeLeft <= 0) {
		// clean up last emission
		if (_activeP) {
			for (i=0;i<_maxP;i++) {
				_particles[i].TTL = 0;
				_particles[i].spawn = 0;
			}
			_activeP = 0;
		}
		return;
	}

	for (i=0;i<_activeP;i++) {
		if (_particles[i].TTL > 0) {
			_particles[i].rect.Location += (_particles[i].velocity * deltaTime);
			_particles[i].angle += _particles[i].angleVelocity * deltaTime;

			if (ParticleImage) {
				ParticleImage->ZIndex = ZIndex;
				ParticleImage->DrawRegion.X(_particles[i].rect.X());
				ParticleImage->DrawRegion.Y(_particles[i].rect.Y());
				ParticleImage->DrawRegion.W(_particles[i].rect.W());
				ParticleImage->DrawRegion.H(_particles[i].rect.H());
				ParticleImage->Draw(_particles[i].RGBA, Image::DRAW_CENTER, _particles[i].angle);
			}
			else {
				_mini->DrawRectangle(_particles[i].rect.X(), _particles[i].rect.Y(),
									_particles[i].rect.X(), _particles[i].rect.Y(), ZIndex,
									_particles[i].rect.W(), _particles[i].rect.H(),
									_particles[i].RGBA, _particles[i].angle);
			}


			_particles[i].TTL -= deltaTime;
		}
	}


	_timeLeft -= deltaTime;

	if (ParticleImage)
		ParticleImage->ZIndex = z;
}


void Emitter::CreateParticle(Vector2 location, float timeToLive, int i) {
	char r,g,b,a;

	// Knows to draw and to revive when dead
	_particles[i].spawn = 1;

	// Generate random attributes
	_particles[i].TTL = RandomRange(RangeTTL.X, timeToLive);

	r = (char)RandomRange(RangeColorRed.X,RangeColorRed.Y); if (r<0) {r=0;} else if (r>255) {r=255;}
	g = (char)RandomRange(RangeColorRed.X,RangeColorRed.Y); if (g<0) {g=0;} else if (g>255) {g=255;}
	b = (char)RandomRange(RangeColorRed.X,RangeColorRed.Y); if (b<0) {b=0;} else if (b>255) {b=255;}
	a = (char)RandomRange(RangeColorRed.X,RangeColorRed.Y); if (a<0) {a=0;} else if (a>255) {a=255;}
	_particles[i].RGBA = (r<<24) | (g<<16) | (b<<8) | a;

	_particles[i].rect.X(RandomRange(RangeStartX.X + location.X, RangeStartX.Y + location.X));
	_particles[i].rect.Y(RandomRange(RangeStartY.X + location.Y, RangeStartX.Y + location.Y));
	_particles[i].rect.W(RandomRange(RangeDimensionW.X, RangeDimensionW.Y));
	_particles[i].rect.H(RandomRange(RangeDimensionH.X, RangeDimensionH.Y));

	_particles[i].velocity.X = RandomRange(RangeVelocityX.X, RangeVelocityX.Y);
	_particles[i].velocity.Y = RandomRange(RangeVelocityY.X, RangeVelocityY.Y);

	_particles[i].angleVelocity = RandomRange(RangeRotation.X, RangeRotation.Y);
}

float RandomRange(float min, float max) {
	srand(time(NULL));

	return min + static_cast<float>(rand())/(static_cast<float>(RAND_MAX/(max-min)));
}
