#ifndef __ROOTRESOURCE_H__
#define __ROOTRESOURCE_H__

#include "hnode-rest.hpp"

class RootResource : public RESTResource
{
    private:

    public:
        RootResource();
       ~RootResource();

        virtual void restGet( RESTRequest *request );

};

#endif //__ROOTRESOURCE_H__
