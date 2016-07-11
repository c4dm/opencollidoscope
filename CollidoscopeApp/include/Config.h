#pragma once

#include <string>
#include <array>
#include "cinder/Color.h"
#include "cinder/Xml.h"


/**
 * Configuration class gathers in one place all the values recided at runtime
 *
 * Reading the configuration from an XML file is partially implemented but not used at the moment
 *
 */ 
class Config
{
public:

    Config();

    // no copies 
    Config( const Config &copy ) = delete;
    Config & operator=(const Config &copy) = delete;

    /* load values for internal field from configuration file. Throws ci::Exception */
    void loadFromFile( std::string&& path );

    std::string getInputDeviceKey() const
    {
        return mAudioInputDeviceKey; 
    }

    /**
     * Returns number of chunks in a wave 
     */ 
    std::size_t getNumChunks() const
    {
        return mNumChunks;
    }

    /** returns wave lenght in seconds */
    double getWaveLen() const
    {
        return mWaveLen;
    }

    /**
     * Returns wave's selection color
     */ 
    ci::Color getWaveSelectionColor(size_t waveIdx) const
    {
        if (waveIdx == 0){
            return cinder::Color(243.0f / 255.0f, 6.0f / 255.0f, 62.0f / 255.0f);
        }
        else{
            return cinder::Color(255.0f / 255.0f, 204.0f / 255.0f, 0.0f / 255.0f);
        }
    }

    /**
     * The size of the ring buffer used to trigger a visual cursor from the audio thread when a new grain is created
     */ 
    std::size_t getCursorTriggerMessageBufSize() const
    {
        return 512;
    }

    /** returns the index of the wave associated to the MIDI channel passed as argument */
    size_t getWaveForMIDIChannel( unsigned char channelIdx )
    {
        return channelIdx;
    }

    double getMaxGrainDurationCoeff() const
    {
        return 8.0;
    }

    double getMaxFilterCutoffFreq() const
    {
        return 22050.;
    }

    double getMinFilterCutoffFreq() const
    {
        return 200.;
    }

    size_t getMaxKeyboardVoices() const
    {
        return 6;
    }

    /**
     * Returns the maximum size of a wave selection in number of chunks.
     */ 
    size_t getMaxSelectionNumChunks() const
    {
        return 37;
    }

    /**
     * The value returned is used when creating the oscilloscope. 
     * The oscilloscope represents the audio output buffer graphically. However it doesn't need to be as refined as the 
     * audio wave and it's downsampled using the following formula :  number of oscilloscope points = size o audio output buffer / getOscilloscopeNumPointsDivider() 
     */ 
    size_t getOscilloscopeNumPointsDivider() const
    {
        return 4;
    }

private:

    void parseWave( const ci::XmlTree &wave, int id );

    std::string mAudioInputDeviceKey;
    std::size_t mNumChunks;
    double mWaveLen;
    std::array< size_t, NUM_WAVES > mMidiChannels; 

};
