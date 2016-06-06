#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include <event2/event_struct.h>
#include <event2/event.h>
#include <event2/event_compat.h>

#include <string>
#include <vector>

// Callback for events
class EventNotify
{
    private:

    public:
        virtual void eventAction( uint32_t EventID ) = 0;
};

class EventSource
{
    private:
        uint32_t    eventID;
        std::string eventName;

        struct event eventData;

        std::vector< EventNotify* > observerList;

    protected:
        static void eventCB( const int arg1, short int which, void *userp );

    public:
        EventSource( uint32_t EventID, std::string eventName );
       ~EventSource();

        struct event *getEventPtr();

        virtual void registerEvent();

        virtual void beforeObservers( const int arg1, short int which );
        virtual void afterObservers( const int arg1, short int which );

        void addObserver( EventNotify *observer );
};

class EventLoop
{
    private:
        std::vector< EventSource* > srcList;

    public:
        EventLoop();
       ~EventLoop();

        void addSource( EventSource *srcObj );

        void start();
        void stop();

};

#endif // __EVENTLOOP_H__

