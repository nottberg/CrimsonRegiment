#include <string.h>
#include <unistd.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include <iostream>

typedef enum crledCommandOperation
{
    CRLED_CMDOP_NOTSET   = 0x0,
    CRLED_CMDOP_PING     = 0x1,
    CRLED_CMDOP_SCHEDULE = 0x2,
    CRLED_CMDOP_CLEAR    = 0x3
}CRLED_CMDOP_T;
        
typedef struct crledCommandPacket
{
    uint32_t opcode;
    uint32_t srcid;
    uint32_t reqnum;
    uint32_t tsec;
    uint32_t tusec;
    uint32_t param1;
    uint32_t param2;
    uint32_t param3;
    uint32_t param4;
    uint32_t addlength;
}CRLED_CMDPKT_T;

class CRLEDCommandPacket
{
    private:
        uint8_t  msgBuf[ 1500 ];
        uint32_t msgLength;

    public:
        CRLEDCommandPacket();
       ~CRLEDCommandPacket();

        void clear();

        uint32_t getOpCode();
        uint32_t getSrcID();
        uint32_t getReqNum();
        uint32_t getTSSec();
        uint32_t getTSUSec();
        uint32_t getParam1();
        uint32_t getParam2();
        uint32_t getParam3();
        uint32_t getParam4();
        uint32_t getAddLength();

        void setOpCode( uint32_t value );
        void setSrcID( uint32_t value );
        void setReqNum( uint32_t value );
        void setTSSec( uint32_t value );
        void setTSUSec( uint32_t value );
        void setParam1( uint32_t value );
        void setParam2( uint32_t value );
        void setParam3( uint32_t value );
        void setParam4( uint32_t value );
        void setAddLength( uint32_t value );

        uint32_t getMessageLength();
        void     setMessageLength( uint32_t value );

        uint32_t getMaxMessageLength();
        uint8_t* getMessageBuffer();
};

CRLEDCommandPacket::CRLEDCommandPacket()
{
    clear();

    msgLength = sizeof( CRLED_CMDPKT_T );
}

CRLEDCommandPacket::~CRLEDCommandPacket()
{

}

void 
CRLEDCommandPacket::clear()
{
    bzero( msgBuf, sizeof( msgBuf ) );
}

uint32_t 
CRLEDCommandPacket::getMessageLength()
{
    return msgLength;
}

void  
CRLEDCommandPacket::setMessageLength( uint32_t value )
{
    msgLength = value;
}

uint8_t* 
CRLEDCommandPacket::getMessageBuffer()
{
    return msgBuf;
}

uint32_t 
CRLEDCommandPacket::getMaxMessageLength()
{
    return sizeof( msgBuf );
}

uint32_t 
CRLEDCommandPacket::getOpCode()
{
    CRLED_CMDPKT_T *header = (CRLED_CMDPKT_T *)msgBuf;

    return ntohl( header->opcode );
}

uint32_t 
CRLEDCommandPacket::getSrcID()
{
    CRLED_CMDPKT_T *header = (CRLED_CMDPKT_T *)msgBuf;

    return ntohl( header->srcid );
}

uint32_t 
CRLEDCommandPacket::getReqNum()
{
    CRLED_CMDPKT_T *header = (CRLED_CMDPKT_T *)msgBuf;

    return ntohl( header->reqnum );
}

uint32_t 
CRLEDCommandPacket::getTSSec()
{
    CRLED_CMDPKT_T *header = (CRLED_CMDPKT_T *)msgBuf;

    return ntohl( header->tsec );
}

uint32_t 
CRLEDCommandPacket::getTSUSec()
{
    CRLED_CMDPKT_T *header = (CRLED_CMDPKT_T *)msgBuf;

    return ntohl( header->tusec );
}

uint32_t 
CRLEDCommandPacket::getParam1()
{
    CRLED_CMDPKT_T *header = (CRLED_CMDPKT_T *)msgBuf;

    return ntohl( header->param1 );
}

