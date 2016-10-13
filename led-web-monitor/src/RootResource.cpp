#include <string>
#include <iostream>
#include <fstream>
using namespace std;

#include "RootResource.hpp"

char gRootPage[] =
"<!DOCTYPE html>"
"<html>"
"<head>"
"    <style>"
"      table {"
"        border-collapse: collapse;"
"      }"
""
"      table, td, th {"
"        padding: 7px;"
"        border-bottom: 1px solid #ddd;"
"        text-align: center;"
"      }"
""
"      td.error {"
"        background-color: #f7a3aa;"
"      }"
""
"      td.ok {"
"        background-color: #b2f4c8;"
"      }"
"    </style>"
"    <script>"
"        function refreshStatus() {"
"          var xhttp = new XMLHttpRequest();"
"          xhttp.onreadystatechange = function() {"
"            if (this.readyState == 4 && this.status == 200) {"
"              var json = JSON.parse(this.responseText);"
"              var nodeTab = \"<table><tr><th>Address</th><th>Rsp Received</th><th>Error</th><th>Response Time (ms)</th></tr>\";"
"              for( var i = 0; i < json.nodes.length; i++ ) {"
"                  nodeTab += \"<tr>\";"
"                  nodeTab += \"<td>\" + json.nodes[i].addr + \"</td>\";"
""
"                  if( json.nodes[i].rspRX == \"false\" )"
"                    nodeTab += \"<td class=\\\"error\\\">no</td>\";"
"                   else"
"                    nodeTab += \"<td class=\\\"ok\\\">yes</td>\";"
""
"                  if( json.nodes[i].error == \"true\" )"
"                    nodeTab += \"<td class=\\\"error\\\">yes</td>\";"
"                   else"
"                    nodeTab += \"<td class=\\\"ok\\\">no</td>\";"
""
"                  nodeTab += \"<td>\" + json.nodes[i].rspMS + \"</td>\";"
"                  nodeTab += \"</tr>\";"
"               }"
"               nodeTab += \"</table>\";"
"              document.getElementById(\"nodeStatusTable\").innerHTML = nodeTab;"
"            }"
"          };"
"          xhttp.open(\"GET\", \"crled/status\", true);"
"          xhttp.send();"
"          }"
""
"        function startSequence() {"
"          var xhttp = new XMLHttpRequest();"
"          xhttp.onreadystatechange = function() {"
"            if (this.readyState == 4) {"
"              if( this.status == 200 )"
"              {"
"                document.getElementById(\"startSeqResult\").innerHTML = \"Request Complete\";"
"              }"
"              else"
"              {"
"                document.getElementById(\"startSeqResult\").innerHTML = \"Request Failed\";"
"              }"
"            }"
"          };"
"          xhttp.open(\"PUT\", \"crled/sequence\", true);"
"          xhttp.send();"
"          }"
""
"        function clearAll() {"
"          var xhttp = new XMLHttpRequest();"
"          xhttp.onreadystatechange = function() {"
"            if (this.readyState == 4) {"
"              if( this.status == 200 )"
"              {"
"                document.getElementById(\"clearSeqResult\").innerHTML = \"Request Complete\";"
"              }"
"              else"
"              {"
"                document.getElementById(\"clearSeqResult\").innerHTML = \"Request Failed\";"
"              }"
"            }"
"          };"
"          xhttp.open(\"DELETE\", \"crled/sequence\", true);"
"          xhttp.send();"
"          }"
"    </script>"
"</head>"
"<body>"
""
"<div id=\"crledctrl\">"
"  <h2>Node Status</h2>"
"  <button type=\"button\" onclick=\"refreshStatus()\">Refresh</button>"
"  <div id=\"nodeStatusTable\">"
"  </div>"
"  <hr/>"
"  <h2>Start Sequence</h2>"
"  <button type=\"button\" onclick=\"startSequence()\">Start Sequence</button>"
"  <div id=\"startSeqResult\">"
"  </div>"
"  <hr/>"
"  <h2>Clear Sequence</h2>"
"  <button type=\"button\" onclick=\"clearAll()\">Clear All</button>"
"  <div id=\"clearSeqResult\">"
"  </div>"
"</div>"
""
"</body>"
"</html>";

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
    //std::string resp;

    cout << "RootResource::restGet -- start" << std::endl;

    // Build response data
    //resp = "<html><head></head><h1>Hello</h1><body></body></html>";

    RESTRepresentation *rspRep = request->getOutboundRepresentation();
    rspRep->setSimpleContent( "text/html", (unsigned char *) gRootPage, sizeof(gRootPage) );

    request->setResponseCode( REST_HTTP_RCODE_OK );
    request->sendResponse();

    return;
}

