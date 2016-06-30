#pragma once


#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Batch.h"


#include "Chunk.h"
#include "DrawInfo.h"

#ifdef USE_PARTICLES
#include "ParticleController.h"
#endif 

#include "cinder/Color.h"
#include "cinder/PolyLine.h"
#include "cinder/Rand.h"

#include <vector>
#include <map>


class DrawInfo;
typedef int SynthID;


using ci::ivec2;
using ci::vec2;
using ci::Color;
using ci::ColorA;

struct Cursor {
    static const int kNoPosition = -100;
    int pos;
    double lastUpdate;
};


class Wave
{
	friend class ParticleController;

public:

	class Selection {
		
	public:

        Selection( Wave * w, Color color );
        
        void setStart( size_t start );

		void setSize( size_t size );
		
        void inline setParticleSpread( float spread ){
            mParticleSpread = spread;
		}

		size_t getStart(void) const { return mSelectionStart; }
		
        size_t getEnd(void) const { return mSelectionEnd; }

		size_t inline getSize(void) const { 
			if (mNull)
				return 0;
			else
				return 1 + mSelectionEnd - mSelectionStart; 
		}

		float inline  getParticleSpread() const { return mParticleSpread; }

		inline void setToNull(){
            mParticleSpread = 1.0f;
			mNull = true;
		}

		inline bool isNull() const{
			return mNull;
		}

		inline const Color & getColor() const{
			return mColor;
		}

    private:

        size_t mSelectionStart;
        
        size_t mSelectionEnd;
        
        float mParticleSpread;

        bool mNull = true;

        Color mColor;

        Wave * mWave;

	}; // class Selection

	

#ifdef USE_PARTICLES
	ParticleController mParticleController;
#endif 

	

	/* there is one cursor for each Synth being played */
	std::map < SynthID, Cursor > mCursors;
	std::vector<int> mCursorsPos;

public:
	
    // note used to identify the loop for cursor position 
    static const int kLoopNote = -1;
	static const cinder::Color CURSOR_CLR;
	/* must be in sync with supercollider durationFactor ControlSpec max */
	static const int MAX_DURATION = 8;
#ifdef USE_PARTICLES
	static const int PARTICLESIZE_COEFF = 40;
#endif

	void reset(bool onlyChunks);

	void setChunk(size_t index, float bottom, float top);

	const Chunk & getChunk(size_t index);

    inline void setCursorPos( SynthID id, int pos, const DrawInfo& di ){

	    Cursor & cursor = mCursors[id];
	    cursor.pos = pos;
	    cursor.lastUpdate = ci::app::getElapsedSeconds();

#ifdef USE_PARTICLES
	    /* if the duration is greater than 1.0 carry on the cursor as a particle
	    the smaller the selection the more particles
	    the bigger the duration the more particles */
	    if (mSelection.getParticleSpread() > 1.0f){
		    /* amountCoeff ranges from 1/8 to 1 */
            const float amountCoeff = (mSelection.getParticleSpread() / MAX_DURATION);
                
            /* get radom point within seleciton as center of the particle */
            vec2 centrePoint; // was former getRandomPoint
            const int randomChunkIndex = ci::Rand::randInt(mSelection.getStart(), mSelection.getEnd() );

            centrePoint.x = di.flipX( 1 + (randomChunkIndex * (2 + Chunk::kWidth)) + Chunk::kWidth / 2 );
            centrePoint.y = di.flipY( di.audioToHeigt(0.0) );

            const float wavePixelLen = mNumChunks * ( 2 + Chunk::kWidth);
		    centrePoint.x *= float(di.getWindowWidth()) / wavePixelLen;

		    mParticleController.addParticles(
                std::max( 1, (int)(amountCoeff * ParticleController::kMaxParticleAdd * mFilterCoeff) ), // amount of particles to add 
			    centrePoint,
                mSelection.getParticleSpread() * PARTICLESIZE_COEFF   // size of the cloud 
			    );
	    }
#endif

		
	}

    void update( double secondsPerChunk, const DrawInfo& di );

    void removeCursor( SynthID id ) { mCursors.erase( id ); }

    // parameter ranges from 0 to 1 
	inline void setselectionAlpha(float alpha){ mFilterCoeff = alpha;}

    void draw( const DrawInfo& di );

	Selection& getSelection() { return mSelection; };

	size_t getSize() const{ return mChunks.size();  }

	void setScopePoint(int index, float audioVal);

    Wave( size_t numChunks, Color selectionColor );

    // no copies 
    Wave( const Wave &copy ) = delete;
    Wave & operator=(const Wave &copy) = delete;


private:
    
    const size_t mNumChunks;

    std::vector<Chunk> mChunks;
    
    Selection mSelection;

    cinder::Color mColor;

    float mFilterCoeff;

    ci::gl::BatchRef mChunkBatch;

};

