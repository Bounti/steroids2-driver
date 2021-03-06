#include "api.h"

#include <chrono>
#include <ctime>

using namespace std;

int main(int argc, char *argv[]) {

  device *io = target_init();

  //unsigned long int data_write = 0xABABABAB10000000;
  //unsigned long int data_read  = 0x2222222222222222;
  //unsigned long int data = 0;
  //uint8_t* i8_packet_r = (uint8_t*) &data_read;
  //uint8_t* i8_packet_w = (uint8_t*) &data_write;
  
  uint8_t i8_packet_w[128] = {
 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB,
 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 
 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 
 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 
 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 
 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 
 0xAB, 0xAB, 0xAB, 0xAB,
 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB,
 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 
 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 
 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 
 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 
 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 
 0xAB, 0xAB, 0xAB, 0xAB
  };
  
  for(int i=0; i<1; i++) {
    //data_write = (unsigned long int)(data++ << 32) | (unsigned long int)0x10000000;
    //printf("Writing %016lx\n", data_write);
    io->send(i8_packet_w, 128);
    //io->send(i8_packet_r, 8);
    //printf("Reading data !");
    //uint8_t buffer[8] = {0};
    //io->receive(buffer, 4);
    //printf("> %016lx\n", *((unsigned long int*)&buffer[0]));
  }

//  int32_t max_cycles = 100000;
//
//  device *io = target_init();
//
//  target_reset(io);
//
//  uint32_t address = 0x2000C000, size = 0x4;
//  uint8_t array[] = {0xAB, 0xAB, 0xAB, 0xAB};
//  uint8_t *wdata = array;
//
//  char* configuration_files = "config.txt";
//
//  /* Test 8bits access */
//  // Read current memory value (4bytes)
//  cout << "Testing 8bits access " << endl;
//  wdata = target_read(io, address, size);
//  wdata[0] += 1;
//  // Then overwrite only first byte
//  target_write(io, address, 0x1, wdata);
//
//  uint8_t *data = target_read(io, address, size);
//  if (memcmp(&data[4], wdata, size) != 0) {
//    cout << "Test " << 1 << " failed...\n";
//    printf("Expecting 0x%02x%02x%02x%02x \n",wdata[0],wdata[1],wdata[2],wdata[3]);
//    printf("But get 0x%02x%02x%02x%02x \n",data[0],data[1],data[2],data[3]);
//    printf("At 0x%08x \n", address);
//    return 0;
//  }
//
//  /* Test 16bits access */
//  cout << "Testing 16bits access " << endl;
//  wdata = target_read(io, address, size);
//  wdata[0] += 1;
//  wdata[1] += 1;
//  target_write(io, address, 0x2, wdata);
//  data = target_read(io, address, size);
//  if (memcmp(&data[4], wdata, size) != 0) {
//    uint16_t *u16_data = (uint16_t *)&data[4];
//    uint16_t *u16_wdata = (uint16_t *)&wdata[0];
//    cout << "Test " << 2 << " failed...";
//    cout << "    Expecting " << std::hex << *u16_wdata;
//    cout << " but get 0x" << std::hex << *u16_data;
//    cout << " at 0x" << std::hex << address;
//    return 0;
//  }
//
//  /* Test 32bits access */
//  cout << "Testing 32bits access " << endl;
//  wdata[0] += 1;
//  wdata[1] += 1;
//  wdata[2] += 1;
//  wdata[3] += 1;
//  target_write(io, address, 0x4, wdata);
//  data = target_read(io, address, size);
//  if (memcmp(&data[4], wdata, size) != 0) {
//    uint32_t *u32_data = (uint32_t *)&data[4];
//    uint32_t *u32_wdata = (uint32_t *)&wdata[0];
//
//    cout << "Test " << 3 << " failed...";
//    cout << "    Expecting " << std::hex << *u32_wdata;
//    cout << " but get 0x" << std::hex << *u32_data;
//    cout << " at 0x" << std::hex << address;
//
//    return 0;
//  }
//
//  // address += 4;
//  delete data;
//
//  cout << "================================================" << endl;
//  cout << "Tested IO         : 8b 16b 32b " << endl;
//  cout << "         :  " << endl;
//  cout << "================================================" << endl;
}
