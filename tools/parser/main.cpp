#include <command.h>
#include <crafter.hpp>
#include <device.hpp>
#include <iostream>
#include <map>
#include <termcolor.hpp>
#include <unistd.h>

int main(int argc, char *argv[]) {

  if( argc < 2 ) {
    printf("./%s <input_file.bin>", argv[0]);
    return 0;
  }

  crafter* my_crafter = new crafter(std::string(argv[1]));

  my_crafter->print_protocol();

  auto crafted_reset  = my_crafter->craft(c_reset, 0, 0);
  //auto crafted_write  = my_crafter->craft(c_write, 0x20000000, 0xaaaaaaaa);
  //auto crafted_read   = my_crafter->craft(c_read, 0x20000000, 0xaaaaaaaa);

  return 1;
}
