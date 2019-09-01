#include "api.h"

#include <chrono>
#include <ctime>

using namespace std;

double print_timediff(const char *prefix, const struct timespec &start,
                      const struct timespec &end) {
  double milliseconds =
      (end.tv_nsec - start.tv_nsec) / 1e6 + (end.tv_sec - start.tv_sec) * 1e3;
  //  printf("%s: %lf milliseconds\n", prefix, milliseconds);

  return milliseconds;
}

int main(int argc, char *argv[]) {

  int32_t max_cycles = 100000;

  if (argc == 2) {
    max_cycles = atoi(argv[1]);
  }

  device *io = target_init();

  //uint8_t *buffer = new uint8_t[8]();
  //io->receive(buffer, 8);

  load_protocol();
  
  target_reset(io);

  uint32_t address = 0x2000C000, size = 0x4;

  uint32_t wdata = {0xBAB00001};

  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  for (auto i = 0; i < max_cycles; i++) {
    wdata += i;

    target_write(io, address, wdata);

    uint32_t data = target_read_u32(io, address);

    if (memcmp((const void*)&data, (const void*)&wdata, 4) != 0) {

      cout << "Test " << i << " failed...";
      cout << "    Expecting " << std::hex << wdata;
      cout << " but get 0x" << std::hex << data;
      cout << " at 0x" << std::hex << address;
      cout << endl;

      return 0;
    }

    // address += 4;
  }

  clock_gettime(CLOCK_MONOTONIC, &end);

  cout << "================================================" << endl;
  cout << "Total operation   :  " << max_cycles << endl;
  cout << "Average time      :  " << print_timediff("clock_gettime", start, end)
       << endl;
  cout << "================================================" << endl;
}
