#include <iostream>

#include "LEDSequencer.hpp"

#if 0
LEDSequenceStep::LEDSequenceStep( CRLSeqStep *configuration )
{

}

LEDSequenceStep::~LEDSequenceStep()
{

}

LS_STEP_UPDATE_RESULT_T 
LEDSequenceStep::init( CRLEDCommandPacket *cmdPkt )
{
    return LS_STEP_UPDATE_RESULT_DONE;
}

LS_STEP_UPDATE_RESULT_T 
LEDSequenceStep::update( struct timeval *curTime, LEDDriver *leds )
{
    std::cout << "LEDSequenceStep::update" << std::endl;

    return LS_STEP_UPDATE_RESULT_DONE;
}

LDStepWaitForStart::LDStepWaitForStart()
: LEDSequenceStep( NULL )
{

}

LDStepWaitForStart::~LDStepWaitForStart()
{

}

LS_STEP_UPDATE_RESULT_T 
LDStepWaitForStart::init( CRLEDCommandPacket *cmdPkt )
{
    startTime.tv_sec  = cmdPkt->getTSSec();
    startTime.tv_usec = cmdPkt->getTSUSec();
}

LS_STEP_UPDATE_RESULT_T 
LDStepWaitForStart::update( struct timeval *curTime, LEDDriver *leds )
{
    std::cout << "LDStepWaitForStart::update" << std::endl;
    std::cout << curTime->tv_sec << ":" << startTime.tv_sec << std::endl;
    std::cout << curTime->tv_usec << ":" << startTime.tv_usec << std::endl;

    // If we are already behind by a full sec, then trigger
    if( curTime->tv_sec > startTime.tv_sec )
        return LS_STEP_UPDATE_RESULT_DONE;

    // If the seconds is the same, check the usec instead.
    if( curTime->tv_sec == startTime.tv_sec )
    {    
        if( curTime->tv_usec >= startTime.tv_usec )
            return LS_STEP_UPDATE_RESULT_DONE;
    }

    // Otherwise keep waiting.
    return LS_STEP_UPDATE_RESULT_CONT;
}

LDStepDelay::LDStepDelay( CRLSeqStep *configuration )
: LEDSequenceStep( configuration )
{

}

LDStepDelay::~LDStepDelay()
{

}

LS_STEP_UPDATE_RESULT_T 
LDStepDelay::init( CRLEDCommandPacket *cmdPkt )
{

}

LS_STEP_UPDATE_RESULT_T 
LDStepDelay::update( struct timeval *curTime, LEDDriver *leds )
{

}

LDStepRegionChange::LDStepRegionChange( CRLSeqStep *configuration )
: LEDSequenceStep( configuration )
{
    switchOn    = false;

    startIndex  = 0;
    endIndex    = -1;

    color.red   = 255;
    color.green = 255;
    color.blue  = 255;
}

LDStepRegionChange::~LDStepRegionChange()
{

}

void 
LDStepRegionChange::setOnOff( bool value )
{
    switchOn = value;
}

void 
LDStepRegionChange::setStartIndex( uint32_t value )
{
    startIndex = value;
}

void 
LDStepRegionChange::setEndIndex( uint32_t value )
{
    endIndex = value;
}

void 
LDStepRegionChange::setBounds( uint32_t start, uint32_t end )
{
    startIndex = start;
    endIndex   = end;
}

void 
LDStepRegionChange::setMaxBounds()
{
    startIndex = 0;
    endIndex   = -1;
}

LS_STEP_UPDATE_RESULT_T 
LDStepRegionChange::init( CRLEDCommandPacket *cmdPkt )
{

}

LS_STEP_UPDATE_RESULT_T 
LDStepRegionChange::update( struct timeval *curTime, LEDDriver *leds )
{
    uint32_t lastIndex = endIndex;

    std::cout << "LDStepRegionChange::update" << std::endl;

    if( lastIndex >= leds->getPixelCount() )
        lastIndex = ( leds->getPixelCount() - 1 );

    std::cout << startIndex << "  " << lastIndex << std::endl;

    for( int x = startIndex; x < lastIndex; x++ )
    {
        if( switchOn == true )
        { 
            leds->setPixel( x, color.red, color.green, color.blue );
        }
        else
        {
            leds->clearPixel( x );
        }
    }

    return LS_STEP_UPDATE_RESULT_DONE;

#if 0
    // Temporary code to exercise things
    if( (CLOCK_TV.tv_sec & 0x1) )
    {
        leds->setPixel( 0, 255, 255, 255 );
        leds->setPixel( 1, 0, 0, 0 );
        leds->setPixel( 2, 255, 255, 255 );
        leds->setPixel( 3, 255, 0, 0 );
        leds->setPixel( 4, 0, 255, 0 );
        leds->setPixel( 5, 0, 0, 255 );
    }
    else
    {
        leds->setPixel( 0, 0, 0, 0 );
        leds->setPixel( 1, 255, 255, 255 );
        leds->setPixel( 2, 0, 0, 0 );
        leds->setPixel( 3, 0, 0, 0 );
        leds->setPixel( 4, 0, 0, 0 );
        leds->setPixel( 5, 0, 0, 0 );
    }
#endif
}

