#include <string.h>
#include <unistd.h>
#include <sys/eventfd.h>

#include <iostream>

#include "ClientEvents.hpp"

SignalEventSource::SignalEventSource( uint32_t evID, std::string evName )
: EventSource( evID, evName )
{

}

SignalEventSource::~SignalEventSource()
{

}

TimerEventSource::TimerEventSource( uint32_t evID, std::string evName )
: EventSource( evID, evName )
{

}

TimerEventSource::~TimerEventSource()
{

}

bool 
TimerEventSource::setup()
{

}

void 
TimerEventSource::registerEvent()
{
    struct timeval TIMER_TV = {1, 0};

    // Add the clock event
    evtimer_set( getEventPtr(), EventSource::eventCB, this );
    evtimer_add( getEventPtr(), &TIMER_TV );
}

void 
TimerEventSource::beforeObservers( const int arg1, short int which )
{
#if 0
    struct timeval CLOCK_TV;

    std::cout << "TimerEventSource::beforeObservers" << std::endl;

    if( gettimeofday( &CLOCK_TV, NULL ) ) 
    {
        perror("gettimeofday()");
        event_loopbreak();
    }

    printf( "timer cb: %d, %d\n", CLOCK_TV.tv_sec, CLOCK_TV.tv_usec );
#endif
}

void 
TimerEventSource::afterObservers( const int arg1, short int which )
{
    struct timeval CLOCK_TV;
    struct timeval NEW_TV;
    struct timeval TIMER_TV = {1, 0};

    std::cout << "TimerEventSource::afterObservers" << std::endl;

    // Get the current time.
    if( gettimeofday( &CLOCK_TV, NULL ) ) 
    {
        perror("gettimeofday()");
        event_loopbreak();
    }

    printf( "timer cb: %d, %d\n", CLOCK_TV.tv_sec, CLOCK_TV.tv_usec );

    // Calculate the time to the next 100ms boundary
    NEW_TV.tv_usec = ( ( CLOCK_TV.tv_usec / 100000 ) * 100000 ) + 100000;
    NEW_TV.tv_sec  = CLOCK_TV.tv_sec;

    if( NEW_TV.tv_usec >= 1000000 )
    {
        NEW_TV.tv_usec -= 1000000;
        NEW_TV.tv_sec  += 1;
    }

    printf( "new time: %d, %d\n", NEW_TV.tv_sec, NEW_TV.tv_usec );

    if( NEW_TV.tv_usec == 0 )
    {
        TIMER_TV.tv_sec  = NEW_TV.tv_sec  - CLOCK_TV.tv_sec - 1;
        TIMER_TV.tv_usec = 1000000 - CLOCK_TV.tv_usec;
    }
    else
    {
        TIMER_TV.tv_sec  = NEW_TV.tv_sec  - CLOCK_TV.tv_sec;
        TIMER_TV.tv_usec = NEW_TV.tv_usec - CLOCK_TV.tv_usec;
    }

    printf( "new inc: %d, %d\n", TIMER_TV.tv_sec, TIMER_TV.tv_usec );

    // Wake up again in a little bit.
    evtimer_add( getEventPtr(), &TIMER_TV );
}

SoftEventSource::SoftEventSource( uint32_t evID, std::string evName )
: EventSource( evID, evName )
{

}

SoftEventSource::~SoftEventSource()
{

}

bool 
SoftEventSource::setup()
{
    eventFD = eventfd( 0, 0 );

    if( eventFD == -1 )
    {
        perror("eventfd()");
        return true;
    }

    return false;
}

void 
SoftEventSource::registerEvent()
{
	// Add the event file descriptor
    event_set( getEventPtr(), eventFD, EV_READ|EV_PERSIST, EventSource::eventCB, this );
    event_add( getEventPtr(), 0 );
}

void 
SoftEventSource::trigger()
{
    uint64_t value = 1;

    write( eventFD, &value, 8);
}

void 
SoftEventSource::clear()
{
    uint64_t value;

    read( eventFD, &value, 8 );
}

