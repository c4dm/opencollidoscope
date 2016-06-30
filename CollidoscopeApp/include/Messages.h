#pragma once


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

/* Messages sent from the audio thread to the graphic wave. 
   This includes the wave chunks when the audio is recorder in the buffer and 
   the cursor position when the grains are reset.
*/
struct RecordWaveMsg
{
    Command cmd;
    std::size_t index;
    float arg1;
    float arg2;
};


inline RecordWaveMsg makeRecordWaveMsg( Command cmd, std::size_t index, float arg1, float arg2 )
{
    RecordWaveMsg msg;
    msg.cmd = cmd;
    msg.index = index;
    msg.arg1 = arg1;
    msg.arg2 = arg2;

    return msg;
}


struct CursorTriggerMsg
{
    Command cmd;
    int synthID;
};

inline CursorTriggerMsg makeCursorTriggerMsg( Command cmd, std::uint8_t synthID )
{
    CursorTriggerMsg msg;

    msg.cmd = cmd;
    msg.synthID = synthID;

    return msg;
}

struct NoteMsg
{
    Command cmd;
    int midiNote;
    double rate;
};

inline NoteMsg makeNoteMsg( Command cmd, int midiNote, double rate )
{
    NoteMsg msg;

    msg.cmd = cmd;
    msg.midiNote = midiNote;
    msg.rate = rate;

    return msg;
}