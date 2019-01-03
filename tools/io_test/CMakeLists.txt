file(GLOB SOURCES main.cpp)

add_executable(io_tests ${SOURCES})

target_link_libraries(io_tests inception_driver_lib pthread usb-1.0)