SocketEventSource::SocketEventSource( uint32_t evID, std::string evName )
: EventSource( evID, evName )
{

}

SocketEventSource::~SocketEventSource()
{

}

bool 
SocketEventSource::setup()
{
    int result;
    static int so_reuseaddr = 1;
    static int so_broadcast = 1;

    socketFD = socket( AF_INET, SOCK_DGRAM, 0 );

    memset( &sin, 0, sizeof(sin) );
    sin.sin_family      = AF_INET;
    sin.sin_addr.s_addr = INADDR_BROADCAST;
    sin.sin_port        = htons( 10260 );
	
    // Allow multiple bind to the broadcast address
    result = setsockopt( socketFD, SOL_SOCKET, SO_REUSEADDR, &so_reuseaddr, sizeof so_reuseaddr );

    // Set the socket for broadcast traffic
    result = setsockopt( socketFD, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof so_broadcast );

    // Bind to a local address
    if( bind( socketFD, (struct sockaddr *) &(sin), sizeof(sin) ) ) 
    {
        perror("bind()");
        return true;
    }

    return false;
#if 0
    int z;
    int x;
    struct sockaddr_in adr;  /* AF_INET */
    int len_inet;            /* length */
    int s;                   /* Socket */
    char dgram[512];         /* Recv buffer */
    static int so_reuseaddr = TRUE;
    static char
    *bc_addr = "127.255.255.2:9097";

    /*
     * Use a server address from the command
     * line, if one has been provided.
     * Otherwise, this program will default
     * to using the arbitrary address
     * 127.0.0.:
     */
    if( argc > 1 )
    /* Broadcast address: */
       bc_addr = argv[1];

    /*
     * Create a UDP socket to use:
     */
    s = socket( AF_INET, SOCK_DGRAM, 0 );
    if( s == -1 )
    {
        displayError("socket()");
    }

    /*
     * Form the broadcast address:
     */
    len_inet = sizeof adr;

    z = mkaddr( &adr, &len_inet, bc_addr, "udp" );

    if( z == -1 )
       displayError("Bad broadcast address");

    /*
     * Allow multiple listeners on the
     * broadcast address:
     */
    z = setsockopt( s, SOL_SOCKET, SO_REUSEADDR, &so_reuseaddr, sizeof so_reuseaddr );

    if( z == -1 )
        displayError("setsockopt(SO_REUSEADDR)");

    /*
     * Bind our socket to the broadcast address:
     */
    z = bind( s, (struct sockaddr *)&adr, len_inet );

    if( z == -1 )
        displayError( "bind(2)" );

    for(;;)
    {
        /*
         * Wait for a broadcast message:
         */
        z = recvfrom( s,      /* Socket */
                      dgram,  /* Receiving buffer */
                      sizeof dgram,/* Max rcv buf size */
                      0,      /* Flags: no options */
                      (struct sockaddr *)&adr, /* Addr */
                      &x);    /* Addr len, in & out */

        if( z < 0 )
            displayError( "recvfrom(2)" ); /* else err */

        fwrite( dgram, z, 1, stdout );
        putchar( '\n' );

        fflush( stdout );
    }

    return 0;
#endif
}

void 
SocketEventSource::registerEvent()
{
	// Add the UDP event
    event_set( getEventPtr(), socketFD, EV_READ|EV_PERSIST, EventSource::eventCB, this );
    event_add( getEventPtr(), 0 );
}

void 
SocketEventSource::eventAction( const int arg1, short int which )
{
    int                bytesRead;
    uint8_t            pktBuf[2048];
    struct sockaddr_in addr; 
    unsigned int       addrLen;

    std::cout << "SocketEventSource::eventAction" << std::endl;

    bytesRead = recvfrom( socketFD, pktBuf, sizeof( pktBuf ), 0, (struct sockaddr *) &addr, &addrLen );

    std::cout << "SocketEventSource::eventAction - byteRead: " << bytesRead << std::endl;

}
