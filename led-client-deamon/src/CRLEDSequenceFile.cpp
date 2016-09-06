#include <string.h>

#include <iostream>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "CRLEDSequenceFile.hpp"

CRLSeqStep::CRLSeqStep()
{

}

CRLSeqStep::~CRLSeqStep()
{

}

bool 
CRLSeqStep::initFromStepNode( void *stepPtr )
{

}

LS_STEP_UPDATE_RESULT_T 
CRLSeqStep::initRT( CRLEDCommandPacket *cmdPkt )
{
    return LS_STEP_UPDATE_RESULT_DONE;
}

LS_STEP_UPDATE_RESULT_T 
CRLSeqStep::updateRT( struct timeval *curTime, LEDDriver *leds )
{
    return LS_STEP_UPDATE_RESULT_DONE;
}

CRLSSWaitForStart::CRLSSWaitForStart()
{

}

CRLSSWaitForStart::~CRLSSWaitForStart()
{

}

bool 
CRLSSWaitForStart::initFromStepNode( void *stepPtr )
{

}

LS_STEP_UPDATE_RESULT_T 
CRLSSWaitForStart::initRT( CRLEDCommandPacket *cmdPkt )
{
    startTime.tv_sec  = cmdPkt->getTSSec();
    startTime.tv_usec = cmdPkt->getTSUSec();

    return LS_STEP_UPDATE_RESULT_DONE;
}

LS_STEP_UPDATE_RESULT_T 
CRLSSWaitForStart::updateRT( struct timeval *curTime, LEDDriver *leds )
{
    std::cout << "LDStepWaitForStart::update" << std::endl;
    std::cout << curTime->tv_sec << ":" << startTime.tv_sec << std::endl;
    std::cout << curTime->tv_usec << ":" << startTime.tv_usec << std::endl;

    // If we are already behind by a full sec, then trigger
    if( curTime->tv_sec > startTime.tv_sec )
        return LS_STEP_UPDATE_RESULT_DONE;

    // If the seconds is the same, check the usec instead.
    if( curTime->tv_sec == startTime.tv_sec )
    {    
        if( curTime->tv_usec >= startTime.tv_usec )
            return LS_STEP_UPDATE_RESULT_DONE;
    }

    // Otherwise keep waiting.
    return LS_STEP_UPDATE_RESULT_CONT;
}

CRLSSClear::CRLSSClear()
{

}

CRLSSClear::~CRLSSClear()
{

}

bool 
CRLSSClear::initFromStepNode( void *stepPtr )
{

}

LS_STEP_UPDATE_RESULT_T 
CRLSSClear::initRT( CRLEDCommandPacket *cmdPkt )
{
    return LS_STEP_UPDATE_RESULT_DONE;
}

LS_STEP_UPDATE_RESULT_T 
CRLSSClear::updateRT( struct timeval *curTime, LEDDriver *leds )
{
    leds->clearAllPixels();
 
    return LS_STEP_UPDATE_RESULT_DONE;
}

CRLSSRegionChange::CRLSSRegionChange()
{
    action = SSRC_ACTION_NOT_SET;
}

CRLSSRegionChange::~CRLSSRegionChange()
{

}

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
      </sequence>
#endif

