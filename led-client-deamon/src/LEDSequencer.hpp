#ifndef __LEDSEQUENCER_H__
#define __LEDSEQUENCER_H__

#include "ledDriver.hpp"
#include "EventLoop.hpp"
#include "ClientEvents.hpp"

class LEDSequencer : public EventNotify
{
    private:

        LEDDriver         *leds;

        TimerEventSource  timer;

    public:
        LEDSequencer();
       ~LEDSequencer();

        void setDriver( LEDDriver *driver );

        void start( EventLoop &loop );
        void stop();

        virtual void eventAction( uint32_t EventID );
};

#endif // __LEDSEQUENCER_H__

