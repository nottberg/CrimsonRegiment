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

    eventSock.setup();
    loop.addSource( &eventSock );
}

void 
ClientMain::run()
{
    loop.start();
}

