#ifndef __CRLED_CONFIG_FILE_HPP__
#define __CRLED_CONFIG_FILE_HPP__

#include <stdint.h>
#include <sys/types.h>  
#include <unistd.h>
#include <strings.h>
#include <arpa/inet.h>

#include <vector>
#include <string>

class CRLEDClientConfig
{
    private:
        struct sockaddr_in addr;
        uint16_t           ledPort;

    public:
        CRLEDClientConfig();
       ~CRLEDClientConfig();

        bool setAddressFromIPV4Str( const char *value );
        bool setPortForLEDCtrl( uint16_t value );

        bool getLEDEndpointAddress( sockaddr_in &addr );
};

class CRLEDConfigFile
{
    private:
        std::vector< CRLEDClientConfig > clientList;

        bool parseClientNode( void *clientPtr );
        bool parseClientList( void *listPtr );

    public:
        CRLEDConfigFile();
       ~CRLEDConfigFile();

        void setConfigFilePath();

        bool load();

        bool getLEDEndpointAddrList( std::vector< struct sockaddr_in > &addrList ); 
};

#endif // __CRLED_CONFIG_FILE_HPP__

