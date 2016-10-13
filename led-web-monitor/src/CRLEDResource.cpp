#include <string>
#include <iostream>
#include <fstream>
using namespace std;

#include "CRLEDResource.hpp"
#include "CRLEDConfigFile.hpp"
#include "CRLEDPacket.hpp"

CRLStatusResource::CRLStatusResource()
{
    setURLPattern( "/crled/status", (REST_RMETHOD_T)(REST_RMETHOD_GET) ); 
}

CRLStatusResource::~CRLStatusResource()
{

}

typedef struct pingTrackStruct
{
    struct in_addr endpoint;

    bool     rspRcvd;
    bool     error;

    struct timeval sendTime;
    struct timeval rspTime;
}PTRACK_T;

void 
CRLStatusResource::restGet( RESTRequest *request )
{
    std::string resp;
    CRLEDConfigFile cfgFile;
    std::vector< struct sockaddr_in > serverList;
    std::vector< PTRACK_T > trackList;
    CRLEDCommandPacket cmdPkt;
    struct timeval curTime;
    struct timeval endTime;
    int sock;

    cout << "CRLStatusResource::restGet -- start" << std::endl;

    // Attempt to load the configuration
    cfgFile.load();

    // Fill the server list
    cfgFile.getLEDEndpointAddrList( serverList );

    cmdPkt.setOpCode( CRLED_CMDOP_PING );
    cmdPkt.setTSSec( 0 );
    cmdPkt.setTSUSec( 0 );

    // set up socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if( sock < 0 ) 
    {
        perror("socket");
    }

    // Send the command packet to all recipients
    int sendIndx = 0;
    for( std::vector< struct sockaddr_in >::iterator it = serverList.begin(); it != serverList.end(); it++, sendIndx++ )
    {
        char tmpBuf[64];
        PTRACK_T trackData;

        trackData.endpoint = it->sin_addr;
        trackData.rspRcvd  = false;
        trackData.error    = false;

        // Get the current time.
        if( gettimeofday( &trackData.sendTime, NULL ) ) 
        {
            trackData.error = true;
            continue;
        }

        cmdPkt.setParam1( (uint32_t) it->sin_addr.s_addr );
        cmdPkt.setParam2( sendIndx );
        cmdPkt.setParam3( 0 );

        std::cout << "Send: " << inet_ntop( AF_INET, &(it->sin_addr), tmpBuf, sizeof(tmpBuf) ) << std::endl;

        if( sendto( sock, cmdPkt.getMessageBuffer(), cmdPkt.getMessageLength(), 0, (struct sockaddr *)&(*it), sizeof(struct sockaddr_in) ) < 0 )
        {
            trackData.error = true;
        }

        // Wait for a response
        while( ( trackData.error == false ) && ( trackData.rspRcvd == false ) && ( trackData.rspTime.tv_sec < ( trackData.sendTime.tv_sec + 2 ) ) )
        {
            int32_t            bytesRead;
            CRLEDCommandPacket cmdPkt;
            struct sockaddr_in addr; 
            unsigned int       addrLen = sizeof( addr );

            bytesRead = recvfrom( sock, cmdPkt.getMessageBuffer(), cmdPkt.getMaxMessageLength(), MSG_DONTWAIT, (struct sockaddr *) &addr, &addrLen );

            // Update the current time.
            gettimeofday( &trackData.rspTime, NULL );

            if( bytesRead < 0 )
            {
                if( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ) )
                    continue;
 
                // Error
                trackData.error = true;
                continue;
            }
            else if( bytesRead < sizeof( CRLED_CMDPKT_T ) )
            {
                trackData.error = true;
                continue;
            }

            switch( cmdPkt.getOpCode() )
            {
                case CRLED_CMDOP_PING_RSP:
                {
                    if( cmdPkt.getParam1() == (uint32_t) trackData.endpoint.s_addr )
                    {
                        // Grab the timestamp
                        if( gettimeofday( &trackData.rspTime, NULL ) ) 
                        {
                            trackData.error = true;
                        }

                        trackData.rspRcvd  = true;
                    }
                }
                break;

                default:
                break;
            }


        }

        // Add the record to the tracking list.
        trackList.push_back( trackData );
    }

    close( sock );

    // Build response data
    resp = "{ \"nodes\": [";

    for( std::vector< PTRACK_T >::iterator it = trackList.begin(); it != trackList.end(); it++ )
    {
        char tmpStr[ 128 ];

        resp += "{";
       
        resp += "\"addr\":";
        if( inet_ntop( AF_INET, &(it->endpoint), tmpStr, sizeof( tmpStr ) ) == NULL )
        {
            resp += "\"\"";
        }
        else
        {
            resp += "\"";
            resp += (const char*) tmpStr; 
            resp += "\"";
        }
        
        resp += ",";
        resp += "\"rspRX\":";
        resp += ( it->rspRcvd == true ? "\"true\"" : "\"false\"" );

        resp += ",";
        resp += "\"error\":";
        resp += ( it->error == true ? "\"true\"" : "\"false\"" );

        uint32_t rspMS;
        rspMS  = (it->rspTime.tv_sec - it->sendTime.tv_sec) * 1000;
        if( it->rspTime.tv_usec >= it->sendTime.tv_usec )
        {
            rspMS += (it->rspTime.tv_usec - it->sendTime.tv_usec) / 1000;
        }
        else
        {
            rspMS += (1000000 - it->sendTime.tv_usec) / 1000;
            rspMS += (it->rspTime.tv_usec) / 1000;
        }

        resp += ",";
        resp += "\"rspMS\":";
        sprintf( tmpStr, "\"%d\"", rspMS );
        resp += (const char *) tmpStr;

        resp += "}";
    }

    resp += "] }";

    RESTRepresentation *rspRep = request->getOutboundRepresentation();
    rspRep->setSimpleContent( "application/json", (unsigned char *) resp.c_str(), resp.size() );

    request->setResponseCode( REST_HTTP_RCODE_OK );
    request->sendResponse();

    return;
}

