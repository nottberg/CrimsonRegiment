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

class CRLSeqStep
{
    private:

    public:
        CRLSeqStep();
       ~CRLSeqStep();

        virtual bool initFromStepNode( void *stepPtr ); 
};

class CRLSeqRecord
{
    private:
        std::vector< CRLSeqStep > stepList;

    public:
        CRLSeqRecord();
       ~CRLSeqRecord();
};

class CRLSeqNode
{
    private:
        std::string nodeID;

        std::vector< CRLSeqRecord > seqList;

    public:
        CRLSeqNode();
       ~CRLSeqNode();

        void setID( std::string value );
        std::string getID();

        void startSequence( uint32_t seqIndx );
        void appendStepToSequence( uint32_t seqIndx, CRLSeqStep *stepObj );
};

class CRLEDSequenceFile
{
    private:
        std::map< std::string, CRLSeqNode> nodeMap;

        CRLSeqStep* createStepForType( std::string typeStr );

        bool parseSequence( void *seqPtr, CRLSeqNode &node );
        bool parseNode( void *nodePtr, CRLSeqNode &node );
        bool parseNodeList( void *listPtr, std::string nodeID );

    public:
        CRLEDSequenceFile();
       ~CRLEDSequenceFile();

        void setSequenceFilePath();

        bool load( std::string nodeID );

};

#endif // __CRLED_SEQUENCE_FILE_HPP__

