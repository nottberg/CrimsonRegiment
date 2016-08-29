#include <string.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "CRLEDConfigFile.hpp"

CRLEDClientConfig::CRLEDClientConfig()
{

}

CRLEDClientConfig::~CRLEDClientConfig()
{

}

bool 
CRLEDClientConfig::setAddressFromIPV4Str( const char *value )
{
    inet_pton( AF_INET, value, &addr );
    return false;
}

bool 
CRLEDClientConfig::setPortForLEDCtrl( uint16_t value )
{
    ledPort = value;
    return false;
}

bool 
CRLEDClientConfig::getLEDEndpointAddress( sockaddr_in &rtnaddr )
{
    memset( &rtnaddr, '\0', sizeof( struct sockaddr_in ) );

    rtnaddr.sin_family = AF_INET;
    rtnaddr.sin_port   = (in_port_t) htons( ledPort );
    rtnaddr.sin_addr   = addr.sin_addr;

    return false;
}

CRLEDMidiKeyBinding::CRLEDMidiKeyBinding()
{
    clear();
}

CRLEDMidiKeyBinding::~CRLEDMidiKeyBinding()
{

}


void 
CRLEDMidiKeyBinding::clear()
{
    keyCode    = 0;
    action     = MMK_ACTION_NOP;
    command    = MMK_CMD_NOT_SET;
    param1     = 0;
    startDelay = 200;
}

void 
CRLEDMidiKeyBinding::setKeyCode( uint32_t value )
{
    keyCode = value;
}

void 
CRLEDMidiKeyBinding::setAction( MMK_ACTION_T value )
{
    action = value;
}

void 
CRLEDMidiKeyBinding::setCommand( MMK_CMD_T value )
{
    command = value;
}

void 
CRLEDMidiKeyBinding::setParam1( uint32_t value )
{
    param1 = value;
}

void 
CRLEDMidiKeyBinding::setStartDelay( uint32_t value )
{
    startDelay = value;
}

uint32_t 
CRLEDMidiKeyBinding::getKeyCode()
{
    return keyCode;
}

MMK_ACTION_T 
CRLEDMidiKeyBinding::getAction()
{
    return action;
}

MMK_CMD_T 
CRLEDMidiKeyBinding::getCommand()
{
    return command;
}

uint32_t 
CRLEDMidiKeyBinding::getParam1()
{
    return param1;
}

uint32_t 
CRLEDMidiKeyBinding::getStartDelay()
{
    return startDelay;
}

CRLEDMidiKeyMap::CRLEDMidiKeyMap()
{

}

CRLEDMidiKeyMap::~CRLEDMidiKeyMap()
{
}

void 
CRLEDMidiKeyMap::setKeyBinding( uint32_t keycode, CRLEDMidiKeyBinding &value )
{
    keyMap.insert( std::pair< uint32_t, CRLEDMidiKeyBinding >( value.getKeyCode(), value ) );
}

void 
CRLEDMidiKeyMap::setDefaultKeyBinding( CRLEDMidiKeyBinding &value )
{
    defaultKey = value;
}

CRLEDMidiKeyBinding& 
CRLEDMidiKeyMap::getKeyBinding( uint32_t keyCode )
{
    std::map< uint32_t, CRLEDMidiKeyBinding >::iterator it = keyMap.find( keyCode );

    if( it == keyMap.end() )
    {
        return defaultKey;
    }

    return it->second;
}

CRLEDConfigFile::CRLEDConfigFile()
{

}

CRLEDConfigFile::~CRLEDConfigFile()
{

}

void 
CRLEDConfigFile::setConfigFilePath()
{

}

