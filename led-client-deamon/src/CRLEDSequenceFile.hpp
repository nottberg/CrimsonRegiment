#ifndef __CRLED_SEQUENCE_FILE_HPP__
#define __CRLED_SEQUENCE_FILE_HPP__

#include <stdint.h>
#include <sys/types.h>  
#include <unistd.h>
#include <strings.h>
#include <arpa/inet.h>

#include <vector>
#include <string>
#include <map>

class CRLEDSequenceFile
{
    private:
//        std::vector< CRLEDClientConfig > clientList;

//        CRLEDMidiKeyMap keyMap;

//        bool parseClientNode( void *clientPtr );
//        bool parseClientList( void *listPtr );

//        bool parseMidiKey( void *keyPtr, CRLEDMidiKeyBinding &value );
//        bool parseMidiKeyMap( void *mapPtr );

    public:
        CRLEDSequenceFile();
       ~CRLEDSequenceFile();

        void setSequenceFilePath();

        bool load( std::string nodeid );

//        bool getLEDEndpointAddrList( std::vector< struct sockaddr_in > &addrList ); 

//        CRLEDMidiKeyBinding& getKeyBinding( uint32_t keyCode );
};

#endif // __CRLED_SEQUENCE_FILE_HPP__

