file(GLOB SOURCES main.cpp)

add_executable(inception-dbg-cli ${SOURCES})

target_link_libraries(inception-dbg-cli driver_lib pthread usb-1.0)
