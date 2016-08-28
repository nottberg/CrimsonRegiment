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

/*
    <client>
      <addr type="ipv4">192.168.2.7</addr>
      <port type="led">10000</port>
    </client>
*/

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
//            else if( xmlStrCmp( cur_node->name, "" ) == 0 )
//            {

//            } 

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


