#ifndef __CLIENTMAIN_H__
#define __CLIENTMAIN_H__

#include "CRLEDClientConfigFile.hpp"
#include "CRLEDSequenceFile.hpp"
#include "EventLoop.hpp"
#include "ClientEvents.hpp"
#include "ledDriver.hpp"
#include "LEDSequencer.hpp"

class ClientMain : public EventNotify
{
    private:
        CRLEDClientConfigFile config;
        CRLEDSequenceFile     seqConfig;

        EventLoop         loop;

        LEDDriver         driver;
        LEDSequencer      sequencer;

        SocketEventSource eventSock;

    public:
        ClientMain();
       ~ClientMain();

        bool setup();

        void run();

        virtual void eventAction( uint32_t eventID );
};

#endif // __CLIENTMAIN_H__

