#include <string.h>
#include <unistd.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include <iostream>

#include <boost/program_options.hpp>

#include "CRLEDPacket.hpp"

namespace po = boost::program_options;

std::vector< struct sockaddr_in > serverList;

int
main( int argc, char **argv )
{
    struct sockaddr_in s;

    int schSeqNum;
    struct timeval curTime;

    // Declare the supported options.
    po::options_description desc("Crimson Regiment LED command line control");
    desc.add_options()
        ("help", "produce help message")
        ("schedule,s", po::value<int>(&schSeqNum), "Schedule a sequence.")
        ("clear,c", "Clear currently running sequences.")
    ;

    po::variables_map vm;
    po::store( po::parse_command_line( argc, argv, desc ), vm );
    po::notify( vm );    

    if (vm.count("help")) 
    {
       std::cout << desc << "\n";
       return 1;
    }

    // Fill the server list
    memset( &s, '\0', sizeof( struct sockaddr_in ) );
    s.sin_family = AF_INET;
    s.sin_port   = (in_port_t)htons( 10260 );
    inet_aton( "192.168.2.7", &s.sin_addr );

    serverList.push_back( s );

    memset( &s, '\0', sizeof( struct sockaddr_in ) );
    s.sin_family = AF_INET;
    s.sin_port   = (in_port_t)htons( 10260 );
    inet_aton( "192.168.2.8", &s.sin_addr );

    serverList.push_back( s );

    // Get the current time.
    if( gettimeofday( &curTime, NULL ) ) 
    {
        perror("gettimeofday()");
        return 1;
    }

    if( vm.count( "schedule" ) ) 
    {
        //struct sockaddr_in s;
        int bcast_sock;
        int result;
        CRLEDCommandPacket cmdPkt;
 
        std::cout << "Sending start sequence: " << schSeqNum << std::endl;

        cmdPkt.setOpCode( CRLED_CMDOP_SCHEDULE );
        cmdPkt.setParam1( schSeqNum );
        cmdPkt.setTSSec( curTime.tv_sec );
        cmdPkt.setTSUSec( curTime.tv_usec );

        bcast_sock = socket( AF_INET, SOCK_DGRAM, 0 );
    
        //memset( &s, '\0', sizeof( struct sockaddr_in ) );
        //s.sin_family      = AF_INET;
        //s.sin_port        = (in_port_t)htons( 10260 );
        //inet_aton( "192.168.2.7", &s.sin_addr );

        for( std::vector< struct sockaddr_in >::iterator it = serverList.begin(); it != serverList.end(); it++ )
        {
            std::cout << "Send" << std::endl;

            if( sendto( bcast_sock, cmdPkt.getMessageBuffer(), cmdPkt.getMessageLength(), 0, (struct sockaddr *)&(*it), sizeof(struct sockaddr_in) ) < 0 )
            {
                perror("sendto");
            }
        }
    } 
    else if( vm.count( "clear" ) )
    {
        //struct sockaddr_in s;
        int bcast_sock;
        int result;
        CRLEDCommandPacket cmdPkt;

        std::cout << "Sending clear sequence" << std::endl;

        cmdPkt.setOpCode( CRLED_CMDOP_CLEAR );
        cmdPkt.setTSSec( curTime.tv_sec );
        cmdPkt.setTSUSec( curTime.tv_usec );

        bcast_sock = socket( AF_INET, SOCK_DGRAM, 0 );
    
        //memset( &s, '\0', sizeof( struct sockaddr_in ) );
        //s.sin_family      = AF_INET;
        //s.sin_port        = (in_port_t)htons( 10260 );
        //inet_aton( "192.168.2.7", &s.sin_addr );

        for( std::vector< struct sockaddr_in >::iterator it = serverList.begin(); it != serverList.end(); it++ )
        {
            std::cout << "Send" << std::endl;

            if( sendto( bcast_sock, cmdPkt.getMessageBuffer(), cmdPkt.getMessageLength(), 0, (struct sockaddr *)&(*it), sizeof(struct sockaddr_in) ) < 0 )
            {
                perror("sendto");
            }
        }
    }

    return 0;
}

