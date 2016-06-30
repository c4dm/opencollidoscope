#include "AudioEngine.h"
#include "cinder/app/App.h"
//FIXME remove App.h include 
#include "Log.h"

using namespace ci::audio;

double chromaticRatios[] = { 
    1, 
    1.0594630943591, 
    1.1224620483089, 
    1.1892071150019, 
    1.2599210498937, 
    1.3348398541685, 
    1.4142135623711, 
    1.4983070768743, 
    1.5874010519653, 
    1.6817928305039, 
    1.7817974362766, 
    1.8877486253586 
};

inline double calculateMidiNoteRatio( int midiNote )
{
    int distanceFromCenter = midiNote - 60; // 60 is the central midi note 

    if ( distanceFromCenter < 0 ){
        int diffAmount = -distanceFromCenter;
        int octaves = diffAmount / 12;
        int intervals = diffAmount % 12;

        return std::pow( 0.5, octaves ) / chromaticRatios[intervals];
    }
    else{
        int octaves = distanceFromCenter / 12;
        int intervals = distanceFromCenter % 12;

        return std::pow( 2, octaves ) * chromaticRatios[intervals];
    }
}


AudioEngine::AudioEngine() 
{}

AudioEngine::~AudioEngine()
{}

void AudioEngine::setup(const Config& config)
{
    
    for ( int i = 0; i < NUM_WAVES; i++ ){
        mCursorTriggerRingBufferPacks[i].reset( new RingBufferPack<CursorTriggerMsg>( 512 ) ); // FIXME 
    }

    /* audio context */
    auto ctx = Context::master();

    /* audio inpu device */
    auto inputDeviceNode = ctx->createInputDeviceNode( Device::getDefaultInput() );
 

    /* route the audio input, which is two channels, to one wave graph for each channel */
    for ( int chan = 0; chan < NUM_WAVES; chan++ ){

        /* one channel router */
        mInputRouterNodes[chan] = ctx->makeNode( new ChannelRouterNode( Node::Format().channels( 1 ) ) );

        /* buffer recorders */  
        mBufferRecorderNodes[chan] = ctx->makeNode( new BufferToWaveRecorderNode( config.getNumChunks(), config.getWaveLen() ) );
        /* this prevents the node from recording before record is pressed */
        mBufferRecorderNodes[chan]->setAutoEnabled( false );

        // route the input part of the audio graph. Two channels input goes into
        // one channel route and to one channel buffer recorder 
        inputDeviceNode >> mInputRouterNodes[chan]->route( chan, 0, 1 ) >> mBufferRecorderNodes[chan];


        // create PGranular loops passing the buffer of the RecorderNode as argument to the contructor 
        // use -1 as ID 
        mPGranularNodes[chan] = ctx->makeNode( new PGranularNode( mBufferRecorderNodes[chan]->getRecorderBuffer(), mCursorTriggerRingBufferPacks[chan]->getBuffer() ) );

        // create filter nodes 
        mLowPassFilterNodes[chan] = ctx->makeNode( new FilterLowPassNode( MonitorNode::Format().channels( 1 ) ) );
        mLowPassFilterNodes[chan]->setCutoffFreq( config.getMaxFilterCutoffFreq() );
        mLowPassFilterNodes[chan]->setQ( 0.707f );
        // create monitor nodes for oscilloscopes 
        mOutputMonitorNodes[chan] = ctx->makeNode( new MonitorNode( MonitorNode::Format().channels( 1 ) ) );

        // all output goes to the filter 
        mPGranularNodes[chan] >> mLowPassFilterNodes[chan];
        
        mOutputRouterNodes[chan] = ctx->makeNode( new ChannelRouterNode( Node::Format().channels( 2 ) ) );

        // filter goes to output 
        mLowPassFilterNodes[chan] >> mOutputRouterNodes[chan]->route( 0, chan, 1 ) >> ctx->getOutput();
        
        // what goes to output goes to scope 
        mLowPassFilterNodes[chan] >> mOutputMonitorNodes[chan];

    }

    ctx->getOutput()->enableClipDetection( false );
    /* enable the whole audio graph */
    inputDeviceNode->enable();
    ctx->enable();
}

size_t AudioEngine::getSampleRate()
{
    return Context::master()->getSampleRate();
}

void AudioEngine::loopOn( size_t waveIdx )
{
    NoteMsg msg = makeNoteMsg( Command::LOOP_ON, 1, 1.0 );
    mPGranularNodes[waveIdx]->getNoteRingBuffer().write( &msg, 1 );
}

void AudioEngine::loopOff( size_t waveIdx )
{
    NoteMsg msg = makeNoteMsg( Command::LOOP_OFF, 0, 0.0 );
    mPGranularNodes[waveIdx]->getNoteRingBuffer().write( &msg, 1 );
}

void AudioEngine::record( size_t waveIdx )
{
    mBufferRecorderNodes[waveIdx]->start();
}

void AudioEngine::noteOn( size_t waveIdx, int midiNote )
{
    
    double midiAsRate = calculateMidiNoteRatio(midiNote);
    NoteMsg msg = makeNoteMsg( Command::NOTE_ON, midiNote, midiAsRate );

    mPGranularNodes[waveIdx]->getNoteRingBuffer().write( &msg, 1 );
}

void AudioEngine::noteOff( size_t waveIdx, int midiNote )
{
    NoteMsg msg = makeNoteMsg( Command::NOTE_OFF, midiNote, 0.0 );
    mPGranularNodes[waveIdx]->getNoteRingBuffer().write( &msg, 1 );
}



void AudioEngine::setSelectionSize( size_t waveIdx, size_t size )
{
    mPGranularNodes[waveIdx]->setSelectionSize( size );
}

void AudioEngine::setSelectionStart( size_t waveIdx, size_t start )
{
    mPGranularNodes[waveIdx]->setSelectionStart( start );
}

void AudioEngine::setGrainDurationCoeff( size_t waveIdx, double coeff )
{
    mPGranularNodes[waveIdx]->setGrainsDurationCoeff( coeff );
}

void AudioEngine::setFilterCutoff( size_t waveIdx, double cutoff )
{
    mLowPassFilterNodes[waveIdx]->setCutoffFreq( cutoff );
}

// ------------------------------------------------------
// ----- methods for communication with main thread -----
// ------------------------------------------------------

size_t AudioEngine::getRecordWaveAvailable( size_t waveIdx )
{
    return mBufferRecorderNodes[waveIdx]->getRingBuffer().getAvailableRead();
}

bool AudioEngine::readRecordWave( size_t waveIdx, RecordWaveMsg* buffer, size_t count )
{
    return mBufferRecorderNodes[waveIdx]->getRingBuffer().read( buffer, count );
}

void AudioEngine::checkCursorTriggers( size_t waveIdx, std::vector<CursorTriggerMsg>& cursorTriggers )
{
    ci::audio::dsp::RingBufferT<CursorTriggerMsg> &ringBuffer = mCursorTriggerRingBufferPacks[waveIdx]->getBuffer();
    CursorTriggerMsg* ringBufferReadArray = mCursorTriggerRingBufferPacks[waveIdx]->getExchangeArray();

    size_t availableRead = ringBuffer.getAvailableRead();
    bool successfulRead = ringBuffer.read( ringBufferReadArray, availableRead );

    if ( successfulRead ){
        for ( size_t i = 0; i < availableRead; i++ ){
            cursorTriggers.push_back( ringBufferReadArray[i] );
        }
    }
}

const ci::audio::Buffer& AudioEngine::getAudioOutputBuffer( size_t waveIdx ) const
{
    return mOutputMonitorNodes[waveIdx]->getBuffer();
}

