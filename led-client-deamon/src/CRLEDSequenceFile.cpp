#include <string.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "CRLEDSequenceFile.hpp"

CRLSeqStep::CRLSeqStep()
{

}

CRLSeqStep::~CRLSeqStep()
{

}

CRLSeqRecord::CRLSeqRecord()
{
//std::vector< CRLSeqStep > stepList;
}

CRLSeqRecord::~CRLSeqRecord()
{

}

CRLSeqNode::CRLSeqNode()
{
// std::string nodeID;
// std::vector< CRLSeqRecord > seqList;
}

CRLSeqNode::~CRLSeqNode()
{

}

void 
CRLSeqNode::setID( std::string value )
{
    nodeID = value;
}

std::string 
CRLSeqNode::getID()
{
    return nodeID;
}

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
#endif

bool 
CRLEDSequenceFile::parseSequence( void *seqPtr, CRLSeqNode &node )
{
    xmlNode  *nodePtr = NULL;
    xmlChar  *kcStr;
    uint32_t  index;

    // Extract the keycode
    kcStr = xmlGetProp( (xmlNode *)seqPtr, (xmlChar *)"index" );

    if( kcStr == NULL )
    {
        return true;
    }

    index = strtol( (const char *)kcStr, NULL, 0 );
    xmlFree( kcStr );

    node.startSequence( index );

#if 0
        <clear/>
        <region-change>
          <action>on</action>
          <range-list>
            <range>
              <start>0</start>
              <end>20</end>
            </range>
          </range-list>
        </region-change>
#endif

    // Traverse the document to pull out the items of interest
    for( nodePtr = ((xmlNode *)seqPtr)->children; nodePtr; nodePtr = nodePtr->next ) 
    {
        if( nodePtr->type == XML_ELEMENT_NODE ) 
        {
            printf( "node type: Element, name: %s\n", nodePtr->name );

            CRLSeqStep *stepObj = createStepForType( (const char*) nodePtr->name );

            if( stepObj == NULL )
            {
                // Unrecognized step type
                continue;
            }

            stepObj->initFromStepNode( nodePtr );

            node.appendStepToSequence( index, stepObj );
        }
    }
}

bool 
CRLEDSequenceFile::parseNode( void *nodePtr, CRLSeqNode &node )
{
    xmlNode  *tmpPtr = NULL;

    // Traverse the document to pull out the items of interest
    for( tmpPtr = ((xmlNode *)nodePtr)->children; tmpPtr; tmpPtr = tmpPtr->next ) 
    {
        if( tmpPtr->type == XML_ELEMENT_NODE ) 
        {
            printf( "node type: Element, name: %s\n", tmpPtr->name );

            if( xmlStrEqual( tmpPtr->name, (xmlChar *)"sequence" ) )
            {
                parseSequence( tmpPtr, node );
            }
        }
    }
}

bool 
CRLEDSequenceFile::parseNodeList( void *listPtr, std::string nodeID )
{
    xmlNode *nodePtr = NULL;

    printf( "parseNodeList\n" );

    // Traverse the document to pull out the items of interest
    for( nodePtr = ((xmlNode *)listPtr)->children; nodePtr; nodePtr = nodePtr->next ) 
    {
        if( nodePtr->type == XML_ELEMENT_NODE ) 
        {
            printf( "node type: Element, name: %s\n", nodePtr->name );

            if( xmlStrEqual( nodePtr->name, (xmlChar *)"node" ) )
            {
                CRLSeqNode nodeObj;
                xmlChar *tmpID;

                // Grab the required id attribute
                tmpID = xmlGetProp( nodePtr, (xmlChar *)"id" );

                if( tmpID == NULL )
                {
                    continue;
                }

                // Set the type
                nodeObj.setID( (const char*)tmpID );

                xmlFree( tmpID );

                if( nodeObj.getID() != nodeID )
                {
                    continue;
                }

                // Parse Node
                parseNode( nodePtr, nodeObj );
       
                // Save the node object
                nodeMap.insert( std::pair< std::string, CRLSeqNode>( nodeObj.getID(), nodeObj ) );
            }
        }
    }
}

bool 
CRLEDSequenceFile::load( std::string nodeID )
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

            if( xmlStrEqual( cur_node->name, (xmlChar *)"node-list" ) )
            {
                parseNodeList( cur_node, nodeID );
            }
        }
    }

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

