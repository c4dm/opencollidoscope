#pragma once

#include "cinder/Area.h"

class DrawInfo
{
public:

    DrawInfo( size_t waveIndex ):
        mWaveIndex( waveIndex ),
        mWindowWidth(0),
        mWindowHeight(0),
        mSelectionBarHeight(0),
        mShrinkFactor(1)
    {}

    void reset( const ci::Area &bounds, float shrinkFactor )
    {
        mWindowWidth = bounds.getWidth();
        mWindowHeight = bounds.getHeight();
        mSelectionBarHeight = mWindowHeight / NUM_WAVES;
        mShrinkFactor = shrinkFactor;
    }

	float audioToHeigt(float audioSample) const {
        /* clip into range [-1.1] */
        if (audioSample < -1.0f) {
            audioSample = -1.0f;
        }
        else if ( audioSample > 1.0f ){
            audioSample = 1.0f;
        }

        /* map from [-1,1] to [0,1] */
		float ratio = (audioSample - (-1.0f)) * 0.5f; // 2 = 1 - (-1) 

		/* get bottom and add the scaled height */
        return ratio * mSelectionBarHeight; //remove  bounds.getY1() bound only needed for size of tier
	}

    float getMaxChunkHeight() const 
    {
        return mSelectionBarHeight * mShrinkFactor;
    }

    float getSelectionBarHeight() const
    {
        return mSelectionBarHeight;
    }

    int32_t getWaveCenterY() const
    {
        if ( mWaveIndex == 0 )
            return mWindowHeight * 0.75f + 1;
        else
            return mWindowHeight / (NUM_WAVES * 2);
    }

	int flipY(int y) const 
    {
        if ( mWaveIndex == 0)
		    return mWindowHeight - y /*+ 24*/;
        else
            return y /*- 24*/;
	}

	int flipX(int x) const
    {
        return x;
	}


    // how much the wave is shrunk on the y axis with respect to the wave's tier 
    float getShrinkFactor() const 
    {
        return mShrinkFactor;
    }

    int32_t getWindowWidth() const
    {
        return mWindowWidth;
    }

    int32_t getWindowHeight() const
    {
        return mWindowHeight;
    }

    DrawInfo( const DrawInfo &original ) = delete;
    DrawInfo & operator=( const DrawInfo &original ) = delete;

private:
    const size_t mWaveIndex;

    int32_t mWindowHeight;
    int32_t mWindowWidth;
    int32_t mSelectionBarHeight;

    float mShrinkFactor;

};
