#ifndef __CRLED_SEQUENCE_FILE_HPP__
#define __CRLED_SEQUENCE_FILE_HPP__

#include <stdint.h>
#include <sys/types.h>  
#include <unistd.h>
#include <strings.h>
#include <arpa/inet.h>

#include <vector>
#include <string>
#include <map>

#include "ledDriver.hpp"
#include "CRLEDPacket.hpp"

// Result value from init and update
typedef enum LEDStepUpdateResultEnum
{
    LS_STEP_UPDATE_RESULT_DONE,
    LS_STEP_UPDATE_RESULT_CONT
}LS_STEP_UPDATE_RESULT_T;

class CRLSeqStep
{
    private:

    public:
        CRLSeqStep();
       ~CRLSeqStep();

        virtual bool initFromStepNode( void *stepPtr ); 

        virtual LS_STEP_UPDATE_RESULT_T initRT( CRLEDCommandPacket *cmdPkt, LEDDriver *leds );

        virtual LS_STEP_UPDATE_RESULT_T updateRT( struct timeval *curTime, LEDDriver *leds );
};

class CRLSSWaitForStart : public CRLSeqStep
{
    private:
        struct timeval startTime;

    public:
        CRLSSWaitForStart();
       ~CRLSSWaitForStart();

        virtual bool initFromStepNode( void *stepPtr ); 

        virtual LS_STEP_UPDATE_RESULT_T initRT( CRLEDCommandPacket *cmdPkt, LEDDriver *leds );

        virtual LS_STEP_UPDATE_RESULT_T updateRT( struct timeval *curTime, LEDDriver *leds );
};

// Clear the string of LEDs
class CRLSSClear : public CRLSeqStep
{
    private:

    public:
        CRLSSClear();
       ~CRLSSClear();

        virtual bool initFromStepNode( void *stepPtr ); 

        virtual LS_STEP_UPDATE_RESULT_T initRT( CRLEDCommandPacket *cmdPkt, LEDDriver *leds );

        virtual LS_STEP_UPDATE_RESULT_T updateRT( struct timeval *curTime, LEDDriver *leds );

};

typedef enum CRLSSRegionChangeActionENUM
{
    SSRC_ACTION_NOT_SET,
    SSRC_ACTION_ON,
    SSRC_ACTION_OFF
}SSRC_ACTION_T;

typedef struct CRLSSRegionChangeRangeStruct
{
    uint32_t startIndex;
    uint32_t endIndex;
    uint8_t  rsvd;
    uint8_t  red;
    uint8_t  green;
    uint8_t  blue;
}SSRC_RANGE_T;

// Change a region of LEDs
class CRLSSRegionChange : public CRLSeqStep
{
    private:
        SSRC_ACTION_T action;

        std::vector< SSRC_RANGE_T > rangeList;

        bool parseRangeEntry( void *rangeNode );

    public:
        CRLSSRegionChange();
       ~CRLSSRegionChange();

        virtual bool initFromStepNode( void *stepPtr ); 

        virtual LS_STEP_UPDATE_RESULT_T initRT( CRLEDCommandPacket *cmdPkt, LEDDriver *leds );

        virtual LS_STEP_UPDATE_RESULT_T updateRT( struct timeval *curTime, LEDDriver *leds );

};

// Record a linear fill region
class CRLSSLinearFillRegion
{
    private:
        uint8_t red;
        uint8_t green;
        uint8_t blue;

        uint32_t startIndex;
        int32_t  increment;
        uint32_t iterations;
        uint32_t delay;

        uint32_t curiter;
        uint32_t nextIndx;
        struct timeval nextTime;

    public:
        CRLSSLinearFillRegion();
       ~CRLSSLinearFillRegion();

        virtual bool initFromStepNode( void *stepPtr ); 

        LS_STEP_UPDATE_RESULT_T initRT( CRLEDCommandPacket *cmdPkt );

        LS_STEP_UPDATE_RESULT_T updateRT( struct timeval *curTime, LEDDriver *leds );
};

// Do a timed fill of a region
class CRLSSLinearFill : public CRLSeqStep
{
    private:
        std::vector< CRLSSLinearFillRegion > regionList;

    public:
        CRLSSLinearFill();
       ~CRLSSLinearFill();

        virtual bool initFromStepNode( void *stepPtr ); 

        virtual LS_STEP_UPDATE_RESULT_T initRT( CRLEDCommandPacket *cmdPkt, LEDDriver *leds );

        virtual LS_STEP_UPDATE_RESULT_T updateRT( struct timeval *curTime, LEDDriver *leds );

};

// Dwell until a point in time
class CRLSSDwell : public CRLSeqStep
{
    private:
        uint32_t       delayMS;

        bool           startFlag;
        struct timeval endTime;

    public:
        CRLSSDwell();
       ~CRLSSDwell();

        virtual bool initFromStepNode( void *stepPtr ); 

        virtual LS_STEP_UPDATE_RESULT_T initRT( CRLEDCommandPacket *cmdPkt, LEDDriver *leds );

        virtual LS_STEP_UPDATE_RESULT_T updateRT( struct timeval *curTime, LEDDriver *leds );

};

// Return to a previous step
class CRLSSGoto : public CRLSeqStep
{
    private:

    public:
        CRLSSGoto();
       ~CRLSSGoto();

        virtual bool initFromStepNode( void *stepPtr ); 

        virtual LS_STEP_UPDATE_RESULT_T initRT( CRLEDCommandPacket *cmdPkt, LEDDriver *leds );

        virtual LS_STEP_UPDATE_RESULT_T updateRT( struct timeval *curTime, LEDDriver *leds );

};