uint32_t 
CRLEDCommandPacket::getParam2()
{
    CRLED_CMDPKT_T *header = (CRLED_CMDPKT_T *)msgBuf;

    return ntohl( header->param2 );
}

uint32_t 
CRLEDCommandPacket::getParam3()
{
    CRLED_CMDPKT_T *header = (CRLED_CMDPKT_T *)msgBuf;

    return ntohl( header->param3 );
}

uint32_t 
CRLEDCommandPacket::getParam4()
{
    CRLED_CMDPKT_T *header = (CRLED_CMDPKT_T *)msgBuf;

    return ntohl( header->param4 );
}

uint32_t 
CRLEDCommandPacket::getAddLength()
{
    CRLED_CMDPKT_T *header = (CRLED_CMDPKT_T *)msgBuf;

    return ntohl( header->addlength );
}

void 
CRLEDCommandPacket::setOpCode( uint32_t value )
{
    CRLED_CMDPKT_T *header = (CRLED_CMDPKT_T *)msgBuf;

    header->opcode = htonl( value );
}

void 
CRLEDCommandPacket::setSrcID( uint32_t value )
{
    CRLED_CMDPKT_T *header = (CRLED_CMDPKT_T *)msgBuf;

    header->srcid = htonl( value );
}

void 
CRLEDCommandPacket::setReqNum( uint32_t value )
{
    CRLED_CMDPKT_T *header = (CRLED_CMDPKT_T *)msgBuf;

    header->reqnum = htonl( value );
}

void 
CRLEDCommandPacket::setTSSec( uint32_t value )
{
    CRLED_CMDPKT_T *header = (CRLED_CMDPKT_T *)msgBuf;

    header->tsec = htonl( value );
}

void 
CRLEDCommandPacket::setTSUSec( uint32_t value )
{
    CRLED_CMDPKT_T *header = (CRLED_CMDPKT_T *)msgBuf;

    header->tusec = htonl( value );
}

void 
CRLEDCommandPacket::setParam1( uint32_t value )
{
    CRLED_CMDPKT_T *header = (CRLED_CMDPKT_T *)msgBuf;

    header->param1 = htonl( value );
}

void 
CRLEDCommandPacket::setParam2( uint32_t value )
{
    CRLED_CMDPKT_T *header = (CRLED_CMDPKT_T *)msgBuf;

    header->param2 = htonl( value );
}

void 
CRLEDCommandPacket::setParam3( uint32_t value )
{
    CRLED_CMDPKT_T *header = (CRLED_CMDPKT_T *)msgBuf;

    header->param3 = htonl( value );
}

void 
CRLEDCommandPacket::setParam4( uint32_t value )
{
    CRLED_CMDPKT_T *header = (CRLED_CMDPKT_T *)msgBuf;

    header->param4 = htonl( value );
}

void 
CRLEDCommandPacket::setAddLength( uint32_t value )
{
    CRLED_CMDPKT_T *header = (CRLED_CMDPKT_T *)msgBuf;

    header->addlength = htonl( value );
}

int
main( int argc, char **argv )
{
    struct sockaddr_in s;
    int bcast_sock;
    int broadcastEnable=1;
    int result;
//    uint8_t msgBuf[1024];
    CRLEDCommandPacket cmdPkt;

    cmdPkt.setOpCode( CRLED_CMDOP_SCHEDULE );

    bcast_sock = socket(AF_INET, SOCK_DGRAM, 0);
    
//    result = setsockopt( bcast_sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable) );

    memset( &s, '\0', sizeof( struct sockaddr_in ) );
    s.sin_family      = AF_INET;
    s.sin_port        = (in_port_t)htons( 10260 );
    inet_aton( "192.168.2.7", &s.sin_addr );

    std::cout << "Send" << std::endl;

    
    if( sendto( bcast_sock, cmdPkt.getMessageBuffer(), cmdPkt.getMessageLength(), 0, (struct sockaddr *)&s, sizeof(struct sockaddr_in) ) < 0 )
    {
        perror("sendto");
        return 1;
    }

    return 0;
}

