#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <stdio.h> 

#include <boost/program_options.hpp>

#include "hnode-rest.hpp"
#include "RootResource.hpp"
#include "CRLEDResource.hpp"

namespace po = boost::program_options;

int
main( int argc, char **argv )
{
    RESTDaemon          Rest;
    RootResource        rootResource;
    CRLStatusResource   statusResource;
    CRLSequenceResource seqResource;

    // Declare the supported options.
    po::options_description desc("Crimson Regiment LED web monitor");
    desc.add_options()
        ("help", "produce help message")
    ;

    po::variables_map vm;
    po::store( po::parse_command_line( argc, argv, desc ), vm );
    po::notify( vm );    

    if( vm.count("help") ) 
    {
       std::cout << desc << "\n";
       return 1;
    }

    // Init the REST resources.
    Rest.registerResource( &seqResource );
    Rest.registerResource( &statusResource );
    Rest.registerResource( &rootResource );

    Rest.setListeningPort( 8080 );
    Rest.start();

    while(1);

    return 0;
}
