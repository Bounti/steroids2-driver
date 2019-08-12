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
  class PROTOCOL;
};

class crafter {
private:

  usb2jtag::PROTOCOL* protocol;
public:

  crafter(std::string config_file);

  ~crafter();

  std::pair<uint8_t *, uint64_t> craft(COMMAND cmd, uint32_t addr_size,
                                              string payload);

  pair<uint8_t *, uint64_t> craft(COMMAND cmd, uint32_t addr_size,
                                         uint32_t payload);

protected:
  void le_copy_to(uint8_t *dst, const char *src, uint32_t size);
};

#endif /* crafter_hpp */
