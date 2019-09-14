file(GLOB SOURCES main.cpp)

add_executable(tester ${SOURCES})

target_link_libraries(tester usb2jtag pthread usb-1.0)