// Perform a transform on existing values
class CRLSSTransform : public CRLSeqStep
{
    private:

    public:
        CRLSSTransform();
       ~CRLSSTransform();

        virtual bool initFromStepNode( void *stepPtr ); 

        virtual LS_STEP_UPDATE_RESULT_T initRT( CRLEDCommandPacket *cmdPkt, LEDDriver *leds );

        virtual LS_STEP_UPDATE_RESULT_T updateRT( struct timeval *curTime, LEDDriver *leds );

};

typedef enum CRLSSSparklePixelStateEnum{
    SPARKLE_PIXEL_STATE_NA,
    SPARKLE_PIXEL_STATE_INIT,
    SPARKLE_PIXEL_STATE_ON,
    SPARKLE_PIXEL_STATE_RAMP_UP,
    SPARKLE_PIXEL_STATE_RAMP_DOWN,
    SPARKLE_PIXEL_STATE_OFF
}SPARKLE_PIXEL_STATE_T;

// SparklePixel
class CRLSSSparklePixel
{
    public:
        SPARKLE_PIXEL_STATE_T state;

        uint32_t onTime;
        uint32_t offTime;
        uint32_t onRampTime;
        uint32_t offRampTime;

        uint8_t pvalue;

        struct timeval nextTime;

        CRLSSSparklePixel();
       ~CRLSSSparklePixel();

};

// Make things sparkle
class CRLSSSparkle : public CRLSeqStep
{
    private:
        std::vector< CRLSSSparklePixel > pixelList;

        bool checkTime( struct timeval *curTime, struct timeval *targetTime );
        void updateTime( struct timeval *curTime, struct timeval *nextTime, uint32_t deltaMS );

    public:
        CRLSSSparkle();
       ~CRLSSSparkle();

        virtual bool initFromStepNode( void *stepPtr ); 

        virtual LS_STEP_UPDATE_RESULT_T initRT( CRLEDCommandPacket *cmdPkt, LEDDriver *leds );

        virtual LS_STEP_UPDATE_RESULT_T updateRT( struct timeval *curTime, LEDDriver *leds );

};

// DecayPixel
class CRLSSDecayPixel
{
    public:
        bool shouldDecay;
        bool ramping;

        struct timeval nextTime;

        uint32_t decayDelay;
        uint32_t decayRamp;

    CRLSSDecayPixel();
   ~CRLSSDecayPixel();
};

// Decay a region
class CRLSSDecay : public CRLSeqStep
{
    private:
        std::vector< CRLSSDecayPixel > pixelList;

        bool checkTime( struct timeval *curTime, struct timeval *targetTime );
        void updateTime( struct timeval *curTime, struct timeval *nextTime, uint32_t deltaMS );

    public:
        CRLSSDecay();
       ~CRLSSDecay();

        virtual bool initFromStepNode( void *stepPtr ); 

        virtual LS_STEP_UPDATE_RESULT_T initRT( CRLEDCommandPacket *cmdPkt, LEDDriver *leds );

        virtual LS_STEP_UPDATE_RESULT_T updateRT( struct timeval *curTime, LEDDriver *leds );

};



class CRLStepFactory
{
    private:

    public:

        static CRLSeqStep* allocateStepForType( std::string type );
        static void freeStep( CRLSeqStep *stepPtr );
};

#define LS_STEP_NOT_ACTIVE  ((uint32_t) -1)

typedef enum LEDSequenceUpdateResultEnum
{
    LS_SEQ_UPDATE_RESULT_DONE,
    LS_SEQ_UPDATE_RESULT_CONT
}LS_SEQ_UPDATE_RESULT_T;

class CRLSeqRecord
{
    private:
        std::vector< CRLSeqStep* > stepList;

        uint32_t activeStep;

    public:
        CRLSeqRecord();
       ~CRLSeqRecord();

        void initialize();

        void appendStep( CRLSeqStep *stepObj );

        LS_SEQ_UPDATE_RESULT_T activateRT( CRLEDCommandPacket *cmdPkt, struct timeval *curTime, LEDDriver *leds );

        LS_SEQ_UPDATE_RESULT_T updateRT( struct timeval *curTime, LEDDriver *leds );
};

#define LS_SEQ_NOT_ACTIVE  ((uint32_t) -1)

class CRLSeqNode
{
    private:
        std::string nodeID;

        uint32_t maxSeqIndx;
        std::vector< CRLSeqRecord > seqList;

        uint32_t activeSeqIndx;

    public:
        CRLSeqNode();
       ~CRLSeqNode();

        void setID( std::string value );
        std::string getID();

        void setMaximumSequenceIndex( uint32_t value );
        uint32_t getMaximumSequenceIndex();

        void newSequence( uint32_t seqIndx );
        void appendStepToSequence( uint32_t seqIndx, CRLSeqStep *stepObj );

        void activateSequence( uint32_t seqIndx, CRLEDCommandPacket *cmdPkt, struct timeval *curTime, LEDDriver *leds );
        void clearActiveSequence();
        void updateSequence( struct timeval *curTime, LEDDriver *leds );
};

class CRLEDSequenceFile
{
    private:
        std::map< std::string, CRLSeqNode> nodeMap;

        bool parseSequence( void *seqPtr, CRLSeqNode &node );
        bool parseNode( void *nodePtr, CRLSeqNode &node );
        bool parseNodeList( void *listPtr, std::string nodeID );

    public:
        CRLEDSequenceFile();
       ~CRLEDSequenceFile();

        void setSequenceFilePath();

        bool load( std::string nodeID );

        CRLSeqNode *getNodeConfig( std::string id );

};

#endif // __CRLED_SEQUENCE_FILE_HPP__

