#include <event.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

static struct timeval CLOCK_TV;

static struct timeval TIMER_TV = {1, 0};

static void gettimeofday_cb( int nothing, short int which, void *ev )
{
    if( gettimeofday( &CLOCK_TV, NULL ) ) 
    {
        perror("gettimeofday()");
        event_loopbreak();
    }

    printf( "timer cb: %d, %d\n", CLOCK_TV.tv_sec, CLOCK_TV.tv_usec );

    evtimer_add( (struct event *)ev, &TIMER_TV );
}

static void udp_cb( const int sock, short int which, void *arg )
{
    struct sockaddr_in server_sin;
    socklen_t server_sz = sizeof( server_sin );
    char buf[ sizeof( CLOCK_TV ) ];

    // Recv the data, store the address of the sender in server_sin
    if( recvfrom( sock, &buf, sizeof(buf) - 1, 0, (struct sockaddr *) &server_sin, &server_sz ) == -1 ) 
    {
        perror("recvfrom()");
        event_loopbreak();
    }

    if( gettimeofday( &CLOCK_TV, NULL ) ) 
    {
        perror("gettimeofday()");
        event_loopbreak();
    }

    printf( "rcv packet: %d, %d\n", CLOCK_TV.tv_sec, CLOCK_TV.tv_usec );

#if 0
    // Copy the time into buf; note, endianess unspecified!
    memcpy( buf, &CLOCK_TV, sizeof(CLOCK_TV) );

	// Send the data back to the client
    if( sendto( sock, buf, sizeof(CLOCK_TV), 0, (struct sockaddr *) &server_sin, server_sz) == -1 ) 
    {
        perror("sendto()");
        event_loopbreak();
    }
#endif
}

int main( int argc, char **argv )
{
    int ret, port, sock, fd[2];

    struct event timer_event, udp_event;
    struct sockaddr_in sin;

    sock = socket( AF_INET, SOCK_DGRAM, 0 );

    memset( &sin, 0, sizeof(sin) );
    sin.sin_family           = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port              = htons( 10000 );
	
    if( bind( sock, (struct sockaddr *) &sin, sizeof(sin) ) ) 
    {
        perror("bind()");
        exit(EXIT_FAILURE);
    }

    // Initialize libevent
    event_init();

    // Add the clock event
    evtimer_set( &timer_event, &gettimeofday_cb, &timer_event );
    evtimer_add( &timer_event, &TIMER_TV );

	// Add the UDP event
    event_set( &udp_event, sock, EV_READ|EV_PERSIST, udp_cb, NULL );
    event_add( &udp_event, 0 );

	// Enter the event loop; does not return.
    event_dispatch();
    close( sock );
    return 0;
}

