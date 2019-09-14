//
//  crafter.hpp
//  debugger-driver2
//
//  Created by <author> on 05/10/2018.
//
//

#ifndef crafter_hpp
#define crafter_hpp

#include <command.h>
#include <map>
#include <string>

using namespace std;

namespace usb2jtag {
  class PROTOCOLS;
  class PROTOCOL;
};

class crafter {
private:

  usb2jtag::PROTOCOLS* protocols;

  int32_t protocol_index = -1;
public:

  crafter(std::string config_file);

  ~crafter();

  void print_protocol();

  bool select_protocol(std::string protocol_name);

  pair<uint8_t *, uint64_t> craft(COMMAND cmd_type, uint32_t data, 
        uint32_t jtag_start, uint32_t jtag_end, uint32_t bitcount,
        uint32_t period, bool write_back);

  pair<uint8_t *, uint64_t> craft(COMMAND cmd, uint32_t addr_size,
                                         uint32_t payload);
};

#endif /* crafter_hpp */
