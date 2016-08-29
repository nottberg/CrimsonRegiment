#include <string.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "CRLEDSequenceFile.hpp"


CRLEDSequenceFile::CRLEDSequenceFile()
{

}

CRLEDSequenceFile::~CRLEDSequenceFile()
{

}

void 
CRLEDSequenceFile::setSequenceFilePath()
{

}

#if 0
bool 
CRLEDSequenceFile::parseClientNode( void *clientPtr )
{
    xmlNode *nodePtr = NULL;
    CRLEDClientConfig clientRecord;

    // Traverse the document to pull out the items of interest
    for( nodePtr = ((xmlNode *)clientPtr)->children; nodePtr; nodePtr = nodePtr->next ) 
    {
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
    }

    clientList.push_back( clientRecord );
}

bool 
CRLEDSequenceFile::parseClientList( void *listPtr )
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
CRLEDSequenceFile::parseMidiKey( void *keyPtr, CRLEDMidiKeyBinding &keyBinding )
{
    xmlNode  *nodePtr = NULL;
    xmlChar  *kcStr;

    // Extract the keycode
    kcStr = xmlGetProp( (xmlNode *)keyPtr, (xmlChar *)"code" );

    if( kcStr != NULL )
    {
        uint32_t  keyCode;

        keyCode = strtol( (const char *)kcStr, NULL, 0 );
        xmlFree( kcStr );

        keyBinding.setKeyCode( keyCode );
    }

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
CRLEDSequenceFile::parseMidiKeyMap( void *mapPtr )
{
    xmlNode *nodePtr = NULL;

    printf( "parseMidiKeyMap\n" );

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
#endif

bool 
CRLEDSequenceFile::load( std::string nodeid )
{
    xmlDoc  *doc          = NULL;
    xmlNode *root_element = NULL;
    xmlNode *cur_node     = NULL;

    // this initialize the library and check potential ABI mismatches
    // between the version it was compiled for and the actual shared
    // library used.
    LIBXML_TEST_VERSION

    // parse the file and get the DOM
    doc = xmlReadFile( "/etc/crled/sequence.xml", NULL, 0 );

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
//                parseClientList( cur_node );
            }
            else if( xmlStrEqual( cur_node->name, (xmlChar *)"midi-key-map" ) )
            {
//                parseMidiKeyMap( cur_node );
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

#if 0
bool 
CRLEDSequenceFile::getLEDEndpointAddrList( std::vector< struct sockaddr_in > &addrList )
{
    sockaddr_in addr;

    addrList.clear();

    for( std::vector< CRLEDClientConfig >::iterator it = clientList.begin(); it != clientList.end(); it++ )
    {
        printf( "getLEDEndpointAddrList\n" );
        it->getLEDEndpointAddress( addr );
        addrList.push_back( addr );
    }
}

CRLEDMidiKeyBinding& 
CRLEDSequenceFile::getKeyBinding( uint32_t keyCode )
{
    return keyMap.getKeyBinding( keyCode );
}
#endif

