file(GLOB SOURCES main.cpp)

add_executable(io_perf ${SOURCES})

target_link_libraries(io_perf usb2jtag pthread usb-1.0)