bool 
CRLSSRegionChange::parseRangeEntry( void *rangeNode )
{
    SSRC_RANGE_T entry;
    xmlNode *nodePtr = NULL;

    // Initialize the entry
    entry.startIndex = 0;
    entry.endIndex   = 0;
    entry.rsvd       = 0;
    entry.red        = 255;
    entry.green      = 255;
    entry.blue       = 255;


    // Traverse the document to pull out the items of interest
    for( nodePtr = ((xmlNode *)rangeNode)->children; nodePtr; nodePtr = nodePtr->next ) 
    {
        if( nodePtr->type == XML_ELEMENT_NODE ) 
        {
            printf( "node type: Element, name: %s\n", nodePtr->name );

            if( xmlStrEqual( nodePtr->name, (xmlChar *)"start" ) )
            {
                xmlChar *cStr;

                cStr = xmlNodeGetContent( nodePtr );

                entry.startIndex = strtol( (const char *)cStr, NULL, 0 );

                xmlFree( cStr );
            }
            else if( xmlStrEqual( nodePtr->name, (xmlChar *)"end" ) )
            {
                xmlChar *cStr;

                cStr = xmlNodeGetContent( nodePtr );

                entry.endIndex = strtol( (const char *)cStr, NULL, 0 );

                xmlFree( cStr );
            } 
            else if( xmlStrEqual( nodePtr->name, (xmlChar *)"color" ) )
            {
                xmlNode *colorPtr = NULL;

                // Default to off when color is specified
                entry.red   = 0;
                entry.green = 0;
                entry.blue  = 0;

                // Traverse the document to pull out the items of interest
                for( colorPtr = ((xmlNode *)nodePtr)->children; colorPtr; colorPtr = colorPtr->next ) 
                {
                    if( colorPtr->type == XML_ELEMENT_NODE ) 
                    {
                        printf( "node type: Element, name: %s\n", colorPtr->name );

                        if( xmlStrEqual( colorPtr->name, (xmlChar *)"red" ) )
                        {
                            xmlChar *cStr;

                            cStr = xmlNodeGetContent( colorPtr );

                            uint32_t value = strtol( (const char *)cStr, NULL, 0 );

                            if( value > 255 )
                                value = 255;

                            entry.red = (uint8_t) value;

                            xmlFree( cStr );     
                        }
                        else if( xmlStrEqual( colorPtr->name, (xmlChar *)"green" ) )
                        {
                            xmlChar *cStr;

                            cStr = xmlNodeGetContent( colorPtr );

                            uint32_t value = strtol( (const char *)cStr, NULL, 0 );

                            if( value > 255 )
                                value = 255;

                            entry.green = (uint8_t) value;

                            xmlFree( cStr );     
                        }
                        else if( xmlStrEqual( colorPtr->name, (xmlChar *)"blue" ) )
                        {
                            xmlChar *cStr;

                            cStr = xmlNodeGetContent( colorPtr );

                            uint32_t value = strtol( (const char *)cStr, NULL, 0 );

                            if( value > 255 )
                                value = 255;

                            entry.blue = (uint8_t) value;

                            xmlFree( cStr );     
                        }
                     }
                }

            }
        }
    }

    rangeList.push_back( entry );
}

bool 
CRLSSRegionChange::initFromStepNode( void *stepPtr )
{
    xmlNode *nodePtr = NULL;

    // Traverse the document to pull out the items of interest
    for( nodePtr = ((xmlNode *)stepPtr)->children; nodePtr; nodePtr = nodePtr->next ) 
    {
        if( nodePtr->type == XML_ELEMENT_NODE ) 
        {
            printf( "node type: Element, name: %s\n", nodePtr->name );

            if( xmlStrEqual( nodePtr->name, (xmlChar *)"action" ) )
            {
                xmlChar *cStr;

                cStr = xmlNodeGetContent( nodePtr );

                if( xmlStrEqual( cStr, (xmlChar *)"on" ) )
                {
                    action = SSRC_ACTION_ON;
                }
                else if( xmlStrEqual( cStr, (xmlChar *)"off" ) )
                {
                    action = SSRC_ACTION_OFF;
                }

                xmlFree( cStr );
            }
            else if( xmlStrEqual( nodePtr->name, (xmlChar *)"range-list" ) )
            {
                xmlNode *rangePtr = NULL;

                // Traverse the document to pull out the items of interest
                for( rangePtr = ((xmlNode *)nodePtr)->children; rangePtr; rangePtr = rangePtr->next ) 
                {
                    if( rangePtr->type == XML_ELEMENT_NODE ) 
                    {
                        printf( "node type: Element, name: %s\n", rangePtr->name );

                        if( xmlStrEqual( rangePtr->name, (xmlChar *)"range" ) )
                        {
                            parseRangeEntry( rangePtr );
                        }
                     }
                }
            }
        }
    }
}

