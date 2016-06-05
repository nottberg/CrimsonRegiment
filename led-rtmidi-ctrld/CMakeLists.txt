# CMakeLists file for cr-led-rtmidi-ctrld
CMAKE_MINIMUM_REQUIRED ( VERSION 2.8.11 )
PROJECT ( cr-led-rtmidi-ctrld )

ADD_DEFINITIONS(-std=c++11)

SET( CR_LED_RTMIDI_CTRLD_SOURCE
     ${CMAKE_SOURCE_DIR}/src/midi_unicast.cpp )

ADD_EXECUTABLE( cr-led-rtmidi-ctrld ${CR_LED_RTMIDI_CTRLD_SOURCE} )

TARGET_LINK_LIBRARIES( cr-led-rtmidi-ctrld -lrtmidi )

