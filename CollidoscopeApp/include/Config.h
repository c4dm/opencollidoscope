#pragma once

#include <string>
#include <array>
#include "cinder/Color.h"
#include "cinder/Xml.h"


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
        return mAudioInputDeviceKey;  // Komplete 1/2
        //return "{0.0.1.00000000}.{a043bc8c-1dd1-4c94-82b4-ad8320cac5a5}"; // Komplete 3/4 
        //return "{0.0.1.00000000}.{828b681b-cc0c-44e1-93c9-5f1f46f5926f}"; // Realtek 
    }

    std::size_t getNumChunks() const
    {
        return mNumChunks;
    }

    /* return wave lenght in seconds */
    double getWaveLen() const
    {
        return mWaveLen;
    }

    ci::Color getWaveSelectionColor(size_t waveIdx) const
    {
        if (waveIdx == 0){
            return cinder::Color(243.0f / 255.0f, 6.0f / 255.0f, 62.0f / 255.0f);
        }
        else{
            return cinder::Color(255.0f / 255.0f, 204.0f / 255.0f, 0.0f / 255.0f);
        }
    }

    std::size_t getCursorTriggerMessageBufSize() const
    {
        return 512;
    }

    // returns the index of the wave associated to the MIDI channel passed as argument 
    size_t getWaveForMIDIChannel( unsigned char channelIdx )
    {
        return channelIdx;
        /*for ( int i = 0; i < NUM_WAVES; i++ ){
            if ( channelIdx == mMidiChannels[i] )
                return i;
        }*/
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

    size_t getMaxSelectionNumChunks() const
    {
        return 37;
    }

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
