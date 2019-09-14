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

  protocols = new PROTOCOLS();

  {
    // Read the existing address book.
    fstream input(config_file.c_str(), ios::in | ios::binary);
    if (!protocols->ParseFromIstream(&input)) {
      return;
    }
  }
}

bool crafter::select_protocol(std::string protocol_name) {

  for( auto i=0; i<protocols->protocols_size(); i++) {
    const ::usb2jtag::PROTOCOL& proto = protocols->protocols(i);
    if( proto.target().compare(protocol_name) == 0) { 
     protocol_index = i; 
     return true;
    }
  }
  return false;
}

crafter::~crafter() {
  google::protobuf::ShutdownProtobufLibrary();
}

void crafter::print_protocol() {

  const usb2jtag::PROTOCOL protocol = protocols->protocols(protocol_index); 

  std::cout << "Protocol" << std::endl;
  std::cout << "    Name: " << protocol.target() << std::endl;

  for( auto i=0; i<protocol.commands_size(); i++) {
    const ::usb2jtag::HL_COMMAND& cmd = protocol.commands(i);

    std::cout << "  Type: " << cmd.type() << std::endl;
    std::cout << "  Size: " << cmd.size() << std::endl;

    for( auto k=0; k<cmd.commands_size(); k++) {
      const ::usb2jtag::JTAG_COMMAND& jtag_cmd = cmd.commands(k);


      std::cout << "      Start           : " << jtag_cmd.jtag_start() << std::endl;
      std::cout << "      Stop            : " << jtag_cmd.jtag_end() << std::endl;
      std::cout << "      Bitcount        : " << jtag_cmd.bitcount() << std::endl;
      std::cout << "      Period          : " << jtag_cmd.period() << std::endl;
      std::cout << "      Payload         : " << jtag_cmd.payload() << std::endl;
      std::cout << "      data_length     : " << jtag_cmd.data_length() << std::endl;
      std::cout << "      data_offset     : " << jtag_cmd.data_offset() << std::endl;
      std::cout << "      Data_is_other   : " << jtag_cmd.data_length() << std::endl;
      std::cout << "      data_is_address : " << jtag_cmd.data_is_other() << std::endl;
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }
}

pair<uint8_t *, uint64_t> crafter::craft(COMMAND cmd_type, uint32_t data, 
        uint32_t jtag_start, uint32_t jtag_end, uint32_t bitcount,
        uint32_t period, bool write_back) {

  uint8_t *packet = NULL;
  const usb2jtag::PROTOCOL protocol = protocols->protocols(protocol_index); 

  // First we look into the jtag_pkt to find the jtag sequence description
  for( auto i=0; i<protocol.commands_size(); i++) {
    const ::usb2jtag::HL_COMMAND& cmd = protocol.commands(i);

    if( cmd.type() == (int)cmd_type ) {

      // allocate required packet size
      packet = new unsigned char[cmd.size()/8];

      for( auto k=0; k<cmd.commands_size(); k++) {
        const ::usb2jtag::JTAG_COMMAND& jtag_cmd = cmd.commands(k);

        unsigned long int* raw_packet = (unsigned long int*)&packet[k*8];
        unsigned long int be_packet = 0;

        be_packet |= (unsigned long int)(jtag_start & 0xF);
        be_packet |= (unsigned long int)(jtag_end   & 0xF) << 4;
        be_packet |= (unsigned long int)(bitcount   & 0x3F) << 8;
        be_packet |= (unsigned long int)(period     & 0x3F) << 14;
        be_packet |= (unsigned long int)(data       & 0xFFFFFFFFFFF) << 20;

       if( write_back )
         be_packet |= 0x8000000000000000;

         *raw_packet = be_packet; 
      }

      return make_pair(packet, cmd.size()/8);
    }
  }
  return make_pair(packet, 0);

}

pair<uint8_t *, uint64_t> crafter::craft(COMMAND cmd_type, uint32_t addr,
                                         uint32_t payload) {

  uint8_t *packet = NULL;
  const usb2jtag::PROTOCOL protocol = protocols->protocols(protocol_index); 

  // First we look into the jtag_pkt to find the jtag sequence description
  for( auto i=0; i<protocol.commands_size(); i++) {
    const ::usb2jtag::HL_COMMAND& cmd = protocol.commands(i);

    if( cmd.type() == (int)cmd_type ) {

      // allocate required packet size
      packet = new unsigned char[cmd.size()/8];

      for( auto k=0; k<cmd.commands_size(); k++) {
        const ::usb2jtag::JTAG_COMMAND& jtag_cmd = cmd.commands(k);

        unsigned long int* raw_packet = (unsigned long int*)&packet[k*8];
        unsigned long int be_packet = 0;

        be_packet |= (unsigned long int)(jtag_cmd.jtag_start() & 0xF);
        be_packet |= (unsigned long int)(jtag_cmd.jtag_end()   & 0xF) << 4;
        be_packet |= (unsigned long int)(jtag_cmd.bitcount()   & 0x3F) << 8;
        be_packet |= (unsigned long int)(jtag_cmd.period()     & 0x3F) << 14;
        be_packet |= (unsigned long int)(jtag_cmd.payload()    & 0xFFFFFFFFFFF) << 20;
        if( jtag_cmd.data_length() != 0 ) {
          unsigned long data;
          if( jtag_cmd.data_is_address() )
            data = addr;
          if( jtag_cmd.data_is_other() )
            data = payload;

          be_packet |= (unsigned long int)(data << jtag_cmd.data_offset()) << 20;
        }

       if( jtag_cmd.has_write_back() && jtag_cmd.write_back() )
         be_packet |= 0x8000000000000000;

         *raw_packet = be_packet; 
      }

      //printf("\n\n");
      //for(auto l=0; l<cmd.size()/8; l+=4) {
      //  printf("%02x%02x%02x%02x\n", packet[l+3], packet[l+2], packet[l+1], packet[l]);
        
        //printf("%02x", (((packet[l+3]&0xF)<<4)|((packet[l+3]&0xF0)>>4)));
        //printf("%02x", (((packet[l+2]&0xF)<<4)|((packet[l+2]&0xF0)>>4)));
        //printf("%02x", (((packet[l+1]&0xF)<<4)|((packet[l+1]&0xF0)>>4)));
        //printf("%02x\n", (((packet[l+0]&0xF)<<4)|((packet[l+0]&0xF0)>>4)));
      //}
      //printf("\n");

      return make_pair(packet, cmd.size()/8);
    }
  }
  return make_pair(packet, 0);
}

