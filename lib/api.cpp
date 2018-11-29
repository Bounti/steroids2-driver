#include "api.h"
#include <command.h>
#include <crafter.hpp>
#include <device.hpp>
#include <iostream>
#include <map>
#include <termcolor.hpp>
#include <unistd.h>

device *init() {
  device *io = new device(0x04B4, 0x00F1, 0);
  io->init();

  return io;
}

void reset(device *io) {
  auto crafted_packet = crafter::craft(c_reset, 0, string());
  io->send(crafted_packet.first, crafted_packet.second);
}

// XXX: WARNING !!
// Fow now only multiple of 4byte (32bits) is accepted as size argument
uint8_t *read(device *io, uint32_t address, uint32_t size) {
  std::string payload = "";

  uint8_t *data = new uint8_t[size]();

  auto crafted_packet = crafter::craft(c_read, address, payload);
  io->send(crafted_packet.first, crafted_packet.second);

  io->receive(data, size * 2);

  return data;
}

void write(device *io, uint32_t address, uint32_t size, uint8_t *data) {
  std::string payload((char *)data, size);

  auto crafted_packet = crafter::craft(c_write, address, payload);
  io->send(crafted_packet.first, crafted_packet.second);
}

void halt() {
  uint32_t data = (0xA05F << 16) | (0 << 1) | (1 << 0));
  std::string payload((char *)data, 4);

  auto crafted_packet = crafter::craft(c_write, 0xE000EDF0, payload);
  io->send(crafted_packet.first, crafted_packet.second);
}

void resume() {
  uint32_t data = (0xA05F << 16) | (1 << 1) | (1 << 0));
  std::string payload((char *)data, 4);

  auto crafted_packet = crafter::craft(c_write, 0xE000EDF0, payload);
  io->send(crafted_packet.first, crafted_packet.second);
}

void read_register(uint32_t reg) {
  std::string payload1((char *)reg, 4);

  auto crafted_packet = crafter::craft(c_write, 0xE000EDF4, payload1);
  io->send(crafted_packet.first, crafted_packet.second);

  std::string payload2 = "";
  crafted_packet = crafter::craft(c_read, 0xE000EDF8, payload2);
  io->send(crafted_packet.first, crafted_packet.second);

  uint8_t *data = new uint8_t[8]();
  io->receive(data, 8);

  return data;
}

void write_register(uint32_t reg, uint32_t value) {
  std::string payload1((char *)value, 4);

  auto crafted_packet = crafter::craft(c_write, 0xE000EDF8, payload1);
  io->send(crafted_packet.first, crafted_packet.second);

  uint32_t reg = reg | (1 << 16);
  std::string payload2((char *)reg, 4);
  crafted_packet = crafter::craft(c_read, 0xE000EDF4, payload2);
  io->send(crafted_packet.first, crafted_packet.second);

  uint8_t *data = new uint8_t[8]();
  io->receive(data, 8);

  return data;
}
