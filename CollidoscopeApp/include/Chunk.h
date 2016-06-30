
#pragma once

#include "cinder/Color.h"
#include "cinder/gl/Batch.h"

class DrawInfo;

class Chunk
{

public:

    const static float kWidth;
    const static float kHalfWidth;

	Chunk( size_t index );

    void inline setTop(float t) { mAudioTop = t; mAnimate = 0.0f; mResetting = false; /* startes the animation to crate a chunk */ }
    void inline setBottom(float b) { mAudioBottom = b; mAnimate = 0.0f; mResetting = false; }
    float inline getTop() const { return mAudioTop; }
    float inline getBottom() const { return mAudioBottom; }

	void reset(){
		mResetting = true;
	}

    void update( const DrawInfo& di );

    void draw( const DrawInfo& di, ci::gl::BatchRef &batch );

    void drawBar( const DrawInfo& di, ci::gl::BatchRef &batch );

	void setAsSelectionStart(bool start){
		isSelectionStart = start;
	}

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
