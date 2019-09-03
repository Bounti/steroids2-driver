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

void load_protocol(std::string protocol_name);

device *target_init();

void target_init_irq(Watcher watcher);

void irq_listener();

void target_irq_stop();

uint64_t target_read_u32(device *io, uint32_t address);

void target_write_u32(device *io, uint32_t address, uint32_t data);

void target_reset(device *io);

#endif
