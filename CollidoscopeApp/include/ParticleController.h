#pragma once

#include "cinder/gl/gl.h"
#include <vector>

/**
 * The ParticleController creates/updates/draws and destroys particles
 */ 
class ParticleController {

    struct Particle {

        ci::vec2	mCloudCenter; // initial positin of the particle 
        ci::vec2	mVel;         // velocity 
        float       mCloudSize;   // how big is the area where particle float around. When a particle hits the 
                                  //   border of the area it gets deflected 

        int			mAge;      // when mAge == mLifeSpan the particle is disposed 
        int			mLifespan; // how long a particle lives
        bool        mFlyOver;  // some particles last longer and fly over the screen and reach the other user

    };

    static const int kMaxParticles = 150;

	std::vector<Particle> mParticles;
    std::vector< ci::vec2 > mParticlePositions;

    // current number of active particles
    size_t mNumParticles;

    ci::gl::VboRef			mParticleVbo;    // virtual buffer object 
    ci::gl::BatchRef		mParticleBatch;

 public:
    /**
     * Every time addParticles is run, up to kMaxParticleAdd are added at once
     */ 
    static const int kMaxParticleAdd = 22;

    ParticleController();

    /**
     * Adds \a amount particles and places them in \a initialLocation. 
     * \cloudSize determines how far the particles can go
     */ 
	void addParticles(int amount, const ci::vec2 &initialLocation, const float cloudSize);
	
    /**
     * Updates position and age of the particles
     */ 
    void updateParticles();

    /**
     * Draws all the particles
     */ 
    inline void draw()
    {
        mParticleBatch->draw();
    }
	
};

