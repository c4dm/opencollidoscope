#pragma once

#include "cinder/Cinder.h"
#include "cinder/audio/Node.h"
#include "cinder/audio/SampleRecorderNode.h"
#include "cinder/audio/dsp/RingBuffer.h"
#include "cinder/Filesystem.h"

#include "Messages.h"

typedef std::shared_ptr<class BufferToWaveRecorderNode>	BufferToWaveRecorderNodeRef;

typedef ci::audio::dsp::RingBufferT<RecordWaveMsg> RecordWaveMsgRingBuffer;

/**
 * A \a Node in the audio graph of the Cinder audio library that records input in a buffer.
 *
 * This class is similar to \a cinder::audio::BufferRecorderNode (it's a derivative work of this class indeed) but it has an additional feature.
 * When recording it uses the audio input samples to compute the size values of the visual chunks. 
 * The chunks values are stored in a ring buffer and fetched by the graphic thread to paint the wave as it gets recorded.
 *
 */
class BufferToWaveRecorderNode : public ci::audio::SampleRecorderNode {
public:

    static const float kRampTime;

    //! Constructor. numChunks is the total number of chunks this biffer has to be borken down in. 
    //! numSeconds lenght of the buffer in seconds 
    BufferToWaveRecorderNode( std::size_t numChunks, double numSeconds );

    //! Starts recording. Resets the write position to zero (call disable() to pause recording).
    void start();
    //! Stops recording. Same as calling disable().
    void stop();

    //! \brief Sets the length of the recording buffer in frames.
    //!
    //! If the write position is non-zero, the old contents will be preserved (by copying it to the newly allocated Buffer).
    //! If \a shrinkToFit is set to `true`, the internal Buffer will be down-sized if necessary, otherwise it will only re-allocate when growing while changing its dimensions to match \a numFrames (default shrinkToFit = false).
    void setNumFrames(size_t numFrames, bool shrinkToFit = false);
    //! Sets the length of the recording buffer in seconds. \see setNumFrames
    void setNumSeconds(double numSeconds, bool shrinkToFit = false);

    //! Returns the length of the recording buffer in frames.
    size_t		getNumFrames() const	{ return mRecorderBuffer.getNumFrames(); }
    //! Returns the length of the recording buffer in seconds.
    double		getNumSeconds() const;

    //! \brief Returns a copy of the recored samples, up to the current write position.
    //!
    //! This method is non locking, and as such any resizing calls must be performed on the same thread or be otherwise synchronized.
    ci::audio::BufferRef	getRecordedCopy() const;

    //! \brief Writes the currently recorded samples to a file at \a filePath
    //!
    //! The encoding format is derived from \a filePath's extension and \a sampleType (default = SampleType::INT_16).
    //! \note throws AudioFileExc if the write request cannot be completed.
    void writeToFile(const ci::fs::path &filePath, ci::audio::SampleType sampleType = ci::audio::SampleType::INT_16);

    //! Returns the frame of the last buffer overrun or 0 if none since the last time this method was called. When this happens, it means the recorded buffer probably has skipped some frames.
    uint64_t getLastOverrun();

    //! returns a reference to the ring buffer when the size values of the chunks is stored, when a new wave is recorder
    RecordWaveMsgRingBuffer& getRingBuffer() { return mRingBuffer; }

    //!returns a pointer to the buffer where the audio is recorder. This is used by the PGranular to create the granular synthesis 
    ci::audio::Buffer* getRecorderBuffer() { return &mRecorderBuffer; }


protected:
    void initialize()				override;
    void process(ci::audio::Buffer *buffer)	override;

    void initBuffers(size_t numFrames);

    static const float kMinAudioVal; 
    static const float kMaxAudioVal;

    ci::audio::BufferDynamic		mRecorderBuffer;
    ci::audio::BufferDynamicRef		mCopiedBuffer;
    std::atomic<uint64_t>	mLastOverrun;

    RecordWaveMsgRingBuffer mRingBuffer;

    const std::size_t mNumChunks;
    const double mNumSeconds;
    std::size_t mNumSamplesPerChunk;
    std::atomic<std::size_t> mChunkIndex;

    size_t mChunkSampleCounter;
    float mChunkMaxAudioVal;
    float mChunkMinAudioVal;

    float mEnvRamp;
    float mEnvRampRate;
    size_t mEnvRampLen;
    size_t mEnvDecayStart;

};

