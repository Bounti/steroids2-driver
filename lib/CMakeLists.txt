ADD_LIBRARY(inception_driver STATIC
  crafter.cpp
  device.cpp
  api.cpp
)

target_link_libraries(inception_driver pthread usb-1.0)

install(TARGETS inception_driver DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

