file(GLOB SOURCES main.cpp)

add_executable(io_tests ${SOURCES})

target_link_libraries(io_tests usb2jtag pthread usb-1.0)
