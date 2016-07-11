
#pragma once

#include "cinder/Color.h"
#include "cinder/gl/Batch.h"

class DrawInfo;

/**
 *
 * A chunk of audio in Collidoscope low-fi visual wave. 
 *
 * The visual wave of Collidoscope is made out of a number of bars that mimics in a low-fi fashion the typical waveform based representation of audio.
 * A Chunk is one of the bars of the visual wave. 
 *
 */

class Chunk
{

public:

    const static float kWidth;
    const static float kHalfWidth;

    /**
     * Constructor, takes as argument the index of this chunk in the wave
     */ 
	Chunk( size_t index );

    /**
     * Sets the top value of this chunk. The value is passed in audio coordinates : [-1.0, 1.0]
     */
    void inline setTop(float t) { mAudioTop = t; mAnimate = 0.0f; mResetting = false; /* startes the animation to crate a chunk */ }
    /**
     * Sets the bottom value of this chunk. The value is passed in audio coordinates : [-1.0, 1.0]
     */
    void inline setBottom(float b) { mAudioBottom = b; mAnimate = 0.0f; mResetting = false; }
    /**
     * Get the top value of this chunk. The value is returned in audio coordinates : [-1.0, 1.0]
     */
    float inline getTop() const { return mAudioTop; }
    /**
     * Get the bottom value of this chunk. The value is returned in audio coordinates : [-1.0, 1.0]
     */
    float inline getBottom() const { return mAudioBottom; }

    /**
     * Reset this chunks. When a chunk is reset it starts shrinking until it disappears.
     *
     */ 
	void reset(){
		mResetting = true;
	}

    /**
     * Called in the graphic loop. It update this chunk. 
     */ 
    void update( const DrawInfo& di );

    /**
     * Called in the graphic loop. It draws this chunk. 
     */ 
    void draw( const DrawInfo& di, ci::gl::BatchRef &batch );

    /**
     * Called in the graphic loop. It draws this chunk all the way to the bottom of the screen. 
     * This method is called when the chunk is the first or last in a selection. 
     */ 
    void drawBar( const DrawInfo& di, ci::gl::BatchRef &batch );

    /**
     * Informs this chunk that it's the first chunk of the selection.
     */ 
	void setAsSelectionStart(bool start){
		isSelectionStart = start;
	}

    /**
     * Informs this chunk that it's the last chunk of the selection.
     */ 
	void setAsSelectionEnd(bool end){
		isSelectionEnd = end;
	}

private:

    float mAudioTop;
    float mAudioBottom;

    float mX;

    float mAnimate = 1.0;
    int mIndex;

    bool isSelectionStart = false;
    bool isSelectionEnd = false;

    bool mResetting = false;

};
