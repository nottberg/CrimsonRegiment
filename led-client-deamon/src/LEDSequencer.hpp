#ifndef __LEDSEQUENCER_H__
#define __LEDSEQUENCER_H__

#include "ledDriver.hpp"
#include "EventLoop.hpp"
#include "ClientEvents.hpp"
#include "CRLEDPacket.hpp"

// The type of operation for this step.
typedef enum LEDStepTypeEnum
{
    LS_STEP_TYPE_SET_AT_TIME,
}LS_STEP_TYPE_T;

// Result value from init and update
typedef enum LEDStepUpdateResultEnum
{
    LS_STEP_UPDATE_RESULT_DONE,
    LS_STEP_UPDATE_RESULT_CONT
}LS_STEP_UPDATE_RESULT_T;

class LEDSequenceStep
{
    private:

    public:
        LEDSequenceStep();
       ~LEDSequenceStep();

        virtual LS_STEP_UPDATE_RESULT_T init( CRLEDCommandPacket *cmdPkt );

        virtual LS_STEP_UPDATE_RESULT_T update( struct timeval *curTime, LEDDriver *leds );
};

class LDStepWaitForStart : public LEDSequenceStep
{
    private:
        struct timeval startTime;

    public:
        LDStepWaitForStart();
       ~LDStepWaitForStart();

        virtual LS_STEP_UPDATE_RESULT_T init( CRLEDCommandPacket *cmdPkt );

        virtual LS_STEP_UPDATE_RESULT_T update( struct timeval *curTime, LEDDriver *leds );
};

class LDStepDelay : public LEDSequenceStep
{
    private:

    public:
        LDStepDelay();
       ~LDStepDelay();

        virtual LS_STEP_UPDATE_RESULT_T init( CRLEDCommandPacket *cmdPkt );

        virtual LS_STEP_UPDATE_RESULT_T update( struct timeval *curTime, LEDDriver *leds );
};

class LDStepRegionOn : public LEDSequenceStep
{
    private:

    public:
        LDStepRegionOn();
       ~LDStepRegionOn();

        virtual LS_STEP_UPDATE_RESULT_T init( CRLEDCommandPacket *cmdPkt );

        virtual LS_STEP_UPDATE_RESULT_T update( struct timeval *curTime, LEDDriver *leds );
};

#define LS_STEP_NOT_ACTIVE  ((uint32_t) -1)

typedef enum LEDSequenceUpdateResultEnum
{
    LS_SEQ_UPDATE_RESULT_DONE,
    LS_SEQ_UPDATE_RESULT_CONT
}LS_SEQ_UPDATE_RESULT_T;

class LEDSequence
{
    private:
        std::vector< LEDSequenceStep > stepList;

        uint32_t activeStep;

    public:
        LEDSequence();
       ~LEDSequence();

        // Clear all of the steps for this sequence
        void clearDefinition();

        // Append a step to the sequence
        void appendDefinitionStep( LEDSequenceStep &step );

        LS_SEQ_UPDATE_RESULT_T startSequence( struct timeval *curTime, LEDDriver *leds, CRLEDCommandPacket *cmdPkt );

        LS_SEQ_UPDATE_RESULT_T updateStep( struct timeval *curTime, LEDDriver *leds );
};

#define LS_SEQ_NOT_ACTIVE  ((uint32_t) -1)

class LEDSequencer : public EventNotify
{
    private:

        LEDDriver         *leds;

        TimerEventSource  timer;

        std::vector< LEDSequence > sequenceArray;

        uint32_t activeSeqNum;

    public:
        LEDSequencer();
       ~LEDSequencer();

        void setDriver( LEDDriver *driver );

        void start( EventLoop &loop );
        void stop();

        void startSequence( uint32_t seqNumber, CRLEDCommandPacket *cmdPkt );
        void clearSequence();

        void clearSequenceDefinition( uint32_t seqNumber );
        void appendToSequenceDefinition( uint32_t seqNumber, LEDSequenceStep &step );

        virtual void eventAction( uint32_t EventID );
};

#endif // __LEDSEQUENCER_H__

