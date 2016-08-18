// qmidiin.cpp
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/timeb.h>
#include <sys/time.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdio.h>
#include <strings.h>

#include <iostream>
#include <cstdlib>
#include <signal.h>

#include "CRLEDConfigFile.hpp"

#include "RtMidi.h"

bool done;
static void finish(int ignore){ done = true; }

#include <boost/program_options.hpp>

#include "CRLEDPacket.hpp"

namespace po = boost::program_options;

#define EXAMPLE_PORT 10000
#define EXAMPLE_ADDR "192.168.2.5"
#define EXAMPLE_ADDR2 "192.168.2.7"

#define PACKET_TYPE_MIDI_IN  1
#define PACKET_TYPE_TIME_UP  2

std::vector< struct sockaddr_in > serverList;

int main( int argc, char **argv )
{
    CRLEDConfigFile cfgFile;
    RtMidiIn *midiin = new RtMidiIn();
    std::vector<unsigned char> message;
    int nBytes, i, sock;
    double stamp;

    struct sockaddr_in s;

    int schSeqNum;
    struct timeval curTime;
    struct timeval futureTime;

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

    // Attempt to load the configuration
    cfgFile.load();

    // Fill the server list
    cfgFile.getLEDEndpointAddrList( serverList );

#if 0
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
#endif
#if 0
  struct sockaddr_in addr;
  struct sockaddr_in addr2;
  int addrlen, sock, cnt;
  struct ip_mreq mreq;
  char packet[1024];
  unsigned long long mestamp_ns;
  struct timeb currentTS;

  bzero((char *)&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(EXAMPLE_PORT);
  addrlen = sizeof(addr);

  bzero((char *)&addr2, sizeof(addr2));
  addr2.sin_family = AF_INET;
  addr2.sin_addr.s_addr = htonl(INADDR_ANY);
  addr2.sin_port = htons(EXAMPLE_PORT);
#endif

    // set up socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if( sock < 0 ) 
    {
        perror("socket");
        exit(1);
    }

    // Check available ports.
    unsigned int nPorts = midiin->getPortCount();
    if( nPorts == 0 ) 
    {
        std::cout << "No ports available!\n";
        goto cleanup;
    }

    midiin->openPort( 1 );

    // Don't ignore sysex, timing, or active sensing messages.
    midiin->ignoreTypes( false, false, false );

    // Install an interrupt handler function.
    done = false;
    (void) signal( SIGINT, finish );

    // Periodically check input queue.
    std::cout << "Reading MIDI from port ... quit with Ctrl-C.\n";
    while( !done ) 
    {
        stamp = midiin->getMessage( &message );

        nBytes = message.size();
        if( nBytes > 0 )
        {
            int     result;
            uint8_t direction = 0;
            uint8_t key       = 0;
            uint8_t intensity = 0;

            CRLEDCommandPacket cmdPkt;

            if( nBytes >= 3 )
            {
                direction = message[0];
                key       = message[1];
                intensity = message[2];
            }

//            std::cout << "stamp = " << stamp << std::endl;

//            for( i=0; i<nBytes; i++ )
//            {
//                printf( "0x%x ", message[i] );
//            }
//            std::cout << std::endl;

            // If it is not a key down then exit
            if( direction != 0x90 )
                continue;

            // Get the current time.
            if( gettimeofday( &curTime, NULL ) ) 
            {
                perror("gettimeofday()");
                return 1;
            }

            // Get the key mapping information
            CRLEDMidiKeyBinding& keyInfo = cfgFile.getKeyBinding( key );

            switch( keyInfo.getAction() )
            {
                case MMK_ACTION_SEND_ALL:
                {
                    // Calculate a time in the future for the nodes to take action, start delay is in milliseconds.
                    futureTime.tv_sec  = curTime.tv_sec;
                    futureTime.tv_usec = curTime.tv_usec + ( keyInfo.getStartDelay() * 1000 );
                    if( futureTime.tv_usec >= 1000000 )
                    {
                        futureTime.tv_sec  += 1;
                        futureTime.tv_usec -= 1000000; 
                    }  

                    // 
                    switch( keyInfo.getCommand() )
                    {    
                        case MMK_CMD_SCHEDULE:
                        {
                            std::cout << "Schedule sequence: " << keyInfo.getParam1() << std::endl;

                            cmdPkt.setOpCode( CRLED_CMDOP_SCHEDULE );
                            cmdPkt.setParam1( keyInfo.getParam1() );
                            cmdPkt.setTSSec( futureTime.tv_sec );
                            cmdPkt.setTSUSec( futureTime.tv_usec );
                        }
                        break;

                        case MMK_CMD_CLEAR:
                        {
                            std::cout << "Sending clear sequence" << std::endl;

                            cmdPkt.setOpCode( CRLED_CMDOP_CLEAR );
                            cmdPkt.setTSSec( futureTime.tv_sec );
                            cmdPkt.setTSUSec( futureTime.tv_usec );
                        }
                        break;

                        default:
                        case MMK_CMD_NOT_SET:
                        break;
                    }

                    // Send the command packet to all recipients
                    for( std::vector< struct sockaddr_in >::iterator it = serverList.begin(); it != serverList.end(); it++ )
                    {
                        char tmpBuf[64];
                        std::cout << "Send: " << inet_ntop( AF_INET, &(it->sin_addr), tmpBuf, sizeof(tmpBuf) ) << std::endl;

                        if( sendto( sock, cmdPkt.getMessageBuffer(), cmdPkt.getMessageLength(), 0, (struct sockaddr *)&(*it), sizeof(struct sockaddr_in) ) < 0 )
                        {
                            perror("sendto");
                        }
                    }

                }
                break;

                default:
                break;
            }

#if 0
            // Determine which request is being made.
            switch( key )
            {
                case 0x25:
                {
                    std::cout << "Sending sequence: 0"<< std::endl;

                    cmdPkt.setOpCode( CRLED_CMDOP_SCHEDULE );
                    cmdPkt.setParam1( 0 );
                    cmdPkt.setTSSec( futureTime.tv_sec );
                    cmdPkt.setTSUSec( futureTime.tv_usec );
                }
                break;

                case 0x27:
                {
                    std::cout << "Sending sequence: 1"<< std::endl;

                    cmdPkt.setOpCode( CRLED_CMDOP_SCHEDULE );
                    cmdPkt.setParam1( 1 );
                    cmdPkt.setTSSec( futureTime.tv_sec );
                    cmdPkt.setTSUSec( futureTime.tv_usec );
                }
                break;

                case 0x29:
                {
                    std::cout << "Sending sequence: 2"<< std::endl;

                    cmdPkt.setOpCode( CRLED_CMDOP_SCHEDULE );
                    cmdPkt.setParam1( 2 );
                    cmdPkt.setTSSec( futureTime.tv_sec );
                    cmdPkt.setTSUSec( futureTime.tv_usec );
                }
                break;

                case 0x2B:
                {
                    std::cout << "Sending sequence: 3"<< std::endl;

                    cmdPkt.setOpCode( CRLED_CMDOP_SCHEDULE );
                    cmdPkt.setParam1( 3 );
                    cmdPkt.setTSSec( futureTime.tv_sec );
                    cmdPkt.setTSUSec( futureTime.tv_usec );
                }
                break;

                default:
                {
                    std::cout << "Sending clear sequence" << std::endl;

                    cmdPkt.setOpCode( CRLED_CMDOP_CLEAR );
                    cmdPkt.setTSSec( futureTime.tv_sec );
                    cmdPkt.setTSUSec( futureTime.tv_usec );
                }
                break;
            }

            for( std::vector< struct sockaddr_in >::iterator it = serverList.begin(); it != serverList.end(); it++ )
            {
                std::cout << "Send" << std::endl;

                if( sendto( sock, cmdPkt.getMessageBuffer(), cmdPkt.getMessageLength(), 0, (struct sockaddr *)&(*it), sizeof(struct sockaddr_in) ) < 0 )
                {
                    perror("sendto");
                }
            }
#endif
#if 0
            /* send */
            addr.sin_addr.s_addr = inet_addr(EXAMPLE_ADDR);
            addr2.sin_addr.s_addr = inet_addr(EXAMPLE_ADDR2);

            ftime( &currentTS );

            packet[0] = PACKET_TYPE_MIDI_IN;

            packet[1]  = ( currentTS.time >> 24 ) & 0xFF;
            packet[2]  = ( currentTS.time >> 16 ) & 0xFF;
            packet[3]  = ( currentTS.time >> 8 ) & 0xFF;
            packet[4]  = ( currentTS.time ) & 0xFF;

            packet[5]  = ( currentTS.millitm >> 8 ) & 0xFF;
            packet[6]  = ( currentTS.millitm ) & 0xFF;

            mestamp_ns = stamp * (1000*1000);
            packet[7]  = ( mestamp_ns >> 56 ) & 0xFF;
            packet[8]  = ( mestamp_ns >> 48 ) & 0xFF;
            packet[9]  = ( mestamp_ns >> 40 ) & 0xFF;
            packet[10]  = ( mestamp_ns >> 32 ) & 0xFF;
            packet[11]  = ( mestamp_ns >> 24 ) & 0xFF;
            packet[12]  = ( mestamp_ns >> 16 ) & 0xFF;
            packet[13]  = ( mestamp_ns >> 8 ) & 0xFF;
            packet[14]  = ( mestamp_ns ) & 0xFF;

            packet[15]  = nBytes & 0xFF;

            for( i=0; i<nBytes; i++ )
            {
                packet[16+i] = message[i];
            }

            cnt = sendto( sock, packet, (16+i), 0, (struct sockaddr *) &addr, addrlen );

            if( cnt < 0 )
            {
                perror("sendto");
                exit(1);
            }

            cnt = sendto( sock, packet, (16+i), 0, (struct sockaddr *) &addr2, addrlen );

            if(cnt < 0)
            {
                perror("sendto");
                exit(1);
            }
#endif
        }
 
        // Sleep for 10 milliseconds ... platform-dependent.
        usleep( 10000 );
    }

  // Clean up
cleanup:
    delete midiin;
    return 0;
}

