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

    // set selection size in samples 
    void setSelectionSize( size_t size )
    {
        mSelectionSize.set( size );
    }

    void setSelectionStart( size_t start )
    {
        mSelectionStart.set( start );
    }

    void setGrainsDurationCoeff( double coeff )
    {
        mGrainDurationCoeff.set( coeff );
    }

    // used for trigger callback in PGRanular 
    void operator()( char msgType, int ID );

    ci::audio::dsp::RingBufferT<NoteMsg>& getNoteRingBuffer() { return mNoteMsgRingBufferPack.getBuffer(); }

protected:
    
    void initialize()							override;

    void process( ci::audio::Buffer *buffer )	override;

private:

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

    void handleNoteMsg( const NoteMsg &msg );

    // pointer to PGranular object 
    std::unique_ptr < collidoscope::PGranular<float, RandomGenerator, PGranularNode > > mPGranularLoop;
    std::array<std::unique_ptr < collidoscope::PGranular<float, RandomGenerator, PGranularNode > >, kMaxVoices> mPGranularNotes;
    std::array<int, kMaxVoices> mMidiNotes;

    // pointer to the random generator struct passed over to PGranular 
    std::unique_ptr< RandomGenerator > mRandomOffset;
    
    /* buffer containing the recorder audio, to pass to PGranular in initialize() */
    ci::audio::Buffer *mGrainBuffer;

    ci::audio::BufferRef mTempBuffer;

    CursorTriggerMsgRingBuffer &mTriggerRingBuffer;
    RingBufferPack<NoteMsg> mNoteMsgRingBufferPack;

    LazyAtomic<size_t> mSelectionSize;
    
    LazyAtomic<size_t> mSelectionStart;
    
    LazyAtomic<double> mGrainDurationCoeff;


};

