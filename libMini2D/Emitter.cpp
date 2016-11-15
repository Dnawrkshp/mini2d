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

#define PI 3.14159265
#define DEG2RAD(x) ((x*PI)/180.0)
#define RAD2DEG(x) ((x*180.0)/PI)

float randomRange(float min, float max);

//---------------------------------------------------------------------------
// Init Functions
//---------------------------------------------------------------------------
Emitter::Emitter(Mini2D * mini, unsigned int maxParticles) : _mini(mini) {
	_maxP = maxParticles;

	// Full range dispersion
	RangeVelocityTheta.Set(0,360);

	// Velocity from 0 to 10
	RangeVelocity.Set(0, 10);

	// Width/Height dimensions from 10 to 50
	RangeDimensionW.Set(10, 50);
	RangeDimensionH.Set(10, 50);

	// Particle life time for 5 to 20 seconds
	RangeTTL.Set(5,20);

	// Color range RGB (0-255), Alpha (255)
	RangeColorRed.Set(0,255);
	RangeColorGreen.Set(0,255);
	RangeColorBlue.Set(0,255);
	RangeColorAlpha.Set(255,255);

	// Set draw region to entire screen
	Clip.Dimension.Set(mini->MAXW, mini->MAXH);
	Clip.Location.Set(Clip.Dimension.X/2,Clip.Dimension.Y/2);

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

	_startLocation.Set(location.X, location.Y);

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
					_particles[i].rect.RectangleAngle += _particles[i].angleVelocity * dt;
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
	float z=0,ix=0,iy=0,iw=0,ih=0,ua=0,ra=0;
	if (ParticleImage) {
		z = ParticleImage->ZIndex;
		ix = ParticleImage->DrawRegion.X();
		iy = ParticleImage->DrawRegion.Y();
		iw = ParticleImage->DrawRegion.W();
		ih = ParticleImage->DrawRegion.H();
		ra = ParticleImage->DrawRegion.RectangleAngle;
		ua = ParticleImage->DrawRegion.UseAnchor;
	}

	alive = 0;
	for (i=0;i<_activeP;i++) {
		if (_particles[i].TTL > 0 && _particles[i].spawn) {
			if (!_pause) {
				_particles[i].rect.Location += (_particles[i].velocity * deltaTime);
				_particles[i].rect.RectangleAngle += _particles[i].angleVelocity * deltaTime;
				_particles[i].TTL -= deltaTime;
			}

			if (ParticleImage) {
				ParticleImage->ZIndex = ZIndex;
				ParticleImage->DrawRegion.X(_particles[i].rect.X());
				ParticleImage->DrawRegion.Y(_particles[i].rect.Y());
				ParticleImage->DrawRegion.W(_particles[i].rect.W());
				ParticleImage->DrawRegion.H(_particles[i].rect.H());
				ParticleImage->DrawRegion.RectangleAngle = _particles[i].rect.RectangleAngle;
				ParticleImage->DrawRegion.UseAnchor = 0;
				ParticleImage->Draw(_particles[i].RGBA);
			}
			else {
				_mini->DrawRectangle(_particles[i].rect.X(), _particles[i].rect.Y(),
									_particles[i].rect.X(), _particles[i].rect.Y(), ZIndex,
									_particles[i].rect.W(), _particles[i].rect.H(),
									_particles[i].RGBA, _particles[i].rect.RectangleAngle);
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
		ParticleImage->DrawRegion.RectangleAngle = ra;
		ParticleImage->DrawRegion.UseAnchor = ua;
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


	t = DEG2RAD(randomRange(RangeVelocityTheta.X, RangeVelocityTheta.Y));
	c = cos(t);
	s = sin(t);
	x = abs(randomRange(RangeVelocity.X, RangeVelocity.Y));
	y = -x;


	_particles[i].velocity.Set(c*x, s*x);

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
