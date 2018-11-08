#ifndef API_H
#define API_H

#include <command.h>
#include <crafter.hpp>
#include <device.hpp>
#include <iostream>
#include <map>
#include <termcolor.hpp>
#include <unistd.h>

device* init();

void reset(device* io);

uint8_t* read(device* io, uint32_t address, uint32_t size);

void write(device* io, uint32_t address, uint32_t size, uint8_t* data);

#endif
