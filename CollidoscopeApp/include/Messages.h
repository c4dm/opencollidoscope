#pragma once

/**
 * Enumeration of all the possible commands exchanged between audio thread and graphic thread.
 *
 */ 
enum class Command {
    // message carrying info about one chunk of recorder audio. 
    WAVE_CHUNK,
    // message sent when a new recording starts. The gui resets the wave upon receiving it. 
    WAVE_START,

    TRIGGER_UPDATE,
    TRIGGER_END,

    NOTE_ON,
    NOTE_OFF,

    LOOP_ON,
    LOOP_OFF
};

/** Message sent from the audio thread to the graphic wave when a new wave is recorded. 
 *  
 *  The graphic thread set the chunks of the wave to reflect the level of the recorded audio. 
 *  The algorithm takes the maximum and minimum value of a group of samples and this becomes the top and bottom of the samples.
 *  It contains the inde
 *  the cursor position when the grains are reset.
 */
struct RecordWaveMsg
{
    Command cmd; // WAVE_CHUNK or WAVE_START
    std::size_t index;
    float arg1;
    float arg2;
};

/**
 * Utility function to create a new RecordWaveMsg.
 */ 
inline RecordWaveMsg makeRecordWaveMsg( Command cmd, std::size_t index, float arg1, float arg2 )
{
    RecordWaveMsg msg;
    msg.cmd = cmd;
    msg.index = index;
    msg.arg1 = arg1;
    msg.arg2 = arg2;

    return msg;
}

/**
 * Message sent from the audio thread to the graphic thread when a new grain is triggered in the granular synthesizer. 
 * This creates a new cursor that travels from the beginning to the end of the selection to graphically represent the evolution of the grain in time. 
 *
 */ 
struct CursorTriggerMsg
{
    Command cmd; // TRIGGER_UPDATE or TRIGGER_END
    int synthID;
};

/**
 * Utility function to create a new CursorTriggerMsg.
 */ 
inline CursorTriggerMsg makeCursorTriggerMsg( Command cmd, std::uint8_t synthID )
{
    CursorTriggerMsg msg;

    msg.cmd = cmd;
    msg.synthID = synthID;

    return msg;
}

/**
 * Message sent from the graphic (main) thread to the audio thread to start a new voice of the granular synthesizer.
 */ 
struct NoteMsg
{
    Command cmd; // NOTE_ON/OFF ot LOOP_ON/OFF 
    int midiNote;
    double rate;
};

/**
 * Utility function to create a new NoteMsg.
 */ 
inline NoteMsg makeNoteMsg( Command cmd, int midiNote, double rate )
{
    NoteMsg msg;

    msg.cmd = cmd;
    msg.midiNote = midiNote;
    msg.rate = rate;

    return msg;
}
