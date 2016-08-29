#ifndef __CRLED_CLIENT_CONFIG_FILE_HPP__
#define __CRLED_CLIENT_CONFIG_FILE_HPP__

#include <stdint.h>
#include <sys/types.h>  
#include <unistd.h>
#include <strings.h>
#include <arpa/inet.h>

#include <vector>
#include <string>
#include <map>

class CRLEDClientConfigFile
{
    private:
        std::string id;
        
        uint32_t ledCnt;

        bool parseLEDConfig( void *ledPtr );
//        bool parseClientList( void *listPtr );

//        bool parseMidiKey( void *keyPtr, CRLEDMidiKeyBinding &value );
//        bool parseMidiKeyMap( void *mapPtr );

    public:
        CRLEDClientConfigFile();
       ~CRLEDClientConfigFile();

        void setSequenceFilePath();

        bool load();

        std::string getID();
        uint32_t    getLEDCount();

};

#endif // __CRLED_CLIENT_CONFIG_FILE_HPP__

