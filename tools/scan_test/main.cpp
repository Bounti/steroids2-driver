#include <command.h>
#include <crafter.hpp>
#include <device.hpp>
#include <iostream>
#include <map>
#include <termcolor.hpp>
#include <unistd.h>

int main(int argc, char *argv[]) {

  device *io = new device(0x04B4, 0x00F1, 0);
  io->init();

  uint32_t pattern = 0;
  if (argc == 2) {
    pattern = strtoul(argv[1], NULL, 16);
  }

  uint32_t data_dwords[] = {pattern};
  uint32_t data_dwords_size = sizeof(data_dwords);

  uint8_t *reception = new uint8_t[data_dwords_size]();
  std::string payload((char *)data_dwords, data_dwords_size);

  crafter* my_crafter = new crafter("./protocol.bin");

  auto crafted_packet = my_crafter->craft(c_scan, data_dwords_size / 4, payload);
  io->send(crafted_packet.first, crafted_packet.second);

  printf("%d", data_dwords_size);

  // Reception is scan payload * 2 because the Inception
  // debugger return 64bits length packet only
  io->receive(reception, data_dwords_size * 2);

  io->close();
}
