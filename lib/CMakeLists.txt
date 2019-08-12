#===------------------------------------------------------------------------===#
#
#
#===------------------------------------------------------------------------===#
# add_subdirectory(Protobuf)
# Protobuf build
# This file generate the protobuf-based parsing library
INCLUDE(FindProtobuf)

FIND_PACKAGE(Protobuf REQUIRED)

SET(GENERATED_DIR "${CMAKE_SOURCE_DIR}/lib/proto")
SET(CMAKE_CURRENT_BINARY_DIR ${GENERATED_DIR})

include_directories(${PROTOBUF_INCLUDE_DIR})
include_directories(${GENERATED_DIR})

PROTOBUF_GENERATE_CPP(PROTO_SRC1 PROTO_HEADER1 ../conf/jtag_cmd.proto)

SET_SOURCE_FILES_PROPERTIES(${PROTO_SRC1} ${PROTO_HEADER1} PROPERTIES GENERATED TRUE)

ADD_LIBRARY(inception_driver STATIC
  ${PROTO_SRC1}
  crafter.cpp
  device.cpp
  api.cpp
)

target_link_libraries(inception_driver pthread usb-1.0 ${PROTOBUF_LIBRARY})

install(TARGETS inception_driver DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

