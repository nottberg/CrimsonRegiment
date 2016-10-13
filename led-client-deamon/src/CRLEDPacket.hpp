#ifndef __CRLED_PACKET_HPP__
#define __CRLED_PACKET_HPP__

#include <stdint.h>
#include <sys/types.h>  
#include <unistd.h>
#include <strings.h>
#include <arpa/inet.h>

typedef enum crledCommandOperation
{
    CRLED_CMDOP_NOTSET   = 0x0,
    CRLED_CMDOP_PING     = 0x1,
    CRLED_CMDOP_SCHEDULE = 0x2,
    CRLED_CMDOP_CLEAR    = 0x3,
    CRLED_CMDOP_PING_RSP = 0x4,
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

#endif // __CRLED_PACKET_HPP__


