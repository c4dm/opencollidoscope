#include "ParticleController.h"
#include "cinder/Rand.h"

#include <type_traits>

using namespace ci;
using std::list;

ParticleController::ParticleController() :
mNumParticles( 0 )

{
    // uses Cinder (and OpenGL) virtual buffer object based drawing
    // see ParticleSphereCPU example in Cinder library 
     
    mParticles.assign( kMaxParticles, Particle() );
    mParticlePositions.assign( kMaxParticles, vec2( -1, -1 ) );

    mParticleVbo = gl::Vbo::create( GL_ARRAY_BUFFER, mParticlePositions, GL_DYNAMIC_DRAW );

    geom::BufferLayout particleLayout;
    particleLayout.append( geom::Attrib::POSITION, 2, sizeof( vec2 ), 0 );

    auto mesh = gl::VboMesh::create( mParticlePositions.size(), GL_POINTS, { { particleLayout, mParticleVbo } } );

    // creates glsl program to run the batch with 
#if ! defined( CINDER_GL_ES )
    auto glsl = gl::GlslProg::create( gl::GlslProg::Format()
        .vertex( CI_GLSL( 150,
            uniform mat4	ciModelViewProjection;
            in vec4			ciPosition;

            void main( void ) {
                gl_Position = ciModelViewProjection * ciPosition;
                gl_PointSize = 1.0;
            }
            ) )
        .fragment( CI_GLSL( 150,
            out vec4 oColor;

            void main( void ) {
                oColor = vec4( 1.0f, 1.0f, 1.0f, 1.0f );
            }
        ) )
    );

    mParticleBatch = gl::Batch::create( mesh, glsl );

#else
    auto glsl = gl::GlslProg::create( gl::GlslProg::Format()
        .vertex( CI_GLSL( 100,
            uniform mat4	ciModelViewProjection;
            attribute vec4			ciPosition;

            void main( void ) {
                gl_Position = ciModelViewProjection * ciPosition;
                gl_PointSize = 1.0;
            }
        ) )
        .fragment( CI_GLSL( 100,
            precision highp float;

            void main( void ) {
                gl_FragColor = vec4( 1, 1, 1, 1 );
            }
        ) ) 
    );

    mParticleBatch = gl::Batch::create( mesh, glsl );
#endif


}

void ParticleController::updateParticles()
{
    // update the positions of the particles and dispose them if they're reached their timespan
    for ( size_t i = 0; i < mNumParticles; i++ ){

        Particle &particle = mParticles[i];
        vec2 &pos = mParticlePositions[i];

        particle.mAge++;


        if ( (!particle.mFlyOver && particle.mAge > particle.mLifespan) 
                || (particle.mFlyOver && particle.mAge >= 300) ){
            // dispose particle 
            mParticles[i] = mParticles[mNumParticles - 1];
            mParticlePositions[i] = mParticlePositions[mNumParticles - 1];
            mParticlePositions[mNumParticles - 1].x = -1.0f;
            mParticlePositions[mNumParticles - 1].y = -1.0f;
            mNumParticles--;
            continue;
        }

        pos += particle.mVel;
        if ( ci::distance( pos, particle.mCloudCenter ) >  particle.mCloudSize && !particle.mFlyOver ){
            particle.mVel = rotate<float>( particle.mVel, 5 );
        }
    }

    // Copy particle data onto the GPU.
	// Map the GPU memory and write over it.
    void *gpuMem = mParticleVbo->mapReplace();
    memcpy( gpuMem, mParticlePositions.data(), mParticlePositions.size() * sizeof( vec2 ) );
    mParticleVbo->unmap();
}

void ParticleController::addParticles(int amount, const vec2 &initialLocation, const float cloudSize)
{
    // reduce the particles liearly to the total number of particles already present 
    // the more particles aleary present the less particle are added
    int reduction = ci::lmap<int>(mNumParticles, 0, kMaxParticles, 0, kMaxParticleAdd);
    amount -= reduction;

    if ( mNumParticles + amount > kMaxParticles ){
		//a.k.a. return if reached kMaxParticles 
        amount = kMaxParticles - mNumParticles;
	}

    if( amount <= 0 )
        return;

	for( size_t i = 0; i < amount; i++ ){
        // init new particle 
        Particle &particle = mParticles[mNumParticles + i];
        vec2 &pos = mParticlePositions[mNumParticles + i];

        pos = initialLocation + Rand::randVec2() * 5.0f; // find a location nearby the initial location
        particle.mCloudCenter = pos;
        particle.mVel = Rand::randVec2() * Rand::randFloat( 1.0f, 5.0f );
        particle.mCloudSize = cloudSize;
        particle.mAge = 0;
        particle.mLifespan = Rand::randInt( 30, 60 );
        particle.mFlyOver = (Rand::randInt( 500 ) == 0);
		
	}
    
    mNumParticles += amount ;

}



