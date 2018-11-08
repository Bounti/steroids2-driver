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

  // device *irq = new device(0x04b4, 0x00f1, 0, 0x02, 0x82);
  // irq->init();

  uint32_t data_dwords[] = {0xABABABAB, 0xABABABAB, 0xABABABAB, 0xABABABAB};
  uint8_t reception[16] = {0};
  std::string payload((char *)data_dwords, 16);

  //auto crafted_packet = crafter::craft(c_reset, 0, string());
  //io->send(crafted_packet.first, crafted_packet.second);

  auto crafted_packet = crafter::craft(c_scan, 0x4, payload);
  io->send(crafted_packet.first, crafted_packet.second);
  io->receive(reception, 16);

  //crafted_packet = crafter::craft(c_reset, 0, string());
  //io->send(crafted_packet.first, crafted_packet.second);

  // for (int i = 0; i < 10; i++) {
  //   cout << termcolor::blue << "wait for next cmd " << i << endl;
  //   usleep(3000000);
  //   io->send((uint8_t *)&data_dwords[0], 4);
  // }

  // crafted_packet = crafter::craft(c_write, 0x20000000, string());
  // io->send(crafted_packet.first, crafted_packet.second);

  io->close();
  // irq->close();

  // crafted_packet = crafter::craft(scan, data);
  // device_jtag->send(crafted_packet.first, crafted_packet.second);
}
