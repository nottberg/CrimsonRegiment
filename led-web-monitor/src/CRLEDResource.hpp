#ifndef __CRLEDRESOURCE_H__
#define __CRLEDRESOURCE_H__

#include "hnode-rest.hpp"

class CRLStatusResource : public RESTResource
{
    private:

    public:
        CRLStatusResource();
       ~CRLStatusResource();

        virtual void restGet( RESTRequest *request );

};

class CRLSequenceResource : public RESTResource
{
    private:

    public:
        CRLSequenceResource();
       ~CRLSequenceResource();

        virtual void restPut( RESTRequest *request );
        virtual void restDelete( RESTRequest *request );
};

#endif //__CRLEDRESOURCE_H__
