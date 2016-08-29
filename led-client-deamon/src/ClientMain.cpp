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
    // Load the client configuration file
    config.load();

    // Load the sequence file
    seqConfig.load( config.getID() );

    // Setup the sequencer
    sequencer.setDriver( &driver );

    driver.start( loop );
    sequencer.start( loop );

    eventSock.addObserver( this );
    eventSock.setup();
    loop.addSource( &eventSock );

    LDStepWaitForStart *startStep = new LDStepWaitForStart;
    LDStepRegionChange *clearStep = new LDStepRegionChange; 
    LDStepRegionChange *onStep    = new LDStepRegionChange; 

    onStep->setBounds( 0, 20 );
    onStep->setOnOff( true );

    sequencer.appendToSequenceDefinition( 0, startStep );
    sequencer.appendToSequenceDefinition( 0, clearStep );
    sequencer.appendToSequenceDefinition( 0, onStep );

    LDStepWaitForStart *startStep1 = new LDStepWaitForStart;
    LDStepRegionChange *clearStep1 = new LDStepRegionChange; 
    LDStepRegionChange *onStep1    = new LDStepRegionChange; 

    onStep1->setBounds( 0, 40 );
    onStep1->setOnOff( true );

    sequencer.appendToSequenceDefinition( 1, startStep1 );
    sequencer.appendToSequenceDefinition( 1, clearStep1 );
    sequencer.appendToSequenceDefinition( 1, onStep1 );

    LDStepWaitForStart *startStep2 = new LDStepWaitForStart;
    LDStepRegionChange *clearStep2 = new LDStepRegionChange; 
    LDStepRegionChange *onStep2    = new LDStepRegionChange; 

    onStep2->setBounds( 0, 60 );
    onStep2->setOnOff( true );

    sequencer.appendToSequenceDefinition( 2, startStep2 );
    sequencer.appendToSequenceDefinition( 2, clearStep2 );
    sequencer.appendToSequenceDefinition( 2, onStep2 );

    LDStepWaitForStart *startStep3 = new LDStepWaitForStart;
    LDStepRegionChange *clearStep3 = new LDStepRegionChange; 
    LDStepRegionChange *onStep3    = new LDStepRegionChange; 

    onStep3->setBounds( 0, 80 );
    onStep3->setOnOff( true );

    sequencer.appendToSequenceDefinition( 3, startStep3 );
    sequencer.appendToSequenceDefinition( 3, clearStep3 );
    sequencer.appendToSequenceDefinition( 3, onStep3 );

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

