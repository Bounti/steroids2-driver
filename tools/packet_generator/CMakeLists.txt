INCLUDE(FindProtobuf)

file(GLOB SOURCES main.cpp)

add_executable(packet_generator ${SOURCES})

target_link_libraries(packet_generator usb2jtag pthread usb-1.0 ${PROTOBUF_LIBRARY})
