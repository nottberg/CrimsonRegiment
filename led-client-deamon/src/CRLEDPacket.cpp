#include <unistd.h>

#include "CRLEDPacket.hpp"

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
