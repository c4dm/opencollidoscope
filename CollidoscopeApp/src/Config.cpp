#include "Config.h"


#include "cinder/Exception.h"
#include "boost/algorithm/string/trim.hpp"

using ci::DataSourceRef;
using ci::XmlTree;
using ci::loadFile;


Config::Config() :
    mAudioInputDeviceKey( "" ),
    mNumChunks(150),
    mWaveLen(2.0)
{

}


void Config::loadFromFile( std::string&& path )
{
    try {
        XmlTree doc( loadFile( path ) );

        XmlTree collidoscope = doc.getChild( "collidoscope" );

        // audio input device 
        mAudioInputDeviceKey = collidoscope.getChild( "audioInputDeviceKey" ).getValue();
        boost::trim( mAudioInputDeviceKey );

        // wave len in seconds 
        std::string waveLenStr = collidoscope.getChild("wave_len").getValue();
        boost::trim(waveLenStr);
        mWaveLen = ci::fromString<double>(waveLenStr);

        // channel for each wave 
        XmlTree waves = collidoscope.getChild( "waves" );

        for ( int i = 0; i < NUM_WAVES; i++ ){
            for ( auto &wave : waves.getChildren() ){
                int id = ci::fromString<int>( wave->getAttribute( "id" ) );
                if ( id == i ){
                    parseWave( *wave, id );
                    break;
                }
            }
        }

    }
    catch ( std::exception &e ) {
        throw ci::Exception( e.what() );
    }
    


}

// thows exception captured in loadFromFile 
void Config::parseWave( const XmlTree &wave, int id )
{
    // midi channel 
    std::string midiChannelStr = wave.getChild( "midiChannel" ).getValue();
    boost::trim( midiChannelStr );

    mMidiChannels[id] = ci::fromString<size_t>( midiChannelStr );

}
