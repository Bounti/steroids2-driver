#include "api.h"

#include <chrono>
#include <ctime>

using namespace std;

double print_timediff(const char *prefix, const struct timespec &start,
                      const struct timespec &end) {
  double milliseconds =
      (end.tv_nsec - start.tv_nsec) / 1e6 + (end.tv_sec - start.tv_sec) * 1e3;
  printf("%s: %lf milliseconds\n", prefix, milliseconds);

  return milliseconds;
}

int main(int argc, char *argv[]) {

  unsigned char operation_type = 'W';

  if (argc == 2) {
    if (argv[1][0] == 'R' or argv[1][0] == 'W')
      operation_type = argv[1][0];
  }

  device *io = target_init();

  load_protocol();

  target_reset(io);

  uint32_t address = 0x20000000, size = 0x4;
  uint32_t wdata = 0xAAAAAAAA;

  cout << "Author  : Corteggiani Nassim" << endl;
  cout << "Contact : nassim.corteggiani@maximintegrated.com" << endl;
  cout << "Brief   : this program measures the operating time for writing";
  cout << " 65000 times the device memory. It makes this measure 1000 times";
  cout << " and then return the average time. " << endl;
  cout << "User can choose between read and write by sending as argument 'R' "
          "or 'W'"
       << endl;

  double time_dataset[100000];

  for (auto i = 0; i < 100000; i++) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    if (operation_type == 'W')
      target_write(io, address, wdata);

    if (operation_type == 'R') {
      auto data = target_read(io, address, size);
      delete data;
    }
    // address += 4;

    clock_gettime(CLOCK_MONOTONIC, &end);

    time_dataset[i] = print_timediff("clock_gettime", start, end);
    
    wdata += i;
  }

  double average_time = 0;
  for (auto i = 0; i < 100000; i++) {
    average_time += time_dataset[i];
  }
  average_time = average_time / 100000;

  cout << "================================================" << endl;
  cout << "Total operation   :  " << 100000 << endl;
  cout << "Average time      :  " << average_time << endl;
  cout << "================================================" << endl;
}
