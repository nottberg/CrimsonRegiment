#include "ClientMain.hpp"

ClientMain::ClientMain()
: driver( "/dev/spidev0.0", 150 )
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
}

void 
ClientMain::run()
{
    loop.start();
}

