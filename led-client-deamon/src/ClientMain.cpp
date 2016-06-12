#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>

#include "ClientMain.hpp"

ClientMain::ClientMain()
: driver( "/dev/spidev0.0", 150 ), eventSock( 1, "eventSock" )
{

}

ClientMain::~ClientMain()
{

}

bool 
ClientMain::setup()
{
    sequencer.setDriver( &driver );

    //driver.start( loop );
    //sequencer.start( loop );

    eventSock.addObserver( this );
    eventSock.setup();
    loop.addSource( &eventSock );

}

void 
ClientMain::run()
{
    loop.start();
}

void 
ClientMain::eventAction( uint32_t eventID )
{
    int                bytesRead;
    uint8_t            pktBuf[2048];
    struct sockaddr_in addr; 
    unsigned int       addrLen;

    std::cout << "ClientMain::eventAction" << std::endl;

    bytesRead = recvfrom( eventSock.getSocketFD(), pktBuf, sizeof( pktBuf ), 0, (struct sockaddr *) &addr, &addrLen );

    std::cout << "ClientMain::eventAction - byteRead: " << bytesRead << std::endl;

    printf( "sin_port: %d\n", ntohs( addr.sin_port ) );
    printf( "saddr: %s\n", inet_ntoa( addr.sin_addr ) );
}

