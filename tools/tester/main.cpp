#include "api.h"

#include <chrono>
#include <ctime>

using namespace std;

int main(int argc, char *argv[]) {

  device *io = target_init();

  unsigned long int data_write = 0xABABABAB10000000;
  unsigned long int data_read  = 0x1000000010000001;
  unsigned long int data = 0;

  uint8_t* i8_packet_r = (uint8_t*) &data_read;
  uint8_t* i8_packet_w = (uint8_t*) &data_write;

  for(int i=0; i<100000; i++) {

    data_write = (unsigned long int)(data++ << 32) | (unsigned long int)0x10000000;

    //printf("Writing %016lx\n", data_write);

    io->send(i8_packet_w, 8);  
    io->send(i8_packet_r, 8);

    printf("Reading data !");
    uint8_t buffer[8] = {0};
    io->receive(buffer, 8);
    printf("> %016lx\n", *((unsigned long int*)&buffer[0]));

  }
  //data = 0x1000000100000000;
  //io->send(i8_packet, 8);


  //res = *((unsigned long int*)&buffer[0]);
  //res = *((unsigned long int*)&buffer[0]) >> 4;
  //return res;  
  //}
}
