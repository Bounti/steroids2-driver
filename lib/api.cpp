#include "api.h"
#include <command.h>
#include <crafter.hpp>
#include <device.hpp>
#include <iostream>
#include <map>
#include <termcolor.hpp>
#include <thread>
#include <unistd.h>
#include <vector>

static bool stopped = true;
static device *irq_io = NULL;
static Watcher watcher;
static crafter* my_crafter;

void load_protocol(std::string protocol_name) {

  my_crafter = new crafter("protocol.bin");

  my_crafter->select_protocol(protocol_name);
}

device *target_init() {
  
  device *io = new device(0x04B4, 0x00F1, 0);

  io->init();

  return io;
}

void irq_listener() {
  uint32_t size = 8;

  while (stopped == false) {

    uint8_t buffer[8] = {0};
    uint32_t value = 0;

    std::cout << "Waiting Trace information" << std::endl;

    irq_io->receive(buffer, 8);

    if (size != 8) {
      if (size == 4) {
        memcpy((void *)&value, &buffer[0], 4);
        std::cout << "Failed to decode READ command with error code " << value
                  << std::endl;
      } else {
        std::cout << "Failed to decode READ command with unknown error code "
                  << value << std::endl;
      }
    } else {
      value |= buffer[4] << 24;
      value |= buffer[5] << 16;
      value |= buffer[6] << 8;
      value |= buffer[7];
      std::cout << "[Trace] Interrupt ID : " << value << std::endl;
      watcher(value);
    }
  }
}

void target_init_irq(Watcher _watcher) {
  watcher = _watcher;

  stopped = true;

  device *irq_io = new device(0x04b4, 0x00f1, 0, 0x02, 0x82);
  irq_io->init();

  std::thread trace_thread(irq_listener);
  trace_thread.detach();
}

void target_irq_stop() { stopped = false; }

void target_reset(device *io) {

  auto crafted_packet = my_crafter->craft(c_reset, 0, 0);

  io->send(crafted_packet.first, crafted_packet.second);
}

uint64_t target_read_u32(device *io, uint32_t address) {
  uint32_t res = 0;

  auto crafted_packet = my_crafter->craft(c_read, address, 0);
  if( crafted_packet.first != NULL) {
  
    io->send(crafted_packet.first, crafted_packet.second);

    uint8_t buffer[8] = {0};
    io->receive(buffer, 8);

    printf("> %016lx\n", *((unsigned long int*)&buffer[0]));

    delete crafted_packet.first;

    res = *((unsigned long int*)&buffer[0]);
    //res = *((unsigned long int*)&buffer[0]) >> 4;
    return res;  
  } else { 
    return 0; 
  }  
}


void target_write_u32(device *io, uint32_t address, uint32_t data) {

  auto crafted_packet = my_crafter->craft(c_write, address, data);
  io->send(crafted_packet.first, crafted_packet.second);
}
