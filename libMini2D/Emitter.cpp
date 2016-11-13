/*
 * Emitter.cpp
 *
 *  Created on: November 12th, 2016
 *  Author: Daniel Gerendasy
 */

#include <time.h>							// time()
#include <math.h>							// Trig

#include <Mini2D/Emitter.hpp>				// Class definition
#include <Mini2D/Units.hpp>					// vector2

float randomRange(float min, float max);
float degToRad(float d);

//---------------------------------------------------------------------------
// Init Functions
//---------------------------------------------------------------------------
Emitter::Emitter(Mini2D * mini, unsigned int maxParticles) : _mini(mini) {
	_maxP = maxParticles;

	RangeVelocityTheta.X = 0;
	RangeVelocityTheta.Y = 360;

	RangeVelocity.X = -10;
	RangeVelocity.Y = 10;

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

	Clip.X(0);
	Clip.Y(0);
	Clip.W(mini->MAXW);
	Clip.H(mini->MAXH);

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

//---------------------------------------------------------------------------
// Draw/Particle Setup Functions
//---------------------------------------------------------------------------
bool Emitter::Start(Vector2 location, float timeToLive) {
	int i;
	float dt = 1/10.f;

	if (!_maxP || RangeTTL.Y <= 0 || RangeDimensionW.Y <= 0 || RangeDimensionH.Y <= 0 || RangeColorAlpha.Y <= 0)
		return 0;

	_startLocation.X = location.X;
	_startLocation.Y = location.Y;

	_pause = 0;
	_loop = 0;

	_activeP = 0;
	while (!_activeP)
		_activeP = (int)randomRange(MinParticles, _maxP);

	_timeLeft = timeToLive;
	if (_timeLeft <= 0) {
		if (_timeLeft < 0)
			_loop = 1;
		_timeLeft = RangeTTL.Y;
	}

	srand(time(NULL));
	for (i = 0; i < _activeP; i++) {
		createParticle(location, i);

		// Here we process the next loop of it's life in order to skip the initial explosion
		if (SkipExplosion) {
			for (float t=0; t <= RangeTTL.Y*2; t+=dt) {
				if (_particles[i].TTL > 0) {
					_particles[i].rect.Location += (_particles[i].velocity * dt);
					_particles[i].angle += _particles[i].angleVelocity * dt;
					_particles[i].TTL-=dt;
				}
				else {
					createParticle(location, i);
				}
			}
		}
	}

	return 1;
}

void Emitter::Draw(float deltaTime) {
	int i,alive;

	// If we're using an image, save all the data values we are about to change
	float z=0,ix=0,iy=0,iw=0,ih=0;
	if (ParticleImage) {
		z = ParticleImage->ZIndex;
		ix = ParticleImage->DrawRegion.X();
		iy = ParticleImage->DrawRegion.Y();
		iw = ParticleImage->DrawRegion.W();
		ih = ParticleImage->DrawRegion.H();
	}

	alive = 0;
	for (i=0;i<_activeP;i++) {
		if (_particles[i].TTL > 0 && _particles[i].spawn) {
			if (!_pause) {
				_particles[i].rect.Location += (_particles[i].velocity * deltaTime);
				_particles[i].angle += _particles[i].angleVelocity * deltaTime;
				_particles[i].TTL -= deltaTime;
			}

			// Ensure we draw within bounds
			Vector2 topLeft = _particles[i].rect.Location - (_particles[i].rect.Dimension/2);
			if (Clip.Contain(&topLeft, &_particles[i].rect.Dimension)) {
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
			}

			alive++;
		}
		else if (Revive && _particles[i].spawn && _timeLeft > 0) {
			createParticle(_startLocation, i);
			alive++;
		}
	}


	if (!_pause && !_loop)
		_timeLeft -= deltaTime;

	// If all the particles have died, end
	if (!alive)
		Stop();

	if (ParticleImage) {
		ParticleImage->ZIndex = z;
		ParticleImage->DrawRegion.X(ix);
		ParticleImage->DrawRegion.Y(iy);
		ParticleImage->DrawRegion.W(iw);
		ParticleImage->DrawRegion.H(ih);
	}
}

void Emitter::createParticle(Vector2 location, int i) {
	char r,g,b,a;
	float c,s,x,y,t;

	// Knows to draw and to revive when dead
	_particles[i].spawn = 1;

	// Generate random attributes
	_particles[i].TTL = randomRange(RangeTTL.X, RangeTTL.Y);

	r = (char)randomRange(RangeColorRed.X,RangeColorRed.Y); if (r<0) {r=0;} else if (r>255) {r=255;}
	g = (char)randomRange(RangeColorGreen.X,RangeColorGreen.Y); if (g<0) {g=0;} else if (g>255) {g=255;}
	b = (char)randomRange(RangeColorBlue.X,RangeColorBlue.Y); if (b<0) {b=0;} else if (b>255) {b=255;}
	a = (char)randomRange(RangeColorAlpha.X,RangeColorAlpha.Y); if (a<0) {a=0;} else if (a>255) {a=255;}
	_particles[i].RGBA = (r<<24) | (g<<16) | (b<<8) | a;

	_particles[i].rect.X(randomRange(RangeStartX.X, RangeStartX.Y) + location.X);
	_particles[i].rect.Y(randomRange(RangeStartY.X, RangeStartX.Y) + location.Y);
	_particles[i].rect.W(randomRange(RangeDimensionW.X, RangeDimensionW.Y));
	_particles[i].rect.H(randomRange(RangeDimensionH.X, RangeDimensionH.Y));


	t = degToRad(randomRange(RangeVelocityTheta.X, RangeVelocityTheta.Y));
	c = cos(t);
	s = sin(t);
	x = abs(randomRange(RangeVelocity.X, RangeVelocity.Y));
	y = -x;

	_particles[i].velocity.X = c * x;
	_particles[i].velocity.Y = s * x;

	_particles[i].angleVelocity = randomRange(RangeRotation.X, RangeRotation.Y);
}

//---------------------------------------------------------------------------
// Status Functions
//---------------------------------------------------------------------------
Emitter::EmitterStatus Emitter::Status() {
	if (_activeP <= 0)
		return EMITTER_DEAD;
	if (_pause)
		return EMITTER_PAUSED;
	return EMITTER_ALIVE;
}

void Emitter::Pause() {
	if (_activeP > 0)
		_pause = 1;
}

void Emitter::Resume() {
	if (_activeP > 0)
		_pause = 0;
}

void Emitter::Stop() {
	int i;

	_timeLeft = 0;
	if (_activeP <= 0)
		return;

	// clean up
	for (i=0;i<_maxP;i++) {
		_particles[i].TTL = 0;
		_particles[i].spawn = 0;
	}
	_activeP = 0;
}

//---------------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------------
float randomRange(float min, float max) {
	return min + static_cast<float>(rand())/(static_cast<float>(RAND_MAX/(max-min)));
}

float degToRad(float d) { while(d>180){d-=360;}while(d<-180){d+=360;} return (d*3.14159f)/180.f; }
