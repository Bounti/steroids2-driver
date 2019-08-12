//
//  crafter.cpp
//  debugger-driver2
//
//  Created by <author> on 05/10/2018.
//
//

#include "crafter.hpp"
#include "proto/jtag_cmd.pb.h"

#include <map>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <string>


#define READ  0
#define WRITE 1
#define RESET 2

using namespace std;
using namespace usb2jtag;

void error(char *f, int l, char *e) {
  printf("[ERROR] In file %s at line %d, m_assertion failed : %s ", f, l, e);
}

#define m_assert(expression)                                                   \
  if (!(expression)) {                                                         \
    error(__FILE__, __LINE__, (char *)#expression);                            \
  }

crafter::crafter(std::string config_file) {

  GOOGLE_PROTOBUF_VERIFY_VERSION;

  protocol = new PROTOCOL();

  {
    // Read the existing address book.
    fstream input(config_file.c_str(), ios::in | ios::binary);
    if (!protocol->ParseFromIstream(&input)) {
      return;
    }
  }
}

crafter::~crafter() { 
  google::protobuf::ShutdownProtobufLibrary();
}

pair<uint8_t *, uint64_t> crafter::craft(COMMAND cmd_type, uint32_t addr,
                                         uint32_t payload) {

  uint8_t *packet = NULL;
  //uint64_t size = sizeof(addr_size);

  // First we look into the jtag_pkt to find the jtag sequence description
  for( auto i=0; i<protocol->commands_size(); i++) {
    const ::usb2jtag::HL_COMMAND& cmd = protocol->commands(i);

    if( cmd.type() == (int)cmd_type ) {

      // allocate required packet size 
      packet = new unsigned char[cmd.size()];

      for( auto k=0; k<protocol->commands_size(); k++) {
        const ::usb2jtag::JTAG_COMMAND& jtag_cmd = cmd.commands(k); 

        unsigned long* raw_packet = (unsigned long*)&packet[k*64];

        *raw_packet |= (jtag_cmd.jtag_start() & 0xF);
        *raw_packet |= (jtag_cmd.jtag_end()   & 0xF)  << 4;
        *raw_packet |= (jtag_cmd.bitcount()   & 0x3F) << 10; 
        *raw_packet |= (jtag_cmd.period()     & 0x3F) << 16;
        *raw_packet |= (jtag_cmd.payload()    & 0xFFFFFFFFFFF) << 22;
        if( jtag_cmd.data_length() != 0 ) {
          unsigned long data;
          if( jtag_cmd.data_is_address() )
            data = addr;
          if( jtag_cmd.data_is_other() )
            data = payload;

          *raw_packet |= (jtag_cmd.payload() | (data << jtag_cmd.data_offset()));
        }
      } 
      return make_pair(packet, cmd.size());
    }
  }
  return make_pair(packet, 0);
}

pair<uint8_t *, uint64_t> crafter::craft(COMMAND cmd_type, uint32_t addr,
                                         std::string payload) {

  uint8_t *packet = NULL;
  //uint64_t size = sizeof(addr_size);
  //uint8_t header[] = {0x00, 0x00, 0x00, 0x00};

  //uint64_t size = sizeof(addr_size);

  // First we look into the jtag_pkt to find the jtag sequence description
  for( auto i=0; i<protocol->commands_size(); i++) {
    const ::usb2jtag::HL_COMMAND& cmd = protocol->commands(i);

    //::usb2jtag::HL_COMMAND_CMD_TYPE HL_COMMAND::type()
    if( cmd.type() == (int)cmd_type ) {

      // allocate required packet size 
      packet = new unsigned char[cmd.size()];

      for( auto k=0; k<protocol->commands_size(); k++) {
        const ::usb2jtag::JTAG_COMMAND& jtag_cmd = cmd.commands(k);
        
        unsigned long* raw_packet = (unsigned long*)&packet[k*64];

        *raw_packet |= (jtag_cmd.jtag_start() & 0xF);
        *raw_packet |= (jtag_cmd.jtag_end()   & 0xF)  << 4;
        *raw_packet |= (jtag_cmd.bitcount()   & 0x3F) << 10; 
        *raw_packet |= (jtag_cmd.period()     & 0x3F) << 16;
        *raw_packet |= (jtag_cmd.payload()    & 0xFFFFFFFFFFF) << 22;
        if( jtag_cmd.data_length() != 0 ) {
          unsigned long data;
          if( jtag_cmd.data_is_address() )
            data = addr;
          if( jtag_cmd.data_is_other() )
            le_copy_to((uint8_t*)&data, payload.c_str(), payload.size());

          *raw_packet |= (jtag_cmd.payload() | (data << jtag_cmd.data_offset()));
        }
      } 
      return make_pair(packet, cmd.size());
    }
  }
  return make_pair(packet, 0);
  /*
   * Generate Header
   */
//  switch (cmd) {
//  case c_scan_start:
//    header[0] = 0x40;
//    m_assert(payload.size() == 0);
//    break;
//  case c_scan_stop:
//    header[0] = 0x50;
//    m_assert(payload.size() == 0);
//    break;
//  case c_scan_loopback:
//    header[0] = 0x60;
//    m_assert(payload.size() == 0);
//    break;
//  case c_scan:
//    header[0] = 0x70;
//    m_assert(payload.size() > 0);
//    m_assert(payload.size() % 4 == 0);
//    break;
//  case c_write:
//    header[0] = 0x10;
//    m_assert(payload.size() > 0);
//    m_assert(payload.size() % 4 == 0);
//    break;
//  case c_read:
//    header[0] = 0x20;
//    break;
//  case c_reset:
//    header[0] = 0x30;
//    m_assert(payload.size() == 0);
//    break;
//  default:
//    break;
//  }
//
//  /*
//   * Generate Payload
//   */
//  // Compute size in byte
//  size += sizeof(header) + payload.size();
//
//  // Allocate packet
//  packet = (uint8_t *)malloc(sizeof(uint8_t) * size);
//
//  // We first copy the header in packet, order is already in little indian
//  memcpy(packet, header, sizeof(header));
//  // Copy address or size packet
//  crafter::le_copy_to(&packet[4], (const char *)&addr_size, sizeof(addr_size));
//
//  // As x86 is in big indian we need to reverse byte order
//  crafter::le_copy_to(&packet[8], payload.c_str(), payload.size());

//  return make_pair(packet, size);
}

void crafter::le_copy_to(uint8_t *dst, const char *src, uint32_t size) {
  uint32_t i;

  m_assert(size < 4088); // USB3 packet limitation

  for (i = 0; i < size; i += 4) {
    dst[i] = src[i + 3];
    dst[i + 1] = src[i + 2];
    dst[i + 2] = src[i + 1];
    dst[i + 3] = src[i];
  }
}
