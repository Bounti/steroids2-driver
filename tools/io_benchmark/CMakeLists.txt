file(GLOB SOURCES main.cpp)

add_executable(io_benchmark ${SOURCES})

target_link_libraries(io_benchmark usb2jtag pthread usb-1.0)
