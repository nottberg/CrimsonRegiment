#include <iostream>

#include "LEDSequencer.hpp"

LEDSequencer::LEDSequencer()
:timer( 1, "LEDSequencerTimer" )
{
    leds = NULL;
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
LEDSequencer::eventAction( uint32_t EventID )
{
    struct timeval CLOCK_TV;

    std::cout << "LEDSequencer::eventAction" << std::endl;

    if( leds == NULL )
        return;
#if 0
    // Immediately apply any update if necessary so
    // the we get it as close to the 100ms boundary
    // as possible.
    leds->performUpdate();

    // Get a timestamp for the sequencer to key off of if necessary
    if( gettimeofday( &CLOCK_TV, NULL ) ) 
    {
        perror("gettimeofday()");
        event_loopbreak();
    }

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

