#pragma once

#include <array>

#include "cinder/audio/Context.h"
#include "cinder/audio/ChannelRouterNode.h"
#include "cinder/audio/MonitorNode.h"
#include "cinder/audio/FilterNode.h"
#include "BufferToWaveRecorderNode.h"
#include "PGranularNode.h"
#include "RingBufferPack.h"

#include "Messages.h"
#include "Config.h"


class AudioEngine
{
public:

    AudioEngine();

    ~AudioEngine();

    // no copies
    AudioEngine( const AudioEngine &copy ) = delete;
    AudioEngine & operator=(const AudioEngine &copy) = delete;

    void setup( const Config& Config );

    size_t getSampleRate();

    void record( size_t index );

    void loopOn( size_t waveIdx );

    void loopOff( size_t waveIdx );

    void noteOn( size_t waveIdx, int note );

    void noteOff( size_t waveIdx, int note );

    size_t getRecordWaveAvailable( size_t index );

    bool readRecordWave( size_t waveIdx, RecordWaveMsg*, size_t count );

    void setSelectionSize( size_t waveIdx, size_t size );

    void setSelectionStart( size_t waveIdx, size_t start );

    void setGrainDurationCoeff( size_t waveIdx, double coeff );

    void setFilterCutoff( size_t waveIdx, double cutoff );

    void checkCursorTriggers( size_t waveIdx, std::vector<CursorTriggerMsg>& cursorTriggers );

    const ci::audio::Buffer& getAudioOutputBuffer( size_t waveIdx ) const;


private:

    // nodes for mic input 
    std::array< ci::audio::ChannelRouterNodeRef, NUM_WAVES > mInputRouterNodes;
    // nodes for recording audio input into buffer. Also sends chunks information through 
    // non-blocking queue 
    std::array< BufferToWaveRecorderNodeRef, NUM_WAVES > mBufferRecorderNodes;
    // pgranulars for loop synths 
    std::array< PGranularNodeRef, NUM_WAVES > mPGranularNodes;


    std::array< ci::audio::ChannelRouterNodeRef, NUM_WAVES > mOutputRouterNodes;
    // nodes to get the audio buffer scoped in the oscilloscope 
    std::array< ci::audio::MonitorNodeRef, NUM_WAVES > mOutputMonitorNodes;
    // nodes for lowpass filtering
    std::array< cinder::audio::FilterLowPassNodeRef, NUM_WAVES> mLowPassFilterNodes;

    std::array< std::unique_ptr< RingBufferPack<CursorTriggerMsg> >, NUM_WAVES > mCursorTriggerRingBufferPacks;

};