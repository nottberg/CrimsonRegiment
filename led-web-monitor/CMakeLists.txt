# CMakeLists file for cr-led-rtmidi-ctrld
CMAKE_MINIMUM_REQUIRED ( VERSION 2.8.11 )
PROJECT ( cr-led-webmon )

FIND_PACKAGE( LibXml2 REQUIRED )
FIND_PACKAGE( Boost REQUIRED COMPONENTS program_options regex )

ADD_DEFINITIONS(-std=c++11)

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES( ${LIBXML2_INCLUDE_DIR} )

SET( CR_LED_WEBMON_SOURCE
     ${CMAKE_SOURCE_DIR}/src/HNRESTContent.cpp
     ${CMAKE_SOURCE_DIR}/src/HNRESTDeamon.cpp
     ${CMAKE_SOURCE_DIR}/src/HNRESTRepresentation.cpp
     ${CMAKE_SOURCE_DIR}/src/HNRESTRequest.cpp
     ${CMAKE_SOURCE_DIR}/src/HNRESTResource.cpp
     ${CMAKE_SOURCE_DIR}/src/CRLEDConfigFile.cpp
     ${CMAKE_SOURCE_DIR}/src/CRLEDPacket.cpp
     ${CMAKE_SOURCE_DIR}/src/RootResource.cpp
     ${CMAKE_SOURCE_DIR}/src/CRLEDResource.cpp
     ${CMAKE_SOURCE_DIR}/src/WebMonitor.cpp )

ADD_EXECUTABLE( cr-led-webmond ${CR_LED_WEBMON_SOURCE} )

TARGET_LINK_LIBRARIES( cr-led-webmond -lmicrohttpd )
TARGET_LINK_LIBRARIES( cr-led-webmond ${Boost_LIBRARIES} )
TARGET_LINK_LIBRARIES( cr-led-webmond ${LIBXML2_LIBRARIES} )

