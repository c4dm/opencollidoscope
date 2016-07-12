#pragma once

#include "cinder/gl/gl.h"
#include <vector>

/**
 * The ParticleController creates/updates/draws and destroys particles
 */ 
class ParticleController {

    struct Particle {

        ci::vec2	mCloudCenter;
        ci::vec2	mVel;
        float       mCloudSize;

        int			mAge;
        int			mLifespan;
        bool        mFlyOver;

    };

    static const int kMaxParticles = 150;

	std::vector<Particle> mParticles;
    std::vector< ci::vec2 > mParticlePositions;

    // current number of active particles
    size_t mNumParticles;

    ci::gl::VboRef			mParticleVbo;
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

