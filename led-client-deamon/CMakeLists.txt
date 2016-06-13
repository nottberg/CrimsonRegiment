# CMakeLists file for cr-led-client
CMAKE_MINIMUM_REQUIRED ( VERSION 2.8.11 )
PROJECT ( cr-led-clientd )

ADD_DEFINITIONS(-std=c++11)

SET( CR_LED_CLIENTD_SOURCE
     ${CMAKE_SOURCE_DIR}/src/ledDriver.cpp
     ${CMAKE_SOURCE_DIR}/src/EventLoop.cpp
     ${CMAKE_SOURCE_DIR}/src/CRLEDPacket.cpp
     ${CMAKE_SOURCE_DIR}/src/ClientEvents.cpp
     ${CMAKE_SOURCE_DIR}/src/ClientMain.cpp
     ${CMAKE_SOURCE_DIR}/src/LEDSequencer.cpp
     ${CMAKE_SOURCE_DIR}/src/client.cpp )

ADD_EXECUTABLE( cr-led-clientd ${CR_LED_CLIENTD_SOURCE} )

TARGET_LINK_LIBRARIES( cr-led-clientd -levent )

