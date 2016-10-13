#include <string>
#include <iostream>
#include <fstream>
using namespace std;

#include "RootResource.hpp"

RootResource::RootResource()
{
    setURLPattern( "/*", (REST_RMETHOD_T)(REST_RMETHOD_GET) ); 
}

RootResource::~RootResource()
{

}

void 
RootResource::restGet( RESTRequest *request )
{
    std::string resp;

    cout << "RootResource::restGet -- start" << std::endl;

    // Build response data
    resp = "<html><head></head><h1>Hello</h1><body></body></html>";

    RESTRepresentation *rspRep = request->getOutboundRepresentation();
    rspRep->setSimpleContent( "text/html", (unsigned char *) resp.c_str(), resp.size() );

    request->setResponseCode( REST_HTTP_RCODE_OK );
    request->sendResponse();

    return;
}