LS_STEP_UPDATE_RESULT_T 
CRLSSRegionChange::initRT( CRLEDCommandPacket *cmdPkt )
{
    return LS_STEP_UPDATE_RESULT_DONE;
}

LS_STEP_UPDATE_RESULT_T 
CRLSSRegionChange::updateRT( struct timeval *curTime, LEDDriver *leds )
{
    std::cout << "LDStepRegionChange::update" << std::endl;

    for( std::vector< SSRC_RANGE_T >::iterator it = rangeList.begin(); it != rangeList.end(); it++ )
    {
        uint32_t lastIndex = it->endIndex;

        if( lastIndex >= leds->getPixelCount() )
            lastIndex = ( leds->getPixelCount() - 1 );

        std::cout << it->startIndex << "  " << lastIndex << "  " << (uint32_t)it->red << " " << (uint32_t)it->green << " " << (uint32_t)it->blue <<  std::endl;

        for( int x = it->startIndex; x < lastIndex; x++ )
        { 
            switch( action )
            {
                case SSRC_ACTION_ON:
                    leds->setPixel( x, it->red, it->green, it->blue );
                break;

                case SSRC_ACTION_OFF:
                    leds->clearPixel( x );
                break;
            }
        }
    }

    return LS_STEP_UPDATE_RESULT_DONE;
}

CRLSSLinearFill::CRLSSLinearFill()
{
    red   = 255;
    green = 255;
    blue  = 255;

    startIndex = 0;
    increment  = 1;
    iterations = 40;
    delay      = 100;
}

CRLSSLinearFill::~CRLSSLinearFill()
{

}

bool 
CRLSSLinearFill::initFromStepNode( void *stepPtr )
{
    xmlNode *nodePtr = NULL;

    // Traverse the document to pull out the items of interest
    for( nodePtr = ((xmlNode *)stepPtr)->children; nodePtr; nodePtr = nodePtr->next ) 
    {
        if( nodePtr->type == XML_ELEMENT_NODE ) 
        {
            printf( "node type: Element, name: %s\n", nodePtr->name );

            if( xmlStrEqual( nodePtr->name, (xmlChar *)"start" ) )
            {
                xmlChar *cStr;

                cStr = xmlNodeGetContent( nodePtr );

                startIndex = strtol( (const char *)cStr, NULL, 0 );

                xmlFree( cStr );
            }
            else if( xmlStrEqual( nodePtr->name, (xmlChar *)"increment" ) )
            {
                xmlChar *cStr;

                cStr = xmlNodeGetContent( nodePtr );

                increment = strtol( (const char *)cStr, NULL, 0 );

                xmlFree( cStr );
            }
            else if( xmlStrEqual( nodePtr->name, (xmlChar *)"interations" ) )
            {
                xmlChar *cStr;

                cStr = xmlNodeGetContent( nodePtr );

                iterations = strtol( (const char *)cStr, NULL, 0 );

                xmlFree( cStr );
            } 
            else if( xmlStrEqual( nodePtr->name, (xmlChar *)"delay" ) )
            {
                xmlChar *cStr;

                cStr = xmlNodeGetContent( nodePtr );

                delay = strtol( (const char *)cStr, NULL, 0 );

                xmlFree( cStr );
            } 
            else if( xmlStrEqual( nodePtr->name, (xmlChar *)"color" ) )
            {
                xmlNode *colorPtr = NULL;

                // Default to off when color is specified
                red   = 0;
                green = 0;
                blue  = 0;

                // Traverse the document to pull out the items of interest
                for( colorPtr = ((xmlNode *)nodePtr)->children; colorPtr; colorPtr = colorPtr->next ) 
                {
                    if( colorPtr->type == XML_ELEMENT_NODE ) 
                    {
                        printf( "node type: Element, name: %s\n", colorPtr->name );

                        if( xmlStrEqual( colorPtr->name, (xmlChar *)"red" ) )
                        {
                            xmlChar *cStr;

                            cStr = xmlNodeGetContent( colorPtr );

                            uint32_t value = strtol( (const char *)cStr, NULL, 0 );

                            if( value > 255 )
                                value = 255;

                            red = (uint8_t) value;

                            xmlFree( cStr );     
                        }
                        else if( xmlStrEqual( colorPtr->name, (xmlChar *)"green" ) )
                        {
                            xmlChar *cStr;

                            cStr = xmlNodeGetContent( colorPtr );

                            uint32_t value = strtol( (const char *)cStr, NULL, 0 );

                            if( value > 255 )
                                value = 255;

                            green = (uint8_t) value;

                            xmlFree( cStr );     
                        }
                        else if( xmlStrEqual( colorPtr->name, (xmlChar *)"blue" ) )
                        {
                            xmlChar *cStr;

                            cStr = xmlNodeGetContent( colorPtr );

                            uint32_t value = strtol( (const char *)cStr, NULL, 0 );

                            if( value > 255 )
                                value = 255;

                            blue = (uint8_t) value;

                            xmlFree( cStr );     
                        }
                     }
                }

            }
        }
    }

}

