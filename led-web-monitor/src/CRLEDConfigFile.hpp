#ifndef __CRLED_CONFIG_FILE_HPP__
#define __CRLED_CONFIG_FILE_HPP__

#include <stdint.h>
#include <sys/types.h>  
#include <unistd.h>
#include <strings.h>
#include <arpa/inet.h>

#include <vector>
#include <string>
#include <map>

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

typedef enum CRLEDMidiKeyBindingActions
{
    MMK_ACTION_NOP,
    MMK_ACTION_SEND_ALL
}MMK_ACTION_T;

typedef enum CRLEDMidiKeyBindingCommands
{
    MMK_CMD_NOT_SET,
    MMK_CMD_SCHEDULE,
    MMK_CMD_CLEAR
}MMK_CMD_T;

class CRLEDMidiKeyBinding
{
    private:
        uint32_t     keyCode;
        MMK_ACTION_T action;
        MMK_CMD_T    command;
        uint32_t     param1;
        uint32_t     startDelay;

    public:
        CRLEDMidiKeyBinding();
       ~CRLEDMidiKeyBinding();

        void clear();

        void setKeyCode( uint32_t value );
        void setAction( MMK_ACTION_T value );
        void setCommand( MMK_CMD_T value );
        void setParam1( uint32_t value );
        void setStartDelay( uint32_t value );

        uint32_t     getKeyCode();
        MMK_ACTION_T getAction();
        MMK_CMD_T    getCommand();
        uint32_t     getParam1();
        uint32_t     getStartDelay();

};

class CRLEDMidiKeyMap
{
    private:
        CRLEDMidiKeyBinding  defaultKey;

        std::map< uint32_t, CRLEDMidiKeyBinding > keyMap; 

    public:
        CRLEDMidiKeyMap();
       ~CRLEDMidiKeyMap();

        void setKeyBinding( uint32_t keycode, CRLEDMidiKeyBinding &value );
        void setDefaultKeyBinding( CRLEDMidiKeyBinding &value );

        CRLEDMidiKeyBinding& getKeyBinding( uint32_t keyCode );
};

class CRLEDConfigFile
{
    private:
        std::vector< CRLEDClientConfig > clientList;

        CRLEDMidiKeyMap keyMap;

        bool parseClientNode( void *clientPtr );
        bool parseClientList( void *listPtr );

        bool parseMidiKey( void *keyPtr, CRLEDMidiKeyBinding &value );
        bool parseMidiKeyMap( void *mapPtr );

    public:
        CRLEDConfigFile();
       ~CRLEDConfigFile();

        void setConfigFilePath();

        bool load();

        bool getLEDEndpointAddrList( std::vector< struct sockaddr_in > &addrList ); 

        CRLEDMidiKeyBinding& getKeyBinding( uint32_t keyCode );
};

#endif // __CRLED_CONFIG_FILE_HPP__

