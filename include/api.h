#ifndef API_H
#define API_H

#include <command.h>
#include <crafter.hpp>
#include <device.hpp>
#include <iostream>
#include <map>
#include <termcolor.hpp>
#include <unistd.h>
#include <vector>

#ifndef WATCHER
#define WATCHER
typedef void (*Watcher)(int);
#endif

void load_protocol();

device *target_init();

void target_init_irq(Watcher watcher);

void irq_listener();

void target_irq_stop();

void target_reset(device *io);

uint8_t *target_read(device *io, uint32_t address, uint32_t size);

uint32_t target_read_u32(device *io, uint32_t address);

void target_write(device *io, uint32_t address, uint32_t size, uint8_t *data);

void target_write(device *io, uint32_t address, uint32_t data);

uint32_t target_read_reg(device *io, uint32_t register_id);

void target_write_reg(device *io, uint32_t register_id, uint32_t value);

void scan_halt(device *io);

void scan_resume(device *io);

std::vector<uint8_t> scan_loopback(device *io, uint32_t size);

void overwrite_hw_state(device *io, std::vector<uint8_t> data);

#endif