LS_STEP_UPDATE_RESULT_T 
CRLSSLinearFill::initRT( CRLEDCommandPacket *cmdPkt )
{
    curiter  = 0;
    nextIndx = startIndex;
}

LS_STEP_UPDATE_RESULT_T 
CRLSSLinearFill::updateRT( struct timeval *curTime, LEDDriver *leds )
{
    std::cout << "CRLSSLinearFill::updateRT - " << curTime->tv_sec << ":" << nextTime.tv_sec << std::endl;
    std::cout << "CRLSSLinearFill::updateRT - " << curTime->tv_usec << ":" << nextTime.tv_usec << std::endl;
    std::cout << "CRLSSLinearFill::updateRT - " << curiter << "  " << nextIndx << std::endl;

    // Check if we have a time stamp to wait for
    if( curiter > 0 )
    {
        // If we are already behind by a full sec, then trigger
        if( curTime->tv_sec < nextTime.tv_sec )
            return LS_STEP_UPDATE_RESULT_CONT;

        // If the seconds is the same, check the usec instead.
        if( curTime->tv_sec == nextTime.tv_sec )
        {    
            if( curTime->tv_usec < nextTime.tv_usec )
                return LS_STEP_UPDATE_RESULT_CONT;
        }

        // Otherwise fall through and take the next step
    }
    else
    {
        // Grab the initial time stamp to base things on.
        nextTime.tv_sec  = curTime->tv_sec;
        nextTime.tv_usec = curTime->tv_usec;
    }

    // Turn on the next led
    leds->setPixel( nextIndx, red, green, blue );

    // Move to the next iteration
    curiter += 1;

    // Check if we are done with this step.
    if( curiter >= iterations )
        return LS_STEP_UPDATE_RESULT_DONE;

    // Calculate the next update parameters
    nextIndx += increment;

    nextTime.tv_usec += ( delay * 1000 );
    while( nextTime.tv_usec >= 1000000 )
    {
        nextTime.tv_sec  += 1;
        nextTime.tv_usec -= 1000000; 
    }  

    // Still in this step wait for next update
    return LS_STEP_UPDATE_RESULT_CONT;
}

CRLSSDwell::CRLSSDwell()
{

}

CRLSSDwell::~CRLSSDwell()
{

}

bool 
CRLSSDwell::initFromStepNode( void *stepPtr )
{

}

LS_STEP_UPDATE_RESULT_T 
CRLSSDwell::initRT( CRLEDCommandPacket *cmdPkt )
{
    return LS_STEP_UPDATE_RESULT_DONE;
}

LS_STEP_UPDATE_RESULT_T 
CRLSSDwell::updateRT( struct timeval *curTime, LEDDriver *leds )
{
    return LS_STEP_UPDATE_RESULT_DONE;
}

