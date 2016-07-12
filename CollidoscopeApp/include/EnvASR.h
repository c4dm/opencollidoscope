#pragma once 

namespace collidoscope {


/* 
 * An ASR envelope with linear shape. It is modeled after the STK envelope classes.
 * The tick() method advances the computation of the envelope one sample and returns the computed sample
 * The class is templated for the type of the samples that each tick of the envelope produces. 
 *
 * Client classes can set/get the current state of the envelope with the
 * respective getter/setter methods
 *
 */
template <typename T>
class EnvASR
{
public:

    /** Possible states of the envelope. Idle means the envelope ouputs 0 */
    enum class State {
        eAttack,
        eSustain,
        eRelease,
        eIdle // before attack after release  
    };

    EnvASR( T sustainLevel, T attackTime, T releaseTime, std::size_t sampleRate ) :
        mSustainLevel( sustainLevel ),
        mState( State::eIdle ),
        mValue( 0 )
        
    {
        if ( attackTime <= 0 )
            attackTime = T( 0.001 );

        if ( releaseTime <= 0 )
            releaseTime = T( 0.001 );
        
        mAttackRate =  T( 1.0 ) / (attackTime * sampleRate);
        mReleaseRate = T( 1.0 ) / (releaseTime * sampleRate);
    }

    /** Produces one sample worth of envelope */
    T tick()
    {

        switch ( mState )
        {

        case State::eIdle: {
            mValue = 0;
        };
            break;

        case State::eAttack: {
            mValue += mAttackRate;
            if ( mValue >= mSustainLevel ){
                mValue = mSustainLevel;
                mState = State::eSustain;
            }
        };
            break;

        case State::eRelease:
            mValue -= mReleaseRate;
            if ( mValue <= 0 ){
                mValue = 0;
                mState = State::eIdle;
            }
            break;
        default:
            break;
        }

        return mValue;

    }

    State getState() const
    {
        return mState;
    }

    void setState( State state )
    {
        mState = state;
    }

private:
    T mSustainLevel;
    T mAttackRate;
    T mReleaseRate;

    // output
    T mValue;

    State mState;

};


}
