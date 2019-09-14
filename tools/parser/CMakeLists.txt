INCLUDE(FindProtobuf)

FIND_PACKAGE(Protobuf REQUIRED)

file(GLOB SOURCES main.cpp)

add_executable(parser ${SOURCES})

target_link_libraries(parser usb2jtag pthread usb-1.0 ${PROTOBUF_LIBRARY})

