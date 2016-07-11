#pragma once

#include "cinder/gl/gl.h"

#include "DrawInfo.h"



/**
 * The oscilloscope that oscillates when Collidoscope is played 
 */ 
class Oscilloscope
{

public:

    /**
     * Constructor, accepts as argument the number of points that make up the oscilloscope line 
     */ 
    Oscilloscope( size_t numPoints ):
        mNumPoints( numPoints ),
        mLine( std::vector<ci::vec2>( numPoints, ci::vec2() ) )
        {}

    /**
     * Sets the value of a point of the oscilloscope. The value is passed as an audio coordinate [-1.0, 1.0].
     * A reference to DrawInfo is passed to calculate the graphic coordinate of the point based on the audio value passed. 
     */ 
    void  setPoint( int index, float audioVal, const DrawInfo &di ){

        if ( audioVal > 1.0f ){
            audioVal = 1.0f;
        }
        else if ( audioVal < -1.0f ){
            audioVal = -1.0f;
        }

        audioVal *= 0.8f;
        // this yRatio is for the bottom scope, the top will be drawn with a translation/4
        // because it's half of the half of the tier where the wave is drawn  
        float yRatio = ((1 + audioVal) / 2.0f) * (di.getWindowHeight() / NUM_WAVES );
        float xRatio = index * (di.getWindowWidth() / (float)mLine.size());

        mLine.getPoints()[index].x = float( di.flipX( int(xRatio) ) );
        mLine.getPoints()[index].y = float( di.flipY( int(yRatio) ) );

        // add the missing line to reach the right of the window
        // indeed the scope starts from 0 to size -1 and adds xRatio
        // to each new point to the line from n-1 to n is missing 
        if (index == mNumPoints - 1){
            xRatio += ( di.getWindowWidth() / mNumPoints );
            xRatio = ceil( xRatio ); // ceil because the division might left one pixel out

            mLine.getPoints()[mNumPoints - 1].x = di.flipX( xRatio ); 
            mLine.getPoints()[mNumPoints - 1].y = di.flipY( yRatio );
        }

    }

    /**
     * Draws this oscilloscope as a cinder::PolyLine2f
     */ 
    void draw()
    {
        ci::gl::color(1.0f, 1.0f, 1.0f);
        ci::gl::draw( mLine );
    }

    size_t getNumPoints() const
    {
        return mNumPoints;
    }

private:
    size_t mNumPoints;
    ci::PolyLine2f  mLine;

};
