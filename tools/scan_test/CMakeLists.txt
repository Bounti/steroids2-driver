file(GLOB SOURCES main.cpp)

add_executable(scan_test ${SOURCES})

target_link_libraries(scan_test inception_driver_lib pthread usb-1.0)