CRLSSGoto::CRLSSGoto()
{

}

CRLSSGoto::~CRLSSGoto()
{

}

bool 
CRLSSGoto::initFromStepNode( void *stepPtr )
{

}

LS_STEP_UPDATE_RESULT_T 
CRLSSGoto::initRT( CRLEDCommandPacket *cmdPkt )
{
    return LS_STEP_UPDATE_RESULT_DONE;
}

LS_STEP_UPDATE_RESULT_T 
CRLSSGoto::updateRT( struct timeval *curTime, LEDDriver *leds )
{
    return LS_STEP_UPDATE_RESULT_DONE;
}

CRLSSTransform::CRLSSTransform()
{

}

CRLSSTransform::~CRLSSTransform()
{

}

bool 
CRLSSTransform::initFromStepNode( void *stepPtr )
{

}

LS_STEP_UPDATE_RESULT_T 
CRLSSTransform::initRT( CRLEDCommandPacket *cmdPkt )
{
    return LS_STEP_UPDATE_RESULT_DONE;
}

LS_STEP_UPDATE_RESULT_T 
CRLSSTransform::updateRT( struct timeval *curTime, LEDDriver *leds )
{
    return LS_STEP_UPDATE_RESULT_DONE;
}

CRLSeqStep* 
CRLStepFactory::allocateStepForType( std::string type )
{
    if( type == "wait-for-start" )
        return new CRLSSWaitForStart;
    else if( type == "clear" )
        return new CRLSSClear;
    else if( type == "region-change" )
        return new CRLSSRegionChange;
    else if( type == "dwell" )
        return new CRLSSDwell;
    else if( type == "goto" )
        return new CRLSSGoto;
    else if( type == "transform" )
        return new CRLSSTransform;
    else if( type == "linear-fill" )
        return new CRLSSLinearFill;
    
    return new CRLSeqStep;
}

void 
CRLStepFactory::freeStep( CRLSeqStep *stepPtr )
{
    delete stepPtr;
}

CRLSeqRecord::CRLSeqRecord()
{
//std::vector< CRLSeqStep > stepList;
}

CRLSeqRecord::~CRLSeqRecord()
{

}

void 
CRLSeqRecord::initialize()
{
    for( std::vector< CRLSeqStep* >::iterator it = stepList.begin(); it != stepList.end(); it++ )
    {
        CRLStepFactory::freeStep( *it );
    }

    printf( "initialize\n" );

    stepList.clear();
}

void 
CRLSeqRecord::appendStep( CRLSeqStep *stepObj )
{
    stepList.push_back( stepObj );
}

LS_SEQ_UPDATE_RESULT_T 
CRLSeqRecord::activateRT( CRLEDCommandPacket *cmdPkt, struct timeval *curTime, LEDDriver *leds )
{
    std::cout << "CRLSeqRecord::activateRT" << std::endl;

    // Back to the beginning
    activeStep = 0;

    // Initialize all of the steps
    for( std::vector< CRLSeqStep* >::iterator it = stepList.begin(); it != stepList.end(); it++ )
    {
        (*it)->initRT( cmdPkt );
    }

    // Run an update step in case we are behind
    return updateRT( curTime, leds );
}

LS_SEQ_UPDATE_RESULT_T 
CRLSeqRecord::updateRT( struct timeval *curTime, LEDDriver *leds )
{
    // Roll through steps until we are told to wait.
    while( (activeStep != LS_STEP_NOT_ACTIVE) && (activeStep < stepList.size() ) )
    {
        std::cout << "LEDSequence::updateStep" << std::endl;

        // Execute the current step and see what it indicates to do.
        switch( stepList[ activeStep ]->updateRT( curTime, leds ) )
        {
            // Done with the current step, move to the next one.
            case LS_STEP_UPDATE_RESULT_DONE:
            {
                activeStep += 1;

                std::cout << activeStep << "  " << stepList.size() << std::endl;

                if( activeStep >= stepList.size() )
                    activeStep = LS_STEP_NOT_ACTIVE;
            }
            break;

            // Continue with the current step at next time boundary.
            case LS_STEP_UPDATE_RESULT_CONT:
                return LS_SEQ_UPDATE_RESULT_CONT;
            break;
        }
    }

    // The sequence is complete.
    return LS_SEQ_UPDATE_RESULT_DONE;
}