LEDSequence::LEDSequence()
{
    activeStep = LS_STEP_NOT_ACTIVE;
}

LEDSequence::~LEDSequence()
{

}

void 
LEDSequence::clearDefinition()
{
    stepList.clear();
}

void 
LEDSequence::appendDefinitionStep( LEDSequenceStep *step )
{
   stepList.push_back( step );
}

LS_SEQ_UPDATE_RESULT_T 
LEDSequence::startSequence( struct timeval *curTime, LEDDriver *leds, CRLEDCommandPacket *cmdPkt )
{
    std::cout << "LEDSequence::startSequence" << std::endl;

    // Back to the beginning
    activeStep = 0;

    // Initialize all of the steps
    for( std::vector< LEDSequenceStep* >::iterator it = stepList.begin(); it != stepList.end(); it++ )
    {
        (*it)->init( cmdPkt );
    }

    // Run an update step in case we are behind
    return updateStep( curTime, leds );
}

LS_SEQ_UPDATE_RESULT_T 
LEDSequence::updateStep( struct timeval *curTime, LEDDriver *leds )
{
    // Roll through steps until we are told to wait.
    while( (activeStep != LS_STEP_NOT_ACTIVE) && (activeStep < stepList.size() ) )
    {
        std::cout << "LEDSequence::updateStep" << std::endl;

        // Execute the current step and see what it indicates to do.
        switch( stepList[ activeStep ]->update( curTime, leds ) )
        {
            // Done with the current step, move to the next one.
            case LS_STEP_UPDATE_RESULT_DONE:
            {
                activeStep += 1;

                std::cout << activeStep << "  " << stepList.size() << std::endl;

                if( activeStep >= stepList.size() )
                    activeStep = LS_STEP_NOT_ACTIVE;
            }
            break;

            // Continue with the current step at next time boundary.
            case LS_STEP_UPDATE_RESULT_CONT:
                return LS_SEQ_UPDATE_RESULT_CONT;
            break;
        }
    }

    // The sequence is complete.
    return LS_SEQ_UPDATE_RESULT_DONE;
}
#endif

LEDSequencer::LEDSequencer()
:timer( 1, "LEDSequencerTimer" )
{
    leds         = NULL;
    cfgNode      = NULL;
}

LEDSequencer::~LEDSequencer()
{

}

void 
LEDSequencer::setDriver( LEDDriver *driver )
{
    leds = driver;
}

void 
LEDSequencer::setNodeConfig( CRLSeqNode *value )
{
    cfgNode = value;
}

void 
LEDSequencer::start( EventLoop &loop )
{
    timer.addObserver( this );

    timer.setup();

    loop.addSource( &timer );
}

void 
LEDSequencer::stop()
{

}

void 
LEDSequencer::startSequence( uint32_t seqNumber, CRLEDCommandPacket *cmdPkt )
{
    struct timeval curTime;

    std::cout << "LEDSequencer::startSequence" << std::endl;

    if( leds == NULL )
        return;

    if( cfgNode == NULL )
        return;

    // Get a timestamp for the sequencer to key off of if necessary
    if( gettimeofday( &curTime, NULL ) ) 
    {
        perror("gettimeofday()");
    }

    cfgNode->activateSequence( seqNumber, cmdPkt, &curTime, leds );
}

void 
LEDSequencer::clearSequence()
{
    std::cout << "LEDSequencer::clearSequence" << std::endl;

    if( cfgNode != NULL )
    {
        cfgNode->clearActiveSequence();
    }

    if( leds != NULL )
    {
        leds->clearAllPixels();
    }
}

void 
LEDSequencer::eventAction( uint32_t EventID )
{
    struct timeval CLOCK_TV;

    //std::cout << "LEDSequencer::eventAction" << std::endl;

    if( leds == NULL )
        return;

    // Immediately apply any update if necessary so
    // the we get it as close to the 100ms boundary
    // as possible.
    leds->processUpdates();

    if( cfgNode == NULL )
        return;

    // Get a timestamp for the sequencer to key off of if necessary
    if( gettimeofday( &CLOCK_TV, NULL ) ) 
    {
        perror("gettimeofday()");
        event_loopbreak();
    }

    // Perform sequence updates
    cfgNode->updateSequence( &CLOCK_TV, leds );
}


