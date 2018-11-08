//
//  crafter.cpp
//  debugger-driver2
//
//  Created by <author> on 05/10/2018.
//
//

#include "crafter.hpp"
#include <map>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <string>

using namespace std;

#define m_assert(expression)                                                   \
  if (!(expression)) {                                                         \
    error(__FILE__, __LINE__, #expression);                                    \
  }

void error(char *f, int l, const char *e) {
  printf("[ERROR] In file %s at line %d, m_assertion failed : %s ", f, l, e);
}

crafter::crafter() {}
crafter::~crafter() {}

pair<uint8_t *, uint64_t> crafter::craft(COMMAND cmd, uint32_t addr_size,
                                         std::string payload) {

  uint8_t *packet = NULL;
  uint64_t size = sizeof(addr_size);
  uint8_t header[] = {0x00, 0x00, 0x00, 0x00};

  /*
   * Generate Header
   */
  switch (cmd) {
  case c_scan:
    header[0] = 0x40;
    m_assert(payload.size() > 0);
    m_assert(payload.size() % 4 == 0);
    break;
  case c_write:
    header[0] = 0x10;
    m_assert(payload.size() > 0);
    m_assert(payload.size() % 4 == 0);
    break;
  case c_read:
    header[0] = 0x20;
    break;
  case c_reset:
    header[0] = 0x30;
    m_assert(payload.size() == 0);
    break;
  default:
    break;
  }

  /*
   * Generate Payload
   */
  // Compute size in byte
  size += sizeof(header) + payload.size();

  // Allocate packet
  packet = (uint8_t *)malloc(sizeof(uint8_t) * size);

  // We first copy the header in packet, order is already in little indian
  memcpy(packet, header, sizeof(header));
  // Copy address or size packet
  crafter::le_copy_to(&packet[4], (const char *)&addr_size, sizeof(addr_size));

  // As x86 is in big indian we need to reverse byte order
  crafter::le_copy_to(&packet[8], payload.c_str(), payload.size());

  return make_pair(packet, size);
}

void crafter::le_copy_to(uint8_t *dst, const char *src, uint32_t size) {
  uint32_t i;

  m_assert(size < 4088); // USB3 packet limitation

  for (i = 0; i < size; i += 4) {
    dst[i] = src[i + 3];
    dst[i + 1] = src[i + 2];
    dst[i + 2] = src[i + 1];
    dst[i + 3] = src[i];
  }
}