CRLSeqNode::CRLSeqNode()
{
    activeSeqIndx = LS_SEQ_NOT_ACTIVE;

    setMaximumSequenceIndex( 10 );
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

void 
CRLSeqNode::setMaximumSequenceIndex( uint32_t value )
{
    maxSeqIndx = value;
    seqList.resize( maxSeqIndx );

    printf( "setMaximumSequenceIndex: %d, %d\n", value, seqList.size() );
}

uint32_t 
CRLSeqNode::getMaximumSequenceIndex()
{
    return maxSeqIndx;
}

void 
CRLSeqNode::newSequence( uint32_t seqIndx )
{
    if( seqIndx >= maxSeqIndx )
        return;

    printf( "newSequence: %d, %d\n", seqIndx, seqList.size() );

    seqList[seqIndx].initialize();
}

void 
CRLSeqNode::appendStepToSequence( uint32_t seqIndx, CRLSeqStep *stepObj )
{
    if( seqIndx >= maxSeqIndx )
        return;

    seqList[ seqIndx ].appendStep( stepObj );
}

void 
CRLSeqNode::activateSequence( uint32_t seqIndx, CRLEDCommandPacket *cmdPkt, struct timeval *curTime, LEDDriver *leds )
{
    if( seqIndx >= maxSeqIndx )
        return;

    activeSeqIndx = seqIndx;

    seqList[ activeSeqIndx ].activateRT( cmdPkt, curTime, leds );
}

void 
CRLSeqNode::clearActiveSequence()
{
    activeSeqIndx = LS_SEQ_NOT_ACTIVE;
}

void 
CRLSeqNode::updateSequence( struct timeval *curTime, LEDDriver *leds )
{
    // If no active sequence then nothing to do.
    if( activeSeqIndx == LS_SEQ_NOT_ACTIVE )
        return;

    // If there is an active sequence then call 
    // it for next steps.
    seqList[ activeSeqIndx ].updateRT( curTime, leds );
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

    printf( "parseseq index: %s\n", kcStr );

    if( kcStr == NULL )
    {
        return true;
    }

    index = strtol( (const char *)kcStr, NULL, 0 );
    xmlFree( kcStr );

    node.newSequence( index );

    printf( "parseseq index2: %d\n", index );
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

            CRLSeqStep *stepObj = CRLStepFactory::allocateStepForType( (const char*) nodePtr->name );

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
                xmlChar *tmpStr;

                // Grab the required id attribute
                tmpStr = xmlGetProp( nodePtr, (xmlChar *)"id" );

                if( tmpStr == NULL )
                {
                    continue;
                }

                // Set the type
                nodeObj.setID( (const char*)tmpStr );

                xmlFree( tmpStr );

                if( nodeObj.getID() != nodeID )
                {
                    continue;
                }

                // Check if the optional max-seqindx attribute has given
                tmpStr = xmlGetProp( nodePtr, (xmlChar *)"max-seqindx" );

                if( tmpStr != NULL )
                {
                    uint32_t maxIndx = strtol( (const char*)tmpStr, NULL, 0 );

                    if( maxIndx > 0 && maxIndx <= 64 )
                        nodeObj.setMaximumSequenceIndex( maxIndx );

                    xmlFree( tmpStr );
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

    printf( "Sequence File load complete\n" );

    return false;
}

CRLSeqNode*
CRLEDSequenceFile::getNodeConfig( std::string id )
{
    std::map< std::string, CRLSeqNode>::iterator it = nodeMap.find( id );

    if( it == nodeMap.end() )
        return NULL;

    return &(it->second);
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

