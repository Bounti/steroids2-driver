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
static bool big_endian = false; // Default to little endian
static crafter* my_crafter;

void load_protocol() {

  my_crafter = new crafter("protocol.bin");
}

device *target_init() {
  device *io = new device(0x04B4, 0x00F1, 0);
  io->init();

  // Check indianess
  // Target device are in little endian
  // Host computer may be in big endian
  uint64_t x = 0x1;

  // +----+----+----+----+
  // |0x00|0x00|0x00|0x01|
  // +----+----+----+----+
  big_endian = (((char*)(&x))[0] == 1);

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

//  uint8_t* packet = new uint8_t[32];
//  unsigned long int* l_pkt = (unsigned long int*)&packet[0];
//
//  *l_pkt = 0x00000000F1000000;
//  
//  l_pkt = (unsigned long int*)&packet[8];
//  *l_pkt = 0x0000000000000000;
//
//  l_pkt = (unsigned long int*)&packet[16];
//  *l_pkt = 0x0000000000000000;
//
//  l_pkt = (unsigned long int*)&packet[24];
//  *l_pkt = 0x0000000000000000;
//
//  io->send(packet, 8);
  io->send(crafted_packet.first, crafted_packet.second);
}

// XXX: WARNING !!
// Fow now only multiple of 4byte (32bits) is accepted as size argument
uint8_t *target_read(device *io, uint32_t address, uint32_t size) {
  std::string payload = "";

  // +4 for header

  uint8_t *data = new uint8_t[size+4]();

  auto crafted_packet = my_crafter->craft(c_read, address, payload);
  io->send(crafted_packet.first, crafted_packet.second);

  io->receive(data, size + 4);

  if(big_endian) {
    uint8_t *res = new uint8_t[size]();

    for (uint32_t i = 4; i < size+4; i += 4) {
      res[i] = data[i + 3];
      res[i + 1] = data[i + 2];
      res[i + 2] = data[i + 1];
      res[i + 3] = data[i];
    }
    delete data;

    return res;
  } else {
    return &data[4];
  }
}

uint32_t target_read_u32(device *io, uint32_t address) {
  uint32_t res = 0;
  std::string payload = "";

  auto crafted_packet = my_crafter->craft(c_read, address, payload);
  io->send(crafted_packet.first, crafted_packet.second);

  uint8_t *buffer = new uint8_t[8]();
  io->receive(buffer, 8);

  res |= buffer[4] << 24;
  res |= buffer[5] << 16;
  res |= buffer[6] << 8;
  res |= buffer[7];

  return res;
}

void target_write(device *io, uint32_t address, uint32_t size, uint8_t *data) {

  if (size == 4) {
    std::string payload((char *)data, size);

    auto crafted_packet = my_crafter->craft(c_write, address, payload);
    io->send(crafted_packet.first, crafted_packet.second);
  } else if (size > 4) {
    uint32_t no_chunk = size / 4;
    uint32_t offset = 0;
    // Process all alligned chunk
    for (auto i = 1; i <= no_chunk; i++) {
      offset = ((i - 1) * 4);
      target_write(io, address + offset, 4, &data[offset]);
    }
    // Process unaligned data
    address = address + (no_chunk * 4);
    offset = (no_chunk * 4);
    target_write(io, address, 4, &data[offset]);
  } else {
    // If its memory alligned then we can do 32bits accesses
    switch (size) {
    case 0:
      return;
    // byte
    case 1: {
      uint8_t *prv_mem;

      prv_mem = target_read(io, address, 4);
      prv_mem[0] = data[0];

      std::string payload((char *)prv_mem, 4);

      auto crafted_packet = my_crafter->craft(c_write, address, payload);
      io->send(crafted_packet.first, crafted_packet.second);
    } break;
      // 16b
    case 2: {
      uint8_t *prv_mem;

      prv_mem = target_read(io, address, 4);
      prv_mem[0] = data[0];
      prv_mem[1] = data[1];

      std::string payload((char *)prv_mem, 4);

      auto crafted_packet = my_crafter->craft(c_write, address, payload);
      io->send(crafted_packet.first, crafted_packet.second);
    } break;
    // 24b
    case 3: {
      uint8_t *prv_mem;

      prv_mem = target_read(io, address, 4);
      prv_mem[0] = data[0];
      prv_mem[1] = data[1];
      prv_mem[2] = data[2];

      std::string payload((char *)prv_mem, 4);

      auto crafted_packet = my_crafter->craft(c_write, address, payload);
      io->send(crafted_packet.first, crafted_packet.second);
    } break;
    }
  }
}

void target_write(device *io, uint32_t address, uint32_t data) {
  // std::string payload((char *)data, 4);

  auto crafted_packet = my_crafter->craft(c_write, address, data);
  io->send(crafted_packet.first, crafted_packet.second);
}

void target_halt(device *io) {
  uint32_t data = (0xA05F << 16) | (0 << 1) | (1 << 0);
  std::string payload((char *)&data, 4);

  auto crafted_packet = my_crafter->craft(c_write, 0xE000EDF0, payload);
  io->send(crafted_packet.first, crafted_packet.second);
}

void target_resume(device *io) {
  uint32_t data = (0xA05F << 16) | (1 << 1) | (1 << 0);
  std::string payload((char *)&data, 4);

  auto crafted_packet = my_crafter->craft(c_write, 0xE000EDF0, payload);
  io->send(crafted_packet.first, crafted_packet.second);
}

uint32_t target_read_reg(device *io, uint32_t reg) {
  uint32_t res = 0;
  std::string payload1((char *)&reg, 4);

  auto crafted_packet = my_crafter->craft(c_write, 0xE000EDF4, payload1);
  io->send(crafted_packet.first, crafted_packet.second);

  std::string payload2 = "";
  crafted_packet = my_crafter->craft(c_read, 0xE000EDF8, payload2);
  io->send(crafted_packet.first, crafted_packet.second);

  uint8_t *buffer = new uint8_t[8]();
  io->receive(buffer, 8);

  res |= buffer[4] << 24;
  res |= buffer[5] << 16;
  res |= buffer[6] << 8;
  res |= buffer[7];

  return res;
}

void target_write_reg(device *io, uint32_t reg, uint32_t value) {
  uint32_t res;
  std::string payload1((char *)&value, 4);

  auto crafted_packet = my_crafter->craft(c_write, 0xE000EDF8, payload1);
  io->send(crafted_packet.first, crafted_packet.second);

  reg = reg | (1 << 16);
  std::string payload2((char *)&reg, 4);
  crafted_packet = my_crafter->craft(c_read, 0xE000EDF4, payload2);
  io->send(crafted_packet.first, crafted_packet.second);
}

void scan_halt(device *io) {}

void scan_resume(device *io) {}

std::vector<uint8_t> scan_loopback(device *io, uint32_t size) {

  uint8_t *reception = new uint8_t[size * 2]();
  std::string payload = "";
  std::vector<uint8_t> ret;

  auto crafted_packet = my_crafter->craft(c_scan_loopback, size, payload);
  io->send(crafted_packet.first, crafted_packet.second);
  io->receive(reception, size * 2);

  std::copy(reception, reception, std::back_inserter(ret));

  delete reception;

  return ret;
}

void overwrite_hw_state(device *io, std::vector<uint8_t> data) {

  uint8_t *reception = new uint8_t[data.size() * 2]();
  std::string payload((char *)data.data(), data.size());

  auto crafted_packet = my_crafter->craft(c_scan, data.size(), payload);
  io->send(crafted_packet.first, crafted_packet.second);
  io->receive(reception, data.size() * 2);

  delete reception;
  return;
}
