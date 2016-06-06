#include <string.h>
#include <sys/eventfd.h>

#include <iostream>

#include "EventLoop.hpp"

EventSource::EventSource( uint32_t evID, std::string evName )
{
    eventID   = evID;
    eventName = evName;
}

EventSource::~EventSource()
{

}

void 
EventSource::eventCB( const int arg1, short int which, void *userp )
{
    EventSource *evObj = (EventSource *) userp;

    evObj->beforeObservers( arg1, which );

    for( std::vector< EventNotify* >::iterator it = evObj->observerList.begin(); it != evObj->observerList.end(); it++ )
    {
        (*it)->eventAction( evObj->eventID );
    }

    evObj->afterObservers( arg1, which );
}

struct event *
EventSource::getEventPtr()
{
    return &eventData;
}

void 
EventSource::registerEvent()
{

}

void 
EventSource::beforeObservers( const int arg1, short int which )
{
    std::cout << "EventSource::beforeObservers" << std::endl;
}

void
EventSource::afterObservers( const int arg1, short int which )
{
    std::cout << "EventSource::afterObservers" << std::endl;
}

void 
EventSource::addObserver( EventNotify *observer )
{
    observerList.push_back( observer );
}

EventLoop::EventLoop()
{


}

EventLoop::~EventLoop()
{


}

void 
EventLoop::addSource( EventSource *srcObj )
{
    srcList.push_back( srcObj );
}

void
EventLoop::start()
{
    // Go through all of the event sources
    // calling register for each.
    for( std::vector< EventSource* >::iterator it = srcList.begin(); it != srcList.end(); it++ )
    {
        (*it)->registerEvent();
    }

	// Enter the event loop; does not return.
    event_dispatch();
}

void
EventLoop::stop()
{

}


