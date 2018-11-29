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
    max_cycles = (uint32_t)argv[1][0];
  }

  device *io = init();

  reset(io);

  uint32_t address = 0x20000000, size = 0x4;

  uint8_t wdata[] = {0xAB, 0xAB, 0xAB, 0xAB};

  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  for (auto i = 0; i < max_cycles; i++) {
    wdata[0] += i;
    wdata[1] += i;
    wdata[2] += i;
    wdata[3] += i;

    write(io, address, size, wdata);

    uint8_t* data = read(io, address, size);

    if (memcmp(&data[4], wdata, size) != 0) {
      uint32_t* u32_data = (uint32_t*)&data[4];
      uint32_t* u32_wdata = (uint32_t*)&wdata[0];

      cout << "Test " << i << " failed...";
      cout << "    Expecting " << std::hex << *u32_wdata;
      cout << " but get 0x" << std::hex << *u32_data;
      cout << " at 0x" << std::hex << address;

      return 0;
    }

    //address += 4;
    delete data;
  }

  clock_gettime(CLOCK_MONOTONIC, &end);

  cout << "================================================" << endl;
  cout << "Total operation   :  " << max_cycles << endl;
  cout << "Average time      :  " << print_timediff("clock_gettime", start, end)
       << endl;
  cout << "================================================" << endl;
}
