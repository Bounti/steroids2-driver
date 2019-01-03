ADD_LIBRARY(inception_driver_lib STATIC
  crafter.cpp
  device.cpp
  api.cpp
)

target_link_libraries(inception_driver_lib pthread usb-1.0)

install(TARGETS inception_driver_lib DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

