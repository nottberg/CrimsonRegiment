# CMakeLists file for cr-led-client
CMAKE_MINIMUM_REQUIRED ( VERSION 2.8.11 )
PROJECT ( cr-led-cli )

FIND_PACKAGE( Boost REQUIRED COMPONENTS program_options )

ADD_DEFINITIONS(-std=c++11)

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIRS} )

SET( CR_LED_CLI_SOURCE
     ${CMAKE_SOURCE_DIR}/src/cr_led_cli.cpp
     ${CMAKE_SOURCE_DIR}/src/CRLEDPacket.cpp )

ADD_EXECUTABLE( cr-led-cli ${CR_LED_CLI_SOURCE} )

TARGET_LINK_LIBRARIES( cr-led-cli ${Boost_LIBRARIES} )

