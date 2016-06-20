#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>

#include "CRLEDPacket.hpp"
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

    driver.start( loop );
    sequencer.start( loop );

    eventSock.addObserver( this );
    eventSock.setup();
    loop.addSource( &eventSock );

    LDStepWaitForStart startStep;
    LDStepRegionOn     onStep;

    sequencer.appendToSequenceDefinition( 0, startStep );
    sequencer.appendToSequenceDefinition( 0, onStep );
}

void 
ClientMain::run()
{
    loop.start();
}

void 
ClientMain::eventAction( uint32_t eventID )
{
    switch( eventID )
    {
        // Receive socket event
        case 1:
        {
            uint32_t           bytesRead;
//            uint8_t            pktBuf[2048];
            CRLEDCommandPacket cmdPkt;
            struct sockaddr_in addr; 
            unsigned int       addrLen = sizeof( addr );

            std::cout << "ClientMain::eventAction" << std::endl;

            bytesRead = recvfrom( eventSock.getSocketFD(), cmdPkt.getMessageBuffer(), cmdPkt.getMaxMessageLength(), 0, (struct sockaddr *) &addr, &addrLen );

            if( bytesRead < sizeof( CRLED_CMDPKT_T ) )
            {
                std::cout << "ClientMain::eventAction - ERROR: short packet: " << bytesRead << std::endl;
                return;
            }

            std::cout << "ClientMain::eventAction - byteRead: " << bytesRead << std::endl;
            printf( "sin_port: %d\n", ntohs( addr.sin_port ) );
            printf( "saddr: %s\n", inet_ntoa( addr.sin_addr ) );

            printf( "opcode: %d\n", cmdPkt.getOpCode() );

            switch( cmdPkt.getOpCode() )
            {
                case CRLED_CMDOP_SCHEDULE:
                {
                    std::cout << "Schedule Command: " << cmdPkt.getParam1() << std::endl;
                    sequencer.startSequence( cmdPkt.getParam1(), &cmdPkt );
                }
                break;

                case CRLED_CMDOP_CLEAR:
                {
                    std::cout << "Clear Command: " << cmdPkt.getParam1() << std::endl;
                    sequencer.clearSequence();
                }
                break;
            }
        }
    }
}