bool 
CRLEDConfigFile::parseClientNode( void *clientPtr )
{
    xmlNode *nodePtr = NULL;

    // Traverse the document to pull out the items of interest
    for( nodePtr = ((xmlNode *)clientPtr)->children; nodePtr; nodePtr = nodePtr->next ) 
    {
        CRLEDClientConfig clientRecord;

        if( nodePtr->type == XML_ELEMENT_NODE ) 
        {
            printf( "node type: Element, name: %s\n", nodePtr->name );

            if( xmlStrEqual( nodePtr->name, (xmlChar *)"addr" ) )
            {
                xmlChar *type;
                xmlChar *cStr;

                type = xmlGetProp( nodePtr, (xmlChar *)"type" );

                if( type != NULL )
                {
                    cStr = xmlNodeGetContent( nodePtr );

                    if( xmlStrEqual( type, (xmlChar *)"ipv4" ) )
                    {
                        clientRecord.setAddressFromIPV4Str( (const char *)cStr );
                    }

                    xmlFree( type );
                    xmlFree( cStr );
                }
            }
            else if( xmlStrEqual( nodePtr->name, (xmlChar *)"port" ) )
            {
                xmlChar *type;
                xmlChar *cStr;

                type = xmlGetProp( nodePtr, (xmlChar *)"type" );

                if( type != NULL )
                {
                    cStr = xmlNodeGetContent( nodePtr );

                    if( xmlStrEqual( type, (xmlChar *)"led" ) )
                    {
                        uint16_t portValue;
                        portValue = strtol( (const char *)cStr, NULL, 0 );
                        clientRecord.setPortForLEDCtrl( portValue );
                    }

                    xmlFree( type );
                    xmlFree( cStr );
                }
            }
        }

        clientList.push_back( clientRecord );
    }
}

bool 
CRLEDConfigFile::parseClientList( void *listPtr )
{
    xmlNode *nodePtr = NULL;

    // Traverse the document to pull out the items of interest
    for( nodePtr = ((xmlNode *)listPtr)->children; nodePtr; nodePtr = nodePtr->next ) 
    {
        if( nodePtr->type == XML_ELEMENT_NODE ) 
        {
            printf( "node type: Element, name: %s\n", nodePtr->name );

            if( xmlStrEqual( nodePtr->name, (xmlChar *)"client" ) )
            {
                parseClientNode( nodePtr );
            }
        }
    }
}

bool 
CRLEDConfigFile::parseMidiKey( void *keyPtr, CRLEDMidiKeyBinding &keyBinding )
{
    xmlNode  *nodePtr = NULL;
    xmlChar  *kcStr;
    uint32_t  keyCode;

    // Extract the keycode
    kcStr = xmlGetProp( (xmlNode *)keyPtr, (xmlChar *)"code" );

    if( kcStr == NULL )
        return true;

    keyCode = strtol( (const char *)kcStr, NULL, 0 );
    xmlFree( kcStr );

    keyBinding.setKeyCode( keyCode );

    // Traverse the document to pull out the items of interest
    for( nodePtr = ((xmlNode *)keyPtr)->children; nodePtr; nodePtr = nodePtr->next ) 
    {

        if( nodePtr->type == XML_ELEMENT_NODE ) 
        {
            printf( "node type: Element, name: %s\n", nodePtr->name );

            if( xmlStrEqual( nodePtr->name, (xmlChar *)"action" ) )
            {
                xmlChar *cStr;

                cStr = xmlNodeGetContent( nodePtr );

                if( xmlStrEqual( cStr, (xmlChar *)"send-all" ) )
                {
                    keyBinding.setAction( MMK_ACTION_SEND_ALL );
                }
                else
                {
                    keyBinding.setAction( MMK_ACTION_NOP );
                }

                xmlFree( cStr );
            }
            else if( xmlStrEqual( nodePtr->name, (xmlChar *)"cmd" ) )
            {
                xmlChar *cStr;

                cStr = xmlNodeGetContent( nodePtr );

                if( xmlStrEqual( cStr, (xmlChar *)"schedule" ) )
                {
                    keyBinding.setCommand( MMK_CMD_SCHEDULE );
                }
                else
                {
                    keyBinding.setCommand( MMK_CMD_CLEAR );
                }

                xmlFree( cStr );
            }
            else if( xmlStrEqual( nodePtr->name, (xmlChar *)"param1" ) )
            {
                xmlChar *cStr;
                uint32_t value;

                cStr = xmlNodeGetContent( nodePtr );

                value = strtol( (const char *)cStr, NULL, 0 );
                keyBinding.setParam1( value );

                xmlFree( cStr );
            }
            else if( xmlStrEqual( nodePtr->name, (xmlChar *)"start-delay" ) )
            {
                xmlChar *cStr;
                uint32_t value;

                cStr = xmlNodeGetContent( nodePtr );

                value = strtol( (const char *)cStr, NULL, 0 );
                keyBinding.setStartDelay( value );

                xmlFree( cStr );
            }
        }

    }
}

