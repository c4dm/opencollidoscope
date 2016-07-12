#pragma once

#include "cinder/Cinder.h"
#include "cinder/audio/Node.h"
#include "cinder/audio/dsp/RingBuffer.h"
#include "boost/optional.hpp"
#include "Messages.h"
#include "RingBufferPack.h"

#include <memory>

#include "PGranular.h"
#include "EnvASR.h"

typedef std::shared_ptr<class PGranularNode> PGranularNodeRef;
typedef ci::audio::dsp::RingBufferT<CursorTriggerMsg> CursorTriggerMsgRingBuffer;


struct RandomGenerator;

/*
A node in the Cinder audio graph that holds a PGranular 
*/
class PGranularNode : public ci::audio::Node
{
public:
    static const size_t kMaxVoices = 6;
    static const int kNoMidiNote = -50;

    explicit PGranularNode( ci::audio::Buffer *grainBuffer, CursorTriggerMsgRingBuffer &triggerRingBuffer );
    ~PGranularNode();

    /** Set selection size in samples */
    void setSelectionSize( size_t size )
    {
        mSelectionSize.set( size );
    }

    /** Set selection start in samples */
    void setSelectionStart( size_t start )
    {
        mSelectionStart.set( start );
    }

    void setGrainsDurationCoeff( double coeff )
    {
        mGrainDurationCoeff.set( coeff );
    }

    /* PGranularNode passes itself as trigger callback in PGranular */
    void operator()( char msgType, int ID );

    ci::audio::dsp::RingBufferT<NoteMsg>& getNoteRingBuffer() { return mNoteMsgRingBufferPack.getBuffer(); }

protected:
    
    void initialize()							override;

    void process( ci::audio::Buffer *buffer )	override;

private:

    // Wraps a std::atomic but get() returns a boost::optional that is set to a real value only when the atomic has changed. 
    //  It is used to avoid calling PGranulat setter methods with *  the same value at each audio callback.
    template< typename T>
    class LazyAtomic
    {
    public:
        LazyAtomic( T val ) :
            mAtomic( val ),
            mPreviousVal( val )
        {}

        void set( T val )
        {
            mAtomic = val;
        }

        boost::optional<T> get()
        {
            const T val = mAtomic;
            if ( val != mPreviousVal ){
                mPreviousVal = val;
                return val;
            }
            else{
                return boost::none;
            }
        }

    private:
        std::atomic<T> mAtomic;
        T mPreviousVal;
    };

    // creates or re-start a PGranular and sets the pitch according to the MIDI note passed as argument
    void handleNoteMsg( const NoteMsg &msg );

    // pointers to PGranular objects 
    std::unique_ptr < collidoscope::PGranular<float, RandomGenerator, PGranularNode > > mPGranularLoop;
    std::array<std::unique_ptr < collidoscope::PGranular<float, RandomGenerator, PGranularNode > >, kMaxVoices> mPGranularNotes;
    // maps midi notes to pgranulars. When a noteOff is received maks sure the right PGranular is turned off
    std::array<int, kMaxVoices> mMidiNotes;

    // pointer to the random generator struct passed over to PGranular 
    std::unique_ptr< RandomGenerator > mRandomOffset;
    
    // buffer containing the recorder audio, to pass to PGranular in initialize()
    ci::audio::Buffer *mGrainBuffer;

    ci::audio::BufferRef mTempBuffer;

    CursorTriggerMsgRingBuffer &mTriggerRingBuffer;
    RingBufferPack<NoteMsg> mNoteMsgRingBufferPack;

    LazyAtomic<size_t> mSelectionSize;
    
    LazyAtomic<size_t> mSelectionStart;
    
    LazyAtomic<double> mGrainDurationCoeff;


};

