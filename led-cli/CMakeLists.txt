# CMakeLists file for cr-led-client
CMAKE_MINIMUM_REQUIRED ( VERSION 2.8.11 )
PROJECT ( cr-led-cli )

ADD_DEFINITIONS(-std=c++11)

SET( CR_LED_CLI_SOURCE
     ${CMAKE_SOURCE_DIR}/src/cr_led_cli.cpp )

ADD_EXECUTABLE( cr-led-cli ${CR_LED_CLI_SOURCE} )


