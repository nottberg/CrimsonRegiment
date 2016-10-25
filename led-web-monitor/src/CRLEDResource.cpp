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
    struct sockaddr_in endpoint;

    bool     rspRcvd;
    bool     error;

    struct timeval sendTime;
    struct timeval rspTime;

    CRLEDCommandPacket cmdPkt;

    uint32_t sendIndex;
    int32_t  linkVal;
    int32_t  linkSig;
}PTRACK_T;

void 
CRLStatusResource::restGet( RESTRequest *request )
{
    std::string resp;
    CRLEDConfigFile cfgFile;
    std::vector< struct sockaddr_in > serverList;
    std::map< uint32_t, PTRACK_T > trackMap;
    CRLEDCommandPacket cmdPkt;
    struct timeval curTime;
    struct timeval endTime;
    int sock;
    FILE   *fp;
    char   *line = NULL;
    size_t  len = 0;
    ssize_t read;
    char    infStr[128];
    int32_t linkVal;
    int32_t linkLevel;
    int32_t linkNoise;
    int32_t infState;

    cout << "CRLStatusResource::restGet -- start" << std::endl;

    // Attempt to load the configuration
    cfgFile.load();

    // Fill the server list
    cfgFile.getLEDEndpointAddrList( serverList );


    // set up socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if( sock < 0 ) 
    {
        perror("socket");
    }

    // Build the tracking structure
    uint32_t sndCnt = 0;
    for( std::vector< struct sockaddr_in >::iterator it = serverList.begin(); it != serverList.end(); it++, sndCnt++ )
    {
        char tmpBuf[64];
        PTRACK_T trackData;

        // Create a record in the tracking list.
        trackMap.insert( std::pair< uint32_t, PTRACK_T >( (uint32_t) it->sin_addr.s_addr, trackData ) );       

        // Get access to the new record
        std::map< uint32_t, PTRACK_T >::iterator tit = trackMap.find( (uint32_t) it->sin_addr.s_addr );

        tit->second.endpoint  = *it; //it->sin_addr;
        tit->second.rspRcvd   = false;
        tit->second.error     = false;
        tit->second.sendIndex = sndCnt;
        tit->second.linkVal   = 0;
        tit->second.linkSig   = 0;

        tit->second.cmdPkt.setOpCode( CRLED_CMDOP_PING );
        tit->second.cmdPkt.setTSSec( 0 );
        tit->second.cmdPkt.setTSUSec( 0 );
        tit->second.cmdPkt.setParam1( (uint32_t) it->sin_addr.s_addr );
        tit->second.cmdPkt.setParam2( sndCnt );
    }

    for( std::map< uint32_t, PTRACK_T >::iterator it = trackMap.begin(); it != trackMap.end(); it++ )
    {
        char tmpBuf[64];

        // Get the current time.
        if( gettimeofday( &it->second.sendTime, NULL ) ) 
        {
            it->second.error = true;
            continue;
        }

        // Default the rspTime to the sendTime
        it->second.rspTime = it->second.sendTime;

        std::cout << "Send: " << inet_ntop( AF_INET, &(it->second.endpoint.sin_addr), tmpBuf, sizeof(tmpBuf) ) << std::endl;

        if( sendto( sock, it->second.cmdPkt.getMessageBuffer(), it->second.cmdPkt.getMessageLength(), 0, (struct sockaddr *)&(it->second.endpoint), sizeof(struct sockaddr_in) ) < 0 )
        {
            it->second.error = true;
        }
    }
    
    // Calculate the endtime
    gettimeofday( &endTime, NULL );
    endTime.tv_sec += 4;


    // Wait for responses (for 4 seconds)
    uint32_t rspCnt = 0;
    while( ( rspCnt < sndCnt ) && ( curTime.tv_sec < endTime.tv_sec ) && ( curTime.tv_usec < endTime.tv_usec ) )
    {
        int32_t            bytesRead;
        CRLEDCommandPacket cmdPkt;
        struct sockaddr_in addr; 
        unsigned int       addrLen = sizeof( addr );

        bytesRead = recvfrom( sock, cmdPkt.getMessageBuffer(), cmdPkt.getMaxMessageLength(), MSG_DONTWAIT, (struct sockaddr *) &addr, &addrLen );

        // Update the current time.
        gettimeofday( &curTime, NULL );

        if( bytesRead < 0 )
        {
            if( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ) )
                continue;
 
            // Error
            continue;
        }
        else if( bytesRead < sizeof( CRLED_CMDPKT_T ) )
        {
            continue;
        }

        switch( cmdPkt.getOpCode() )
        {
            case CRLED_CMDOP_PING_RSP:
            {
         
                // Find the cooresponding send record
                std::map< uint32_t, PTRACK_T >::iterator it = trackMap.find( (uint32_t) addr.sin_addr.s_addr );

                // If not found then move on.
                if( it == trackMap.end() )
                    continue;       

                // Verify the send index
                if( cmdPkt.getParam2() != it->second.sendIndex )
                    continue;

                it->second.rspRcvd = true;
                it->second.rspTime = curTime;
                it->second.linkVal = cmdPkt.getParam3();
                it->second.linkSig = cmdPkt.getParam4();

                rspCnt += 1;
            }
            break;

            default:
            break;
        }

    }

    close( sock );

    // Get the signal strength for our wlan0 connection
    bool wFound = false;
    char tmpStr[ 128 ];

    fp = fopen( "/proc/net/wireless", "r");
    if( fp != NULL )
    {
        while( ( read = getline( &line, &len, fp ) ) != -1 ) 
        {
            //printf("Retrieved line of length %zu :\n", read);
            //printf("%s", line);

            // Attempt to extract out some fields
            sscanf( line, "%s %d %d%*[. ] %d%*[. ] %d%*[. ]", infStr, &infState, &linkVal, &linkLevel, &linkNoise );

            // Check for the wlan0 file we are intersted in.
            if( strncmp( infStr, "wlan0", 5 ) == 0 )
            {
                printf( "%s %d %d %d %d\n", infStr, infState, linkVal, linkLevel, linkNoise );
                wFound = true;
                break; 
            }
        }

        fclose( fp );
        if( line )
            free( line );
    }

    // Build response data
    resp = "{";

    resp += "\"linkQuality\":";
    sprintf( tmpStr, "\"%d\"", ( wFound ? linkVal : 0 ) );
    resp += (const char *) tmpStr;

    resp += ",";
    resp += "\"linkSignal\":";
    sprintf( tmpStr, "\"%d\"", ( wFound ? linkLevel : 0 ) );
    resp += (const char *) tmpStr;

    resp += ",";
    resp += "\"nodes\": [";

    uint32_t indx = 0;
    for( std::map< uint32_t, PTRACK_T >::iterator it = trackMap.begin(); it != trackMap.end(); it++, indx++ )
    {
        if( indx == 0 )
        {
            resp += "{";
        }
        else
        {
            resp += ",{";
        }
       
        resp += "\"addr\":";
        if( inet_ntop( AF_INET, &(it->second.endpoint.sin_addr), tmpStr, sizeof( tmpStr ) ) == NULL )
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
        resp += ( it->second.rspRcvd == true ? "\"true\"" : "\"false\"" );

        resp += ",";
        resp += "\"error\":";
        resp += ( it->second.error == true ? "\"true\"" : "\"false\"" );

        resp += ",";
        resp += "\"linkQuality\":";
        sprintf( tmpStr, "\"%d\"", it->second.linkVal );
        resp += (const char *) tmpStr;

        resp += ",";
        resp += "\"linkSignal\":";
        sprintf( tmpStr, "\"%d\"", it->second.linkSig );
        resp += (const char *) tmpStr;

        uint32_t rspMS;
        rspMS  = (it->second.rspTime.tv_sec - it->second.sendTime.tv_sec) * 1000;
        if( it->second.rspTime.tv_usec >= it->second.sendTime.tv_usec )
        {
            rspMS += (it->second.rspTime.tv_usec - it->second.sendTime.tv_usec) / 1000;
        }
        else
        {
            rspMS += (1000000 - it->second.sendTime.tv_usec) / 1000;
            rspMS += (it->second.rspTime.tv_usec) / 1000;
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
    CRLEDConfigFile cfgFile;
    std::vector< struct sockaddr_in > serverList;
    CRLEDCommandPacket cmdPkt;
    struct timeval curTime;
    struct timeval futureTime;
    int sock;
    uint32_t seqIndex = 0;

    cout << "CRLSequenceResource::restPUT -- start" << std::endl;

    // Attempt to load the configuration
    cfgFile.load();

    // Fill the server list
    cfgFile.getLEDEndpointAddrList( serverList );

    // Check for a query parameter
    std::string qvalue;
    if( request->getInboundRepresentation()->getQueryParameter( "seqIndex", qvalue ) == false )
    {
        // The query parameter exists, try to turn it into a number
        seqIndex = strtol( qvalue.c_str(), NULL, 0 );
    }

    std::cout << "Starting sequence: " << seqIndex << std::endl;

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

    cmdPkt.setOpCode( CRLED_CMDOP_SCHEDULE );
    cmdPkt.setTSSec( futureTime.tv_sec );
    cmdPkt.setTSUSec( futureTime.tv_usec );

    cmdPkt.setParam1( seqIndex );

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


