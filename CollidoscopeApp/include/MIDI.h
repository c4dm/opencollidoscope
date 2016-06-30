#pragma once

#include "RtMidi.h"
#include <memory>
#include <mutex>
#include <array>

class Config;


namespace collidoscope {


class MIDIException : public std::exception
{
public:

    MIDIException( std::string message ) : mMessage( message ) {}

    virtual const std::string& getMessage( void ) const { return mMessage; }

#ifdef _WINDOWS
    const char* what() const override { return mMessage.c_str(); }
#else
    const char* what() const noexcept override { return mMessage.c_str(); }
#endif

protected:
    std::string mMessage;
};

class MIDIMessage
{
    friend class MIDI;
public:

    enum class Voice { eNoteOn, eNoteOff, ePitchBend, eControlChange, eIgnore };

    Voice getVoice() { return mVoice; }

    unsigned char getChannel() { return mChannel; }

    unsigned char getData_1() { return mData1; }

    unsigned char getData_2() { return mData2; }

private:

    Voice mVoice = Voice::eIgnore;
    unsigned char mChannel;
    unsigned char mData1;
    unsigned char mData2;

    
};


class MIDI
{

public:

    MIDI();
    ~MIDI();

    void setup( const Config& );

    void checkMessages( std::vector< MIDIMessage >&  );

private:

    static void RtMidiInCallback( double deltatime, std::vector<unsigned char> *message, void *userData );

    MIDIMessage parseRtMidiMessage( std::vector<unsigned char> *message );

    // messages to pass to checkMessages caller
    std::vector< MIDIMessage > mMIDIMessages;
    std::array< MIDIMessage, NUM_WAVES > mPitchBendMessages;
    std::array< MIDIMessage, NUM_WAVES > mFilterMessages;



    std::vector< std::unique_ptr <RtMidiIn> > mInputs;
    std::mutex mMutex;
};



}  // collidsocope } 
