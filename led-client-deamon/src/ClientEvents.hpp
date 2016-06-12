#ifndef __CLIENTEVENTS_H__
#define __CLIENTEVENTS_H__

#include <event2/event_struct.h>
#include <event2/event.h>
#include <event2/event_compat.h>

#include <string>
#include <vector>

#include "EventLoop.hpp"


class SignalEventSource : public EventSource
{
    private:

    public:
        SignalEventSource( uint32_t EventID, std::string eventName );
       ~SignalEventSource();

};

class TimerEventSource : public EventSource
{
    private:

    public:
        TimerEventSource( uint32_t EventID, std::string eventName );
       ~TimerEventSource();

        bool setup();

        virtual void registerEvent();

        virtual void beforeObservers( const int arg1, short int which );
        virtual void afterObservers( const int arg1, short int which );

};

class SoftEventSource :public EventSource
{
    private:
        int eventFD;

    public:
        SoftEventSource( uint32_t EventID, std::string eventName );
       ~SoftEventSource();

        bool setup();

        virtual void registerEvent();

        // Trigger the event
        void trigger();

        // Clear the trigger
        void clear();
};

class SocketEventSource : public EventSource
{
    private:
        struct sockaddr_in sin;    

        int socketFD;

    public:
        SocketEventSource( uint32_t EventID, std::string eventName );
       ~SocketEventSource();

        bool setup();

        int getSocketFD();

        virtual void registerEvent();
        virtual void eventAction( const int arg1, short int which );

};

#endif // __CLIENTEVENTS_H__

