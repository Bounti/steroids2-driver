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
};

class crafter {
private:

  usb2jtag::PROTOCOLS* protocols;
public:

  crafter(std::string config_file);

  ~crafter();

  void print_protocol();

  pair<uint8_t *, uint64_t> craft(COMMAND cmd, uint32_t addr_size,
                                         uint32_t payload);

protected:
  void le_copy_to(uint8_t *dst, const char *src, uint32_t size);
};

#endif /* crafter_hpp */
