#ifndef __CLIENTMAIN_H__
#define __CLIENTMAIN_H__

#include "EventLoop.hpp"
#include "ClientEvents.hpp"
#include "ledDriver.hpp"
#include "LEDSequencer.hpp"

class ClientMain
{
    private:
        EventLoop        loop;

        LEDDriver        driver;
        LEDSequencer     sequencer;

    public:
        ClientMain();
       ~ClientMain();

        bool setup();

        void run();

};

#endif // __CLIENTMAIN_H__

