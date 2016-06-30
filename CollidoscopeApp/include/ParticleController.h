#pragma once

#include "cinder/gl/gl.h"
#include <vector>


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

    size_t mNumParticles;

    ci::gl::VboRef			mParticleVbo;
    ci::gl::BatchRef		mParticleBatch;

 public:
    static const int kMaxParticleAdd = 22;

    ParticleController();
	void addParticles(int amount, const ci::vec2 &initialLocation, const float cloudSize);
	
    void updateParticles();

    inline void draw()
    {
        mParticleBatch->draw();
    }
	
};

