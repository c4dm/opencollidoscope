#include "PGranularNode.h"

#include "cinder/audio/Context.h"

#include "cinder/Rand.h"

// generate random numbers from 0 to max 
// it's passed to PGranular to randomize the phase offset at grain creation 
struct RandomGenerator
{

    RandomGenerator( size_t max ) : mMax( max )
    {}

    size_t operator()() const {
        return ci::Rand::randUint( mMax );
    }

    size_t mMax;
};
// FIXME maybe use only one random gen 

PGranularNode::PGranularNode( ci::audio::Buffer *grainBuffer, CursorTriggerMsgRingBuffer &triggerRingBuffer ) :
    Node( Format().channels( 1 ) ),
    mGrainBuffer(grainBuffer),
    mSelectionStart( 0 ),
    mSelectionSize( 0 ),
    mGrainDurationCoeff( 1 ),
    mTriggerRingBuffer( triggerRingBuffer ),
    mNoteMsgRingBufferPack( 128 )
{
    for ( int i = 0; i < kMaxVoices; i++ ){
        mMidiNotes[i] = kNoMidiNote;

    }
}


PGranularNode::~PGranularNode()
{
}

void PGranularNode::initialize()
{
    mTempBuffer = std::make_shared< ci::audio::Buffer >( getFramesPerBlock() );

    mRandomOffset.reset( new RandomGenerator( getSampleRate() / 100 ) ); // divided by 100 corresponds to times 0.01 in the time domain 

    /* create the PGranular object for looping */
    mPGranularLoop.reset( new collidoscope::PGranular<float, RandomGenerator, PGranularNode>( mGrainBuffer->getData(), mGrainBuffer->getNumFrames(), getSampleRate(), *mRandomOffset, *this, -1 ) );

    /* create the PGranular object for notes */
    for ( size_t i = 0; i < kMaxVoices; i++ ){
        mPGranularNotes[i].reset( new collidoscope::PGranular<float, RandomGenerator, PGranularNode>( mGrainBuffer->getData(), mGrainBuffer->getNumFrames(), getSampleRate(), *mRandomOffset, *this, i ) );
    }

}

void PGranularNode::process (ci::audio::Buffer *buffer )
{
    // only update PGranular if the atomic value has changed from the previous time

    const boost::optional<size_t> selectionSize = mSelectionSize.get();
    if ( selectionSize ){
        mPGranularLoop->setSelectionSize( *selectionSize );
        for ( size_t i = 0; i < kMaxVoices; i++ ){
            mPGranularNotes[i]->setSelectionSize( *selectionSize );
        }
    }

    const boost::optional<size_t> selectionStart = mSelectionStart.get();
    if ( selectionStart ){
        mPGranularLoop->setSelectionStart( *selectionStart );
        for ( size_t i = 0; i < kMaxVoices; i++ ){
            mPGranularNotes[i]->setSelectionStart( *selectionStart );
        }
    }

    const boost::optional<double> grainDurationCoeff = mGrainDurationCoeff.get();
    if ( grainDurationCoeff ){
        mPGranularLoop->setGrainsDurationCoeff( *grainDurationCoeff );
        for ( size_t i = 0; i < kMaxVoices; i++ ){
            mPGranularNotes[i]->setGrainsDurationCoeff( *grainDurationCoeff );
        }
    }

    // check messages to start/stop notes or loop 
    size_t availableRead = mNoteMsgRingBufferPack.getBuffer().getAvailableRead();
    mNoteMsgRingBufferPack.getBuffer().read( mNoteMsgRingBufferPack.getExchangeArray(), availableRead );
    for ( size_t i = 0; i < availableRead; i++ ){
        handleNoteMsg( mNoteMsgRingBufferPack.getExchangeArray()[i] );
    }

    // process loop if not idle 
    if ( !mPGranularLoop->isIdle() ){
        /* buffer is one channel only so I can use getData */
        mPGranularLoop->process( buffer->getData(), mTempBuffer->getData(), buffer->getSize() );
    }

    // process notes if not idle 
    for ( size_t i = 0; i < kMaxVoices; i++ ){
        if ( mPGranularNotes[i]->isIdle() )
            continue;

        mPGranularNotes[i]->process( buffer->getData(), mTempBuffer->getData(), buffer->getSize() );

        if ( mPGranularNotes[i]->isIdle() ){
            // this note became idle so update mMidiNotes
            mMidiNotes[i] = kNoMidiNote;
        }
            
    }
}

void PGranularNode::operator()( char msgType, int ID ) {

    switch ( msgType ){
    case 't':  { // trigger 
        CursorTriggerMsg msg = makeCursorTriggerMsg( Command::TRIGGER_UPDATE, ID ); // put ID 
        mTriggerRingBuffer.write( &msg, 1 );
    };
        break;

    case 'e': // end envelope 
        CursorTriggerMsg msg = makeCursorTriggerMsg( Command::TRIGGER_END, ID ); // put ID 
        mTriggerRingBuffer.write( &msg, 1 );
        break;
    }

    
}

void PGranularNode::handleNoteMsg( const NoteMsg &msg )
{
    switch ( msg.cmd ){
    case Command::NOTE_ON: {
        bool synthFound = false;

        for ( int i = 0; i < kMaxVoices; i++ ){
            // note was already on, so re-attack
            if ( mMidiNotes[i] == msg.midiNote ){
                mPGranularNotes[i]->noteOn( msg.rate );
                synthFound = true;
                break;
            }
        }

        if ( !synthFound ){
            // then look for a free synth 
            for ( int i = 0; i < kMaxVoices; i++ ){

                if ( mMidiNotes[i] == kNoMidiNote ){
                    mPGranularNotes[i]->noteOn( msg.rate );
                    mMidiNotes[i] = msg.midiNote;
                    synthFound = true;
                    break;
                }
            }
        }
    };
        break;

    case Command::NOTE_OFF: {
        for ( int i = 0; i < kMaxVoices; i++ ){
            if ( !mPGranularNotes[i]->isIdle() && mMidiNotes[i] == msg.midiNote ){
                mPGranularNotes[i]->noteOff();
                break;
            }
        }
    };
        break;

    case Command::LOOP_ON: {
        mPGranularLoop->noteOn( 1.0 );
    };
        break;

    case Command::LOOP_OFF: {
        mPGranularLoop->noteOff();
    };
        break;
    default:
        break;
    }
}