bool 
CRLEDConfigFile::parseMidiKeyMap( void *mapPtr )
{
    xmlNode *nodePtr = NULL;

    // Traverse the document to pull out the items of interest
    for( nodePtr = ((xmlNode *)mapPtr)->children; nodePtr; nodePtr = nodePtr->next ) 
    {
        if( nodePtr->type == XML_ELEMENT_NODE ) 
        {
            printf( "node type: Element, name: %s\n", nodePtr->name );

            if( xmlStrEqual( nodePtr->name, (xmlChar *)"key" ) )
            {
                CRLEDMidiKeyBinding keyBinding;
       
                parseMidiKey( nodePtr, keyBinding );

                keyMap.setKeyBinding( keyBinding.getKeyCode(), keyBinding );
            }
            else if( xmlStrEqual( nodePtr->name, (xmlChar *)"key-default" ) )
            {
                CRLEDMidiKeyBinding keyBinding;
       
                parseMidiKey( nodePtr, keyBinding );

                keyMap.setDefaultKeyBinding( keyBinding );
            }

        }
    }
}

bool 
CRLEDConfigFile::load()
{
    xmlDoc  *doc          = NULL;
    xmlNode *root_element = NULL;
    xmlNode *cur_node     = NULL;

    // this initialize the library and check potential ABI mismatches
    // between the version it was compiled for and the actual shared
    // library used.
    LIBXML_TEST_VERSION

    // parse the file and get the DOM
    doc = xmlReadFile( "/etc/crled/ctrl.conf", NULL, 0 );

    if( doc == NULL ) 
    {
        printf( "error: could not parse file %s\n", "/etc/crledctrl.conf" );
        return true;
    }

    // Get the root element node 
    root_element = xmlDocGetRootElement( doc );

    // Traverse the document to pull out the items of interest
    for( cur_node = root_element->children; cur_node; cur_node = cur_node->next ) 
    {
        if( cur_node->type == XML_ELEMENT_NODE ) 
        {
            printf( "node type: Element, name: %s\n", cur_node->name );

            if( xmlStrEqual( cur_node->name, (xmlChar *)"client-list" ) )
            {
                parseClientList( cur_node );
            }
            else if( xmlStrEqual( cur_node->name, (xmlChar *)"midi-key-map" ) == 0 )
            {
                parseMidiKeyMap( cur_node );
            } 

        }

    }

    //print_element_names( root_element );

    // free the document 
    xmlFreeDoc( doc );

    // Free the global variables that may
    // have been allocated by the parser.
    xmlCleanupParser();

    return false;
}

bool 
CRLEDConfigFile::getLEDEndpointAddrList( std::vector< struct sockaddr_in > &addrList )
{
    sockaddr_in addr;

    addrList.clear();

    for( std::vector< CRLEDClientConfig >::iterator it = clientList.begin(); it != clientList.end(); it++ )
    {
        it->getLEDEndpointAddress( addr );
        addrList.push_back( addr );
    }
}

CRLEDMidiKeyBinding& 
CRLEDConfigFile::getKeyBinding( uint32_t keyCode )
{
    return keyMap.getKeyBinding( keyCode );
}


