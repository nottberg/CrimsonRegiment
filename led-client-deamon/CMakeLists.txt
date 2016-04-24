# CMakeLists file for cr-led-client
CMAKE_MINIMUM_REQUIRED ( VERSION 2.8.11 )
PROJECT ( cr-led-clientd )

ADD_EXECUTABLE( cr-led-clientd src/client.cpp src/ledDriver.cpp )

TARGET_LINK_LIBRARIES( cr-led-clientd -levent )

