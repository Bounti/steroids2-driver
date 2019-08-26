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

void crafter::print_protocol() {

  std::cout << "Protocol" << std::endl;
  std::cout << "    Name: " << protocol->target() << std::endl;

  for( auto i=0; i<protocol->commands_size(); i++) {
    const ::usb2jtag::HL_COMMAND& cmd = protocol->commands(i);

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

pair<uint8_t *, uint64_t> crafter::craft(COMMAND cmd_type, uint32_t addr,
                                         uint32_t payload) {

  uint8_t *packet = NULL;
  //uint64_t size = sizeof(addr_size);

  // First we look into the jtag_pkt to find the jtag sequence description
  for( auto i=0; i<protocol->commands_size(); i++) {
    const ::usb2jtag::HL_COMMAND& cmd = protocol->commands(i);

    if( cmd.type() == (int)cmd_type ) {

      // allocate required packet size
      packet = new unsigned char[cmd.size()/8];

      for( auto k=0; k<cmd.commands_size(); k++) {
        const ::usb2jtag::JTAG_COMMAND& jtag_cmd = cmd.commands(k);

        unsigned long int* raw_packet = (unsigned long int*)&packet[k*8];
        unsigned long int be_packet = 0;
        raw_packet[0] = 0;

        be_packet |= (unsigned long int)(jtag_cmd.jtag_start() & 0xF) << 60;
        be_packet |= (unsigned long int)(jtag_cmd.jtag_end()   & 0xF)  << 56;
        be_packet |= (unsigned long int)(jtag_cmd.bitcount()   & 0x3F) << 48;
        be_packet |= (unsigned long int)(jtag_cmd.period()     & 0x3F) << 42;
        be_packet |= (unsigned long int)(jtag_cmd.payload()    & 0xFFFFFFFFFFF) << 42;
        if( jtag_cmd.data_length() != 0 ) {
          unsigned long data;
          if( jtag_cmd.data_is_address() )
            data = addr;
          if( jtag_cmd.data_is_other() )
            data = payload;

          be_packet |= (unsigned long int)(data << jtag_cmd.data_offset()) << 42;
        }

         *raw_packet = be_packet;
         //*raw_packet = 0x1100000000000000;
          // switch to little endian
          //*raw_packet = 0;
          //*raw_packet |= ((be_packet & 0xFF00000000000000) >> 56);
          //*raw_packet |= ((be_packet & 0x00FF000000000000) >> 40);
          //*raw_packet |= ((be_packet & 0x0000FF0000000000) >> 24);
          //*raw_packet |= ((be_packet & 0x000000FF00000000) >> 8);
          //*raw_packet |= ((be_packet & 0x00000000FF000000) << 8);
          //*raw_packet |= ((be_packet & 0x0000000000FF0000) << 24);
          //*raw_packet |= ((be_packet & 0x000000000000FF00) << 40);
          //*raw_packet |= ((be_packet & 0x00000000000000FF) << 56);

          // switch word to little endian too
          //raw_packet[0] = ((raw_packet[0] >> 32)& 0xFFFFFFFF) | ((raw_packet[0] & 0xFFFFFFFF) << 32);
          //
          //raw_packet[0] = ((raw_packet[0] & 0xFFFFFFFF) << 32) | ((raw_packet[0] >> 32)& 0xFFFFFFFF);  
      }

      printf("\n\n");
      for(auto l=0; l<cmd.size()/8; l++) {
        if( l != 0 && l % 4 == 0 )
          printf("\n");
        printf("%02x", packet[l]);
      }
      printf("\n");

      return make_pair(packet, cmd.size()/8);
    }
  }
  return make_pair(packet, 0);
}

pair<uint8_t *, uint64_t> crafter::craft(COMMAND cmd_type, uint32_t addr,
                                         std::string payload) {

  // We need to split payload in 32bits chunk 
  if( payload.size() > 4) {
    uint32_t ui32_payload = 0; 

    // If our payload is not 4bytes aligned, we add 4bytes more.
    uint32_t extension = ((payload.size() % 4) == 1 ? 4 : 0);

    uint8_t *packet = new uint8_t[(payload.size()/4)+extension];

    uint32_t size = 0;

    // For each 4byte chunk
    for(auto k=0; k<payload.size(); k+=4) {
      for(auto i=k; i<payload.size(); i++) {
        // little endian
        ui32_payload = (payload[i] << (24 - (i*8)));
      }
      auto packet_chunk = craft(cmd_type, addr, ui32_payload);

      size += packet_chunk.second;

      memcpy((void*)packet[k],(void*) packet_chunk.first, packet_chunk.second);
      delete packet_chunk.first;
    }
    //ui32_payload = (payload[0] << 24) | (payload[1] << 16) | (payload[2] << 8) | (payload[3]);
    return craft(cmd_type, addr, ui32_payload); 
  } else {
    
    uint32_t ui32_payload = 0; 

    for(auto i=0; i<payload.size(); i++) {
      // little endian
      ui32_payload = (payload[i] << (24 - (i*8)));
    }
    //ui32_payload = (payload[0] << 24) | (payload[1] << 16) | (payload[2] << 8) | (payload[3]);
    return craft(cmd_type, addr, ui32_payload);
  } 
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