CRLSequenceResource::CRLSequenceResource()
{
    setURLPattern( "/crled/sequence", (REST_RMETHOD_T)(REST_RMETHOD_PUT|REST_RMETHOD_DELETE) ); 
}

CRLSequenceResource::~CRLSequenceResource()
{

}

void 
CRLSequenceResource::restPut( RESTRequest *request )
{
    std::string resp;

    cout << "CRLSequenceResource::restGet -- start" << std::endl;


    std::cout << "Sending clear sequence" << std::endl;

    request->setResponseCode( REST_HTTP_RCODE_OK );
    request->sendResponse();

    return;
}

void 
CRLSequenceResource::restDelete( RESTRequest *request )
{
    std::string resp;
    CRLEDConfigFile cfgFile;
    std::vector< struct sockaddr_in > serverList;
    CRLEDCommandPacket cmdPkt;
    struct timeval curTime;
    struct timeval futureTime;
    int sock;

    cout << "CRLSequenceResource::restDelete -- start" << std::endl;

    // Attempt to load the configuration
    cfgFile.load();

    // Fill the server list
    cfgFile.getLEDEndpointAddrList( serverList );

    std::cout << "Sending clear sequence" << std::endl;

    // Get the current time.
    if( gettimeofday( &curTime, NULL ) ) 
    {
        perror("gettimeofday()");
        //return 1;
    }

    // Calculate a time in the future for the nodes to take action, start delay is in milliseconds.
    futureTime.tv_sec  = curTime.tv_sec;
    futureTime.tv_usec = curTime.tv_usec + 10000;
    if( futureTime.tv_usec >= 1000000 )
    {
        futureTime.tv_sec  += 1;
        futureTime.tv_usec -= 1000000; 
    }  

    cmdPkt.setOpCode( CRLED_CMDOP_CLEAR );
    cmdPkt.setTSSec( futureTime.tv_sec );
    cmdPkt.setTSUSec( futureTime.tv_usec );

    // set up socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if( sock < 0 ) 
    {
        perror("socket");
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

    close( sock );

    request->setResponseCode( REST_HTTP_RCODE_OK );
    request->sendResponse();
}


